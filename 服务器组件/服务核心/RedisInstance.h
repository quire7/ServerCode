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

///< ����ǲ���ʱ���ǲ�����ָ��Ԫ��֮ǰ������ָ��Ԫ��֮��
typedef enum INSERT_POS
{
	BEFORE,			///< ���뵽ָ��Ԫ��֮ǰ��
	AFTER
} E_INSERT_POS;

/**
*@brief ������� hiredis ���ڱ����� redis-server �����ӡ�
* ���Ӵ���:
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
	*@brief ��ʼ��������Ϣ
	*
	*���ڳ�ʼ�����ӵ���ϸ��Ϣ���ڡ�connect() ֮ǰʹ�á�Ҳ����ֱ�ӵ���
	*bool connect( const std::string &host ,const uint16_t port, const std::string& password , const uint32_t timeout );����ʼ����
	*/
	void init(const std::string &host = "127.0.0.1", const uint16_t port = 6379, const std::string& pass = "", const uint32_t timeout = 0);

	/**
	*@brief ���ӡ�redis ���ݿ�
	*
	* init() ֮����ô˷������������ݿ�
	*
	*@return �ɹ����ء�true,ʧ�ܷ��ء�false
	*/
	bool connect();

	/**
	*@brief �Ͽ��롡redis �ڴ����ݿ�����
	*/
	void disConnect();

	/**
	*@brief ���ӡ�redis ���ݿ�֮����Ҫ��֤��
	*
	*@param [in] ��¼�����롣
	*@return �ɹ����ء�true, ʧ�ܷ��ء�false.
	*/
	bool auth(const std::string& password)  throw (NullReplyException);

	/**
	*@brief ��ʼ��������Ϣ�������ӡ�
	*
	* ��ʼ��������Ҫ����Ϣ�������Ҫ��password �ǿ���ô���Զ���auth ��֤��
	*
	*@param [in]  host .  redis-server �� ip��
	*@param [in] port .redis-server �� port ��
	*@param [in] password . redis-server ��֤���룬�˲������Բ������ɲ��ᷢ����֤��
	*@param [in] timeout . ���ӵĳ�ʱʱ�䡣
	*@return true �ɹ�����false ʧ�ܡ�
	*/
	bool connect(const std::string &host, const uint16_t port, const std::string& password = "", const uint32_t timeout = 0);

	/**
	* @brief ��������������
	*
	* @return ���������������ء�true, ��ͨ���ء�false.
	*/
	bool ping();

	/**
	* @brief ��������������糤����;
	*
	* @return ���������������ء�true, ��ͨ���ء�false.
	*/
	bool enableKeepAlive();

	/**
	*@brief �������ӡ�redis ��������
	*
	*@return �����ɹ���true, ����ʧ�ܡ�false.
	*/
	bool reconnect();

	/**
	* @brief ����Ƿ����ӹ���redis ��������
	*
	* @return �Ѿ����ӡ�true,û�����ӹ���false.
	*/
	inline bool isConneced()
	{
		return _connected;
	}

	/**
	* @brief ��ȡ�����ԭ��
	*
	* ��return ���ط��������ԭ��.
	*/
	const std::string getErrorStr() const;

	/**
	* @brief ֱ�ӷ���ָ���������redis ��������
	*
	* ��param [in] format ��ʽ���Ӹ��ơ�
	* ��param [in] ... ָ������ַ�����
	* @return NULL ,redis ִ��ʧ�ܣ����߶Ͽ����ӡ��ɹ����ء�redisReply ָ�롣
	*/
	redisReply* redisCmd(const char *format, ...);

	/**
	* @brief ����һ��Ԫ��
	* @return false����ʧ�ܣ�true���óɹ�
	*/
	bool set(const std::string& key, const std::string& value) throw (NullReplyException);
	/**
	* @brief ��ȡһ��Ԫ��
	* @return false��ȡʧ�ܣ�true��ȡ�ɹ�
	*/
	bool get(const std::string& key, std::string& value) throw (NullReplyException);
	/**
	* @brief Ԫ���Ƿ����
	* @return false��ȡʧ�ܣ�true��ȡ�ɹ�
	*/
	bool exists(const std::string& key) throw (NullReplyException);
	/**
	* @brief ɾ��Ԫ��
	* @param [out] retval ɾ��������
	* @return false��ȡʧ�ܣ�true��ȡ�ɹ� 
	*/
	bool del(const std::string& key, int64_t& retval) throw (NullReplyException);


	///////////////////////////////// list �ķ��� /////////////////////////////////////
	/**
	* @brief ��list��߲���һ��Ԫ��
	* @param [in] retval ����ɹ���list����
	* @return false����ʧ�ܣ�true����ɹ�
	*/
	bool lpush(const std::string& key, const std::string& value, int64_t& retval) throw (NullReplyException);
	/**
	* @brief valueΪ�����list��ߵ���һ��Ԫ��valueΪ����ֵ
	* @param [in] value ������Ԫ��ֵ
	* @return false����ʧ�ܣ�true�����ɹ�
	*/
	bool lpop(const std::string& key, std::string& value) throw (NullReplyException);

	/**
	* @brief ��ָ��Key�����������У�ɾ��ǰcount��ֵ����value��Ԫ�ء�
	* @param [in] value ������Ԫ��ֵ
	* @param [in] retval ���б���ɾ��ֵΪvalue��Ԫ�ظ���
	* @param [in] count ���count����0����ͷ��β������ɾ�������countС��0�����β��ͷ������ɾ����
						���count����0����ɾ�����������е���value��Ԫ�ء�
						���ָ����Key�����ڣ���ֱ�ӷ���0��
	* @return false����ʧ�ܣ�true�����ɹ�
	*/
	bool lrem(const std::string& key, std::string& value, int64_t& retval, int count=0)throw (NullReplyException);

	/**
	* @brief ��ȡlistָ�������ڵ�Ԫ��
	* @param [in] start ���俪ʼ�±꣬stop ��������±�, valueList����list
	*/
	bool lrange(const std::string &key, uint32_t start, int32_t stop, ValueList& valueList)	throw (NullReplyException);

	/**
	* @brief ��list�ұ߲���һ��Ԫ��
	* @param [in] retval ����ɹ���list����
	* @return false����ʧ�ܣ�true����ɹ�
	*/
	bool rpush(const std::string& key, const std::string& value, int64_t& retval) throw (NullReplyException);

	/**
	* @brief ��list�ұߵ���һ��Ԫ��
	* @param [in] value ������Ԫ��ֵ
	* @return false����ʧ�ܣ�true�����ɹ�
	*/
	bool rpop(const std::string& key, std::string& value) throw (NullReplyException);

	/**
	* @brief ��ֵΪkey��list�в���һ��Ԫ��
	* @param [in] position:BEFORE��߲��룬AFTER�ұ߲��룻value:����ֵ��retval�������list����
	*/
	bool linsert(const std::string& key, INSERT_POS position, const std::string& pivot,	const std::string& value, int64_t& retval) throw (NullReplyException);

	/**
	* @brief ��ȡԪ���±�Ϊindex��ֵ
	*
	*/
	bool lindex(const std::string& key, int32_t index, std::string& value) throw (NullReplyException);

	/**
	* @brief ��ȡlist����
	* @param [in] retval ��ȡ���ĳ���
	*/
	bool llen(const std::string& key, uint64_t& retval) throw (NullReplyException);

	//////////////////////////////   hash �ķ��� //////////////////////////////////////

	/**
	* @brief�ӹ�ϣ����ȡ����key��field����Ӧ��valueֵ
	* @param [in] key �Ǽ������൱�ڱ���
	* @param [in] filed ���ֶ���
	* @param [out] value �ǻ�ȡ��ֵ
	* @return true �ɹ���ȡ��false��ȡʧ��
	* @warning ��ȡʧ�� valueΪ""(string��ʼ��Ĭ��ֵ)
	*/
	bool hget(const std::string& key, const std::string& filed, std::string& value)	throw (NullReplyException);

	/**
	* @brief���ù�ϣ������key��field����Ӧ��valueֵ
	* @param [in] key �Ǽ������൱�ڱ���
	* @param [in] filed ���ֶ���
	* @param [in] value ��������������Ӧ��ֵ
	* @param [out] retval 0:field�Ѵ����Ҹ�����value �� 1�� field�����ڣ��½�field�ҳɹ�������value
	* @return true �ɹ���ȡ��false��ȡʧ��
	* @warning ����ʧ�� retvalΪ0���ɹ�Ϊ1
	*/
	bool hset(const std::string& key, const std::string& filed, const std::string& value, int64_t& retval) throw (NullReplyException);

	/*
	* @brief����������ò����и�����Field/Value�ԡ��������ĳ��Field�Ѿ����ڣ�������ֵ����ԭ��ֵ�����Key�����ڣ��򴴽���Key��ͬʱ�趨�����е�Field/Value�� 
	* @param [in] key �Ǽ������൱�ڱ���
	* @param [in] valueMap ���õ�ֵ��map<string,string>���ͣ�
	* @return true �ɹ���ȡ��false��ȡʧ��
	*/
	bool hmset(const std::string& key, const ValueMap& valueMap, int64_t& retval) throw (NullReplyException);

	/**
	* @brief��ϣ����key����Ӧ��field��һ�������Ƿ����
	* @param [in] key �Ǽ������൱�ڱ���
	* @param [in] filed ���ֶ���
	*/
	bool hexists(const std::string& key, const std::string& filed) throw (NullReplyException);

	/**
	* @briefɾ����ϣ����key����Ӧ��field��һ������
	* @param [in] key �Ǽ������൱�ڱ���
	* @param [in] filed ���ֶ���
	* @param [out] retval��ɾ����field����
	* @return true �ɹ���ȡ��false��ȡʧ��
	*@warning ɾ��ʧ�� retvalΪ0���ɹ�Ϊ1
	*/
	bool hdel(const std::string& key, const std::string& filed, uint32_t& retval) throw (NullReplyException);

	/**
	* @briefȡ�ù�ϣ����key����Ӧ����������
	* @param [in] key �Ǽ������൱�ڱ���
	* @param [out] valueMap ��ȡ��ֵ��map<string,string>���ͣ�
	* @return true �ɹ���ȡ��false��ȡʧ��
	* @warning ��ȡʧ�� valueMapΪ��
	*/
	bool hgetall(const std::string& key, ValueMap& valueMap) throw (NullReplyException);

	/*
	* @�������Ӵ洢���ֶ��д洢����������ϣ������������������ڣ��µ�key����ϣ����������ֶβ����ڣ�ֵ������Ϊ0֮ǰ���в�����
	* @param [in] key �Ǽ������൱�ڱ���
	* @param [in] field �ֶ�
	* @
	*/
	bool hincrby(const std::string& key, const std::string& filed, int nIncValue, int64_t& retval) throw (NullReplyException);

protected:
	/**
	*@brief  �� reply->type ΪREDIS_REPLY_ARRY ���͵�Ԫ�ػ�ȡ������䵽��valueList �б�.
	*
	*@param [in] reply  . the data return from redis-server.
	*@param [out] valueList .�ӡ�reply ��ȡ�Ķ�������.
	*@return �ɹ����� true, ʧ�ܷ��ء�false.
	*/
	bool _getArryToList(redisReply* reply, ValueList& valueList);

	/**
	*@brief  �� reply->type ΪREDIS_REPLY_ARRY ���͵�Ԫ�ػ�ȡ������䵽valueMap �б�.
	*
	*hgetall ������������Ϊ�ֶ���,ż��Ϊ�ֶ����ݡ��ԡ��ֶ���=�ֶ����ݵķ�ʽ���ڡ�valueMap �
	*
	*@param [in] reply  . the data return from redis-server.
	*@param [out] valueMap .�� reply ��ȡ��much of pair ���ݴ洢����valueMap.
	*@return �ɹ����� true, ʧ�ܷ��ء�false.
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