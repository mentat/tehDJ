// --*-c++-*--
/*
 
    Circular Mutex Held Queue
    Copyright (C) 2002  Jesse Lovelace <jllovela@eos.ncsu.edu>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef CIRC_QUEUE_H
#define CIRC_QUEUE_H

#include "wx/thread.h"

template <class T>
class CircQueue
{
public:

    CircQueue(unsigned short size, unsigned short buckets) 
        :mSize(size), mBuckets(buckets), mMutex(), mCondFull(mMutex), mCondEmpty(mMutex)
    {
        mBuffer = new T*[buckets];
        for (unsigned short i = 0; i < buckets; i++)
            mBuffer[i] = new T[size];
        mTail=mHead=mLength = 0;
        mLevels = new unsigned long[buckets];
    }

    ~CircQueue()
    {
        for (unsigned short i = 0; i < mBuckets; i++)
            delete [] mBuffer[i];
        delete [] mBuffer;
        delete [] mLevels;
    }

    float level()
    {
        return float(mLength)/float(mBuckets);
    }

    bool push(const T * in, unsigned long level)
    {
        wxMutexLocker locker(mMutex);
        while(isFull())
        {
            // wait on queue for one second then return false
            mCondFull.Wait(1);
            if (isFull())
                return false;
        }

        memcpy(mBuffer[mTail], in, mSize);
        mLevels[mTail] = level;

        mTail = (mTail + 1) % mBuckets;

        mLength++;
        if (mLength == 1)
            mCondEmpty.Broadcast();
        return true;
    }

    bool pop(T * data, unsigned long & level)
    {
        level = 0;
        wxMutexLocker locker(mMutex);
        while(isEmpty())
        {
            // wait on queue for 2 seconds then return false
            mCondEmpty.Wait(1);
            if (isEmpty())
              return false;
        }

        //T * tmp = new T[mSize];
        memcpy(data, mBuffer[mHead], mSize);
        level = mLevels[mHead];

        mHead = (mHead + 1) % mBuckets;

        mLength--;

        if (mLength == mBuckets - 1)
            mCondFull.Broadcast();

        return true;
    }

    void Flush()
    {
        wxMutexLocker locker(mMutex);
        mTail=mHead=mLength = 0;
    }


    wxMutex mMutex;
    wxCondition mCondFull, mCondEmpty;

private:

     bool isFull()
    {
        if (mLength == mBuckets)
            return true;
        return false;
    }

    bool isEmpty()
    {
        if (mLength == 0)
            return true;
        return false;
    }

    unsigned short mLength;
    const unsigned short mSize;
    const unsigned short mBuckets;
    T ** mBuffer;
    unsigned long * mLevels;

    unsigned short mHead;
    unsigned short mTail;

};

#endif
