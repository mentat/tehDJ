/*
 * tehDJ - A realtime mp3 dj-ing system
 * (C) 2002 - Jesse Lovelace
*/
#include "wx/wx.h"
#include "wx/thread.h"

#include "djCodec.h"
#include "djOutput.h"
#include "djThread.h"
#include "djGui.h"
#include "djVisual.h"
#include "../tehDJ_wdr.h"

ProducerThread::ProducerThread(PortAudioOutput * out, CoDec * codec, MainFrame * frame)
: wxThread(), mOut(out), mCodec(codec), m_frame(frame), mSeek(false)
{
    mSize = out->mFrames * 4;
    mPcm = new unsigned char[mSize];
}

ProducerThread::~ProducerThread()
{
    delete [] mPcm;

}

void ProducerThread::WriteText(const wxString& text)
{
    wxString msg;

    // before doing any GUI calls we must ensure that this thread is the only
    // one doing it!

    wxMutexGuiEnter();

    msg << text;
    m_frame->WriteText(msg);

    wxMutexGuiLeave();

}

void ProducerThread::SeekTo(int numer, int denom)
{
    mNewNumer = numer;
    mNewDenom = denom;
    mSeek = true;

}

void *ProducerThread::Entry()
{
    WriteText("Producer started...");
    wxString info;

    unsigned long actual;

    while(true)
    {
        if (TestDestroy())
            break;

        if (mSeek)
        {
            WriteText("seeking");
            mCodec->Seek(mNewNumer, mNewDenom);
            mSeek = false;
        }

        if (mCodec->Ok())
        {
            actual = mSize;

            if (mCodec->GetPCMBlock(mPcm, &actual)) // get block from decoder
            {
                if (m_frame->m_visual)
                    m_frame->m_visual->Eat((short *)mPcm, actual, 16, 2);
                 // wait on queue
                while (!mOut->mQueue->push(mPcm, actual))
                {
                    if (TestDestroy())
                        return NULL;

                }
            }
            else
            {
                wxLogDebug("Songdone.");
                if (TestDestroy())
                    break;

                wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, ID_STOP);
                wxPostEvent( m_frame, event );

            }
        }
        else
            wxThread::Sleep(400);
    }

    
    return NULL;

}

void ProducerThread::OnExit()
{

}


AudioThread::AudioThread(PortAudioOutput * out, MainFrame * frame)
: wxThread(), mOut(out), m_frame(frame)
{
    mRequest = NO_REQ;
}

void *AudioThread::Entry()
{
    WriteText("Audio started...");
    while(true)
    {
        if (TestDestroy())
            break;

        switch (mRequest)
        {
        case NO_REQ: break;
        case STOP: if (mState != STOP) 
                   { mOut->Stop(); mState = STOP; } mRequest = NO_REQ; break;
        case PLAY: if (mState != PLAY) 
                   { mOut->Play(true); mState = PLAY; } mRequest = NO_REQ; break;
        case PAUSE: if (mState == PAUSE) {mOut->SetPause(); mState = PLAY; }
                    else {mOut->SetPause(); mState = PAUSE;} mRequest = NO_REQ; break;
        default: wxLogDebug("Onknown state in AudioThread.");
        }
        wxThread::Sleep(300);
    }

    return NULL;
}

void AudioThread::WriteText(const wxString& text)
{
    wxString msg;

    wxMutexGuiEnter();

    msg << text;
    m_frame->WriteText(msg);

    wxMutexGuiLeave();

}

void AudioThread::OnExit()
{

}
