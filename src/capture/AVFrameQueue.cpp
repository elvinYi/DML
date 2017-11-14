#include "capture/AVFrameQueue.h"


namespace MediaCoreAVFrame {

	BaseFrame::BaseFrame()
		: mBuffer(NULL), mSize(1), mTimestamp(0), mIndex(0)
	{
	}

	BaseFrame::BaseFrame(const UInt8* buffer, SInt32 size, SInt64 timestamp, SInt64 index)
	{
		mBuffer = (UInt8*)buffer;
		mSize = size;
		mTimestamp = timestamp;
		mIndex = index;
	}

	SInt32 BaseFrame::setBuffer(const UInt8* buffer)
	{
		mBuffer = (UInt8*)buffer;
		return OK;
	}

	SInt32 BaseFrame::setSize(const SInt32 size)
	{
		mSize = size;
		return OK;
	}

	SInt32 BaseFrame::setTimestamp(const SInt64 timestamp)
	{
		mTimestamp = timestamp;
		return OK;
	}

	SInt32 BaseFrame::setIndex(const SInt64 index)
	{
		mIndex = index;
		return OK;
	}

	UInt8* BaseFrame::getBuffer()
	{
		return mBuffer;
	}

	SInt32 BaseFrame::getSize()
	{
		return mSize;
	}

	SInt64 BaseFrame::getTimestamp()
	{
		return mTimestamp;
	}

	SInt64 BaseFrame::getIndex()
	{
		return mIndex;
	}


	BaseFrameQueue::BaseFrameQueue(SInt32 trackIndex, SInt32 maxBlockCount)
		: mTotalCount(maxBlockCount),
		mWriteIndex(0),
		mReadIndex(0),
		mUsedCount(0),
		mSizeEachBlock(0),
		mTrackIndex(trackIndex),
		mFrameArray(NULL)
	{
		mFrameArray = new BaseFrame[mTotalCount];
	}


	BaseFrameQueue::~BaseFrameQueue()
	{
		for (SInt32 i = 0; i < mTotalCount; i++)
		{
			UInt8* data = mFrameArray[i].getBuffer();
			if (data)
			{
				delete[] data;
				data = NULL;
			}
		}
		if (mFrameArray)
		{
			delete[] mFrameArray;
			mFrameArray = NULL;
		}
	}

	SInt32 BaseFrameQueue::allocateMemory(int sizeEachBlock)
	{
		if (sizeEachBlock <= 0)
			return BAD_VALUE;
		mSizeEachBlock = sizeEachBlock;
		for (int i = 0; i < mTotalCount; i++)
		{
			UInt8* data = new UInt8[mSizeEachBlock];
			mFrameArray[i].setBuffer(data);
		}

		clearData();
		return OK;
	}

	SInt32 BaseFrameQueue::releaseInputFrame(BaseFrame* inputUnit)
	{
		BaseFrame* currEmptyBlock = dequeueEmptyFrame();
		if (currEmptyBlock == inputUnit)
		{
			// put current block to used.
			enqueueEmptyFrame();
			return OK;
		}
		else
			return DEAD_OBJECT;
	}

	SInt32 BaseFrameQueue::putData(BaseFrame* inputUnit)
	{
		if (inputUnit == NULL || inputUnit->getBuffer() == NULL)
			return BAD_VALUE;

		// byte array boundary check.
		if (inputUnit->getSize() > mSizeEachBlock)
			return NO_MEMORY;

		BaseFrame* currEmptyBlock = dequeueEmptyFrame();
		if (currEmptyBlock == NULL)
			return NOT_ENOUGH_DATA;

		memcpy(currEmptyBlock->getBuffer(), inputUnit->getBuffer(), inputUnit->getSize());
		currEmptyBlock->setSize(inputUnit->getSize());
		currEmptyBlock->setTimestamp(inputUnit->getTimestamp());
		currEmptyBlock->setIndex(inputUnit->getIndex());

		enqueueEmptyFrame();

		return OK;
	}

