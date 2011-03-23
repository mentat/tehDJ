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

#include "wx/wx.h"
#include "wx/thread.h"

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#include "math.h"
#include "portaudio.h"

#include "djGui.h"
#include "dj.h"
#include "AudioThread.h"

DECLARE_APP(tehDJ)

AudioThread::AudioThread(MainFrame *frame, 
                         unsigned long sampleRate, bool stereo,unsigned long framesPerBuffer)
                         : wxThread()
{
    m_frame = frame;

    mStatus = doPlay;

    mFramesPerBuffer = framesPerBuffer;
    mRate = sampleRate;
    mStereo = stereo;

    
    int i;
    /* initialise sinusoidal wavetable */
    for( i=0; i<TABLE_SIZE; i++ )
    {
        data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
    }
    data.left_phase = data.right_phase = 0;

    mData = new short[framesPerBuffer * 2];

}

AudioThread::~AudioThread()
{
    delete [] mData;
}
    
    // thread execution starts here
void *AudioThread::Entry()
{
    PaError err;
    err = Pa_OpenStream(
              &mPortStream,
              paNoDevice,/* default input device */
              0,              /* no input */
              paInt16,  /* 32 bit floating point input */
              NULL,
              Pa_GetDefaultOutputDeviceID(), /* default output device */
              2,          /* stereo output */
              paInt16,      /* 32 bit floating point output */
              NULL,
              mRate,
              2,
              0,              /* number of buffers, if zero then use default minimum */
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              AudioThread::CallbackWrapper,
              this );
    
    if( err != paNoError )
    {
        WriteText(wxT("There was a PortAudio Error on start\n"));
        Pa_CloseStream( mPortStream );
        return NULL;
    }

    while(true)
    {
        if ( TestDestroy() )
        {
            WriteText("Audio Thread Destroyed\n");
            break;
        }

        if (mStatus == doPlay)
        {
            PaError err; 
            err = Pa_StartStream( mPortStream );
            if( err != paNoError )
            {
                WriteText(wxT("There was a PortAudio Error on start\n"));
                break;
            }
            else
                mStatus = Playing;
        }
        if (mStatus == doPause)
        {
            PaError err;
            err = Pa_StopStream( mPortStream );
            if( err != paNoError ) 
            {
                WriteText(wxT("There was a PortAudio Error on stop\n"));
                break;
            }
            else
                mStatus = Paused;

        }

        wxThread::Sleep(500);
    }
    Pa_AbortStream( mPortStream );
    Pa_CloseStream( mPortStream );

    return NULL;

}

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
void AudioThread::OnExit()
{
#if 0
    WriteText("OnExit Audio Thread\n");
    wxCriticalSectionLocker locker(wxGetApp().m_critsect);

    wxArrayThread& threads = wxGetApp().m_threads;
    threads.Remove(this);

    if ( threads.IsEmpty() )
    {
        // signal the main thread that there are no more threads left if it is
        // waiting for us
        if ( wxGetApp().m_waitingUntilAllDone )
        {
            wxGetApp().m_waitingUntilAllDone = FALSE;

            wxMutexLocker lock(wxGetApp().m_mutexAllDone);
            wxGetApp().m_condAllDone.Signal();
        }
    }
#endif

}

void AudioThread::WriteText(const wxString& text)
{
    wxString msg;

    // before doing any GUI calls we must ensure that this thread is the only
    // one doing it!

    wxMutexGuiEnter();

    msg << text;
    m_frame->WriteText(msg);

    wxMutexGuiLeave();

}

int AudioThread::CallbackWrapper(void *inputBuffer, void *outputBuffer, 
        unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
{
    AudioThread * mySelf = (AudioThread *)userData;
    return mySelf->PortAudioCallBack(inputBuffer, outputBuffer, framesPerBuffer, outTime, userData);

}

int AudioThread::PortAudioCallBack(void *inputBuffer, void *outputBuffer, 
        unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
{

    
    m_frame->mQueue->pop(mData);

    short *out = (short*)outputBuffer;
    unsigned long i, index = 0;
    int finished = 0;
    (void) outTime; /* Prevent unused variable warnings. */
    (void) inputBuffer;
    for( i=0; i<framesPerBuffer; i++ )
    {
        //*out = data[index++];
        *out++ = mData[index++];  /* left */

        //*out = data[index++];
        *out++ = mData[index++];  /* right */
    }
    return finished;


}
