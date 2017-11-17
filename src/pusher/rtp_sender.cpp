#include "pusher/rtp_sender.h"
#include <WS2tcpip.h>
#include <common_define.h>
#include <stdio.h>
#include <stdexcept>
#include <stdlib.h>
#include <time.h>

#define random(x) (rand()%x)
#define RTP_VERSION			2
#define RTP_HEADER_SIZE		12
#define DEFUALT_VIDEO_SRC_PT 97
#define OBLIGATE_SIZE 20
#define THIS_FILE "dml_sender.c"
RtpSession::RtpSession(const std::string& destIp, uint16_t port, uint32_t ssrc, uint32_t csrcCnt,uint32_t csrc, uint32_t memberId, 
	uint16_t localPort /*= 0*/, int16_t maxpacketsize)
	:_maxPacketSize(maxpacketsize),
	pSendBuf(NULL),
	ts(0),
	ssrc(ssrc),
	csrcCnt(csrcCnt),
	csrc(csrc),
	memberId(memberId)
{
	if (maxpacketsize >= sizeof(RtpHeader))
		pSendBuf = new uint8_t[maxpacketsize];
	else
		throw std::runtime_error("too small packet size, must more than 12 Byte");

	maxVideoPayloadSize = maxpacketsize - OBLIGATE_SIZE - sizeof(RtpHeader) - sizeof(VideoPacketHeader);
	memset(&outRtpHeader, 0, sizeof(RtpHeader));
	srand((int)time(0));
	seq = random(65535);
	outRtpHeader.v = 2;
	outRtpHeader.pt = DEFUALT_VIDEO_SRC_PT;
	outRtpHeader.seq = (uint16_t)htons((uint16_t)seq);
	outRtpHeader.ssrc = htonl(ssrc);
	outRtpHeader.ts = htonl(0);
	
	WSAStartup(0x202, &wsaData);
	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (_socket == INVALID_SOCKET)
		throw std::runtime_error("invalid socket");

	u_long nonblocking = 1;
	//ioctlsocket(_socket, FIONBIO, &nonblocking);

	_destTo.sin_family = AF_INET;
	_destTo.sin_port = htons(port);
	//_destTo.sin_addr.s_addr = inet_addr(destIp.c_str());
	inet_pton(AF_INET, destIp.c_str(), (void*)&_destTo.sin_addr.s_addr);

	if (localPort != 0){
		struct sockaddr_in sockAddr;
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(localPort);
		sockAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(_socket, (const sockaddr*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR){
#ifndef NPRINT
#ifdef _WIN32
			printf("bind error: %d\n", WSAGetLastError());
#else
			printf("bind error: %d\n", errno);
#endif
#endif
			closesocket(_socket);
			throw std::runtime_error("bind error");
		}
	}
}

RtpSession::RtpSession(uint32_t destIp, uint16_t port, uint32_t ssrc, uint32_t csrcCnt, uint32_t csrc,uint32_t memberId,
	uint16_t localPort /*= 0*/, int16_t maxpacketsize)
	:_maxPacketSize(maxpacketsize),
	pSendBuf(NULL),
	seq(0),
	ts(0),
	ssrc(ssrc),
	csrcCnt(csrcCnt),
	csrc(csrc),
	memberId(memberId)
{
	if (maxpacketsize >= sizeof(RtpHeader))
		pSendBuf = new uint8_t[maxpacketsize];
	else
		throw std::runtime_error("too small packet size, must more than 12 Byte");
	maxVideoPayloadSize = maxpacketsize - OBLIGATE_SIZE - sizeof(RtpHeader) - sizeof(VideoPacketHeader);

	memset(&outRtpHeader, 0, sizeof(RtpHeader));
	seq = random(65535);;
	outRtpHeader.v = 2;
	outRtpHeader.pt = DEFUALT_VIDEO_SRC_PT;
	srand((int)time(0));
	outRtpHeader.seq = (uint16_t)htons((uint16_t)seq);
	outRtpHeader.ssrc = htonl(ssrc);
	outRtpHeader.ts = htonl(0);

	WSAStartup(0x202, &wsaData);
	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (_socket == INVALID_SOCKET)
		throw std::runtime_error("invalid socket");

	u_long nonblocking = 1;
	ioctlsocket(_socket, FIONBIO, &nonblocking);

	_destTo.sin_family = AF_INET;
	_destTo.sin_port = htons(port);
	_destTo.sin_addr.s_addr = htonl(destIp);

	if (localPort != 0){
		struct sockaddr_in sockAddr;
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(localPort);
		sockAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(_socket, (const sockaddr*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR){
#ifndef NPRINT
#ifdef _WIN32
			printf("bind error: %d\n", WSAGetLastError());
#else
			printf("bind error: %d\n", errno);
#endif
#endif
			closesocket(_socket);
			throw std::runtime_error("bind error");
		}
	}
}

RtpSession::~RtpSession()
{
	if (_socket != INVALID_SOCKET)
		closesocket(_socket);
	if (pSendBuf != NULL)
		delete[] pSendBuf;
	WSACleanup();
}

int32_t RtpSession::sendVideoFrame(const uint8_t* pFrameBuf, int32_t frameLength, uint32_t frameType,uint32_t frameNum, int8_t pt,
	bool m, int32_t timestampInc)
{
	uint32_t  totalPkts;
	ts = ts + timestampInc;
	if (frameLength % maxVideoPayloadSize > 0){
		totalPkts = frameLength / maxVideoPayloadSize + 1;
	}else{
		totalPkts = frameLength / maxVideoPayloadSize;
	}
	uint32_t sendOffset = 0;
	for (uint32_t pktIndex = 0; pktIndex < totalPkts; pktIndex++){
		uint32_t leftSendSize = frameLength - sendOffset;
		uint32_t videoSendSize;
		if (leftSendSize >= maxVideoPayloadSize){
			videoSendSize = maxVideoPayloadSize;
		}else{
			videoSendSize = leftSendSize;
		}
		encodeVideoRtpPacket(pFrameBuf + sendOffset, videoSendSize, pt, m, ts, csrcCnt, csrc, memberId, frameNum, pktIndex, totalPkts, frameType);
		int ret;
		ret = sendto(_socket, (const char*)pSendBuf, videoSendSize + sizeof(RtpHeader) + sizeof(VideoPacketHeader) + csrcCnt * sizeof(uint32_t),
			0, (const sockaddr*)&_destTo, sizeof(_destTo));
		if (ret <=0){
			Log::d(THIS_FILE, "Send Rtp Failed! FrameNum:%d PktIndex:%d TotalPkts:%d ", frameNum, pktIndex, totalPkts);
		}
		sendOffset += videoSendSize;
	}
	return NO_ERROR;
}

int32_t RtpSession::encodeRtp(int32_t pt, int32_t m, int32_t payloadLength, int32_t timestampInc)
{
	if (!payloadLength){
		return NO_ERROR;
	}

	seq++;

	outRtpHeader.p = 0;
	outRtpHeader.x = 0;
	outRtpHeader.cc = 0;
	outRtpHeader.pt = (uint8_t)pt;
	outRtpHeader.m = (uint16_t)m;
	outRtpHeader.seq = htons((uint16_t)seq);
	outRtpHeader.ts = htonl(ts);

	memcpy(pSendBuf, &outRtpHeader, sizeof(RtpHeader));

	return NO_ERROR;
}

int32_t RtpSession::setCSRC(uint32_t csrc_cnt,uint32_t csrc, uint32_t memberId)
{
	RtpHeader * pRtp = (RtpHeader*)pSendBuf;

	pRtp->cc = csrc_cnt;

	uint32_t * pFirstCSRC = (uint32_t*)(pSendBuf + sizeof(RtpHeader));
	uint32_t * pSecondCSRC = (uint32_t*)(((uint8_t*)pFirstCSRC) + sizeof(uint32_t));

	*pFirstCSRC = htonl(csrc);
	*pSecondCSRC = htonl(memberId);

	return NO_ERROR;
}

int32_t RtpSession::encodeVideoRtpPacket(const uint8_t* buf, uint32_t payloadLength, int8_t pt, bool m, int32_t timestampInc, 
	uint32_t csrcCnt, uint32_t csrc, uint32_t memberId, uint32_t frameNum, uint32_t pktIndex, uint32_t totalPkts, uint32_t frameType)
{
	if (!buf){
		return 1;
	}

	encodeRtp(pt, m, payloadLength, timestampInc);
	setCSRC(csrcCnt, csrc, memberId);
	setVideoHeader(frameNum, pktIndex, totalPkts, frameType, csrcCnt);

	uint32_t totalHeaderSize = sizeof(RtpHeader) + csrcCnt * sizeof(uint32_t) + sizeof(VideoPacketHeader);
	memcpy(pSendBuf + totalHeaderSize, buf, payloadLength);

	return NO_ERROR;
}

int32_t RtpSession::setVideoHeader(uint32_t frameNum, uint32_t pktIndex, uint32_t totalPkts, uint32_t frameType,uint32_t csrcCnt)
{
	VideoPacketHeader * pVideoHeader = (VideoPacketHeader*)(pSendBuf + sizeof(RtpHeader) + csrcCnt * sizeof(uint32_t));

	pVideoHeader->frameNumber = htonl(frameNum);
	pVideoHeader->framePacketIndex = htons((uint16_t)pktIndex);
	pVideoHeader->frameTotalPackets = htons((uint16_t)totalPkts);
	pVideoHeader->frameType = frameType;

	return NO_ERROR;
}

uint32_t RtpSession::getSSRC()
{
	return ssrc;
}

uint32_t RtpSession::getMemberid()
{
	return memberId;
}

