/*
 * RedisPacket.h
 *
 *  Created on: 2016年5月19日
 *      Author: 58
 */

#ifndef BASE_NET_PROTOCOL_REDISPACKET_H_
#define BASE_NET_PROTOCOL_REDISPACKET_H_

#include <wedis/base/net/StreamBuffer.h>
#include <wedis/base/util/CommonType.h>
#include <wedis/base/util/Global.h>
#include <wedis/base/util/Noncopyable.h>
#include <wedis/base/util/TSingleton.h>
#include <map>
#include <vector>
#include <string>

namespace wRedis {

enum EM_REDIS_CMD {
	//========== Basic Command ==================
	CMD_PING,
	CMD_AUTH,
	CMD_DB_SIZE,
	CMD_FLUSH_DB,
	CMD_FLUSH_ALL,
	CMD_SELECT,
	CMD_SAVE,
	CMD_BG_SAVE,
	CMD_BG_REWRITE_AOF,
	CMD_LAST_SAVE_TIME,
	CMD_INFO,
	CMD_INFO_CLIENTS,
	CMD_SLAVE_OF,
	CMD_SLAVE_OF_NONE,
	CMD_TIME,

	//========== Key Command =====================
	CMD_KEYS,
	CMD_DEL,
	CMD_EXISTS,
	CMD_MOVE,
	CMD_RENAME,
	CMD_RENAMENX,
	CMD_PERSIST,
	CMD_EXPIRE,
	CMD_PEXPIRE,
	CMD_EXPIREAT,
	CMD_PEXPIREAT,
	CMD_TTL,
	CMD_PTTL,
	CMD_RANDOMKEY,
	CMD_TYPE,
	CMD_DUMP,
	CMD_SCAN,
	CMD_SORT,

	//========== String Command ==================
	CMD_APPEND,
	CMD_GET,
	CMD_SET,
	CMD_GET_SET,
	CMD_STRLEN,
	CMD_SETEX,
	CMD_PSETEX,
	CMD_SETNX,
	CMD_SETRANGE,
	CMD_GETRANGE,
	CMD_MGET,
	CMD_MSET,
	CMD_MSETNX,
	CMD_DECR,
	CMD_INCR,
	CMD_DECRBY,
	CMD_INCRBY,
	CMD_INCRBYFLOAT,

	//========== list Command ==================
	CMD_LPUSH,
	CMD_RPUSH,
	CMD_LPUSHX,
	CMD_RPUSHX,
	CMD_LRANGE,
	CMD_LPOP,
	CMD_RPOP,
	CMD_LLEN,
	CMD_LREM,
	CMD_LSET,
	CMD_LINDEX,
	CMD_LTRIM,
	CMD_LINSERT,
	CMD_RPOPLPUSH,

	//========== hash map Command ==============
	CMD_HSET,
	CMD_HSETNX,
	CMD_HGET,
	CMD_HEXISTS,
	CMD_HLEN,
	CMD_HDEL,
	CMD_HINCRBY,
	CMD_HINCRBYFLOAT,
	CMD_HGETALL,
	CMD_HKEYS,
	CMD_HVALS,
	CMD_HMGET,
	CMD_HMSET,
	CMD_HSCAN,

	//========== set Command ===================
	CMD_SADD,
	CMD_SCARD,
	CMD_SISMEMBER,
	CMD_SMEMBERS,
	CMD_SPOP,
	CMD_SREM,
	CMD_SRANDMEMBER,
	CMD_SMOVE,
	CMD_SDIFF,
	CMD_SDIFFSTORE,
	CMD_SINTER,
	CMD_SINTERSTORE,
	CMD_SUNION,
	CMD_SUNIONSTORE,
	CMD_SSCAN,

	//========== zset Command ==================
	CMD_ZADD,
	CMD_ZCARD,
	CMD_ZCOUNT,
	CMD_ZINCRBY,
	CMD_ZRANGE,
	CMD_ZRANGEBYSCORE,
	CMD_ZRANK,
	CMD_ZREM,
	CMD_ZREVRANGE,
	CMD_ZREVRANK,
	CMD_ZSCORE,
	CMD_ZREVRANGEBYSCORE,
	CMD_ZREMRANGEBYRANK,
	CMD_ZREMRANGEBYSCORE,
	CMD_ZLEXCOUNT,
	CMD_ZRANGEBYLEX,
	CMD_ZREVRANGEBYLEX,
	CMD_ZSCAN,

	//========== bit set Command =================
	CMD_SETBIT,
	CMD_GETBIT,
	CMD_BITCOUNT,
	CMD_BITPOS,

	//========== transaction Command ==============
	CMD_MULTI,
	CMD_DISCARD,
	CMD_WATCH,
	CMD_UNWATCH,
	CMD_EXEC,

	//========== geo Command ======================
	CMD_GEOADD,
	CMD_GEODIST,
	CMD_GEOHASH,
	CMD_GEOPOS,
	CMD_GEORADIS,
	CMD_GEORADISBYNAME,

	//========== Pub/Sub Command ==================
	CMD_PUBLISH,
	CMD_SUBSCRIBE,
	CMD_PSUBSCRIBE,
	CMD_UNSUBSCRIBE,
	CMD_PUNSUBSCRIBE,

