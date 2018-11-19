#include "StdAfx.h"
#include "RedisInstance.h"
#include <vector>

using namespace std;

// 错误描述。
const char* CRedisInstance::_errDes[ERR_BOTTOM] =
{
	"No error.",
	"NULL pointer ",
	"No connection to the redis server.",
	"Inser Error,pivot not found.",
	"key not found",
	"hash field not found",
	"error index"
};

CRedisInstance::CRedisInstance()
{
	_redCtx = NULL;
	_host.clear();
	_password.clear();
	_port = 0;
	_timeout = 0;
	_connected = false;
	_errStr = _errDes[ERR_NO_ERROR];
}

void CRedisInstance::init(const std::string& host, const uint16_t port, const std::string& password, const uint32_t timeout)
{
	_host = host;
	_port = port;
	_password = password;
	_timeout = timeout;
}

bool CRedisInstance::_getError(const redisReply* reply)  throw (NullReplyException)
{
	_errStr = _errDes[ERR_NO_ERROR];
	if (reply == NULL)
	{
		_errStr = _errDes[ERR_NULL];
		throw NullReplyException();
	}

	// have error
	if (reply->type == REDIS_REPLY_ERROR)
	{
		_errStr = reply->str;
		return true;
	}
	else
	{
		return false;
	}
}

bool CRedisInstance::_getError(const redisContext* redCtx)
{
	_errStr = _errDes[ERR_NO_ERROR];
	if (redCtx == NULL)
	{
		_errStr = _errDes[ERR_NULL];
		return true;
	}
	if (redCtx->err != 0)
	{
		_errStr = redCtx->errstr;
		return true;
	}
	else
	{
		return false;
	}
}

bool CRedisInstance::_getString(const redisReply* pReply, std::string& value)
{
	bool ret = false;
	if (_getError(pReply))
	{
		ret = false;
	}
	else
	{
		// 失败
		if (NULL == pReply->str)
		{
			_errStr = _errDes[ERR_NO_KEY];
			value = "";
			ret = false;
		}
		else
		{
			value = pReply->str;
			ret = true;
		}
	}

	if (NULL != pReply)
	{
		freeReplyObject((void*)pReply);
	}

	return ret;
}

bool CRedisInstance::_getInt(const redisReply* pReply, int64_t& retval)
{
	bool ret = false;
	if (_getError(pReply))
	{
		ret = false;
	}
	else
	{
		retval = pReply->integer;
		ret = true;
	}

	if (NULL != pReply)
	{
		freeReplyObject((void*)pReply);
	}

	return ret;
}

bool CRedisInstance::_getBool(const redisReply* pReply)
{
	bool bRet = false;
	if (_getError(pReply))
	{
		bRet = false;
	}
	else
	{
		bRet = true;
	}

	if (NULL != pReply)
	{
		freeReplyObject((void*)pReply);
	}

	return bRet;
}

bool CRedisInstance::auth(const std::string& password)  throw (NullReplyException)
{
	if (!_connected)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	_password = password;
	redisReply *reply = static_cast<redisReply *>(redisCommand(_redCtx, "AUTH %s", _password.c_str()));

	return _getBool(reply);
}

bool CRedisInstance::connect(void)
{
	if (_connected)
	{
		disConnect();
	}

	struct timeval timeoutVal;
	timeoutVal.tv_sec = _timeout;
	timeoutVal.tv_usec = 0;

	_redCtx = redisConnectWithTimeout(_host.c_str(), _port, timeoutVal);
	if (_getError(_redCtx))
	{
		if (NULL != _redCtx)
		{
			redisFree(_redCtx);
			_redCtx = NULL;
		}
		_connected = false;
		return false;
	}

	_connected = true;
	return true;
}

void CRedisInstance::disConnect()
{
	if (_connected && NULL != _redCtx)
	{
		redisFree(_redCtx);
		_redCtx = NULL;
	}
	_connected = false;
}

bool CRedisInstance::connect(const std::string &host, const uint16_t port, const std::string& password, const uint32_t timeout)
{
	// Init attribute.
	init(host, port, password, timeout);

	return (connect());
}

bool CRedisInstance::ping()
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	redisReply *reply = static_cast<redisReply *>(redisCommand(_redCtx, "PING"));

	return _getBool(reply);
}

bool CRedisInstance::enableKeepAlive()
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	return (REDIS_OK == redisEnableKeepAlive(_redCtx));
}

bool CRedisInstance::reconnect()
{
	return (connect());
}

const std::string CRedisInstance::getErrorStr() const
{
	return _errStr;
}

redisReply* CRedisInstance::redisCmd(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	redisReply* reply = static_cast<redisReply *>(redisvCommand(_redCtx, format, ap));
	va_end(ap);
	return reply;
}

bool CRedisInstance::set(const std::string& key, const std::string& value) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;

	redisReply *reply = redisCmd("SET %s %s", key.c_str(), value.c_str());

	return _getBool(reply);
}

