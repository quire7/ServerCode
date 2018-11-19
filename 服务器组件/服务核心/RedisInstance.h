#pragma once

#include "ServiceCoreHead.h"

#include "..\..\Redis-3.0\deps\hiredis\hiredis.h"
#include <list>
#include <map>
#include <stdint.h>

#pragma warning (disable:4290)

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN( typeName )	\
	typeName(const typeName &);		\
	typeName & operator=(const typeName &)
#endif

typedef std::list<std::string> ValueList;
typedef std::map<std::string, std::string> ValueMap;

class SERVICE_CORE_CLASS NullReplyException : public std::runtime_error
{
public:
	NullReplyException() : std::runtime_error("reply NULL")	{}
};

///< 这个是插入时候是插入在指定元素之前，或者指定元素之后
typedef enum INSERT_POS
{
	BEFORE,			///< 插入到指定元素之前。
	AFTER
} E_INSERT_POS;

/**
*@brief 此类基于 hiredis 用于保持与 redis-server 的链接。
* 例子代码:
CRedisInstance con;
if ( !con.connect( "127.0.0.1", 6379, "521110", 5 ) )
{
std::cout << "connect error " << con.getErrorStr( ) << std::endl;
return 0;
}

std::string value;
if ( !con.hget( "newHash", "testing", value ) )
{
std::cout << "hget error " << con.getErrorStr( ) << std::endl;
}
*/
class SERVICE_CORE_CLASS CRedisInstance
{
public:
	CRedisInstance(void);

	virtual ~CRedisInstance(void);

	/**
	*@brief 初始化链接信息
	*
	*用于初始化链接的详细信息。在　connect() 之前使用。也可以直接调用
	*bool connect( const std::string &host ,const uint16_t port, const std::string& password , const uint32_t timeout );来初始化。
	*/
	void init(const std::string &host = "127.0.0.1", const uint16_t port = 6379, const std::string& pass = "", const uint32_t timeout = 0);

	/**
	*@brief 链接　redis 数据库
	*
	* init() 之后调用此方法来链接数据库
	*
	*@return 成功返回　true,失败返回　false
	*/
	bool connect();

	/**
	*@brief 断开与　redis 内存数据库链接
	*/
	void disConnect();

	/**
	*@brief 链接　redis 数据库之后，需要认证。
	*
	*@param [in] 登录的密码。
	*@return 成功返回　true, 失败返回　false.
	*/
	bool auth(const std::string& password)  throw (NullReplyException);

	/**
	*@brief 初始化链接信息并且链接。
	*
	* 初始化链接需要的信息。如果需要　password 非空那么就自动　auth 认证。
	*
	*@param [in]  host .  redis-server 的 ip。
	*@param [in] port .redis-server 的 port 。
	*@param [in] password . redis-server 认证密码，此参数可以不填，不填旧不会发起认证。
	*@param [in] timeout . 链接的超时时间。
	*@return true 成功，　false 失败。
	*/
	bool connect(const std::string &host, const uint16_t port, const std::string& password = "", const uint32_t timeout = 0);

	/**
	* @brief 检查与服务器网络
	*
	* @return 网络链接正常返回　true, 不通返回　false.
	*/
	bool ping();

	/**
	* @brief 保持与服务器网络长连接;
	*
	* @return 网络链接正常返回　true, 不通返回　false.
	*/
	bool enableKeepAlive();

	/**
	*@brief 重新链接　redis 服务器。
	*
	*@return 重连成功　true, 重连失败　false.
	*/
	bool reconnect();

	/**
	* @brief 检查是否链接过　redis 服务器。
	*
	* @return 已经链接　true,没有链接过　false.
	*/
	inline bool isConneced()
	{
		return _connected;
	}

	/**
	* @brief 获取错误的原因
	*
	* ＠return 返回发生错误的原因.
	*/
	const std::string getErrorStr() const;

	/**
	* @brief 直接发送指令参数给　redis 服务器。
	*
	* ＠param [in] format 格式化子复制。
	* ＠param [in] ... 指令及数据字符串。
	* @return NULL ,redis 执行失败，或者断开连接。成功返回　redisReply 指针。
	*/
	redisReply* redisCmd(const char *format, ...);

