
#include <wedis/base/net/Connector.h>
#include <wedis/base/util/Log.h>
#include <wedis/base/util/Alloctor.h>

namespace wRedis {

/**
 * 构造函数
 */
Connector::Connector() {
	m_GUID.cleanUp();
	m_pSocket 			  = NULLPTR;
	m_pSocketInputStream  = NULLPTR;
	m_pSocketOutputStream = NULLPTR;
	m_pParser             = NULLPTR;
}

/**
 * 析钩函数
 */
Connector::~Connector() {
	W_DELETE(m_pSocketInputStream);
	W_DELETE(m_pSocketOutputStream);
	W_DELETE(m_pSocket);
	W_DELETE(m_pParser);
}

/**
 * 初始化socket
 * @param eType RAW|TCP|UDP
 */
BOOL Connector::init(EM_SOCKET_TYPE eType, Codec* pParser) {
	AssertRetFalse(pParser);
	m_pParser = pParser;

	m_pSocket = new Socket(eType);
	Assert(m_pSocket);
	Assert(m_pSocket->create());

	m_pSocketInputStream = new InputStream(m_pSocket, DEFAULT_INPUT_STREAM_SIZE, DEFAULT_INPUT_STREAM_MAX_SIZE);
	Assert(m_pSocketInputStream);

	m_pSocketOutputStream = new OutputStream(m_pSocket, DEFAULT_OUTPUT_STREAM_SIZE, DEFAULT_OUTPUT_STREAM_MAX_SIZE);
	Assert(m_pSocketOutputStream);
	return TRUE;
}

/**
 * 初始化socket
 * @param eType RAW|TCP|UDP
 */
BOOL Connector::init(EM_SOCKET_TYPE eType, const I8* pHost, const UI16 uPort, Codec* pParser) {
	AssertRetFalse(pParser);
	m_pParser = pParser;

	m_pSocket = new Socket(eType, pHost, uPort);
	Assert(m_pSocket);
	Assert(m_pSocket->create());

	Assert(m_pSocket->isValid());

	m_pSocketInputStream = new InputStream(m_pSocket, DEFAULT_INPUT_STREAM_SIZE, DEFAULT_INPUT_STREAM_MAX_SIZE);
	Assert(m_pSocketInputStream);

	m_pSocketOutputStream = new OutputStream(m_pSocket, DEFAULT_OUTPUT_STREAM_SIZE, DEFAULT_OUTPUT_STREAM_MAX_SIZE);
	Assert(m_pSocketOutputStream);

	return TRUE;
}

/**
 * 清空
 */
void Connector::cleanUp() {
	if (m_pSocket)
		disconnect();

	if (m_pSocketInputStream)
		m_pSocketInputStream->reset();

	if (m_pSocketOutputStream)
		m_pSocketOutputStream->reset();

	m_GUID.cleanUp();
}

BOOL Connector::isNonBlocking() const {
	AssertRetFalse(m_pSocket);
	return m_pSocket->isNonBlocking();
}

SHostAndPort* Connector::getHostAndPort() {
	if(NULLPTR == m_pSocket) return NULLPTR;
	return &m_pSocket->getHostAndPort();
}

void Connector::setHostAndPort(SHostAndPort& hp) {
	if(NULLPTR == m_pSocket) return;
	m_pSocket->close();
	m_pSocket->setDest(hp.m_IP, hp.m_uPort);
}

/**
 * 断开连接
 */
void Connector::disconnect() {
	AssertRetEmpty(m_pSocket);
	m_pSocket->close();
}

/**
 * 连接是否有效
 */
BOOL Connector::isValid() {
	if(NULLPTR == m_pSocket)  return FALSE;
	if(!m_pSocket->isValid()) return FALSE;
	return TRUE;
}

/**
 * recv from socket to inputstream
 */
BOOL Connector::procInput() {
	UI32 ret = m_pSocketInputStream->fill();
	if(SOCKET_ERROR >= (I32)ret) {
		Logger.error("m_pSocketInputStream->Fill ret:%d, errno:%d, errstr:%s", (I32)ret, errno, strerror(errno));
		return FALSE;
	}

	return TRUE;
}

/**
 * 处理对端已关闭了连接的接口
 */
BOOL Connector::procCommand() {
	return TRUE;
}

/**
 * 执行socket send
 */
BOOL Connector::procOutput() {
	AssertRetFalse(m_pSocketOutputStream);
	UI32 size = m_pSocketOutputStream->getBuffSize();
	if(0 == size) {
		Logger.warn("Connector ProcessOutput GetBuffSize size is 0");
		return TRUE;
	}

	UI32 ret = m_pSocketOutputStream->flush();
	if(SOCKET_ERROR >= (I32)ret) {
		Logger.error("m_pSocketOutputStream->Flush ret:%d, errno:%d, errstr:%s", (I32)ret, errno, strerror(errno));
		return FALSE;
	}
	return TRUE;
}

/**
 * 写入缓冲区
 */
BOOL Connector::sendPacket(void* pPacket) {
	Assert(m_pParser);
	Assert(m_pSocketOutputStream);
	return m_pParser->encode(pPacket, m_pSocketOutputStream);
}

/**
 * 写入缓冲区
 */
BOOL Connector::sendPriorityPacket(void* pPacket) {
	Assert(m_pParser);
	Assert(m_pSocketOutputStream);
	return m_pParser->encode(pPacket, m_pSocketOutputStream, TRUE);
}

}