	SInt32 BaseFrameQueue::releaseOutputFrame(BaseFrame* outputUnit)
	{
		BaseFrame* currentUsedBlock = dequeueUsedFrame();
		if (outputUnit == currentUsedBlock)
		{
			// put current block to empty.
			enqueueUsedFrame();
			return OK;
		}
		else
			return DEAD_OBJECT;
	}

	BaseFrame* BaseFrameQueue::getInputFrame()
	{
		return dequeueEmptyFrame();
	}

	BaseFrame* BaseFrameQueue::getOutputFrame()
	{
		return dequeueUsedFrame();
	}

	SInt32 BaseFrameQueue::getTotolCount()
	{
		return mTotalCount;
	}

	SInt32 BaseFrameQueue::getEmptyCount()
	{
		return mTotalCount - mUsedCount;
	}

	SInt32 BaseFrameQueue::getData(BaseFrame* outputUnit)
	{
		if (outputUnit == NULL || outputUnit->getBuffer() == NULL)
			return BAD_VALUE;

		BaseFrame* currUsedBlock = dequeueUsedFrame();
		if (currUsedBlock == NULL)
			return NOT_ENOUGH_DATA;

		memcpy(outputUnit->getBuffer(), currUsedBlock->getBuffer(), currUsedBlock->getSize());
		outputUnit->setSize(currUsedBlock->getSize());
		outputUnit->setTimestamp(currUsedBlock->getTimestamp());
		outputUnit->setIndex(currUsedBlock->getIndex());

		enqueueUsedFrame();

		return OK;
	}

	SInt32 BaseFrameQueue::peekData(BaseFrame** outputUnit)
	{
		if (outputUnit == NULL)
			return BAD_VALUE;

		BaseFrame* currUsedBlock = dequeueUsedFrame();
		if (currUsedBlock == NULL)
			return NOT_ENOUGH_DATA;

		*outputUnit = currUsedBlock;

		return OK;
	}

	bool BaseFrameQueue::isUsedBlockEmpty()
	{
		if (mUsedCount == 0)
			return true;
		else
			return false;
	}

	bool BaseFrameQueue::isUsedBlockFull()
	{
		if (mUsedCount == mTotalCount)
			return true;
		else
			return false;
	}

	void BaseFrameQueue::clearData()
	{
		mWriteIndex = 0;
		mReadIndex = 0;
		mUsedCount = 0;
	}

	BaseFrame* BaseFrameQueue::dequeueEmptyFrame()
	{
		if (isUsedBlockFull())
			return NULL;

		return &mFrameArray[mWriteIndex];
	}

	BaseFrame* BaseFrameQueue::dequeueUsedFrame()
	{
		if (isUsedBlockEmpty())
			return NULL;

		return &mFrameArray[mReadIndex];
	}

	void BaseFrameQueue::enqueueEmptyFrame()
	{
		if (isUsedBlockFull() == true)
		{
			return;
		}
		mWriteIndex += 1;
		if (mWriteIndex == mTotalCount)
		{
			mWriteIndex = 0;
		}
		mUsedCount += 1;
	}

	void BaseFrameQueue::enqueueUsedFrame()
	{
		if (isUsedBlockEmpty())
		{
			return;
		}
		mReadIndex += 1;
		if (mReadIndex == mTotalCount)
		{
			mReadIndex = 0;
		}
		mUsedCount -= 1;
	}

	VideoFrame::VideoFrame()
		:BaseFrame()
		, mWidth(0)
		, mHeight(0)
		, mColorFormat(VIDEO_COLOR_FORMAT_YUV420P)
	{
		int a = 0;
	}

	VideoFrame::VideoFrame(const UInt8* buffer, SInt32 size, SInt64 timestamp,
		SInt64 index, SInt32 width, SInt32 height, ColorFormat colorFormat)
		:BaseFrame(buffer, size, timestamp, index)
	{
		//BaseFrame(buffer, size, timestamp, index);
		mWidth = width;
		mHeight = height;
		mColorFormat = colorFormat;
	}