	/**
	* @brief 设置一个元素
	* @return false设置失败，true设置成功
	*/
	bool set(const std::string& key, const std::string& value) throw (NullReplyException);
	/**
	* @brief 获取一个元素
	* @return false获取失败，true获取成功
	*/
	bool get(const std::string& key, std::string& value) throw (NullReplyException);
	/**
	* @brief 元素是否存在
	* @return false获取失败，true获取成功
	*/
	bool exists(const std::string& key) throw (NullReplyException);
	/**
	* @brief 删除元素
	* @param [out] retval 删除的数量
	* @return false获取失败，true获取成功 
	*/
	bool del(const std::string& key, int64_t& retval) throw (NullReplyException);


	///////////////////////////////// list 的方法 /////////////////////////////////////
	/**
	* @brief 从list左边插入一个元素
	* @param [in] retval 插入成功后list长度
	* @return false插入失败，true插入成功
	*/
	bool lpush(const std::string& key, const std::string& value, int64_t& retval) throw (NullReplyException);
	/**
	* @brief value为空则从list左边弹出一个元素value为弹出值
	* @param [in] value 弹出的元素值
	* @return false弹出失败，true弹出成功
	*/
	bool lpop(const std::string& key, std::string& value) throw (NullReplyException);

	/**
	* @brief 在指定Key关联的链表中，删除前count个值等于value的元素。
	* @param [in] value 弹出的元素值
	* @param [in] retval 从列表中删除值为value的元素个数
	* @param [in] count 如果count大于0，从头向尾遍历并删除，如果count小于0，则从尾向头遍历并删除。
						如果count等于0，则删除链表中所有等于value的元素。
						如果指定的Key不存在，则直接返回0。
	* @return false弹出失败，true弹出成功
	*/
	bool lrem(const std::string& key, std::string& value, int64_t& retval, int count=0)throw (NullReplyException);

	/**
	* @brief 获取list指定区间内的元素
	* @param [in] start 区间开始下标，stop 区间结束下标, valueList区间list
	*/
	bool lrange(const std::string &key, uint32_t start, int32_t stop, ValueList& valueList)	throw (NullReplyException);

	/**
	* @brief 从list右边插入一个元素
	* @param [in] retval 插入成功后list长度
	* @return false插入失败，true插入成功
	*/
	bool rpush(const std::string& key, const std::string& value, int64_t& retval) throw (NullReplyException);

	/**
	* @brief 从list右边弹出一个元素
	* @param [in] value 弹出的元素值
	* @return false弹出失败，true弹出成功
	*/
	bool rpop(const std::string& key, std::string& value) throw (NullReplyException);

	/**
	* @brief 健值为key的list中插入一个元素
	* @param [in] position:BEFORE左边插入，AFTER右边插入；value:插入值；retval：插入后list长度
	*/
	bool linsert(const std::string& key, INSERT_POS position, const std::string& pivot,	const std::string& value, int64_t& retval) throw (NullReplyException);

	/**
	* @brief 获取元素下标为index的值
	*
	*/
	bool lindex(const std::string& key, int32_t index, std::string& value) throw (NullReplyException);

	/**
	* @brief 获取list长度
	* @param [in] retval 获取到的长度
	*/
	bool llen(const std::string& key, uint64_t& retval) throw (NullReplyException);

	//////////////////////////////   hash 的方法 //////////////////////////////////////

	/**
	* @brief从哈希表中取出以key和field所对应的value值
	* @param [in] key 是键名，相当于表名
	* @param [in] filed 是字段名
	* @param [out] value 是获取的值
	* @return true 成功获取，false获取失败
	* @warning 获取失败 value为""(string初始化默认值)
	*/
	bool hget(const std::string& key, const std::string& filed, std::string& value)	throw (NullReplyException);

	/**
	* @brief设置哈希表中以key和field所对应的value值
	* @param [in] key 是键名，相当于表名
	* @param [in] filed 是字段名
	* @param [in] value 是以上两参数对应的值
	* @param [out] retval 0:field已存在且覆盖了value ； 1： field不存在，新建field且成功设置了value
	* @return true 成功获取，false获取失败
	* @warning 设置失败 retval为0，成功为1
	*/
	bool hset(const std::string& key, const std::string& filed, const std::string& value, int64_t& retval) throw (NullReplyException);

