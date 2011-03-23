// --*-c++-*--
/*
    $Id: NChat.cpp,v 1.4 2002/07/04 22:45:44 mentat Exp $
 
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

#ifndef DJ_AUDIO_THREAD_H
#define DJ_AUDIO_THREAD_H
#define TABLE_SIZE 200

#include "portaudio.h"

class AudioThread : public wxThread
{
public:

    enum Status { Exit, Error, Init, NoData, doPlay, Playing, doPause, Paused };

    AudioThread(MainFrame *frame, unsigned long sampleRate, bool stereo,unsigned long framesPerBuffer);
    ~AudioThread();
    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

    void Play() { if ((mStatus != Error) && (mStatus != Playing)) mStatus = doPlay; }
    void Pause() { if ((mStatus != Error) && (mStatus != Paused)) mStatus = doPause; }

    void WriteText(const wxString& text);

    static int CallbackWrapper(void *inputBuffer, void *outputBuffer, 
        unsigned long framesPerBuffer, PaTimestamp outTime, void *userData);

    int PortAudioCallBack(void *inputBuffer, void *outputBuffer, 
        unsigned long framesPerBuffer, PaTimestamp outTime, void *userData);
   
private:

    struct paTestData
    {
        float sine[TABLE_SIZE];
        int left_phase;
        int right_phase;
    };

    short * mData;

    paTestData data;

    PortAudioStream *mPortStream;

    Status mStatus;

    unsigned long mFramesPerBuffer;
    unsigned long mRate;
    bool mStereo;
    
    // frame callback
    MainFrame *m_frame;

};
#endif
