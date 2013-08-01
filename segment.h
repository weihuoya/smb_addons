#pragma once
#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include <ICTCLAS50.h>

class ICTCLAS
{
public:
	ICTCLAS(const char* path = nullptr);
	~ICTCLAS();

	bool initialize(const char* dir = 0);
	bool release();

	bool set_posmap(int map);

	int import_dict_from_file(const char* dict_file);
	int import_dict(const char* dict, int len);

	int save_dict();

	void set_codetype(eCodeType code);

	int segment(const char* text, int len, std::vector<std::string>& group);

private:
	static bool is_initialized;
	eCodeType code_;
};

#endif