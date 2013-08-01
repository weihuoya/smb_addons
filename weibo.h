#pragma once
#ifndef _WEIBO_H_
#define _WEIBO_H_


struct User
{
	unsigned long long id;
	unsigned long long created_at;
	std::string screen_name;
	int followers_count;
	int friends_count;
	int statuses_count;
};

struct Weibo
{
	unsigned long long id;
	unsigned long long uid;
	unsigned long long sid;
	unsigned long long created_at;
	int reposts_count;
	int comments_count;
	std::string content;

	//std::vector<float> tfidf;
	std::vector< std::pair<size_t, float> > tfidf;
	std::vector<std::string> urls;
	std::vector<std::string> topics;
	std::vector<std::string> emoticon;
	std::vector<std::string> mentions;
	std::vector<std::string> tokens;

	void resolve(const char *text, int len);
};

bool WeiboInitialize(const char* file);

std::ostream& operator<<(std::ostream& out, const User& user);
std::ostream& operator<<(std::ostream& out, const Weibo& weibo);


#endif