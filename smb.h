#pragma once
#ifndef _HELLO_H_
#define _HELLO_H_

#include <node.h>

using namespace v8;


class SMB : public node::ObjectWrap
{
	typedef std::tuple< SMB*, std::string, Persistent<Function>, bool> BUNDLE1;
	typedef std::tuple< SMB*, mongo::Date_t, mongo::Date_t, Persistent<Function>> BUNDLE2;

public:
	static void Init(Handle<Object> target);

private:
	SMB() {}
	~SMB() {}

	static Handle<Value> New(const Arguments& args);

	static Handle<Value> Initialize(const Arguments& args);
    static void doing_initialize(uv_work_t *req);
	static void after_initialize(uv_work_t *req, int status);

	static Handle<Value> Analyze(const Arguments& args);
	static void doing_analyze(uv_work_t *req);
	static void after_analyze(uv_work_t *req, int status);

	static Handle<Value> Release(const Arguments& args);
	//static void doing_release(uv_work_t *req);
	//static void after_release(uv_work_t *req);
};


template<typename T1>
inline std::tuple<T1>* new_tuple(T1&& x1)
{
	return new std::tuple<T1>(x1);
}

template<typename T1, typename T2>
inline std::tuple<T1, T2>* new_tuple(T1& x1, T2& x2)
{
	return new std::tuple<T1, T2>(x1, x2);
}

template<typename T1, typename T2, typename T3>
inline std::tuple<T1, T2, T3>* new_tuple(const T1& x1, const T2& x2, const T3& x3)
{
	return new std::tuple<T1, T2, T3>(x1, x2, x3);
}

template<typename T1, typename T2, typename T3, typename T4>
inline std::tuple<T1, T2, T3, T4>* new_tuple(T1& x1, T2& x2, T3& x3, T4& x4)
{
	return new std::tuple<T1, T2, T3, T4>(x1, x2, x3, x4);
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
inline std::tuple<T1, T2, T3, T4, T5>* new_tuple(T1& x1, T2& x2, T3& x3, T4& x4, T5& x5)
{
	return new std::tuple<T1, T2, T3, T4, T5>(x1, x2, x3, x4, x5);
}


#endif