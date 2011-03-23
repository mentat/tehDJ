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
#ifndef DJ_THREAD_H
#define DJ_THREAD_H

class PortAudioOutput;
class CoDecModule;
class MainFrame;

#define PLAY    700
#define STOP    701
#define PAUSE   702
#define AUDIO_ERROR   704
#define NO_REQ  705

// commands

class ProducerThread: public wxThread
{
public:
    ProducerThread(PortAudioOutput * out, CoDec * codec, MainFrame * frame);
    ~ProducerThread();

    virtual void *Entry();
    void WriteText(const wxString& text);

    virtual void OnExit();

    void SeekTo(int numer, int denom);

    bool stopped;

private:
  
    // seeking
    bool mSeek;
    int mNewNumer;
    int mNewDenom;
    // end seeking

    // real buffer level
    unsigned long mLevel;

    // frame callback
    MainFrame * m_frame;

    // work buffer
    unsigned long mSize;
    unsigned char * mPcm;
    
    // pointer to modules
    PortAudioOutput * mOut;
    CoDec * mCodec;

};

class AudioThread: public wxThread
{
public:
    AudioThread(PortAudioOutput * out, MainFrame * frame);

    virtual void *Entry();
    void WriteText(const wxString& text);

    virtual void OnExit();
    //enum states { PLAY = 0, STOP, PAUSE, ERROR };

    void DoPause() { mRequest = PAUSE; }
    void DoPlay() { mRequest = PLAY; }
    void DoStop() { mRequest = STOP; }


private:

    int mRequest;
    int mState;
    MainFrame * m_frame;
    PortAudioOutput * mOut;
    //CoDecModule * mCodec;

};


#endif
