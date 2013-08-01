#include "stdafx.h"
#include "config.h"
#include "weibo.h"
#include "analyze.h"

#include <uv.h>
#include "smb.h"

using namespace std;

/**
 * SMB
**/
void SMB::Init(Handle<Object> target)
{
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	tpl->SetClassName(String::NewSymbol("SMB"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->PrototypeTemplate()->Set(String::NewSymbol("initialize"), FunctionTemplate::New(Initialize)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("analyze"), FunctionTemplate::New(Analyze)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("release"), FunctionTemplate::New(Release)->GetFunction());

	target->Set(String::NewSymbol("SMB"), Persistent<Function>::New(tpl->GetFunction()));
}

Handle<Value> SMB::New(const Arguments& args)
{
	HandleScope scope;
	SMB* obj = new SMB();
	obj->Wrap(args.This());
	return args.This();
}

//smb.initialize
Handle<Value> SMB::Initialize(const Arguments& args)
{
	HandleScope scope;

	if(args.Length() !=  2 || !args[0]->IsString() || !args[1]->IsFunction())
	{
		return ThrowException(Exception::TypeError(String::New("arguments expect: config file path!")));
	}

	SMB* self = ObjectWrap::Unwrap<SMB>(args.This());
	string config = *String::AsciiValue( args[0]->ToString() );
	Local<Function> callback = Local<Function>::Cast(args[1]);

	uv_work_t* req = new uv_work_t;
	uv_loop_t* loop = uv_default_loop();
	req->data = new BUNDLE1(self, config, Persistent<Function>::New(callback), false);
	uv_queue_work(loop, req, doing_initialize, after_initialize);

	self->Ref();
	return Undefined();
}

void SMB::doing_initialize(uv_work_t *req)
{
	BUNDLE1& bundle = *static_cast<BUNDLE1*>(req->data);
	string& str = std::get<1>(bundle);
	bool& ret = std::get<3>(bundle);
	ret = WeiboInitialize(str.c_str());
}

void SMB::after_initialize(uv_work_t *req, int status)
{
	HandleScope scope;

	BUNDLE1& bundle = *static_cast<BUNDLE1*>(req->data);
	Persistent<Function>& callback = std::get<2>(bundle);
	Handle<Value> arg;

	if(!std::get<3>(bundle))
	{
		Handle<Object> global = Context::GetCurrent()->Global();
		Handle<Value> value = global->Get(String::New("Error"));
		if(value->IsObject())
		{
			Handle<Object> error = Handle<Object>::Cast(value);
			Handle<Value> msg = String::New("[SMB] weibo initialize failed");
			arg = error->CallAsConstructor(1, &msg);
		}
	}

	v8::TryCatch try_catch;
	callback->Call(Context::GetCurrent()->Global(), 0, NULL);
	if(try_catch.HasCaught())
	{
		node::FatalException(try_catch);
	}

	std::get<0>(bundle)->Unref();
	callback.Dispose();
	delete &bundle;
}

//smb.analyze
Handle<Value> SMB::Analyze(const Arguments& args)
{
	HandleScope scope;

	if(args.Length() < 3 || !args[0]->IsDate() || !args[1]->IsDate() || !args[2]->IsFunction())
	{
		return ThrowException(Exception::TypeError(String::New("arguments expect: start date and end date")));
	}
	
	SMB* self = ObjectWrap::Unwrap<SMB>(args.This());
	mongo::Date_t start = (unsigned long long )(v8::Date::Cast( *args[0] )->NumberValue());
	mongo::Date_t end = (unsigned long long )(v8::Date::Cast( *args[1] )->NumberValue());
	Local<Function> callback = Local<Function>::Cast(args[2]);

	uv_work_t* req = new uv_work_t;
	uv_loop_t* loop = uv_default_loop();
	req->data = new BUNDLE2(self, start, end, Persistent<Function>::New(callback));
	uv_queue_work(loop, req, doing_analyze, after_analyze);
	
	self->Ref();

	return Undefined();
}

void SMB::doing_analyze(uv_work_t *req)
{
	BUNDLE2& bundle = * static_cast<BUNDLE2*>(req->data);
	SMB* self = std::get<0>(bundle);
	mongo::Date_t start = std::get<1>(bundle);
	mongo::Date_t end = std::get<2>(bundle);

	WeiboAnalyze analyze;
	analyze.cluster(start, end);
}

void SMB::after_analyze(uv_work_t *req, int status)
{
	HandleScope scope;

	BUNDLE2& bundle = *static_cast<BUNDLE2*>(req->data);
	Persistent<Function>& callback = std::get<3>(bundle);

	v8::TryCatch try_catch;
	callback->Call(Context::GetCurrent()->Global(), 0, NULL);
	if(try_catch.HasCaught())
	{
		node::FatalException(try_catch);
	}

	cout<<"[SMB] weibo data analyze finished"<<endl;

	std::get<0>(bundle)->Unref();
	callback.Dispose();
	delete &bundle;
}

//smb.release
Handle<Value> SMB::Release(const Arguments& args)
{
	HandleScope scope;
	Handle<Value> arg;

	return scope.Close(arg);
}


//exports addon object
void Init(Handle<Object> target)
{
	SMB::Init(target);
}

NODE_MODULE(smb, Init)