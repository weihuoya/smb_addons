#include "stdafx.h"
#include "config.h"
#include "weibo.h"
#include "query.h"
#include "vsm.h"
#include "analyze.h"


using namespace std;


static unsigned int HashFunction(const char *buf, int len) 
{
    unsigned int hash = 5381;
	const unsigned char *p = (const unsigned char*)buf;
    while (len--) hash = ((hash << 5) + hash) + (*p++); /* hash * 33 + c */
    return hash;
}

static unsigned int ClusterID(mongo::Date_t start, mongo::Date_t end)
{
	string text;
	stringstream sout;
	sout<<start<<'-'<<end;
	text = sout.str();
	return HashFunction(text.c_str(), text.size());
}


WeiboAnalyze::WeiboAnalyze()
{
	WeiboConfig* conf = CONF::instance();
	conn_.connect(conf->db_server);
	conn_.ensureIndex(conf->ns_record, BSON("id"<<1), true);
	conn_.ensureIndex(conf->ns_cluster, BSON("sid"<<1), true);
}


void WeiboAnalyze::cluster(mongo::Date_t start, mongo::Date_t end, int count)
{
	unsigned int k, id, total = 0;
	User user;
	WeiboCluster vsm;
	UserQuery user_query(conn_);
	WeiboQuery weibo_query(conn_);
	WeiboCluster::Document weibo;
	WeiboCluster::Collection collection;
	WeiboCluster::Cluster result;
	WeiboCluster::Cluster::const_iterator ii;

	id = ClusterID(start, end);
	mongo::Date_t now = MongoDate();
	WeiboConfig* conf = CONF::instance();

	weibo_query.query(make_pair(start, end), count);

	weibo = make_shared<Weibo>();
	while(weibo_query.more(*weibo))
	{
		total += 1;

		//根据微博信息过滤微博
		if(!weibo->mentions.empty() || weibo->sid ||
			weibo->content.size() < conf->status_length || 
			weibo->reposts_count < conf->status_repost ||
			weibo->comments_count < conf->status_comment)
			continue;

		//根据用户信息过滤微博
		if( user_query.findOne(weibo->uid, user) && 
			(user.statuses_count < conf->user_statuses || 
			user.friends_count < conf->user_friends || 
			user.followers_count < conf->user_followers) )
			continue;

		collection.push_back(weibo);
		weibo = make_shared<Weibo>();
	}

	k = collection.size();
	if(k > 1600) k /= 100; 
	else if(k > 100) k = 16;
	else return;

	vsm.DocumentCluster(result, collection, k);
	k = result.size();

	for(k = 0, ii = result.begin(); ii != result.end(); ++k, ++ii)
	{
		std::for_each(ii->begin(), ii->end(), [this, &conf, k, id](const WeiboCluster::Document& doc){
			this->conn_.insert( conf->ns_cluster, BSON("id"<<(double)id<<"k"<<k<<"sid"<<(double)doc->id));
		});
	}

	this->conn_.insert(conf->ns_record, BSON("id"<<(double)id<<"date_start"<<start<<"date_end"<<end<<"status_total"<<total<<"status_count"<<collection.size()<<"k"<<k<<"timestamp"<<now));
}