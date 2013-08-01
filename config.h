#pragma once

#include <boost/property_tree/ptree.hpp>
#include "singleton.h"

/*
class SimpleConfig
{
public:
	enum CONFKEY
	{
		SERVER,
		USER,
		STATUS,
		CLUSTER,
		NOISE,
		FRIEND,
		FOLLOWER,
		REPOST,
		COMMENT
	};

	bool initialize(const char* file);
	
	std::string ICTCLAS() const;
	std::string DBSetting(CONFKEY key) const;
	std::string Dictionary(CONFKEY key) const;

	int UserFilter(CONFKEY key) const;
	int StatusFilter(CONFKEY key) const;

private:
	boost::property_tree::ptree tree_;
};
*/



struct WeiboConfig
{
	int user_statuses, user_followers, user_friends;
	int status_length, status_repost, status_comment;
	std::string db_server, ns_user, ns_status, ns_cluster, ns_record;
	std::string dict_user, dict_noise;
	std::string ICTCLAS;

	bool initialize(const char* file);
};

typedef Singleton<WeiboConfig> CONF;