	SInt32 VideoFrame::setWidth(const SInt32 width)
	{
		mWidth = width;
		return OK;
	}

	SInt32 VideoFrame::setHeight(const SInt32 height)
	{
		mHeight = height;
		return OK;
	}

	SInt32 VideoFrame::setColorFormat(const ColorFormat colorfomat)
	{
		mColorFormat = colorfomat;
		return OK;
	}

	SInt32 VideoFrame::getWidth()
	{
		return mWidth;
	}

	SInt32 VideoFrame::getHeight()
	{
		return mHeight;
	}

	MediaPlugin::ColorFormat VideoFrame::getColotFormat()
	{
		return mColorFormat;
	}

	AudioFrame::AudioFrame()
		:BaseFrame()
		, mChannels(0)
		, mSampleRate(0)
		, mBitRate(0)
	{

	}

	AudioFrame::AudioFrame(const UInt8* buffer, SInt32 size, SInt64 timestamp,
		SInt64 index, SInt32 channels, SInt32 sampleRate, SInt32 bitRate)
	{
		BaseFrame(buffer, size, timestamp, index);
		mChannels = channels;
		mSampleRate = sampleRate;
		mBitRate = bitRate;
	}

	SInt32 AudioFrame::setChannels(const SInt32 channels)
	{
		mChannels = channels;
		return OK;
	}

	SInt32 AudioFrame::setSampleRate(const SInt32 sampleRate)
	{
		mSampleRate = sampleRate;
		return OK;
	}

	SInt32 AudioFrame::setBitRate(const SInt32 bitRate)
	{
		mBitRate = bitRate;
		return OK;
	}

	SInt32 AudioFrame::getChannels()
	{
		return mChannels;
	}

	SInt32 AudioFrame::getSampleRate()
	{
		return mSampleRate;
	}

	SInt32 AudioFrame::getBitRate()
	{
		return mBitRate;
	}

	VideoFrameQueue::VideoFrameQueue(SInt32 trackIndex, SInt32 maxBlockCount)
		:BaseFrameQueue(trackIndex, maxBlockCount)
	{
		mVideoArray = new VideoFrame[mTotalCount];
	}

	VideoFrameQueue::~VideoFrameQueue()
	{
		for (SInt32 i = 0; i < mTotalCount; i++)
		{
			UInt8* data = mVideoArray[i].getBuffer();
			if (data)
			{
				delete[] data;
				data = NULL;
			}
		}
		if (mVideoArray)
		{
			delete[] mVideoArray;
			mVideoArray = NULL;
		}
	}

	SInt32 VideoFrameQueue::allocateMemory(int sizeEachBlock)
	{
		if (sizeEachBlock <= 0)
			return BAD_VALUE;
		mSizeEachBlock = sizeEachBlock;
		for (int i = 0; i < mTotalCount; i++)
		{
			UInt8* data = new UInt8[mSizeEachBlock];
			mVideoArray[i].setBuffer(data);
		}

		clearData();
		return OK;
	}

	SInt32 VideoFrameQueue::releaseInputFrame(VideoFrame* inputUnit)
	{
		VideoFrame* currEmptyBlock = dequeueEmptyFrame();
		if (currEmptyBlock == inputUnit)
		{
			// put current block to used.
			enqueueEmptyFrame();
			return OK;
		}
		else
			return DEAD_OBJECT;
	}

	SInt32 VideoFrameQueue::releaseOutputFrame(VideoFrame* outputUnit)
	{
		VideoFrame* currentUsedBlock = dequeueUsedFrame();
		if (outputUnit == currentUsedBlock)
		{
			// put current block to empty.
			enqueueUsedFrame();
			return OK;
		}
		else
			return DEAD_OBJECT;
	}

