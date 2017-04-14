/*
 * RedisPacket.cpp
 *
 *  Created on: 2016年5月19日
 *      Author: 58
 */

#include <wedis/base/protocol/RedisCmd.h>
#include <wedis/base/util/Log.h>
#include <wedis/base/util/StringSplit.h>
#include <wedis/base/util/Tools.h>
#include <wedis/base/util/Alloctor.h>
#include <string>

namespace wRedis {

const SRedisCmdBase gRedisCmds[] = {
	//========== Basic Command ==================
	{ CMD_PING, 			"PING", 			EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_AUTH, 			"AUTH", 			EM_REDIS_FK, 	FALSE, FALSE },
	{ CMD_DB_SIZE, 			"DBSIZE", 			EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_FLUSH_DB, 		"FLUSHDB", 			EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_FLUSH_ALL, 		"FLUSHALL", 		EM_REDIS_NK,	FALSE, TRUE  },
	{ CMD_SELECT, 			"SELECT", 			EM_REDIS_FK, 	FALSE, FALSE },
	{ CMD_SAVE, 			"SAVE", 			EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_BG_SAVE, 			"BGSAVE", 			EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_BG_REWRITE_AOF, 	"BGREWRITEAOF", 	EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_LAST_SAVE_TIME, 	"LASTSAVETIME", 	EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_INFO, 			"INFO", 			EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_INFO_CLIENTS, 	"INFO CLIENTS", 	EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_SLAVE_OF, 		"SLAVEOF", 			EM_REDIS_FK, 	FALSE, FALSE },
	{ CMD_SLAVE_OF_NONE, 	"SLAVEOFNONE", 		EM_REDIS_NK, 	FALSE, FALSE },
	{ CMD_TIME, 			"TIME", 			EM_REDIS_NK, 	FALSE, FALSE },

	//========== Key Command =====================
	{ CMD_KEYS, 			"KEYS",		 		EM_REDIS_NK, 	FALSE, FALSE },
	{ CMD_DEL, 				"DEL",		 		EM_REDIS_MK, 	TRUE,  TRUE  },
	{ CMD_EXISTS, 			"EXISTS",		 	EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_MOVE, 			"MOVE",			 	EM_REDIS_FK, 	FALSE, TRUE  },
	{ CMD_RENAME, 			"RENAME",		 	EM_REDIS_DK, 	TRUE,  TRUE  },
	{ CMD_RENAMENX, 		"RENAMENX",		 	EM_REDIS_DK, 	TRUE,  TRUE  },
	{ CMD_PERSIST, 			"PERSIST",		 	EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_EXPIRE, 			"EXPIRE",		 	EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_PEXPIRE, 			"PEXPIRE",		 	EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_EXPIREAT, 		"EXPIREAT",		 	EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_PEXPIREAT, 		"PEXPIREAT",		EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_TTL, 				"TTL",			 	EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_PTTL, 			"PTTL",			 	EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_RANDOMKEY, 		"RANDOMKEY",		EM_REDIS_NK, 	FALSE, FALSE },
	{ CMD_TYPE, 			"TYPE",				EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_DUMP, 			"DUMP",				EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_SCAN, 			"SCAN", 			EM_REDIS_NK, 	FALSE, FALSE },
	{ CMD_SORT, 			"SORT",				EM_REDIS_U_KEY, TRUE,  TRUE  },

	//========== String Command ==================
	{ CMD_APPEND,			"APPEND",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_GET, 				"GET", 				EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_SET, 				"SET", 				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_GET_SET, 			"GETSET", 			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_STRLEN, 			"STRLEN", 			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_SETEX, 			"SETEX", 			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_PSETEX, 			"PSETEX", 			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_SETNX, 			"SETNX", 			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_SETRANGE, 		"SETRANGE", 		EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_GETRANGE, 		"GETRANGE", 		EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_MGET, 			"MGET", 			EM_REDIS_MK, 	TRUE,  FALSE },
	{ CMD_MSET, 			"MSET", 			EM_REDIS_MFK,	TRUE,  TRUE  },
	{ CMD_MSETNX, 			"MSETNX", 			EM_REDIS_MFK,	FALSE, TRUE  },
	{ CMD_DECR, 			"DECR", 			EM_REDIS_FK,	TRUE,  TRUE  },
	{ CMD_INCR, 			"INCR", 			EM_REDIS_FK,	TRUE,  TRUE  },
	{ CMD_DECRBY, 			"DECRBY", 			EM_REDIS_FK,	TRUE,  TRUE  },
	{ CMD_INCRBY, 			"INCRBY", 			EM_REDIS_FK,	TRUE,  TRUE  },
	{ CMD_INCRBYFLOAT, 		"INCRBYFLOAT",	 	EM_REDIS_FK,	TRUE,  TRUE  },
			
	//========== list Command ==================
	{ CMD_LPUSH,			"LPUSH",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_RPUSH,			"RPUSH",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_LPUSHX,			"LPUSHX",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_RPUSHX,			"RPUSHX",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_LRANGE,			"LRANGE",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_LPOP,				"LPOP",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_RPOP,				"RPOP",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_LLEN,				"LLEN",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_LREM,				"LREM",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_LSET,				"LSET",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_LINDEX,			"LINDEX",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_LTRIM,			"LTRIM",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_LINSERT,			"LINSERT",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_RPOPLPUSH,		"RPOPLPUSH",		EM_REDIS_DK, 	TRUE,  TRUE  },

	//========== hash map Command ==============
	{ CMD_HSET,				"HSET",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_HSETNX,			"HSETNX",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_HGET,				"HGET",				EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_HEXISTS,			"HEXISTS",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_HLEN,				"HLEN",				EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_HDEL,				"HDEL",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_HINCRBY,			"HINCRBY",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_HINCRBYFLOAT,		"HINCRBYFLOAT",		EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_HGETALL,			"HGETALL",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_HKEYS,			"HKEYS",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_HVALS,			"HVALS",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_HMGET,			"HMGET",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_HMSET,			"HMSET",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_HSCAN,			"HSCAN",			EM_REDIS_FK, 	TRUE,  FALSE },

	//========== set Command ===================
	{ CMD_SADD,				"SADD",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_SCARD,			"SCARD",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_SISMEMBER,		"SISMEMBER",		EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_SMEMBERS,			"SMEMBERS",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_SPOP,				"SPOP",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_SREM,				"SREM",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_SRANDMEMBER,		"SRANDMEMBER",		EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_SMOVE,			"SMOVE",			EM_REDIS_DK, 	FALSE, TRUE  },
	{ CMD_SDIFF,			"SDIFF",			EM_REDIS_MK, 	FALSE, FALSE },
	{ CMD_SDIFFSTORE,		"SDIFFSTORE",		EM_REDIS_MK, 	FALSE, TRUE  },
	{ CMD_SINTER,			"SINTER",			EM_REDIS_MK, 	FALSE, FALSE },
	{ CMD_SINTERSTORE,		"SINTERSTORE",		EM_REDIS_MK, 	FALSE, TRUE  },
	{ CMD_SUNION,			"SUNION",			EM_REDIS_MK, 	FALSE, FALSE },
	{ CMD_SUNIONSTORE,		"SUNIONSTORE",		EM_REDIS_MK, 	FALSE, TRUE  },
	{ CMD_SSCAN,			"SSCAN",			EM_REDIS_FK, 	TRUE,  FALSE },

	//========== zset Command ==================
	{ CMD_ZADD,				"ZADD",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_ZCARD,			"ZCARD",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZCOUNT,			"ZCOUNT",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZINCRBY,			"ZINCRBY",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_ZRANGE,			"ZRANGE",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZRANGEBYSCORE,	"ZRANGEBYSCORE",	EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZRANK,			"ZRANK",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZREM,				"ZREM",				EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_ZREVRANGE,		"ZREVRANGE",		EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZREVRANK,			"ZREVRANK",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZSCORE,			"ZSCORE",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZREVRANGEBYSCORE,	"ZREVRANGEBYSCORE",	EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZREMRANGEBYRANK,	"ZREMRANGEBYRANK",	EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_ZREMRANGEBYSCORE,	"ZREMRANGEBYSCORE",	EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_ZLEXCOUNT,		"ZLEXCOUNT",		EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZRANGEBYLEX,		"ZRANGEBYLEX",		EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZREVRANGEBYLEX,	"ZREVRANGEBYLEX",	EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_ZSCAN,			"ZSCAN",			EM_REDIS_FK, 	TRUE,  FALSE },

	//========== bit set Command ===============
	{ CMD_SETBIT,			"SETBIT",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_GETBIT,			"GETBIT",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_BITCOUNT,			"BITCOUNT",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_BITPOS,			"BITPOS",			EM_REDIS_FK, 	TRUE,  FALSE },

	//========== transaction Command ===========
	{ CMD_MULTI,			"MULTI",			EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_DISCARD,			"DISCARD",			EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_WATCH,			"WATCH",			EM_REDIS_MK, 	FALSE, TRUE  },
	{ CMD_UNWATCH,			"UNWATCH",			EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_EXEC,				"EXEC",				EM_REDIS_NK, 	FALSE, TRUE  },

	//========== geo Command ===================
	{ CMD_GEOADD,			"GEOADD",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_GEODIST,			"GEODIST",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_GEOHASH,			"GEOHASH",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_GEOPOS,			"GEOPOS",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_GEORADIS,			"GEORADIS",			EM_REDIS_FK, 	TRUE,  FALSE },
	{ CMD_GEORADISBYNAME,	"GEORADISBYNAME",	EM_REDIS_FK, 	TRUE,  FALSE },

	//========== Pub/Sub Command ===============
	{ CMD_PUBLISH,			"PUBLISH",			EM_REDIS_FK, 	FALSE,  TRUE },
	{ CMD_SUBSCRIBE,		"SUBSCRIBE",		EM_REDIS_MK, 	FALSE,  TRUE },
	{ CMD_PSUBSCRIBE,		"PSUBSCRIBE",		EM_REDIS_MK, 	FALSE,  TRUE },
	{ CMD_UNSUBSCRIBE,		"UNSUBSCRIBE",		EM_REDIS_MK, 	FALSE,  TRUE },
	{ CMD_PUNSUBSCRIBE,		"PUNSUBSCRIBE",		EM_REDIS_MK, 	FALSE,  TRUE },

	//========== HyperLogLog Command ===========
	{ CMD_PFADD,			"PFADD",			EM_REDIS_FK, 	TRUE,  TRUE  },
	{ CMD_PFCOUNT,			"PFCOUNT",			EM_REDIS_MK, 	TRUE,  FALSE },
	{ CMD_PFMERGE,			"PFMERGE",			EM_REDIS_MK, 	TRUE,  TRUE  },

	//========== script Command ================
	{ CMD_EVAL,				"EVAL",				EM_REDIS_U_KEY, TRUE,  TRUE  },
	{ CMD_EVALSHA,			"EVALSHA",			EM_REDIS_U_KEY, TRUE,  TRUE  },
	{ CMD_SCRIPT,			"SCRIPT",			EM_REDIS_MK,    TRUE,  TRUE  },
	{ CMD_SCRIPT_FLUSH,		"SCRIPT FLUSH",		EM_REDIS_NK,    FALSE, TRUE  },

	//========== sentinel Command ==============
	{ CMD_SENTINEL, 		"SENTINEL", 		EM_REDIS_SK, 	FALSE, FALSE },

	//========== cluster Command ===============
	{ CMD_ASKING, 			"ASKING", 			EM_REDIS_NK, 	FALSE, TRUE  },
	{ CMD_CLUSTER_NODES, 	"CLUSTER NODES", 	EM_REDIS_NK, 	FALSE, FALSE },
};
		
const I8* getRedisCmdName(EM_REDIS_CMD eCmd) {
	if(eCmd == TOTAL_CMD_COUNT) {
		return NULLPTR;
	}
			
	return gRedisCmds[(I32)eCmd].m_Name;
}
		

RedisCmd::RedisCmd(EM_REDIS_CMD eCmd, BOOL bSkip)
	: m_Cmd(eCmd, bSkip) {}

RedisCmd::~RedisCmd() {}

WString::WString(const I8* pBuf, UI32 uLen/* = INVALID_VALUE*/) {
	m_bDestory = FALSE;
	m_pBuf = (I8*) pBuf;
	if(NULLPTR == pBuf) {
		m_uLen = 0;
	}
	else {
		if(INVALID_VALUE == (I32)uLen) {
			m_uLen = Tools::Strlen(pBuf);
		}
		else {
			m_uLen = uLen;
		}
	}
}

WString::WString(const WString& w) {
	m_bDestory = TRUE;
	if(0 == w.m_uLen) {
		m_pBuf = NULLPTR;
		m_uLen = 0;
		return;
	}

	if(m_bDestory) {
		m_pBuf = (I8*)W_MALLOC(w.m_uLen);
		memcpy(m_pBuf, w.m_pBuf, w.m_uLen);
	}
	else {
		m_pBuf = w.m_pBuf;
	}

	m_uLen = w.m_uLen;
}

RedisMCmd::RedisMCmd(EM_REDIS_CMD eCmd, BOOL bSkip/* = FALSE*/)
	: RedisCmd(eCmd, bSkip) {
	const I8* pBuf = getRedisCmdName(eCmd);
	append(pBuf);
}

void RedisMCmd::append(const I8* pBuf) {
	if(NULLPTR == pBuf) return;

	WString ws(pBuf);
	append(ws);
}

void RedisMCmd::append(const I8* pBuf, const UI32 uLen) {
	if(NULLPTR == pBuf) return;

	WString ws(pBuf, uLen);
	append(ws);
}

void RedisMCmd::append(std::string& str) {
	WString ws(str.c_str(), str.size());
	append(ws);
}

void RedisMCmd::append(WString& ws) {
	SRedisCmdBase base = gRedisCmds[(I32)getCmd()];
	I32 nVectorSize = m_Vector.size();
	if(
		  (EM_REDIS_FK  == base.m_eKeyPos && 1 == nVectorSize)
	   || (EM_REDIS_SK  == base.m_eKeyPos && 2 == nVectorSize)
	   || (EM_REDIS_DK  == base.m_eKeyPos && (1 == nVectorSize || 2 == nVectorSize))
	   || (EM_REDIS_MFK == base.m_eKeyPos && 1 == (nVectorSize % 2))
       || (EM_REDIS_MK  == base.m_eKeyPos && 1 <= nVectorSize)
		)
	{
		m_Keys.push_back(ws);
	}
	m_Vector.push_back(ws);
}

void RedisMCmd::appendKey(const I8* pBuf) {
	if(NULLPTR == pBuf) return;

	WString ws(pBuf);
	m_Keys.push_back(ws);
	m_Vector.push_back(ws);
}

void RedisMCmd::appendKey(std::string& str) {
	WString ws(str.c_str(), str.size());
	m_Keys.push_back(ws);
	m_Vector.push_back(ws);
}

BOOL RedisMCmd::encode(OutputStream* pStream) {
	Assert(pStream);
	I32 size = m_Vector.size();
	if(0 == size) return FALSE;

	I32 wsize = 0;
	BOOL bCheck = FALSE;
	bCheck = (1 == pStream->write("*", 1));
	if(!bCheck) {
		return FALSE;
	}
	wsize += 1;

	I8 buf[20] = {0};
	I32 nDigitCnt = Tools::CountDigits((UI64)size);
	bCheck = (nDigitCnt == pStream->write(Tools::Itoa(size, buf, 10), nDigitCnt));
	if(!bCheck) {
		pStream->revert(wsize);
		return FALSE;
	}
	wsize += nDigitCnt;

	bCheck = (2 == pStream->write(CRLF, 2));
	if(!bCheck) {
		pStream->revert(wsize);
		return FALSE;
	}
	wsize += 2;

	std::vector<WString>::iterator it;
	for(it = m_Vector.begin(); it != m_Vector.end(); ++it) {
		WString& ws = *it;

		bCheck = (1 == pStream->write("$", 1));
		if(!bCheck) {
			pStream->revert(wsize);
			return FALSE;
		}
		wsize += 1;

		memset(buf, 0, sizeof(buf));
		nDigitCnt = Tools::CountDigits((UI64)ws.m_uLen);
		bCheck = (nDigitCnt == pStream->write(Tools::Itoa(ws.m_uLen, buf, 10), nDigitCnt));
		if(!bCheck) {
			pStream->revert(wsize);
			return FALSE;
		}
		wsize += nDigitCnt;

		bCheck = (2 == pStream->write(CRLF, 2));
		if(!bCheck) {
			pStream->revert(wsize);
			return FALSE;
		}
		wsize += 2;

		bCheck = (ws.m_uLen == (UI32)pStream->write(ws.m_pBuf, ws.m_uLen));
		if(!bCheck) {
			pStream->revert(wsize);
			return FALSE;
		}
		wsize += ws.m_uLen;

		bCheck = (2 == pStream->write(CRLF, 2));
		if(!bCheck) {
			pStream->revert(wsize);
			return FALSE;
		}
		wsize += 2;
	}

	return TRUE;
}

BOOL RedisMCmd::encode(I8* pStream, OUT I32* pOutLen) {
	if(NULLPTR == pStream) return FALSE;

	I32 size = m_Vector.size();
	if(0 == size) return FALSE;

	std::string s("*");

	I8 buf[20] = {0};
	s += Tools::Itoa(size, buf, 10);
	s += CRLF;

	std::vector<WString>::iterator it;
	for(it = m_Vector.begin(); it != m_Vector.end(); ++it) {
		WString& ws = *it;

		s += "$";
		memset(buf, 0, sizeof(buf));
		s += Tools::Itoa(ws.m_uLen, buf, 10);
		s += CRLF;

		std::string tmp(ws.m_pBuf, ws.m_uLen);
		s += tmp;
		s += CRLF;
	}

	if(NULLPTR != pOutLen) {
		*pOutLen = s.size();
	}
	memcpy(pStream, s.c_str(), s.size());

	return TRUE;
}

RedisCmd* RedisMCmd::clone() {
	RedisMCmd* pObj = new RedisMCmd(m_Cmd.m_eCmd, m_Cmd.m_bSkip);
	AssertRetNull(pObj);

	pObj->m_Keys.clear();
	for(UI32 i = 0; i < m_Keys.size(); ++i) {
		WString& s = m_Keys[i];
		WString w(s);
		pObj->m_Keys.push_back(w);
	}

	for(UI32 i = 0; i < m_Vector.size(); ++i) {
		WString& s = m_Vector[i];
		if(0 < i) {
			WString w(s);
			pObj->m_Vector.push_back(w);
		}
	}

	return pObj;
}

RedisNoKeyCmd::RedisNoKeyCmd(EM_REDIS_CMD eCmd, const I8* pCmdName, const I8* pBytes) 
	: RedisCmd(eCmd, FALSE) {
	m_pCmdName = (I8*)pCmdName;
	m_pBytes = (I8*)pBytes;
	m_uLen = Tools::Strlen(pBytes);
}

RedisNoKeyCmd::~RedisNoKeyCmd() {
	W_FREE(m_pBytes);
	cleanUp();
}

void RedisNoKeyCmd::cleanUp() {
	m_pBytes = NULLPTR;
	m_pCmdName = NULLPTR;
	m_uLen     = 0;
}

BOOL RedisNoKeyCmd::encode(OutputStream * pStream) {
	Assert(pStream);
	return m_uLen == (UI32)pStream->write(m_pBytes, m_uLen);
}

BOOL RedisNoKeyCmd::encode(I8* pStream, OUT I32* pOutLen) {
	if(!pStream) return FALSE;
	if(NULLPTR != pOutLen) {
		*pOutLen = m_uLen;
	}

	memcpy(pStream, m_pBytes, m_uLen);
	return TRUE;
}

RedisCmd* RedisNoKeyCmd::clone() {
	return this;
}

IMPLEMENT_SINGLETON(RedisNoKeyCmdMgr)

RedisNoKeyCmdMgr::RedisNoKeyCmdMgr() {
	init();
}

RedisNoKeyCmdMgr::~RedisNoKeyCmdMgr() {
	std::map<EM_REDIS_CMD, RedisNoKeyCmd*>::iterator it;
	for(it = m_Map.begin(); it != m_Map.end(); it++) {
		W_DELETE(it->second);
	}

	m_Map.clear();
}


RedisCmd* RedisNoKeyCmdMgr::get(EM_REDIS_CMD eCmd) {
	std::map<EM_REDIS_CMD, RedisNoKeyCmd*>::iterator it = m_Map.find(eCmd);
	if(it == m_Map.end()) return NULLPTR;

	return (RedisCmd*) it->second;
}

void RedisNoKeyCmdMgr::init() {
	_register(CMD_PING);
	_register(CMD_TIME);
	_register(CMD_DB_SIZE);
	_register(CMD_FLUSH_DB);
	_register(CMD_FLUSH_ALL);
	_register(CMD_SAVE);
	_register(CMD_BG_SAVE);
	_register(CMD_BG_REWRITE_AOF);
	_register(CMD_LAST_SAVE_TIME);
	_register(CMD_SLAVE_OF_NONE);
	_register(CMD_ASKING);
	_register(CMD_CLUSTER_NODES);
	_register(CMD_RANDOMKEY);
	_register(CMD_MULTI);
	_register(CMD_DISCARD);
	_register(CMD_UNWATCH);
	_register(CMD_EXEC);
	_register(CMD_SCRIPT_FLUSH);
}


void RedisNoKeyCmdMgr::_register(EM_REDIS_CMD eCmd) {
	__ENTER_FUNCTION
	const I8* pCmdName = getRedisCmdName(eCmd);
	AssertWithThrow(pCmdName);

	StringSplit spliter;
	I32 nCnt = spliter.doSplit(pCmdName, " ");
	if(INVALID_VALUE == nCnt) {
		Logger.error("RedisNoKeyCmdMgr::_register split %s failed.", pCmdName);
		return;
	}

	I8 digit[10] = {0};
	std::string s("*");
	s += Tools::Itoa(nCnt, digit, 10);
	s += CRLF;

	I8* pStr = NULLPTR;
	for(I32 i = 0; i < nCnt; ++i) {
		pStr = spliter.getLine(i);
		if(NULLPTR == pStr) {
			Logger.error("RedisNoKeyCmdMgr::_register split %s failed in for[%d].", pCmdName, i);
			return;
		}

		s += "$";
		I32 strLen = Tools::Strlen(pStr);
		memset(digit, 0, sizeof(digit));
		s += Tools::Itoa(strLen, digit, 10);
		s += CRLF;
		s += pStr;
		s += CRLF;
	}

	// malloc pbytes
	I32 nSize = s.size();
	I8* pBytes = (I8*)W_MALLOC(nSize + 1);
	Assert(pBytes);
	memset(pBytes, 0, nSize + 1);

	Tools::Strncpy(pBytes, s.c_str(), nSize);
	RedisNoKeyCmd* pCmd = new RedisNoKeyCmd(eCmd, pCmdName, pBytes);
	Assert(pCmd);

	// m_Map add 
	m_Map.insert(std::pair<EM_REDIS_CMD, RedisNoKeyCmd*>(eCmd, pCmd));

	__LEAVE_FUNCTION
}


} /* namespace wRedis */
