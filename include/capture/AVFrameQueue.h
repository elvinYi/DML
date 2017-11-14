#ifndef _AV_FRAME_QUEUE_
#define _AV_FRAME_QUEUE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util/Errors.h"
#include <util/BaseDataTypes.h>
#include "util/MediaFormat.h"

using namespace MediaPlugin;

namespace MediaCoreAVFrame {

	class BaseFrame
	{
	public:
		BaseFrame();
		BaseFrame(const UInt8* buffer, SInt32 size, SInt64 timestamp, SInt64 index);
		virtual ~BaseFrame(){};

	public:
		SInt32 setBuffer(const UInt8* buffer);
		SInt32 setSize(const SInt32 size);
		SInt32 setTimestamp(const SInt64 timestamp);
		SInt32 setIndex(const SInt64 index);
		UInt8* getBuffer();
		SInt32 getSize();
		SInt64 getTimestamp();
		SInt64 getIndex();

	private:
		UInt8* mBuffer;
		SInt32 mSize;
		SInt64 mTimestamp;
		SInt64 mIndex;

	};

	class VideoFrame :public BaseFrame
	{
	public:
		VideoFrame();
		VideoFrame(const UInt8* buffer, SInt32 size, SInt64 timestamp,
			SInt64 index, SInt32 width, SInt32 height, ColorFormat colorFormat);
		~VideoFrame(){};
		SInt32 setWidth(const SInt32 width);
		SInt32 setHeight(const SInt32 height);
		SInt32 setColorFormat(const ColorFormat colorfomat);
		SInt32 getWidth();
		SInt32 getHeight();
		ColorFormat getColotFormat();

	private:
		SInt32 mWidth;
		SInt32 mHeight;
		ColorFormat mColorFormat;
	};

	class AudioFrame :public BaseFrame
	{
	public:
		AudioFrame();
		AudioFrame(const UInt8* buffer, SInt32 size, SInt64 timestamp,
			SInt64 index, SInt32 channels, SInt32 sampleRate, SInt32 bitRate);;
		~AudioFrame(){};

		SInt32 setChannels(const SInt32 channels);
		SInt32 setSampleRate(const SInt32 sampleRate);
		SInt32 setBitRate(const SInt32 bitRate);
		SInt32 getChannels();
		SInt32 getSampleRate();
		SInt32 getBitRate();
	private:
		SInt32 mChannels;
		SInt32 mSampleRate;
		SInt32 mBitRate;
	};


	class BaseFrameQueue
	{
	public:
		BaseFrameQueue(SInt32 trackIndex, SInt32 maxBlockCount);
		virtual ~BaseFrameQueue();
		SInt32 allocateMemory(int sizeEachBlock);
		SInt32 releaseInputFrame(BaseFrame* inputUnit);
		SInt32 putData(BaseFrame* inputUnit);
		SInt32 releaseOutputFrame(BaseFrame* outputUnit);
		BaseFrame* getInputFrame();
		BaseFrame* getOutputFrame();
		SInt32 getTotolCount();
		SInt32 getEmptyCount();
		SInt32 getData(BaseFrame* outputUnit);
		SInt32 peekData(BaseFrame** outputUnit);
		bool isUsedBlockEmpty();
		bool isUsedBlockFull();
		void clearData();

		BaseFrame* dequeueEmptyFrame();
		BaseFrame* dequeueUsedFrame();
		void enqueueEmptyFrame();
		void enqueueUsedFrame();
	protected:
		BaseFrame* mFrameArray;
		SInt32 mSizeEachBlock;
		SInt32 mTotalCount;
		SInt32 mWriteIndex;
		SInt32 mReadIndex;
		SInt32 mUsedCount;
		SInt32 mTrackIndex;
	};

	class VideoFrameQueue :public BaseFrameQueue
	{
	public:
		VideoFrameQueue(SInt32 trackIndex, SInt32 maxBlockCount);
		~VideoFrameQueue();

		SInt32 allocateMemory(int sizeEachBlock);
		SInt32 releaseInputFrame(VideoFrame* inputUnit);
		SInt32 releaseOutputFrame(VideoFrame* outputUnit);
		VideoFrame* getInputFrame();
		VideoFrame* getOutputFrame();
		SInt32 putData(VideoFrame* inputUnit);
		SInt32 getData(VideoFrame* outputUnit);
		SInt32 peekData(VideoFrame** outputUnit);
		
	private:
		VideoFrame* dequeueEmptyFrame();
		VideoFrame* dequeueUsedFrame();
	private:
		VideoFrame* mVideoArray;
	};

	class AudioFrameQueue :public BaseFrameQueue
	{
	public:
		AudioFrameQueue(SInt32 trackIndex, SInt32 maxBlockCount);
		~AudioFrameQueue();
		SInt32 allocateMemory(int sizeEachBlock);
		SInt32 releaseInputFrame(AudioFrame* inputUnit);
		SInt32 releaseOutputFrame(AudioFrame* outputUnit);
		AudioFrame* getInputFrame();
		AudioFrame* getOutputFrame();
		SInt32 putData(AudioFrame* inputUnit);
		SInt32 getData(AudioFrame* outputUnit);
		SInt32 peekData(AudioFrame** outputUnit);

	private:
		AudioFrame* dequeueEmptyFrame();
		AudioFrame* dequeueUsedFrame();
	private:
		AudioFrame* mAudioArray;

	};

};
#endif