bool CRedisInstance::get(const std::string& key, std::string& value) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	redisReply *reply = redisCmd("GET %s", key.c_str());

	return _getString(reply, value);
}

bool CRedisInstance::exists(const std::string& key) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("exists %s", key.c_str());

	int64_t retval = 0;

	if (_getInt(reply, retval)&&(retval==0))
	{
		ret = true;
	}

	return ret;
}

bool CRedisInstance::del(const std::string& key, int64_t& retval) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	redisReply *reply = redisCmd("del %s", key.c_str());

	return _getInt(reply, retval);
}

CRedisInstance::~CRedisInstance()
{
	disConnect();
}

////////////////////////////////// list 类的方法 ////////////////////////////////////////

bool CRedisInstance::lpush(const std::string& key, const std::string& value, int64_t& retval) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	retval = 0;

	redisReply *reply = redisCmd("LPUSH %s %s", key.c_str(), value.c_str());

	return _getInt(reply, retval);

}

bool CRedisInstance::lpop(const std::string& key, std::string& value) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	redisReply *reply = redisCmd("LPOP %s", key.c_str());
	return _getString(reply, value);
}

bool CRedisInstance::lrem(const std::string& key, std::string& value, int64_t& retval, int count) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	redisReply *reply = redisCmd("LREM %s %d %s", key.c_str(), count, value.c_str());
	return _getInt(reply, retval);
}

bool CRedisInstance::_getArryToList(redisReply* reply, ValueList& valueList)
{
	if (NULL == reply)
	{
		return false;
	}

	std::size_t num = reply->elements;

	for (std::size_t i = 0; i < num; i++)
	{
		valueList.push_back(reply->element[i]->str);
	}

	//	ValueList::iterator it = valueList.begin();
	//
	//	for( ; it != valueList.end(); it++ )
	//	{
	//		std::cout << "valueList: "<< *it << std::endl;
	//	}
	return true;
}

bool CRedisInstance::_getArryToMap(redisReply* reply, ValueMap& valueMap)
{
	if (NULL == reply)
	{
		return false;
	}

	std::size_t num = reply->elements;

	for (std::size_t i = 0; i < num; i += 2)
	{
		valueMap.insert(std::pair<std::string, std::string>(reply->element[i]->str,	reply->element[i + 1]->str));
	}

	return true;
}

bool CRedisInstance::lrange(const std::string &key, uint32_t start, int32_t end, ValueList& valueList) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("LRANGE %s %d %d", key.c_str(), start, end);

	if (_getError(reply))
	{
		ret = false;
	}
	else
	{
		if (REDIS_REPLY_ARRAY == reply->type && 0 == reply->elements) //<  key是list类型但 start > end
		{
			_errStr = std::string(_errDes[ERR_INDEX]) + " or " + _errDes[ERR_NO_KEY];
			ret = false;
		}
		else
		{
			_getArryToList(reply, valueList);
			ret = true;
		}
	}

	if (NULL != reply)
	{
		freeReplyObject(reply);
	}

	return ret;
}

bool CRedisInstance::rpush(const std::string& key, const std::string& value, int64_t& retval) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	retval = 0;

	redisReply *reply = redisCmd("RPUSH %s %s", key.c_str(), value.c_str());

	return _getInt(reply, retval);
}

bool CRedisInstance::rpop(const std::string& key, std::string& value) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	redisReply *reply = redisCmd("RPOP %s", key.c_str());

	return _getString(reply, value);
}

bool CRedisInstance::lindex(const std::string& key, int32_t index, std::string& value) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("LINDEX %s %d", key.c_str(), index);

	if (_getError(reply))
	{
		ret = false;
	}
	else
	{
		// 失败
		if (REDIS_REPLY_NIL == reply->type)
		{
			_errStr = std::string(_errDes[ERR_NO_KEY]) + " or " + _errDes[ERR_INDEX];
			value = "";
			ret = false;
		}
		else
		{
			value = reply->str;
			ret = true;
		}
	}

	if (NULL != reply)
	{
		freeReplyObject(reply);
	}

	return ret;
}

bool CRedisInstance::linsert(const std::string& key, INSERT_POS position, const std::string& pivot,	const std::string& value, int64_t& retval) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	std::string pos;

	if (BEFORE == position)
	{
		pos = "BEFORE";
	}
	else if (AFTER == position)
	{
		pos = "AFTER";
	}

	bool ret = false;
	redisReply *reply = redisCmd("LINSERT %s %s %s %s", key.c_str(), pos.c_str(), pivot.c_str(), value.c_str());

	if (_getError(reply))	//< 不是list 类型
	{
		ret = false;
	}
	else
	{
		if (REDIS_REPLY_INTEGER == reply->type)
		{
			if (reply->integer == -1)
			{
				_errStr = _errDes[ERR_NO_PIVOT];
				ret = false;
			}
			else if (reply->integer == 0)
			{
				_errStr = _errDes[ERR_NO_KEY];
				ret = false;
			}
			else
			{
				retval = reply->integer;
				ret = true;
			}
		}
		else
		{
			ret = false;
		}
	}

	if (NULL != reply)
	{
		freeReplyObject(reply);
	}

	return ret;
}

