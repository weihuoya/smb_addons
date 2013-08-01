#include "stdafx.h"
#include "weibo.h"
#include "query.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time.hpp>

//
CircleQuery::CircleQuery(mongo::DBClientConnection& conn):ns_("sina-weibo.circle"), conn_(conn)
{
	fields_ = BSON("_id"<<0<<"uid"<<1<<"cid"<<1);
}

unsigned long long CircleQuery::count(mongo::BSONObj query)
{
	return conn_.count(ns_, query);
}


//
UserQuery::UserQuery(mongo::DBClientConnection& conn):ns_("sina-weibo.user"), conn_(conn)
{
	fields_ = BSON("_id"<<0<<"id"<<1<<"screen_name"<<1<<"created_at"<<1<<"followers_count"<<1<<"friends_count"<<1<<"statuses_count"<<1);
}

void UserQuery::build(User& user, const mongo::BSONObj& obj)
{
	user.id = obj.getField("id").numberLong();
	user.screen_name = obj.getField("screen_name").String();
	user.created_at = obj.getField("created_at").Date();
	user.followers_count = obj.getField("followers_count").numberInt();
	user.friends_count = obj.getField("friends_count").numberInt();
	user.statuses_count = obj.getField("statuses_count").numberInt();
}

unsigned long long UserQuery::count(mongo::BSONObj query)
{
	return conn_.count(ns_, query);
}


bool UserQuery::findOne(long long uid, User& user)
{
	bool ret = false;
	mongo::BSONObj obj;
	mongo::Query query = QUERY("id"<<uid);
	obj = conn_.findOne(ns_, query, &fields_);
	if(!obj.isEmpty())
	{
		build(user, obj);
		ret = true;
	}
	return ret;
}


bool UserQuery::query(const std::string& username, int count, int skip)
{
	std::string regex;
	mongo::Query query;

	regex += ".*";
	regex += username;
	regex += ".*";

	query = QUERY("screen_name"<<mongo::BSONRegEx(regex)).sort("id", 1);

	cursor_ = conn_.query(ns_, query, count, skip, &fields_);
	return !!cursor_ && cursor_->more();
}


bool UserQuery::query(const std::pair<long long, long long>& user_range, int count, int skip)
{
	mongo::Query query;

	if(user_range.first == 0 && user_range.second == 0)
	{
		
	}
	else if(user_range.first == 0)
	{
		query = QUERY("id"<<mongo::LT<<user_range.second);
	}
	else if(user_range.second == 0)
	{
		query = QUERY("id"<<mongo::GTE<<user_range.first);
	}
	else
	{
		query = QUERY("id"<<mongo::GTE<<user_range.first<<mongo::LT<<user_range.second);
	}

	query.sort("id");

	cursor_ = conn_.query(ns_, query, count, skip, &fields_);
	return !!cursor_ && cursor_->more();
}

bool UserQuery::more(User& user)
{
	bool ret = false;

	if( cursor_->more() )
	{
		mongo::BSONObj obj;
		obj = cursor_->next();

		build(user, obj);
		ret = true;
	}

	return ret;
}


//
WeiboQuery::WeiboQuery(mongo::DBClientConnection& conn):ns_("sina-weibo.status"), conn_(conn)
{
	fields_ = BSON("_id"<<0<<"id"<<1<<"uid"<<1<<"sid"<<1<<"created_at"<<1<<"reposts_count"<<1<<"comments_count"<<1<<"text"<<1);
}

unsigned long long WeiboQuery::count(mongo::BSONObj query)
{
	return conn_.count(ns_, query);
}

bool WeiboQuery::query(const std::pair<mongo::Date_t, mongo::Date_t>& time_range, int count, int skip)
{
	mongo::Query query = QUERY("created_at"<<mongo::GTE<<time_range.first<<mongo::LT<<time_range.second).sort("reposts_count", -1);
	cursor_ = conn_.query(ns_, query, count, skip, &fields_);
	return !!cursor_;
}

bool WeiboQuery::query(long long uid, int count, int skip)
{
	mongo::Query query = QUERY("uid"<<uid).sort("reposts_count", -1);
	cursor_ = conn_.query(ns_, query, count, skip, &fields_);
	return !!cursor_ && cursor_->more();
}

void WeiboQuery::build(Weibo& weibo, const mongo::BSONObj& obj)
{
	weibo.id = obj.getField("id").numberLong();
	weibo.uid = obj.getField("uid").numberLong();
	weibo.sid = obj.getField("sid").numberLong();
	weibo.created_at = obj.getField("created_at").Date();
	weibo.reposts_count = obj.getField("reposts_count").numberInt();
	weibo.comments_count = obj.getField("comments_count").numberInt();
	weibo.content = obj.getField("text").String();
	weibo.resolve(weibo.content.c_str(), weibo.content.size());
}

bool WeiboQuery::more(Weibo& weibo)
{
	bool ret = false;

	if( cursor_->more() )
	{
		mongo::BSONObj obj;
		obj = cursor_->next();

		build(weibo, obj);
		ret = true;
	}

	return ret;
}






//
mongo::Date_t MongoDate(const int year, const int month, const int day)
{
	boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
	try
	{
		boost::posix_time::ptime ptime_date(boost::gregorian::date(year,month,day));
		boost::posix_time::time_duration const diff = ptime_date - epoch;
		long long ms = diff.total_milliseconds();
		return mongo::Date_t(ms);
	}
	catch (std::out_of_range& oor)
	{
		std::cerr << year << "-" << month << "-" << day << " is an invalid date: " << oor.what() << std::endl;
		return mongo::Date_t(0);
	}
}


//"2001-12-06 12:33:45"
mongo::Date_t MongoDate(const char* date)
{
	boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
	try
	{
		boost::posix_time::ptime ptime_date = boost::posix_time::time_from_string(date);
		boost::posix_time::time_duration diff = ptime_date - epoch;
		long long ms = diff.total_milliseconds();
		return mongo::Date_t(ms);
	}
	catch (std::out_of_range& oor)
	{
		std::cerr << '"' << date << "\" is an invalid date: " << oor.what() << std::endl;
		return mongo::Date_t();
	}
}


mongo::Date_t MongoDate()
{
	boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
	boost::posix_time::ptime ptime_date( boost::gregorian::day_clock::local_day() );
	//boost::posix_time::second_clock::local_time();
	//boost::gregorian::day_clock::local_day();
	boost::posix_time::time_duration diff = ptime_date - epoch;

	long long ms = diff.total_milliseconds();
	return mongo::Date_t(ms);
}