	/*
	* @brief逐对依次设置参数中给出的Field/Value对。如果其中某个Field已经存在，则用新值覆盖原有值。如果Key不存在，则创建新Key，同时设定参数中的Field/Value。 
	* @param [in] key 是键名，相当于表名
	* @param [in] valueMap 设置的值（map<string,string>类型）
	* @return true 成功获取，false获取失败
	*/
	bool hmset(const std::string& key, const ValueMap& valueMap, int64_t& retval) throw (NullReplyException);

	/**
	* @brief哈希表中key所对应的field这一项内容是否存在
	* @param [in] key 是键名，相当于表名
	* @param [in] filed 是字段名
	*/
	bool hexists(const std::string& key, const std::string& filed) throw (NullReplyException);

	/**
	* @brief删除哈希表中key所对应的field这一项内容
	* @param [in] key 是键名，相当于表名
	* @param [in] filed 是字段名
	* @param [out] retval：删除的field个数
	* @return true 成功获取，false获取失败
	*@warning 删除失败 retval为0，成功为1
	*/
	bool hdel(const std::string& key, const std::string& filed, uint32_t& retval) throw (NullReplyException);

	/**
	* @brief取得哈希表中key所对应的所有内容
	* @param [in] key 是键名，相当于表名
	* @param [out] valueMap 获取的值（map<string,string>类型）
	* @return true 成功获取，false获取失败
	* @warning 获取失败 valueMap为空
	*/
	bool hgetall(const std::string& key, ValueMap& valueMap) throw (NullReplyException);

	/*
	* @用于增加存储在字段中存储由增量键哈希的数量。如果键不存在，新的key被哈希创建。如果字段不存在，值被设置为0之前进行操作。
	* @param [in] key 是键名，相当于表名
	* @param [in] field 字段
	* @
	*/
	bool hincrby(const std::string& key, const std::string& filed, int nIncValue, int64_t& retval) throw (NullReplyException);

protected:
	/**
	*@brief  从 reply->type 为REDIS_REPLY_ARRY 类型的元素获取数据填充到　valueList 列表.
	*
	*@param [in] reply  . the data return from redis-server.
	*@param [out] valueList .从　reply 提取的多条数据.
	*@return 成功返回 true, 失败返回　false.
	*/
	bool _getArryToList(redisReply* reply, ValueList& valueList);

	/**
	*@brief  从 reply->type 为REDIS_REPLY_ARRY 类型的元素获取数据填充到valueMap 列表.
	*
	*hgetall 返回数据奇数为字段名,偶数为字段数据。以　字段名=字段数据的方式存在　valueMap 里。
	*
	*@param [in] reply  . the data return from redis-server.
	*@param [out] valueMap .从 reply 提取的much of pair 数据存储到　valueMap.
	*@return 成功返回 true, 失败返回　false.
	*/
	bool _getArryToMap(redisReply* reply, ValueMap& valueMap);

	bool _getError(const redisContext* redCtx);

	bool _getError(const redisReply* reply)  throw (NullReplyException);

	bool _getString(const redisReply* pReply, std::string& value);

	bool _getInt(const redisReply* pReply, int64_t& illRsp);

	bool _getBool(const redisReply* pReply);

	inline redisContext* _getCtx() const
	{
		return _redCtx;
	}

private:
	DISALLOW_COPY_AND_ASSIGN(CRedisInstance);

	redisContext *_redCtx;		///< redis connector context

	std::string _host;         		///< redis host
	uint16_t _port;         		///< redis sever port
	std::string _password;         	///< redis server password
	uint32_t _timeout;      		///< connect timeout second
	bool _connected;			///< if connected

	///< error number
	enum E_ERROR_NO
	{
		ERR_NO_ERROR = 0,
		ERR_NULL,
		ERR_NO_CONNECT,
		ERR_NO_PIVOT,
		ERR_NO_KEY,
		ERR_NO_FIELD,
		ERR_INDEX,
		ERR_BOTTOM
	};
	std::string _errStr;		///< Describe the reason for error..

	static const char* _errDes[ERR_BOTTOM];	///< describe error
};