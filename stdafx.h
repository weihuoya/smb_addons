#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define STRICT_TYPED_ITEMIDS
#define _CRT_SECURE_NO_WARNINGS
//#define _CPPUNWIND


#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <stack>
#include <memory>
#include <map>
#include <set>
#include <iterator>
#include <hash_map>
#include <iostream>
#include <fstream>
#include <functional>

#include <tchar.h>
#include <windows.h>

#include <mongo\client\dbclient.h>
//#include <ICTCLAS50.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "ICTCLAS50.lib")

#ifdef _DEBUG
#pragma comment(lib, "mongoclientd.lib")
#else
#pragma comment(lib, "mongocliento.lib")
#endif

//#ifdef _strnicmp
//#udef strnicmp
#define strnicmp _strnicmp
//#endif