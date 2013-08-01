#include "stdafx.h"
#include "config.h"

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


using std::string;
using namespace boost::property_tree;


bool WeiboConfig::initialize(const char* file)
{
	ptree tree;
	bool ret = true;

	try
	{
		read_json(file, tree);

		ICTCLAS = tree.get<string>("ICTCLAS");

		dict_user = tree.get<string>("dictionary.user");
		dict_noise = tree.get<string>("dictionary.noise");

		db_server = tree.get<string>("database.server");
		ns_user = tree.get<string>("database.user");
		ns_status = tree.get<string>("database.status");
		ns_cluster = tree.get<string>("database.cluster");
		ns_record = tree.get<string>("database.record");

		user_statuses = tree.get<int>("filter.user.status");
		user_friends = tree.get<int>("filter.user.friend");
		user_followers = tree.get<int>("filter.user.follower");
		
		status_length = tree.get<int>("filter.status.length");
		status_repost = tree.get<int>("filter.status.repost");
		status_comment = tree.get<int>("filter.status.comment");
	}
	catch(ptree_error& e)
	{
		std::cerr<<e.what()<<std::endl;
		ret = false;
	}

	return ret;
}


/*
bool SimpleConfig::initialize(const char* file)
{
	bool ret = true;

	try
	{
		read_json(file, tree_);
	}
	catch(ptree_error& e)
	{
		ret = false;
	}

	return ret;
}


string SimpleConfig::ICTCLAS() const
{
	return tree_.get<string>("ICTCLAS");
}

string SimpleConfig::DBSetting(CONFKEY key) const
{
	string k;
	switch(key)
	{
	case SERVER:
		k = "server";
		break;
	case USER:
		k = "user";
		break;
	case STATUS:
		k = "status";
		break;
	case CLUSTER:
		k = "cluster";
		break;
	default:
		return k;
	}

	return tree_.get<string>(k);
}

string SimpleConfig::Dictionary(CONFKEY key) const
{
	string k;
	switch(key)
	{
	case USER:
		k = "user";
		break;
	case NOISE:
		k = "noise";
		break;
	default:
		return k;
	}

	return tree_.get<string>(k);
}


int SimpleConfig::UserFilter(CONFKEY key) const
{
	string k;
	switch(key)
	{
	case STATUS:
		k = "user";
		break;
	case FRIEND:
		k = "friend";
		break;
	case FOLLOWER:
		k = "follower";
		break;
	default:
		return 0;
	}

	return tree_.get<int>(k);
}

int SimpleConfig::StatusFilter(CONFKEY key) const
{
	string k;
	switch(key)
	{
	case STATUS:
		k = "length";
		break;
	case REPOST:
		k = "repost";
		break;
	case COMMENT:
		k = "comment";
		break;
	default:
		return 0;
	}

	return tree_.get<int>(k);
}
*/