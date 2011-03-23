// --*-c++-*--
/*
    tehDJ - Mp3 DJ System
    Copyright (C) 2002  Jesse Lovelace
 
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
#ifndef DJ_OUTPUT_H
#define DJ_OUTPUT_H

#include <vector>
#include "djCodec.h"
#include "CircQueue.h"
#include "djThread.h"
#include "portaudio.h"

using namespace std;

class Equalizer;
class Output
{
public:
    Output(CoDec * hModule, Equalizer * hEqualizer): m_pCoDec(hModule), m_pEqualizer(hEqualizer){}

	virtual void RefillBuffers() = 0;
	virtual void SetPause() = 0;
    virtual bool Play(bool stereo) = 0;
    virtual bool Stop() = 0;
	virtual bool IsOutputComplete() = 0;
	virtual void Flush() = 0;
	virtual void BindToFile(const char* pcFile) = 0;
    
protected:
   
	CoDec* m_pCoDec;
	Equalizer * m_pEqualizer;

};

class PortAudioOutput: public Output
{
public:
    PortAudioOutput(CoDec * hModule, Equalizer * hEqualizer, int framesPerBuffer, int buffers);
    ~PortAudioOutput();
    
    void RefillBuffers();
	void SetPause();
	bool IsOutputComplete();
	void Flush();
	void BindToFile(const char* pcFile);
    bool Play(bool stereo);
    bool Stop();

    unsigned char * mTmpBuffer;
    CircQueue<unsigned char> * mQueue;
    int mFrames;
    int mBuffers;

    friend class AudioThread;

    struct DeviceInfo
    {
        string name;
        vector<unsigned int> sampleRates;
        vector<unsigned long> sampleFormats;
        int id; // pa deveice ID
        int inch; // in channels
        int outch; // out channels
        bool sampleRange; // if true any range some sampleRates[0] to sampleRates[1]
        bool isDefaultIn;   // true if this device is default device
        bool isDefaultOut;
    };

    vector<DeviceInfo> mDevices;

    string mCurrentDevice;
    unsigned long mCurrentFormat;
    unsigned long mCurrentRate;
    int mCurrentID;

private:

    bool mIsPaused;
    bool mIsPlaying;

    wxMutex mStreamLock;
    PortAudioStream *mPortStream;

};

#endif