	VideoFrame* VideoFrameQueue::getInputFrame()
	{
		return dequeueEmptyFrame();
	}

	VideoFrame* VideoFrameQueue::getOutputFrame()
	{
		return dequeueUsedFrame();
	}

	SInt32 VideoFrameQueue::putData(VideoFrame* inputUnit)
	{
		if (inputUnit == NULL || inputUnit->getBuffer() == NULL)
			return BAD_VALUE;

		// byte array boundary check.
		if (inputUnit->getSize() > mSizeEachBlock)
			return NO_MEMORY;

		VideoFrame* currEmptyBlock = dequeueEmptyFrame();
		if (currEmptyBlock == NULL)
			return NOT_ENOUGH_DATA;

		memcpy(currEmptyBlock->getBuffer(), inputUnit->getBuffer(), inputUnit->getSize());
		currEmptyBlock->setSize(inputUnit->getSize());
		currEmptyBlock->setTimestamp(inputUnit->getTimestamp());
		currEmptyBlock->setIndex(inputUnit->getIndex());
		currEmptyBlock->setWidth(inputUnit->getWidth());
		currEmptyBlock->setHeight(inputUnit->getHeight());
		currEmptyBlock->setColorFormat(inputUnit->getColotFormat());

		enqueueEmptyFrame();

		return OK;
	}

	SInt32 VideoFrameQueue::getData(VideoFrame* outputUnit)
	{
		if (outputUnit == NULL || outputUnit->getBuffer() == NULL)
			return BAD_VALUE;

		VideoFrame* currUsedBlock = dequeueUsedFrame();
		if (currUsedBlock == NULL)
			return NOT_ENOUGH_DATA;

		memcpy(outputUnit->getBuffer(), currUsedBlock->getBuffer(), currUsedBlock->getSize());
		outputUnit->setSize(currUsedBlock->getSize());
		outputUnit->setTimestamp(currUsedBlock->getTimestamp());
		outputUnit->setIndex(currUsedBlock->getIndex());
		outputUnit->setWidth(currUsedBlock->getWidth());
		outputUnit->setHeight(currUsedBlock->getHeight());
		outputUnit->setColorFormat(currUsedBlock->getColotFormat());

		enqueueUsedFrame();

		return OK;
	}

	SInt32 VideoFrameQueue::peekData(VideoFrame** outputUnit)
	{
		if (outputUnit == NULL)
			return BAD_VALUE;

		VideoFrame* currUsedBlock = dequeueUsedFrame();
		if (currUsedBlock == NULL)
			return NOT_ENOUGH_DATA;

		*outputUnit = currUsedBlock;

		return OK;
	}

	VideoFrame* VideoFrameQueue::dequeueEmptyFrame()
	{
		if (isUsedBlockFull())
			return NULL;

		return &mVideoArray[mWriteIndex];
	}

	VideoFrame* VideoFrameQueue::dequeueUsedFrame()
	{
		if (isUsedBlockEmpty())
			return NULL;

		return &mVideoArray[mReadIndex];
	}

	AudioFrameQueue::AudioFrameQueue(SInt32 trackIndex, SInt32 maxBlockCount)
		:BaseFrameQueue(trackIndex, maxBlockCount)
	{
		mAudioArray = new AudioFrame[mTotalCount];
	}

	AudioFrameQueue::~AudioFrameQueue()
	{
		for (SInt32 i = 0; i < mTotalCount; i++)
		{
			UInt8* data = mAudioArray[i].getBuffer();
			if (data)
			{
				delete[] data;
				data = NULL;
			}
		}
		if (mAudioArray)
		{
			delete[] mAudioArray;
			mAudioArray = NULL;
		}
	}

	SInt32 AudioFrameQueue::allocateMemory(int sizeEachBlock)
	{
		if (sizeEachBlock <= 0)
			return BAD_VALUE;
		mSizeEachBlock = sizeEachBlock;
		for (int i = 0; i < mTotalCount; i++)
		{
			UInt8* data = new UInt8[mSizeEachBlock];
			mAudioArray[i].setBuffer(data);
		}

		clearData();
		return OK;
	}

