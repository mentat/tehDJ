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
#ifndef DJ_SETUP_H
#define DJ_SETUP_H

#include "djConfig.h"

class PortAudioOutput;
class wxComboBox;
class wxRadioBox;
class wxSpinCtrl;
class wxTextCtrl;
class wxSlider;
class wxChoice;

class AudioPrefs: public wxFrame
{
public:
    AudioPrefs(PortAudioOutput * out, wxWindow * parent, const wxString& title, const wxPoint& pos, const wxSize& size,
        long style = wxDEFAULT_FRAME_STYLE);

    void OnCloseWindow(wxCloseEvent& event);

    void OnBufferScroll(wxScrollEvent & event);
    void OnBufferText(wxCommandEvent & event);
    void OnChangeDevice(wxCommandEvent & event);
    void OnChangeRate(wxCommandEvent & event);
    void OnChangeSamples(wxSpinEvent & event);

private:

    void SaveSettings();

    void LoadDevice(int dev);

    unsigned long m_rate;
    unsigned long m_ratemin;
    unsigned long m_ratemax;
    unsigned short m_latency;
    unsigned long m_buffers;
    unsigned short m_samples;
    unsigned short m_device;

    unsigned short m_bits;
    unsigned short m_channels;
    bool mSRange; // sample rate can be range

    unsigned short m_engine;
    unsigned short m_thread_priority;
    unsigned short m_dither;

    PortAudioOutput * mOut;

    wxComboBox * mDevice;
    wxComboBox * mRate;
    wxComboBox * mBits;
    wxRadioBox * mChannels;
    wxSpinCtrl * mSamples;
    wxTextCtrl * mBuffers;
    wxSlider * mBuffCtrl;
    wxTextCtrl * mLatency;
    wxChoice * mDecoder;
    wxChoice * mThreadPriority;
    wxChoice * mDither;

    config & mConf;

    DECLARE_EVENT_TABLE()

};


#endif
