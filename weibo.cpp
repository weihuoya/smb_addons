#include "stdafx.h"
#include "config.h"
#include "weibo.h"
#include "utf8.h"
#include "segment.h"



using namespace std;


static shared_ptr<ICTCLAS> segman;
static map<string, int> noise;


static int noise_initialize(const char* dic)
{
	string term;
	ifstream fin(dic);
	noise.clear();
	while(getline(fin, term)) if(!term.empty()) noise.insert(make_pair(term, 0));
	return noise.size();
}


static bool token_filter(const string& sym)
{
	bool flag;
	const char *p, *end;
	int len, count;
	unsigned int unicode;
	
	flag = true, p = sym.c_str(), len = sym.size(), end = p + len;

	if( len < 1 || noise.find(sym) != noise.end() )
		return flag;

	while(flag && p < end)
	{
		len = end - p;
		count = utf8_mbtowc(&unicode, (const unsigned char*)p, len);
		if(count < 1) break;

		flag = (unicode > 0 && unicode < '0') ||
			(unicode > '9' && unicode < '@') ||
			(unicode > 'Z' && unicode < 'a') ||
			(unicode > 'z' && unicode < 256) ||
			(unicode > 8200 && unicode < 9750) ||
			(unicode > 12288 && unicode < 12312) ||
			(unicode > 65280 && unicode < 65374);

		p += count;
	}

	return flag;
}


bool WeiboInitialize(const char* file)
{
	bool ret = false;
	WeiboConfig* conf = CONF::instance();
	if( conf->initialize(file) )
	{
		noise_initialize(conf->dict_noise.c_str());
		segman = make_shared<ICTCLAS>(conf->ICTCLAS.c_str());
		segman->import_dict_from_file(conf->dict_user.c_str());
		ret = !!segman;
	}
	return ret;
}

void Weibo::resolve(const char *text, int len)
{
	bool handled;
	int count;
	const char *q, *p, *end;
	unsigned int unicode;
	string elem, content;
	vector<string>::iterator iter;

	this->urls.clear();
	this->topics.clear();
	this->emoticon.clear();
	this->mentions.clear();
	this->tokens.clear();

	p = text;
	end = p + len;

	while(p < end)
	{
		len = end - p;
		handled = false;
		count = utf8_mbtowc(&unicode, (unsigned char*)p, len);
		if(count < 1) 
		{
			p += 1;
			continue;
		}

		switch(unicode)
		{
		case '[':
			q = ++p;
			while(q != end && *q != ']') ++q;
			if(q == end || q - p < 2 || q - p > 20)
			{
				--p;
				break;
			}
#if (_MSC_VER > 1600)
			this->emoticon.emplace_back(p, q);
#else
			this->emoticon.push_back(string(p, q));
#endif
			handled = true;
			p = ++q;
			break;

		case '@'://@
			q = ++p;
			while(q < end)
			{
				count = utf8_mbtowc(&unicode, (unsigned char*)q, len);
				if( count > 0 && ( unicode < '0' || unicode == 12289 || (unicode > '9' && unicode < 'A') || (unicode > 65280 && unicode < 65310) ) ) break;
				q += count;
			}

			if(q - p < 3 || q - p > 48)
			{
				--p;
				break;
			}
#if (_MSC_VER > 1600)
			this->mentions.emplace_back(p, q);
#else
			this->mentions.push_back(string(p, q));
#endif
			handled = true;
			p = q + count;
			break;

		case '#':
			while(*p == '#')
			{
				q = ++p;
				while(q != end && *q != '#') ++q;
				if(q == end || q - p > 100)
				{
					if(!handled) --p;
					break;
				}
#if (_MSC_VER > 1600)
				this->topics.emplace_back(p, q);
#else
				this->topics.push_back(string(p, q));
#endif
				content.append(p-1, q+1);
				handled = true;
				p = q;
			}
			break;

		case 'h':
			if(len > 6 && strncmp(p, "http://", 7) == 0)
			{
				q = p;
				while(q != end && *q > 0 && *q < 127 && *q != ' ') ++q;
#if (_MSC_VER > 1600)
				this->urls.emplace_back(p, q);
#else
				this->urls.push_back(string(p, q));
#endif
				handled = true;
				p = q;
				break;
			}
		}

		if(!handled)
		{
			content.append(p, count);
			p += count;
		}
	}

	segman->segment(content.c_str(), content.size(), this->tokens);

	iter = unique(this->mentions.begin(), this->mentions.end());
	this->mentions.resize( distance(this->mentions.begin(), iter) );

	iter = remove_if(this->tokens.begin(), this->tokens.end(), token_filter);
	this->tokens.resize( distance(this->tokens.begin(), iter) );
}


std::ostream& operator<<(std::ostream& out, const User& user)
{
	//boost::posix_time::ptime pt = boost::posix_time::from_time_t(user.created_at);
	out<<"ID: "<<user.id<<", NAME: "<<user.screen_name<<endl;
	out<<"ST: "<<user.statuses_count<<", FR: "<<user.friends_count<<", FL: "<<user.followers_count<<endl;
	out<<"DT: "<<mongo::Date_t(user.created_at).toString()<<endl;
	return out;
}


std::ostream& operator<<(std::ostream& out, const Weibo& weibo)
{
	out<<"ID: "<<weibo.id<<endl<<"TEXT: "<<weibo.content<<endl;

	out<<"L: ";
	copy(weibo.urls.begin(), weibo.urls.end(), ostream_iterator<string>(out, " "));
	out<<endl;

	out<<"#: ";
	copy(weibo.topics.begin(), weibo.topics.end(), ostream_iterator<string>(out, " ")); 
	out<<endl;

	out<<"@: ";
	copy(weibo.mentions.begin(), weibo.mentions.end(), ostream_iterator<string>(out, " "));
	out<<endl;

	out<<"E: ";
	copy(weibo.emoticon.begin(), weibo.emoticon.end(), ostream_iterator<string>(out, " "));
	out<<endl;

	out<<"T: ";
	copy(weibo.tokens.begin(), weibo.tokens.end(), ostream_iterator<string>(out, " "));
	out<<endl;

	return out;
}