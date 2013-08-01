#include "stdafx.h"
#include "segment.h"


bool ICTCLAS::is_initialized = false;


ICTCLAS::ICTCLAS(const char* path) : code_(CODE_TYPE_UTF8)
{
	initialize(path);
}

ICTCLAS::~ICTCLAS()
{
	release();
}

bool ICTCLAS::initialize(const char* dir)
{
	if(!is_initialized) is_initialized = ICTCLAS_Init(dir);
	return is_initialized;
}

bool ICTCLAS::release()
{
	if(is_initialized) return ICTCLAS_Exit();
	return false;
}

bool ICTCLAS::set_posmap(int map)
{
	return ICTCLAS_SetPOSmap(map);
}

int ICTCLAS::import_dict_from_file(const char* dict_file)
{
	return ICTCLAS_ImportUserDictFile(dict_file, code_);
}

int ICTCLAS::import_dict(const char* dict, int len)
{
	return ICTCLAS_ImportUserDict(dict, len, code_);
}

int ICTCLAS::save_dict()
{
	return ICTCLAS_SaveTheUsrDic();
}

void ICTCLAS::set_codetype(eCodeType code)
{
	code_ = code;
}

int ICTCLAS::segment(const char* text, int len, std::vector<std::string>& group)
{
	std::string token;
	char *out, *sp, *ep;
	int ret, size;
	
	size = 0;
	out = new char[len * 6];
	ret = ICTCLAS_ParagraphProcess(text, len, out, code_, false);
	out[ret] = 0;

	ep = sp = out;
	while(*ep != 0)
	{
		while(*sp != 0 && *sp == ' ') 
			++sp;

		ep = sp;

		while(*ep != 0 && *ep != ' ') 
			++ep;

		if(ep > sp )
		{
			token.assign(sp, ep);
			group.push_back(token);
			size += 1;
			sp = ep;
		}
	}
		
	return size;
}