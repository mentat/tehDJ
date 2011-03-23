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

#ifndef DJMAD_GUI_H
#define DJMAD_GUI_H

#include "CircQueue.h"
#include "djConfig.h"
#include "wx/spinctrl.h"
#include "wx/slider.h"

class wxGLControl;
class AudioPrefs;
class wxVisual;
class ProducerThread;

class MainFrame : public wxFrame
{
public:
    MainFrame(bool runsetup, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE);
    ~MainFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnLoad(wxCommandEvent& event);
    void OnStart(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnBack(wxCommandEvent& event);
    void OnForward(wxCommandEvent& event);
    void OnPrefs(wxCommandEvent& event);
    void OnTimer(wxTimerEvent & event);
    void OnLocationScroll(wxScrollEvent & event);
    void OnCloseWindow(wxCloseEvent& event);
    void OnResetReg(wxCommandEvent & event);


    void OnUIUpdate(wxUpdateUIEvent & event);

    void WriteText(const wxString& text) 
    { wxLogMessage(text); }
    
    wxGLControl * m_gl;


private:
    friend class ProducerThread;
    // event timer
    wxTimer mTimer;

    // codec and audio output
    CoDec * m_codec;
    PortAudioOutput * m_output;

    // threads
    ProducerThread * m_producer;
    AudioThread * m_audio;

    // gui items
    wxTextCtrl * m_songname;
    wxTextCtrl * m_songtime;

    // current filename
    wxString mSongname;
    wxString mFilename;

    wxVisual * m_visual;

    DECLARE_EVENT_TABLE()
};

class EQFrame: public wxFrame
{
public:
    EQFrame(wxWindow * parent, const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);
    void OnScroll(wxScrollEvent & event);

private:
    int mBands[9];
    DECLARE_EVENT_TABLE()


};


#endif