	SInt32 AudioFrameQueue::releaseInputFrame(AudioFrame* inputUnit)
	{
		AudioFrame* currEmptyBlock = dequeueEmptyFrame();
		if (currEmptyBlock == inputUnit)
		{
			// put current block to used.
			enqueueEmptyFrame();
			return OK;
		}
		else
			return DEAD_OBJECT;
	}

	SInt32 AudioFrameQueue::releaseOutputFrame(AudioFrame* outputUnit)
	{
		AudioFrame* currentUsedBlock = dequeueUsedFrame();
		if (outputUnit == currentUsedBlock)
		{
			// put current block to empty.
			enqueueUsedFrame();
			return OK;
		}
		else
			return DEAD_OBJECT;
	}

	AudioFrame* AudioFrameQueue::getInputFrame()
	{
		return dequeueEmptyFrame();
	}

	AudioFrame* AudioFrameQueue::getOutputFrame()
	{
		return dequeueUsedFrame();
	}

	SInt32 AudioFrameQueue::putData(AudioFrame* inputUnit)
	{
		if (inputUnit == NULL || inputUnit->getBuffer() == NULL)
			return BAD_VALUE;

		// byte array boundary check.
		if (inputUnit->getSize() > mSizeEachBlock)
			return NO_MEMORY;

		AudioFrame* currEmptyBlock = dequeueEmptyFrame();
		if (currEmptyBlock == NULL)
			return NOT_ENOUGH_DATA;

		memcpy(currEmptyBlock->getBuffer(), inputUnit->getBuffer(), inputUnit->getSize());
		currEmptyBlock->setSize(inputUnit->getSize());
		currEmptyBlock->setTimestamp(inputUnit->getTimestamp());
		currEmptyBlock->setIndex(inputUnit->getIndex());
		currEmptyBlock->setBitRate(inputUnit->getBitRate());
		currEmptyBlock->setChannels(inputUnit->getChannels());
		currEmptyBlock->setSampleRate(inputUnit->getSampleRate());

		enqueueEmptyFrame();

		return OK;
	}

	SInt32 AudioFrameQueue::getData(AudioFrame* outputUnit)
	{
		if (outputUnit == NULL || outputUnit->getBuffer() == NULL)
			return BAD_VALUE;

		AudioFrame* currUsedBlock = dequeueUsedFrame();
		if (currUsedBlock == NULL)
			return NOT_ENOUGH_DATA;

		memcpy(outputUnit->getBuffer(), currUsedBlock->getBuffer(), currUsedBlock->getSize());
		outputUnit->setSize(currUsedBlock->getSize());
		outputUnit->setTimestamp(currUsedBlock->getTimestamp());
		outputUnit->setIndex(currUsedBlock->getIndex());
		outputUnit->setBitRate(currUsedBlock->getBitRate());
		outputUnit->setChannels(currUsedBlock->getChannels());
		outputUnit->setSampleRate(currUsedBlock->getSampleRate());

		enqueueUsedFrame();

		return OK;
	}

	SInt32 AudioFrameQueue::peekData(AudioFrame** outputUnit)
	{
		if (outputUnit == NULL)
			return BAD_VALUE;

		AudioFrame* currUsedBlock = dequeueUsedFrame();
		if (currUsedBlock == NULL)
			return NOT_ENOUGH_DATA;

		*outputUnit = currUsedBlock;

		return OK;
	}

	AudioFrame* AudioFrameQueue::dequeueEmptyFrame()
	{
		if (isUsedBlockFull())
			return NULL;

		return &mAudioArray[mWriteIndex];
	}

	AudioFrame* AudioFrameQueue::dequeueUsedFrame()
	{
		if (isUsedBlockEmpty())
			return NULL;

		return &mAudioArray[mReadIndex];
	}

};