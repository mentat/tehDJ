/*
 * tehDJ - A realtime mp3 dj-ing system
 * (C) 2002 - Jesse Lovelace
*/
#include "wx/wx.h"

#include "portaudio.h"
#include "CircQueue.h"

#include "djOutput.h"
#include "djVisual.h"
#include "inlines.h"


int paAudioCallBack(void *inputBuffer, void *outputBuffer, 
        unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
{
    PortAudioOutput * obj = (PortAudioOutput *)userData;
    unsigned long   i, 
                    index = 0, 
                    level = 0;
    short *         out = (short*)outputBuffer;

    // mutex held
    if(!obj->mQueue->pop(obj->mTmpBuffer, level))
    {
        wxLogDebug("Queue timeout."); //wxThread::Yield();
        level = 0;
    }
    // end mutex held

    // do visual
    

    unsigned char * data = obj->mTmpBuffer;
    int             finished = 0;

    (void) outTime; /* Prevent unused variable warnings. */
    (void) inputBuffer;

    if ((level/4) < framesPerBuffer) // if the buffer is not totally filled, fill rest with 0
    {
        for( i=0; i<(level/4); i++ )
        {
            *out = data[index++];
            *out++ = (data[index++] << 8);  /* left */

            *out = data[index++];
            *out++ = (data[index++] << 8);  /* right */
        }
        for( i = (level/4); i < framesPerBuffer; i++)
        {
            *out++ = 0;
            *out++ = 0;
        }

        finished = 1; // mark stream as finished
    }
    else
        for( i=0; i<framesPerBuffer; i++ )
        {
            *out = data[index++];
            *out++ = (data[index++] << 8);  /* left */

            *out = data[index++];
            *out++ = (data[index++] << 8);  /* right */
        }

    return finished;


}
PortAudioOutput::PortAudioOutput(CoDec * hModule, Equalizer * hEqualizer, int framesPerBuffer, int buffers)
: Output(hModule, hEqualizer), mQueue(NULL), mTmpBuffer(NULL)
{
    wxMutexLocker lock(mStreamLock);
    mFrames = framesPerBuffer;
    mBuffers = buffers;

    // initialize PortAudio
    PaError err;
    const    PaDeviceInfo *pdi;
    int numDevices = 0;
    
    err = Pa_Initialize();
    if( err != paNoError )
    {
        wxLogError(wxT("PortAudio error: %s\n"), Pa_GetErrorText( err ) );
        return;
    }

    // get device info

    numDevices = Pa_CountDevices();
    printf("Number of devices = %d\n", numDevices );
    for( int i=0; i<numDevices; i++ )
    {
        DeviceInfo it;
        mDevices.push_back(it);
        DeviceInfo &info = mDevices[i];

        pdi = Pa_GetDeviceInfo( i );
        
        if( i == Pa_GetDefaultInputDeviceID() )
            info.isDefaultIn = true;
        else
            info.isDefaultIn = false;

        if( i == Pa_GetDefaultOutputDeviceID() )
            info.isDefaultOut = true;
        else
            info.isDefaultOut = false;

        info.name = pdi->name;
        info.inch = pdi->maxInputChannels;
        info.outch = pdi->maxOutputChannels;

        if( pdi->numSampleRates == -1 )
        {
            info.sampleRange = true;
            info.sampleRates.push_back(pdi->sampleRates[0]);
            info.sampleRates.push_back(pdi->sampleRates[1]);
        }
        else
        {
            info.sampleRange = false;
            for( int j=0; j <pdi->numSampleRates; j++ )
            {
                info.sampleRates.push_back(pdi->sampleRates[j] );
            }
        }

        if( pdi->nativeSampleFormats & paInt8 )        
            info.sampleFormats.push_back(paInt8);
        if( pdi->nativeSampleFormats & paUInt8 )       
            info.sampleFormats.push_back(paUInt8);
        if( pdi->nativeSampleFormats & paInt16 )       
            info.sampleFormats.push_back(paInt16);
        if( pdi->nativeSampleFormats & paInt32 )       
            info.sampleFormats.push_back(paInt32);
        if( pdi->nativeSampleFormats & paFloat32 )     
            info.sampleFormats.push_back(paFloat32);
        if( pdi->nativeSampleFormats & paInt24 )       
            info.sampleFormats.push_back(paInt24);
        if( pdi->nativeSampleFormats & paPackedInt24 ) 
            info.sampleFormats.push_back(paPackedInt24);
    }

    // setup initial buffers

    mQueue = new CircQueue<unsigned char>(framesPerBuffer * 4, buffers);
    mTmpBuffer = new unsigned char[framesPerBuffer * 4];

    mIsPlaying = false;
    mIsPaused = false;
    

    // setup default device for playback
    mCurrentID = Pa_GetDefaultOutputDeviceID();
    pdi = Pa_GetDeviceInfo( mCurrentID );
    
    mCurrentDevice = pdi->name;
    mCurrentFormat = paInt16;
    mCurrentRate = 0;
    
    if (pdi->numSampleRates == -1)
    {
        if ((pdi->sampleRates[0] <= 44100) && (pdi->sampleRates[1] >= 44100))
            mCurrentRate = 44100;
        else
        {
            mCurrentRate = pdi->sampleRates[0];
            wxLogDebug("Strange sample rate, %d", mCurrentRate);
        }
    }
    else
    {
        for (int w = 0; w < pdi->numSampleRates; i++)
        {
            if (pdi->sampleRates[w] == 44100)
            {
                mCurrentRate = 44100;
            }
        }
        if (mCurrentRate != 44100)
            if (pdi->numSampleRates <= 0)
            {
                wxLogError("No sample rates found!");
            }
            else
                mCurrentRate = pdi->sampleRates[0];
    }

}

PortAudioOutput::~PortAudioOutput()
{
    wxMutexLocker lock(mStreamLock);
    if (mIsPlaying)
        Pa_AbortStream( mPortStream );
    Pa_Terminate();

    delete mQueue;
    delete [] mTmpBuffer;

}

   
void PortAudioOutput::RefillBuffers()
{

}

void PortAudioOutput::SetPause()
{
    wxMutexLocker lock(mStreamLock);
    PaError err;

    if (!mIsPaused)
    {
        err = Pa_StopStream( mPortStream );
        mIsPaused = true;
    }
    else
    {
        err = Pa_StartStream( mPortStream );
        mIsPaused = false;
    }

    if( err != paNoError ) 
    {
        wxLogError(wxT("There was a PortAudio Error on pause\n"));
    }

}

bool PortAudioOutput::Play(bool stereo)
{
    wxMutexLocker lock(mStreamLock);
    if (mIsPlaying)
    {
        wxLogDebug("We are already playing!");
        return true;
    }
    //mQueue->Flush();


    // todo: fix case were soundcard is only mono
    int channels;
    if (stereo)
        channels = 2;
    else
        channels = 1;
    
    PaError err;
    err = Pa_OpenStream(
              &mPortStream,
              paNoDevice,/* default input device */
              0,              /* no input */
              mCurrentFormat,  /* 16 bit signed int point input */
              NULL,
              mCurrentID, /* default output device */
              channels,          /* stereo output */
              mCurrentFormat,      /* 16 bit signed int point output */
              NULL,
              mCurrentRate,
              mFrames,
              0,              /* number of buffers, if zero then use default minimum */
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              paAudioCallBack,
              this );
    
    if( err != paNoError )
    {
        wxLogError(wxT("There was a PortAudio Error on open\n"));
        wxLogError(wxT("PortAudio error: %s\n"), Pa_GetErrorText( err ) );
        Pa_CloseStream( mPortStream );
        return false;
    }

    err = Pa_StartStream( mPortStream );
    if( err != paNoError )
    {
        wxLogError(wxT("There was a PortAudio Error on start\n"));
        wxLogError(wxT("PortAudio error: %s\n"), Pa_GetErrorText( err ) );
        Pa_CloseStream( mPortStream );
        return false;
    }
    mIsPlaying = true;
    return true;

}

bool PortAudioOutput::Stop()
{
    wxMutexLocker lock(mStreamLock);
    if (mIsPlaying)
    {
        PaError err;
        Pa_StopStream( mPortStream );
        err = Pa_CloseStream( mPortStream );
        mIsPlaying = false;
        return true;
    }
    else
    {
        wxLogDebug("We are already stopped!");
        return true;
    }

}


bool PortAudioOutput::IsOutputComplete()
{
    return false;

}
void PortAudioOutput::Flush()
{
    wxMutexLocker lock(mStreamLock);
    Stop();
    mQueue->Flush();
}

void PortAudioOutput::BindToFile(const char* pcFile)
{
}