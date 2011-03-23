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
#include "wx/config.h"

#include "dj.h"
#include "djCodec.h"
#include "djOutput.h"
#include "djThread.h"
#include "djGui.h"
#include "djConfig.h"
#include "glbutton.h"
#include "portaudio.h"

IMPLEMENT_APP(tehDJ)


tehDJ::tehDJ()
{

}

tehDJ::~tehDJ()
{
    // the mutex must be unlocked before being destroyed
    //m_mutexAllDone.Unlock();
}

bool tehDJ::OnInit()
{
    bool runsetup = false;
    wxConfig *config = new wxConfig("tehDJ");

    if (!config->HasEntry("Device"))
    {
        // first time, run setup??
        runsetup = true;
        mConf.loaded = false;
    }
    else
    {
        mConf.audio.buffers = config->Read("OutputBuffers", 04);
        mConf.audio.bufferSize = config->Read("BufferSize", 01204);
        mConf.audio.device = config->Read("Device", (long)0 );
        mConf.audio.sampleFormat = config->Read("SampleFormat", 16);
        mConf.audio.sampleRate = config->Read("SampleRate", 44100);
        mConf.audio.channels = config->Read("Channels", 2);
        mConf.decoder.thread_priority = config->Read("DecoderPriority", (long)0);
        mConf.decoder.engine = config->Read("DecoderEngine", "MAD");
        mConf.decoder.engine_id = config->Read("DecoderEngineId", (long)0);
        mConf.decoder.dither = config->Read("DitherType", (long)0);


        mConf.loaded = true;
    }

    delete config;

    // create the main application window
    MainFrame *frame = new MainFrame(runsetup, _T("teh DJ"),
                                 wxPoint(50, 50));

    return TRUE;
}

int tehDJ::OnExit()
{

    wxConfig *config = new wxConfig("tehDJ");

    if (mConf.loaded == true)
    {
        config->Write("OutputBuffers", mConf.audio.buffers);
        config->Write("BufferSize", mConf.audio.bufferSize);
        config->Write("Device", mConf.audio.device);
        config->Write("SampleFormat", mConf.audio.sampleFormat);
        config->Write("SampleRate", mConf.audio.sampleRate);
        config->Write("Channels", mConf.audio.channels);
        config->Write("DecoderPriority", mConf.decoder.thread_priority);
        config->Write("DecoderEngine", mConf.decoder.engine);
        config->Write("DecoderEngineId", mConf.decoder.engine_id);
        config->Write("DitherType", mConf.decoder.dither);
    }
    
    delete config;

    return 0;
}