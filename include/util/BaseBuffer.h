#ifndef BASE_BUFFER_H_
#define BASE_BUFFER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <BaseDataTypes.h>
#include "Errors.h"

#include <crtdbg.h>

#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace MediaPlugin {

	class BaseBuffer
	{
	public:
		BaseBuffer()
			: mData(NULL), mLength(0), mTimestamp(0), mFlags(0), mDts(0)
		{

		}

		BaseBuffer(const UInt8* buffer, SInt32 size, SInt64 timestamp, SInt32 flags, SInt64 dts = 0)
		{
			mData = (UInt8*)buffer;
			mLength = size;
			mTimestamp = timestamp;
			mFlags = flags;
			mDts = dts;
		}

		virtual ~BaseBuffer() {};

	public:
		SInt32 setData(const UInt8* data)
		{
			mData = (UInt8*)data;
			return OK;
		}

		SInt32 setLength(const SInt32 length)
		{
			mLength = length;
			return OK;
		}

		SInt32 setTimestamp(const SInt64 timestamp)
		{
			mTimestamp = timestamp;
			return OK;
		}

		SInt32 setFlags(const SInt32 flags)
		{
			mFlags = flags;
			return OK;
		}

		SInt32 setDts(const SInt64 dts)
		{
			mDts = dts;
			return OK;
		}

		UInt8* getData()
		{
			return mData;
		}

		SInt32 getLength()
		{
			return mLength;
		}

		SInt64 getTimestamp()
		{
			return mTimestamp;
		}

		SInt32 getFlags()
		{
			return mFlags;
		}

		SInt64 getDts()
		{
			return mDts;
		}

	private:
		// disable some class feature.
		BaseBuffer& operator=(BaseBuffer& inputBuffer) {}
		BaseBuffer(const BaseBuffer &) {}

	private:
		UInt8* mData;
		SInt32 mLength;
		SInt64 mTimestamp;
		SInt32 mFlags;
		SInt64 mDts;
	};

	class BaseBufferQueue
	{
	public:
		BaseBufferQueue(SInt32 trackIndex, SInt32 maxBlockCount)
			: mTotalCount(maxBlockCount),
			mWriteIndex(0),
			mReadIndex(0),
			mUsedCount(0),
			mSizeEachBlock(0),
			mTrackIndex(trackIndex),
			mBufferArray(NULL)
		{
			mBufferArray = new BaseBuffer[mTotalCount];
		}

		virtual ~BaseBufferQueue()
		{
			for (SInt32 i = 0; i < mTotalCount; i++)
			{
				UInt8* data = mBufferArray[i].getData();
				if (data)
				{
					delete[] data;
					data = NULL;
				}
			}
			if (mBufferArray)
			{
				delete[] mBufferArray;
				mBufferArray = NULL;
			}
		}

	public:
		SInt32 allocateMemory(int sizeEachBlock)
		{
			if (sizeEachBlock <= 0)
				return BAD_VALUE;
			mSizeEachBlock = sizeEachBlock;
			for (int i = 0; i < mTotalCount; i++)
			{
				UInt8* data = new UInt8[mSizeEachBlock];
				mBufferArray[i].setData(data);
			}

			clearData();
			return OK;
		}

		//++rayman(20151104): 
		// getInputBuffer: is an optimization of avoid copying memory from external
		// buffer unit.
		// this routine will get an internal buffer queue empty block reference.
		// return: the current empty block reference from buffer queue.
		//
		BaseBuffer* getInputBuffer()
		{
			return dequeueEmptyBuffer();
		}

		//
		// releaseInputBuffer: should be invoked correspond to getInutBuffer.
		// @param inputUnit: is the reference of internal empty buffer queue.
		//
		SInt32 releaseInputBuffer(BaseBuffer* inputUnit)
		{
			BaseBuffer* currEmptyBlock = dequeueEmptyBuffer();
			if (currEmptyBlock == inputUnit)
			{
				// put current block to used.
				enqueueEmptyBuffer();
				return OK;
			}
			else
				return DEAD_OBJECT;
		}
		//--rayman(20151104)

		SInt32 putData(BaseBuffer* inputUnit)
		{
			if (inputUnit == NULL || inputUnit->getData() == NULL)
				return BAD_VALUE;

			// byte array boundary check.
			if (inputUnit->getLength() > mSizeEachBlock)
				return NO_MEMORY;

			BaseBuffer* currEmptyBlock = dequeueEmptyBuffer();
			if (currEmptyBlock == NULL)
				return NOT_ENOUGH_DATA;

			memcpy(currEmptyBlock->getData(), inputUnit->getData(), inputUnit->getLength());
			currEmptyBlock->setLength(inputUnit->getLength());
			currEmptyBlock->setTimestamp(inputUnit->getTimestamp());
			currEmptyBlock->setFlags(inputUnit->getFlags());
			currEmptyBlock->setDts(inputUnit->getDts());

			//        if (inputUnit->getFlags() & 0x1)
			//        {
			//             printf("data(0x%x) length(%d) pts(%lld), write idx(%d)\n", 
			//                 inputUnit->getData(), inputUnit->getLength(), inputUnit->getTimestamp(), mWriteIndex);
			//        }
			// put current block to used.
			enqueueEmptyBuffer();

			return OK;
		}

		//++rayman(20151104): 
		// getOutputBuffer: is an optimization of avoid copying memory from 
		// internal buffer unit.
		// this routine will get an internal buffer queue used block reference.
		// return: the current used block reference from buffer queue.
		//
		BaseBuffer* getOutputBuffer()
		{
			return dequeueUsedBuffer();
		}

		//
		// releaseOutputBuffer: should be invoked correspond to getOutputBuffer.
		// @param outputUnit: is the reference of internal used buffer queue.
		//
		SInt32 releaseOutputBuffer(BaseBuffer* outputUnit)
		{
			BaseBuffer* currentUsedBlock = dequeueUsedBuffer();
			if (outputUnit == currentUsedBlock)
			{
				// put current block to empty.
				enqueueUsedBuffer();
				return OK;
			}
			else
				return DEAD_OBJECT;
		}
		//--rayman(20151104)

		SInt32 getData(BaseBuffer* outputUnit)
		{
			if (outputUnit == NULL || outputUnit->getData() == NULL)
				return BAD_VALUE;

			BaseBuffer* currUsedBlock = dequeueUsedBuffer();
			if (currUsedBlock == NULL)
				return NOT_ENOUGH_DATA;

			memcpy(outputUnit->getData(), currUsedBlock->getData(), currUsedBlock->getLength());
			outputUnit->setLength(currUsedBlock->getLength());
			outputUnit->setTimestamp(currUsedBlock->getTimestamp());
			outputUnit->setFlags(currUsedBlock->getFlags());
			outputUnit->setDts(currUsedBlock->getDts());

			//        if (currUsedBlock->getFlags() & 0x1)
			//        {
			//             printf("data(0x%x) length(%d) pts(%lld), read idx(%d)\n", 
			//                 currUsedBlock->getData(), currUsedBlock->getLength(), currUsedBlock->getTimestamp(), mReadIndex);
			//        }
			// put current block to empty.
			enqueueUsedBuffer();

			return OK;
		}

		SInt32 peekData(BaseBuffer** outputUnit)
		{
			if (outputUnit == NULL)
				return BAD_VALUE;

			BaseBuffer* currUsedBlock = dequeueUsedBuffer();
			if (currUsedBlock == NULL)
				return NOT_ENOUGH_DATA;

			*outputUnit = currUsedBlock;

			return OK;
		}

		void clearData()
		{
			mWriteIndex = 0;
			mReadIndex = 0;
			mUsedCount = 0;
		}

		SInt32 getTotolCount()
		{
			return mTotalCount;
		}

		SInt32 getEmptyCount()
		{
			return mTotalCount - mUsedCount;
		}

	public:
		bool isUsedBlockEmpty()
		{
			if (mUsedCount == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool isUsedBlockFull()
		{
			if (mUsedCount == mTotalCount)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	private:

		// get an empty buffer from queue.
		BaseBuffer* dequeueEmptyBuffer()
		{
			if (isUsedBlockFull())
			{
				return NULL;
			}

			//printf("ptr = (%d)\n", &mBufferArray[mWriteIndex]);
			return &mBufferArray[mWriteIndex];
		}

		// put current empty buffer to queue (set it to used buffer).
		void enqueueEmptyBuffer()
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

		// get an used buffer from queue.
		BaseBuffer* dequeueUsedBuffer()
		{
			if (isUsedBlockEmpty())
			{
				return NULL;
			}
			return &mBufferArray[mReadIndex];
		}

		// put current used buffer to queue (set it to empty buffer).
		void enqueueUsedBuffer()
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

	private:
		BaseBuffer* mBufferArray;
		SInt32 mSizeEachBlock;
		SInt32 mTotalCount;
		SInt32 mWriteIndex;
		SInt32 mReadIndex;
		SInt32 mUsedCount;
		SInt32 mTrackIndex;

	};

}

#endif //BASE_BUFFER_H_