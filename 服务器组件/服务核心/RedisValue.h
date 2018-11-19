#ifndef _REDIS_VALUE_H_
#define _REDIS_VALUE_H_

#include "Stdafx.h"
#include <map>
#include <string>

namespace Redis
{
	class Value
	{
	public:
		Value();
		Value(bool v);
		Value(BYTE v);
		Value(WORD v);
		Value(DWORD v);
		Value(LONG v);
		Value(LONGLONG v);
		Value(const std::string& v);
		Value(const char* v);
		Value(const Value& v);

	public:
		bool ToBool();
		BYTE ToByte();
		WORD ToWord();
		DWORD ToDword();
		LONG ToLong();
		LONGLONG ToScore();

	private:
		std::string		m_StringVal;
	};

	typedef std::map<std::string, Value> ValueMap;

	class ValueManager
	{
	protected:
		ValueMap	mValues;

	public:
		Value& operator [](const std::string& key);
		Value& operator [](const char* key);
	};
};

extern Redis::Value NullValue;

#endif