bool CRedisInstance::llen(const std::string& key, uint64_t& retval) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("LLEN %s", key.c_str());

	if (_getError(reply))
	{
		ret = false;
	}
	else
	{
		if (REDIS_REPLY_INTEGER == reply->type && (0 == reply->integer))
		{
			_errStr = _errDes[ERR_NO_KEY];
			ret = false;
		}
		else
		{
			retval = reply->integer;
			ret = true;
		}
	}

	if (NULL != reply)
	{
		freeReplyObject(reply);
	}

	return ret;
}

//////////////////hash方法/////////////////////////////
bool CRedisInstance::hget(const std::string& key, const std::string& filed, std::string& value) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("HGET %s %s", key.c_str(), filed.c_str());

	if (_getError(reply))
	{
		ret = false;
	}
	else
	{
		if (REDIS_REPLY_NIL == reply->type)
		{
			_errStr = std::string(_errDes[ERR_NO_KEY]) + " or " +
				_errDes[ERR_NO_FIELD];
			ret = false;
		}
		else
		{
			value = reply->str;
			ret = true;
		}
	}
	if (NULL != reply)
	{
		freeReplyObject(reply);
	}
	else
	{

	}

	return ret;
}

bool CRedisInstance::hset(const std::string& key, const std::string& filed,	const std::string& value, int64_t& retval) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	redisReply *reply = redisCmd("HSET %s %s %s", key.c_str(), filed.c_str(), value.c_str());

	return _getInt(reply, retval);

}

bool CRedisInstance::hmset(const std::string& key, const ValueMap& valueMap, int64_t& retval) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	vector<string> ary;
	ary.push_back("hmset");
	ary.push_back(key);
	map<string, string>::const_iterator it = valueMap.begin();
	while (it != valueMap.end())
	{
		if ((it->first).empty() )
		{
			ASSERT(FALSE);
		}
		//else if ((it->second).empty())
		//{
		//	ary.push_back(it->first);
		//	ary.push_back("0");
		//}
		else
		{
			ary.push_back(it->first);
			ary.push_back(it->second);
		}

		it++;
	}

	vector<const char *> argv(ary.size());
	vector<size_t> argvlen(ary.size());

	int32_t j = 0;
	for (vector<string>::const_iterator it = ary.begin(); it != ary.end(); ++it, ++j)
	{
		argv[j] = it->c_str(), argvlen[j] = it->size();
	}

	redisReply *reply = (redisReply*)redisCommandArgv(_redCtx, argv.size(), &(argv[0]), &(argvlen[0]));

	return _getInt(reply, retval);
}

bool CRedisInstance::hexists(const std::string& key, const std::string& filed) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("hexists %s", key.c_str());

	int64_t retval = 0;
	if (_getInt(reply, retval) && (retval == 0))
	{
		ret = true;
	}

	return ret;
}

bool CRedisInstance::hdel(const std::string& key, const std::string& filed, uint32_t& retval) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("HDEL %s %s", key.c_str(), filed.c_str(), retval);

	if (_getError(reply))
	{
		ret = false;
	}
	else
	{
		if (REDIS_REPLY_INTEGER == reply->type && 0 == reply->integer)
		{
			_errStr = std::string(_errDes[ERR_NO_KEY]) + " or " + _errDes[ERR_NO_FIELD];
		}
		//std::cout<<"type = "<<reply->type<<"   integer = "<< reply->integer<<std::endl;
		//std::cout<<"str = " << reply->str<<std::endl;
		else
		{
			retval = reply->integer;
			ret = true;
		}
	}

	if (NULL != reply)
	{
		freeReplyObject(reply);
	}

	return ret;
}

bool CRedisInstance::hgetall(const std::string& key, ValueMap& valueMap) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("HGETALL %s", key.c_str());

	if (_getError(reply))
	{
		ret = false;
	}
	else
	{

		if (REDIS_REPLY_ARRAY == reply->type && 0 == reply->elements)
		{
			_errStr = _errDes[ERR_NO_KEY];

		}
		else
		{
			_getArryToMap(reply, valueMap);
			ret = true;
		}
	}

	if (NULL != reply)
	{
		freeReplyObject(reply);
	}

	return ret;
}

bool CRedisInstance::hincrby(const std::string& key, const std::string& filed, int nIncValue, int64_t& retval) throw (NullReplyException)
{
	if (!_connected || !_redCtx)
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	redisReply *reply = redisCmd("HINCRBY %s %s %d", key.c_str(), filed.c_str(), nIncValue);

	return _getInt(reply, retval);
}