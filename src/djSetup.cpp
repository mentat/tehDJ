/*
 * tehDJ - A realtime mp3 dj-ing system
 * (C) 2002 - Jesse Lovelace
*/
#include "wx/wx.h"
#include "wx/config.h"
#include "wx/spinctrl.h"
#include "wx/slider.h"
#include "wx/choice.h"
#include "wx/slider.h"
#include "wx/spinctrl.h"
#include "wx/radiobox.h"

#include <sstream>
#include "dj.h"
#include "../tehDJ_wdr.h"
#include "djSetup.h"
#include "portaudio.h"
#include "djOutput.h"

#include "bitmaps/deejay.xpm"

BEGIN_EVENT_TABLE(AudioPrefs, wxFrame)
    EVT_CLOSE(AudioPrefs::OnCloseWindow)
    EVT_COMMAND_SCROLL_ENDSCROLL(ID_PREF_AUDIO_BUFFERS, AudioPrefs::OnBufferScroll)
    EVT_TEXT(ID_PREF_AUDIO_BUFFERS_CTRL, AudioPrefs::OnBufferText)
    EVT_COMBOBOX(ID_PREF_AUDIO_DEVICE, AudioPrefs::OnChangeDevice)
    EVT_COMBOBOX(ID_PREF_AUDIO_RATE, AudioPrefs::OnChangeRate)
    EVT_TEXT(ID_PREF_AUDIO_RATE, AudioPrefs::OnChangeRate)
    EVT_SPINCTRL(ID_PREF_AUDIO_SAMPLES, AudioPrefs::OnChangeSamples)
END_EVENT_TABLE()

DECLARE_APP(tehDJ)


wxString DoITOA(long int i)
{
    stringstream s;
    s << i;
    return wxString(s.str().c_str());
}

