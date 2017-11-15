#ifndef _RTP_SENDER_H_
#define  _RTP_SENDER_H_

#include <winsock2.h>
#include <string>
#include <stdint.h>


#define MAX_MTU_SIZE 1400

#pragma pack(1)
typedef struct RtpHeader
{
	uint16_t cc:4;			    /// csrc�б�����(4bit)
	uint16_t x:1;				/// ����ͷλ(1bit)
	uint16_t p:1;				/// ���λ(1bit)
	uint16_t v:2;				/// �汾��(2bit)
	uint16_t pt:7;				/// ��������(7bit)
	uint16_t m:1;			    /// ���λ(1bit)
	uint16_t seq;				/// ���к�(16bit)
	uint32_t ts;				/// ʱ���(32bit)
	uint32_t ssrc;				/// ��Դ��ʾ(32bit)
}RtpHeader;

typedef struct  VideoPacketHeader
{
	uint32_t frameNumber;
	uint16_t framePacketIndex;
	uint16_t frameTotalPackets;
	uint8_t  frameType;
}VideoPacketHeader;
#pragma pack()

class RtpSession
{
public:
	RtpSession(const std::string& destIp, uint16_t port, uint32_t ssrc, uint32_t csrcCnt, uint32_t csrc,uint32_t memberId,
		uint16_t localPort = 0, int16_t maxpacketsize = MAX_MTU_SIZE);
	RtpSession(uint32_t destIp, uint16_t port, uint32_t ssrc, uint32_t csrcCnt, uint32_t csrc, uint32_t memberId,
		uint16_t localPort = 0, int16_t maxpacketsize = MAX_MTU_SIZE);
	~RtpSession();
public:
	int32_t sendVideoFrame(const uint8_t* pFrameBuf, int32_t frameLength, uint32_t frameType, uint32_t frameNum, int8_t pt,
		bool m, int32_t timestampInc);
	uint32_t getSSRC();
	uint32_t getMemberid();
private:
	int32_t encodeVideoRtpPacket(const uint8_t* buf, uint32_t payloadLength, int8_t pt, bool m, int32_t timestampInc,
		uint32_t csrcCnt, uint32_t csrc, uint32_t memberId, uint32_t frameNum, uint32_t pktIndex, uint32_t totalPkts, uint32_t frameType);
	int32_t encodeRtp(int32_t pt, int32_t m, int32_t payloadLength, int32_t timestampInc);
	int32_t setCSRC(uint32_t csrc_cnt,uint32_t csrc, uint32_t memberId);
	int32_t setVideoHeader(uint32_t frameNum, uint32_t pktIndex, uint32_t totalPkts, uint32_t frameType, uint32_t csrcCnt);

	int16_t _maxPacketSize;
	uint32_t maxVideoPayloadSize;
	uint8_t* pSendBuf;
	uint32_t seq;
	uint32_t ts;
	uint32_t ssrc;
	uint32_t csrc;
	uint32_t csrcCnt;
	uint32_t memberId;
	RtpHeader outRtpHeader;
	SOCKET _socket;
	WSADATA  wsaData;
	struct sockaddr_in _destTo;
};

#endif // !_RTP_SENDER_H_