	//========== HyperLogLog Command ==============
	CMD_PFADD,
	CMD_PFCOUNT,
	CMD_PFMERGE,

	//========== Script Command ===================
	CMD_EVAL,
	CMD_EVALSHA,
	CMD_SCRIPT,
	CMD_SCRIPT_FLUSH,

	//========== sentinel Command =================
	CMD_SENTINEL,

	//========== cluster Command ==================
	CMD_ASKING,
	CMD_CLUSTER_NODES,
			
	TOTAL_CMD_COUNT,			
};


enum EM_REDIS_CMD_KEY_POS {
	EM_REDIS_NK,           // no key
	EM_REDIS_FK,           // cmd key ... 只有一个key 就在cmd后边
	EM_REDIS_MK,           // cmd key1 key2 ... keyn   key就在cmd后边
	EM_REDIS_DK,           // cmd key1 key2 val        只有两个key
	EM_REDIS_MFK,          // cmd key1 v1 key2 v2 key3 v3   多key 在奇数位置
	EM_REDIS_SK,           // cmd type key  key在第二个位置，且只有一个
	EM_REDIS_U_KEY,        // eval 之类的不好找规律的key  则使用appendkey 方法调用添加key
};

typedef struct SRedisCmdBase {
	EM_REDIS_CMD m_eCmd;
	const I8* m_Name;
	EM_REDIS_CMD_KEY_POS m_eKeyPos;
	BOOL m_bCluster;
	BOOL m_bMaster;
}SRedisCmdBase;

extern const SRedisCmdBase gRedisCmds[];
extern const I8* getRedisCmdName(EM_REDIS_CMD eCmd);

struct WString {
	WString(const I8* pBuf, UI32 uLen = INVALID_VALUE);
	WString(const WString& w);
	~WString() {
		if(m_bDestory)
			W_FREE(m_pBuf);
	}

	BOOL m_bDestory;
	UI32 m_uLen;
	I8* m_pBuf;
};

struct RedisBaseCmd {
public:
	RedisBaseCmd(EM_REDIS_CMD eCmd, BOOL bSkip)
		: m_eCmd(eCmd), m_bSkip(bSkip) {}

	~RedisBaseCmd() {}

	inline BOOL isSkip() const { return m_bSkip; }

	EM_REDIS_CMD m_eCmd;
	BOOL m_bSkip;
};

class RedisCmd : public Noncopyable {
public:
	RedisCmd(EM_REDIS_CMD eCmd, BOOL bSkip);
	virtual ~RedisCmd();

public:
	virtual BOOL encode(OutputStream* pStream) = 0;
	virtual BOOL encode(I8* pStream, OUT I32* pOutLen) = 0;
	virtual RedisCmd* clone() = 0;
public:
	inline RedisBaseCmd& cmd() { return m_Cmd; }
	inline EM_REDIS_CMD getCmd() const { return m_Cmd.m_eCmd; }
	inline std::vector<WString>& keys() { return m_Keys; }
	inline BOOL canCluster() { return gRedisCmds[(I32)m_Cmd.m_eCmd].m_bCluster; }
	inline const I8*  name() { return gRedisCmds[(I32)m_Cmd.m_eCmd].m_Name; }
	inline BOOL isMaster() { return gRedisCmds[(I32)m_Cmd.m_eCmd].m_bMaster; }
	inline BOOL isSkip() { return m_Cmd.m_bSkip; }
protected:
	std::vector<WString> m_Keys;
	RedisBaseCmd m_Cmd;
};

class RedisMCmd : public RedisCmd {
public:
	RedisMCmd(EM_REDIS_CMD eCmd, BOOL bSkip = FALSE);
	virtual ~RedisMCmd() {}

public:
	void append(const I8* pBuf);
	void append(const I8* pBuf, const UI32 nLen);
	void append(std::string& str);
	void append(WString& ws);
	void appendKey(const I8* pBuf);
	void appendKey(std::string& str);
public:
	virtual BOOL encode(OutputStream* pStream);
	virtual BOOL encode(I8* pStream, OUT I32* pOutLen);
	virtual RedisCmd* clone();
private:
	std::vector<WString> m_Vector;
};

class RedisNoKeyCmd : public RedisCmd {
public:
	RedisNoKeyCmd(EM_REDIS_CMD eCmd, const I8* pCmdName, const I8* pBytes);
	virtual ~RedisNoKeyCmd();
public:
	virtual BOOL encode(OutputStream* pStream);	
	virtual BOOL encode(I8* pStream, OUT I32* pOutLen);
	virtual RedisCmd* clone();

private:
	void cleanUp();		

private:
	I8* m_pCmdName;
	I8* m_pBytes;
	UI32 m_uLen;
};


class RedisNoKeyCmdMgr {
	DECLARE_SINGLETON(RedisNoKeyCmdMgr)
public:
	RedisNoKeyCmdMgr();
	~RedisNoKeyCmdMgr();

public:
	RedisCmd* get(EM_REDIS_CMD eCmd);

private:
	void init();
	void _register(EM_REDIS_CMD eCmd);

private:
	std::map<EM_REDIS_CMD, RedisNoKeyCmd*> m_Map;
	
};


} /* namespace wRedis */

#endif /* BASE_NET_PROTOCOL_REDISPACKET_H_ */
