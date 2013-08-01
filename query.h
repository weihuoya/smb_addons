#pragma once


class CircleQuery
{
public:
	enum CIRCLE_TYPE
	{
		FRIEND = 0,
		FOLLOWER = 1
	};

	CircleQuery(mongo::DBClientConnection& conn);

	unsigned long long count(mongo::BSONObj query = mongo::BSONObj());
	
	template<typename Fn>
	unsigned long long foreach(long long uid, CIRCLE_TYPE type, Fn func)
	{
		bool ret = true;
		long long i = 0, next = 0, limit;
		mongo::BSONObj obj;
		mongo::Query query;
		std::shared_ptr<mongo::DBClientCursor> cursor;

		limit = this->count(BSON("uid"<<uid<<"type"<<(int)type));

		while(ret && i < limit)
		{
			query = QUERY("uid"<<uid<<"type"<<(int)type<<"cid"<<mongo::GTE<<next).sort("cid", 1);
			cursor = conn_.query(ns_, query, 512, 0, &fields_);
			ret = !!cursor && cursor->more();

			while(cursor->more())
			{
				obj = cursor->next();
				next = obj.getField("cid").numberLong();
				func(next);
				i += 1;
			}
			next += 1;
		}

		return limit;
	}

private:
	std::string ns_;
	mongo::DBClientConnection& conn_;
	mongo::BSONObj fields_;
	//std::shared_ptr<mongo::DBClientCursor> cursor_;
};


class UserQuery
{
public:
	UserQuery(mongo::DBClientConnection& conn);

	bool findOne(long long uid, User& user);
	unsigned long long count(mongo::BSONObj query = mongo::BSONObj());

	bool query(const std::string& username, int count = 0, int skip = 0);
	bool query(const std::pair<long long, long long>& user_range, int count = 0, int skip = 0);
	bool more(User& user);

	void build(User& user, const mongo::BSONObj& obj);

	template<typename Fn>
	unsigned long long foreach(Fn func)
	{
		bool ret = true;
		long long i = 0, next = 0, limit;
		User user;
		mongo::BSONObj obj;
		mongo::Query query;
		std::shared_ptr<mongo::DBClientCursor> cursor;

		limit = this->count();

		while(ret && i < limit)
		{
			query = QUERY("id"<<mongo::GTE<<next).sort("id", 1);
			cursor = conn_.query(ns_, query, 512, 0, &fields_);
			ret = !!cursor && cursor->more();

			while(cursor->more())
			{
				obj = cursor->next();
				this->build(user, obj);
				func(user);
				i += 1;
			}
			next = user.id + 1;
		}

		return limit;
	}

private:
	std::string ns_;
	mongo::DBClientConnection& conn_;
	mongo::BSONObj fields_;
	std::shared_ptr<mongo::DBClientCursor> cursor_;
};


class WeiboQuery
{
public:
	WeiboQuery(mongo::DBClientConnection& conn);

	unsigned long long count(mongo::BSONObj query = mongo::BSONObj());

	bool query(long long uid, int count = 0, int skip = 0);
	bool query(const std::pair<mongo::Date_t, mongo::Date_t>& time_range, int count = 0, int skip = 0);
	bool more(Weibo& weibo);

	void build(Weibo& weibo, const mongo::BSONObj& obj);

private:
	std::string ns_;
	mongo::DBClientConnection& conn_;
	mongo::BSONObj fields_;
	std::shared_ptr<mongo::DBClientCursor> cursor_;
};


mongo::Date_t MongoDate();
mongo::Date_t MongoDate(const char* date); //"2001-12-06 12:33:45"
mongo::Date_t MongoDate(const int year, const int month, const int day);
