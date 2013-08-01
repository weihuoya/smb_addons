#pragma once


// user  : 'sina-weibo.user'
// status: 'sina-weibo.status'
// result: 'sina-weibo.cluster'
// format: {id: 0, k: 0, sid: 0}
// cluster_count : k
// content_filter: min_length
// repost_filter : min_repost
// comment_filter: min_comment

class WeiboAnalyze
{
public:
	WeiboAnalyze();
	void cluster(mongo::Date_t start, mongo::Date_t end, int count = 0);

private:
	mongo::DBClientConnection conn_;
};