AudioPrefs::AudioPrefs(PortAudioOutput * out, wxWindow * parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(parent, -1, title, pos, size, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION),
        mConf(wxGetApp().mConf)
{
    wxIcon appIcon(deejay_xpm);
    SetIcon(appIcon);  // Set the icon

    mOut = out;

    if (mOut->mDevices.size() == 0)
    {
        wxMessageBox("There are no audio devices found on this system.", "Audio Error", wxOK, this);
        Destroy();
        return;
    }

    wxPanel * panel = new wxPanel(this);   
    wxSizer * sizer = djPrefs(panel, true, true);
    sizer->SetSizeHints(this);
    SetSize(sizer->GetMinSize());

    mDevice = (wxComboBox *)FindWindow(ID_PREF_AUDIO_DEVICE);
    mRate = (wxComboBox *)FindWindow(ID_PREF_AUDIO_RATE);
    mBits = (wxComboBox *)FindWindow(ID_PREF_AUDIO_BITS);
    mChannels = (wxRadioBox *)FindWindow(ID_PREF_AUDIO_STEREO);
    mSamples = (wxSpinCtrl *)FindWindow(ID_PREF_AUDIO_SAMPLES);
    mBuffers = (wxTextCtrl *)FindWindow(ID_PREF_AUDIO_BUFFERS_CTRL);
    mBuffCtrl = (wxSlider *)FindWindow(ID_PREF_AUDIO_BUFFERS);
    mLatency = (wxTextCtrl *)FindWindow(ID_PREF_AUDIO_LATENCY);
    mDecoder = (wxChoice *)FindWindow(ID_PREF_MP3_ENGINE);
    mThreadPriority = (wxChoice *)FindWindow(ID_PREF_MP3_THREAD);
    mDither = (wxChoice *)FindWindow(ID_PREF_MP3_DITHER);
 
    mDevice->Clear();

    for (unsigned int i = 0; i < mOut->mDevices.size(); i++)
    {
        if (mOut->mDevices[i].outch > 0)
            mDevice->Append(mOut->mDevices[i].name.c_str());
    }


    if (mConf.loaded)
    {
        mDevice->SetSelection(mConf.audio.device);
        LoadDevice(mConf.audio.device);
    }
    else
    {
        mDevice->SetSelection(0);
        LoadDevice(0);
    }

    Show(true);
    MakeModal(true);

}
void AudioPrefs::OnCloseWindow(wxCloseEvent& event)
{
    //config & conf = wxGetApp().mConf;
    SaveSettings();
    MakeModal(false);
    Destroy();
}

void AudioPrefs::OnBufferScroll(wxScrollEvent & event)
{
    m_buffers = mBuffCtrl->GetValue();

    wxLogDebug("Buffer size %s", DoITOA(m_buffers));
    mBuffers->SetValue(DoITOA(m_buffers));
    m_latency = int(1000.0 * float(m_buffers) * float(m_samples) / float(m_rate));

    mLatency->SetValue(DoITOA(m_latency) + wxString("ms"));


}

void AudioPrefs::OnBufferText(wxCommandEvent & event)
{

    if (!mBuffers->GetValue().ToULong(&m_buffers))
    {
        wxLogDebug("Invalid buffer value.");
        m_buffers = 4;
        mBuffers->SetValue("4");
    }

    mBuffCtrl->SetValue(m_buffers);

    if ((m_rate < m_ratemin) || (m_rate > m_ratemax))
    {
        mLatency->SetValue("Invalid Rate!");
    }
    else
    {
        m_latency = int(1000.0 * float(m_buffers) * float(m_samples) / float(m_rate));

        mLatency->SetValue(DoITOA(m_latency) + wxString("ms"));
    }
}

void AudioPrefs::OnChangeRate(wxCommandEvent & event)
{
    if (!mRate->GetValue().ToULong(&m_rate))
    {
        wxLogDebug("Invalid rate value.");
        m_rate = 44100;
        mRate->SetValue("44100");
    }

    if ((m_rate < m_ratemin) || (m_rate > m_ratemax))
    {
        mLatency->SetValue("Invalid Rate!");
    }
    else
    {
        m_latency = int(1000.0 * float(m_buffers) * float(m_samples) / float(m_rate));

        mLatency->SetValue(DoITOA(m_latency) + wxString("ms"));
    }

}

void AudioPrefs::OnChangeSamples(wxSpinEvent & event)
{
    m_samples = mSamples->GetValue();
    m_latency = int(1000.0 * float(m_buffers) * float(m_samples) / float(m_rate));

    if ((m_rate < m_ratemin) || (m_rate > m_ratemax))
    {
        mLatency->SetValue("Invalid Rate!");
    }
    else
    {
        m_latency = int(1000.0 * float(m_buffers) * float(m_samples) / float(m_rate));

        mLatency->SetValue(DoITOA(m_latency) + wxString("ms"));
    }

}

void AudioPrefs::OnChangeDevice(wxCommandEvent & event)
{
    wxLogDebug("OnChangeDevice");

    int dev = mDevice->GetSelection();
    LoadDevice(dev);

    
}

void AudioPrefs::SaveSettings()
{

    m_device = mDevice->GetSelection();
    mRate->GetValue().ToULong(&m_rate);
    mBits->GetValue().ToULong((unsigned long *)&m_bits);
    
    if (mChannels->GetSelection() == 0)
        m_channels = 2;
    else
        m_channels = 1;

    m_samples = mSamples->GetValue();

    m_buffers = mBuffCtrl->GetValue();

    m_engine = mDecoder->GetSelection();
    m_thread_priority = mThreadPriority->GetSelection();
    m_dither = mDither->GetSelection();

    mConf.audio.device = m_device;
    mConf.audio.sampleFormat = m_bits;
    mConf.audio.buffers = m_buffers;
    mConf.audio.bufferSize = m_samples;
    mConf.audio.sampleRate = m_rate;
    mConf.decoder.thread_priority = m_thread_priority;
    mConf.decoder.dither = m_dither;
    mConf.decoder.engine_id = m_engine;
    mConf.audio.channels = m_channels;
}

void AudioPrefs::LoadDevice(int dev)
{
    bool supports44k = false;

    if (dev == -1)
    {
        wxLogDebug("Invalid selection");
        return;
    }

    wxASSERT((dev < mOut->mDevices.size()) && (dev >= 0));
    
    m_device = dev;
    
    mRate->Clear();
    mBits->Clear();

    // set sample rates
    if (!mOut->mDevices[dev].sampleRange)
    {
        mSRange = false;
        mRate->SetWindowStyle( wxCB_READONLY );
    }
    else
    {
        mSRange = true;
        mRate->SetWindowStyle(0);
    }

    int i;
    for (i = 0; i < mOut->mDevices[dev].sampleRates.size(); i++)
    {
        if (mOut->mDevices[dev].sampleRates[i] == 44100)
            supports44k = true;
        mRate->Append(DoITOA(mOut->mDevices[dev].sampleRates[i]));
    }

    m_ratemin = mOut->mDevices[dev].sampleRates[0];
    m_ratemax = mOut->mDevices[dev].sampleRates[mOut->mDevices[dev].sampleRates.size() - 1];

    if (mSRange && !supports44k)
        if ( ( m_ratemin <= 44100) && (m_ratemax >= 44100) )
            supports44k = true;
  
    // set sample formats
    bool s8 = false, s16 = false, s24 = false;
    for (i = 0; i < mOut->mDevices[dev].sampleFormats.size(); i++)
    {
        switch(mOut->mDevices[dev].sampleFormats[i])
        {
        case paInt16: s16 = true; break;
        case paInt24: s24 = true; break;
        case paInt8: s8 = true; break;
        default: break;
        }
    }

    if (s8) mBits->Append("8");
    if (s16) mBits->Append("16");
    if (s24) mBits->Append("24");
    
    if (mConf.loaded && (mConf.audio.device == dev))
    {
        m_bits = mConf.audio.sampleFormat;
        m_buffers = mConf.audio.buffers;
        m_samples = mConf.audio.bufferSize;
        m_rate = mConf.audio.sampleRate;
        m_thread_priority = mConf.decoder.thread_priority;
        m_dither = mConf.decoder.dither;
        m_engine = mConf.decoder.engine_id;
        m_channels = mConf.audio.channels;

    }
    else
    {
        // load defaults
        if (s16)
            mConf.audio.sampleFormat = m_bits = 16;
        else
            if (s24)
                mConf.audio.sampleFormat = m_bits = 24;
            else if (s8)
                mConf.audio.sampleFormat = m_bits = 8;
            else wxASSERT(1 == 0);
        if (supports44k)
            mConf.audio.sampleRate = m_rate = 44100;
        else
            mConf.audio.sampleRate = m_rate = mOut->mDevices[dev].sampleRates[0];

        mConf.audio.buffers = m_buffers = 4;
        mConf.audio.bufferSize = m_samples = 1024;

        if (!mConf.loaded)
        {
            mConf.decoder.thread_priority = m_thread_priority = 0; // normal
            mConf.decoder.dither = m_dither = 0;
            mConf.decoder.engine_id = m_engine = 0; // MAD
        }

        if (mOut->mDevices[dev].outch >= 2)
            mConf.audio.channels = m_channels = 2;
        else
            mConf.audio.channels = m_channels = 1;

        mConf.audio.device = dev;
        mConf.loaded = true;

    }

    int latency = int(1000.0 * float(mConf.audio.buffers) * float(mConf.audio.bufferSize) / float(mConf.audio.sampleRate));
    
    mBuffers->SetValue(DoITOA(mConf.audio.buffers));
    mBuffCtrl->SetValue(mConf.audio.buffers);
    mSamples->SetValue(m_samples);
    mBits->SetValue(DoITOA(m_bits));
    mRate->SetValue(DoITOA(m_rate));

    if (m_channels >= 2)
    {
        mChannels->SetSelection(0);
        mChannels->Enable(true);
    }
    else
    {
        mChannels->SetSelection(1);
        mChannels->Enable(false);
    }


    mDither->SetSelection(m_dither);
    mThreadPriority->SetSelection(m_thread_priority);
    mDecoder->SetSelection(m_engine);

    mLatency->SetValue(DoITOA(latency) + wxString("ms"));
}
