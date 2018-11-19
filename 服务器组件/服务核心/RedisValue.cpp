#include "Stdafx.h"
#include "RedisValue.h"

using namespace Redis;

Redis::Value NullValue;

Value::Value()
{
}

Value::Value(bool v)
{
	m_StringVal = v ? "true" : "false";
}

Value::Value(BYTE v)
{
	char buff[4] = { 0 };
	sprintf_s(buff, sizeof(buff), "%d", v);
	m_StringVal = buff;
}

Value::Value(WORD v)
{
	char buff[8] = { 0 };
	sprintf_s(buff, sizeof(buff), "%d", v);
	m_StringVal = buff;
}

Value::Value(DWORD v)
{
	char buff[16] = { 0 };
	sprintf_s(buff, sizeof(buff), "%u", v);
	m_StringVal = buff;
}

Value::Value(LONG v)
{
	char buff[32] = { 0 };
	sprintf_s(buff, sizeof(buff), "%ld", v);
	m_StringVal = buff;
}

Value::Value(LONGLONG v)
{
	char buff[32] = { 0 };
	sprintf_s(buff, sizeof(buff), "%I64d", v);
	m_StringVal = buff;
}

Value::Value(const std::string& v)
{
	m_StringVal = v;
}

Value::Value(const char* v)
{
	m_StringVal = v;
}

Value::Value(const Value& v)
{
	m_StringVal = v.m_StringVal;
}

bool Value::ToBool()
{
	if (m_StringVal.compare("true") == 0)
		return true;

	return false;
}

BYTE Value::ToByte()
{
	return atoi(m_StringVal.c_str());
}

WORD Value::ToWord()
{
	return atoi(m_StringVal.c_str());
}

DWORD Value::ToDword()
{
	return atoi(m_StringVal.c_str());
}

LONG Value::ToLong()
{
	return atol(m_StringVal.c_str());
}

LONGLONG Value::ToScore()
{
	return atoll(m_StringVal.c_str());
}

Value& ValueManager::operator [](const std::string& key)
{
	ValueMap::const_iterator it = mValues.find(key);
	if (it != mValues.end())
	{
		return mValues[key];
	}

	return NullValue;
}

Value& ValueManager::operator [](const char* key)
{
	ValueMap::const_iterator it = mValues.find(key);
	if (it != mValues.end())
	{
		return mValues[key];
	}

	return NullValue;
}