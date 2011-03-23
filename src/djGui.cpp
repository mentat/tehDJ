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
#pragma warning(disable:4267)
#include <sstream>
#include <string>
/********************wxWindows****************************/
#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/thread.h"
#include "wx/thread.h"
#include "wx/dynarray.h"
#include "wx/filedlg.h"
#include "wx/file.h"
#include "wx/timer.h"
#include "wx/config.h"
#include "../tehDJ_wdr.h"
/********************wxWindows****************************/

#include "NLog.h"
#include "dj.h"
#include "djVisual.h"
#include "djCodec.h"
#include "djOutput.h"
#include "djThread.h"
#include "djSetup.h"
#include "glbutton.h"
#include "djGui.h"

#include "bitmaps/deejay.xpm"

using namespace std;

DECLARE_APP(tehDJ)

enum
{
    DJ_Quit = 1,
    DJ_Audio,
    DJ_Timer,
    DJ_ResetReg,
    DJ_About = wxID_ABOUT
};

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_UPDATE_UI(-1, MainFrame::OnUIUpdate)
    EVT_MENU(DJ_Quit,  MainFrame::OnQuit)
    EVT_MENU(DJ_About, MainFrame::OnAbout)
    EVT_MENU(DJ_Audio, MainFrame::OnPrefs)
    EVT_MENU(DJ_ResetReg, MainFrame::OnResetReg)
    EVT_BUTTON(ID_LOAD, MainFrame::OnLoad)
    EVT_BUTTON(ID_START, MainFrame::OnStart)
    EVT_BUTTON(ID_STOP, MainFrame::OnStop)
    EVT_BUTTON(ID_PAUSE, MainFrame::OnPause)
    EVT_BUTTON(ID_BACK, MainFrame::OnBack)
    EVT_BUTTON(ID_FORWARD, MainFrame::OnForward)
    //EVT_IDLE(MainFrame::OnIdle)
    EVT_TIMER(DJ_Timer, MainFrame::OnTimer)
    EVT_CLOSE(MainFrame::OnCloseWindow)
    //EVT_COMMAND_SCROLL_THUMBRELEASE(ID_LOCATION, MainFrame::OnLocationScroll)
    EVT_COMMAND_SCROLL_ENDSCROLL(ID_LOCATION, MainFrame::OnLocationScroll)
END_EVENT_TABLE()

MainFrame::MainFrame(bool runsetup, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, -1, title, pos, size, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION), m_producer(NULL) 
{
    m_visual = NULL;
    wxIcon appIcon(deejay_xpm);
    SetIcon(appIcon);  // Set the icon
    // ---------- setup menus ------------
    wxMenu *menuFile = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    wxMenu *opMenu = new wxMenu;
    helpMenu->Append(DJ_About, _T("&About...\tF1"), _T("Show about dialog"));
    menuFile->Append(DJ_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));
    opMenu->Append(DJ_Audio, _T("Audio Settings"), _T("Edit audio settings"));
    opMenu->Append(DJ_ResetReg, _T("Reset Registry"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(opMenu, _T("&Options"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);

    // log device
    guiLog * log = new guiLog("Log", 300, 50, 300, 200, this);

    // ---------- create panel -------------
    wxPanel *panel = new wxPanel(this);

    wxStaticText * txt = new wxStaticText(panel, ID_VISUAL, "hi");
    //m_visual = new wxMonoScope(panel, ID_VISUAL, wxDefaultPosition, wxSize(90,20));
    //m_visual->SetBufferSize(8192);


    wxSizer * sizer = djMainSmall(panel, false, true);
    sizer->SetSizeHints(this);
    SetSize(sizer->GetMinSize());

    // ---------- find controls ------------
    m_songname = (wxTextCtrl *)FindWindow(ID_SONGNAME);
    m_songtime = (wxTextCtrl *)FindWindow(ID_SONGTIME);


#if 0
    wxFrame * frm = new wxFrame(this, -1, "Virtual Turntable", wxPoint(500, 50), wxSize(400,300));
    wxPanel * pnl = new wxPanel(frm);
    wxGLControl * gl = new wxGLControl(pnl, -1, wxPoint(0, 0), wxSize(400, 300), wxSUNKEN_BORDER);
    gl->LoadLWO("turntable.lwo" );
    frm->Show(TRUE);

    EQFrame * eqf = new EQFrame(this, "Equalizer", wxPoint(50, 450), wxSize(270, 200));
    eqf->Show(TRUE);

#endif

    m_codec = new MADCoDec();
    m_output = new PortAudioOutput(m_codec, NULL, 8192, 4);

   // m_output->SetVisual(m_visual);
    /*m_producer = new ProducerThread(m_output, m_codec, this);
    
    if ( m_producer->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!\n"));
    }*/
    //m_producer->Run();

    //m_audio = new AudioThread(m_output, this);
    //if ( m_audio->Create() != wxTHREAD_NO_ERROR )
    //{
    //   wxLogError(wxT("Can't create thread!\n"));
    //}
    //m_audio->Run();

    mTimer.SetOwner(this, DJ_Timer);

    Show(true);
    wxLogDebug("Hi");

    if (runsetup)
        AudioPrefs * pref = new AudioPrefs(m_output, this, "Audio Preferences", wxPoint(10,10), wxSize(400,300));

}

MainFrame::~MainFrame()
{
    if (m_producer)
        m_producer->Delete();
    
    delete m_codec;
    delete m_output;

}

void MainFrame::OnCloseWindow(wxCloseEvent& event)
{
    wxLogDebug("OnCloseWindow");
    Destroy();
}
// event handlers

void MainFrame::OnResetReg(wxCommandEvent & event)
{
    wxConfig *config = new wxConfig("tehDJ");

    config->DeleteAll();

    delete config;

    wxGetApp().mConf.loaded = false;

}

void MainFrame::OnPrefs(wxCommandEvent & event)
{

    AudioPrefs * pref = new AudioPrefs(m_output, this, "Audio Preferences", wxPoint(10,10), wxSize(400,300));

}

void MainFrame::OnPause(wxCommandEvent& event)
{
    m_output->SetPause();
}

void MainFrame::OnBack(wxCommandEvent& event)
{
}

void MainFrame::OnForward(wxCommandEvent& event)
{
}

void MainFrame::OnLocationScroll(wxScrollEvent & event)
{
    wxLogDebug("OnScroll");
    wxSlider * sldr = (wxSlider *)FindWindow(ID_LOCATION);
    int loc = sldr->GetValue();

    if (m_producer)
        m_producer->SeekTo(loc, 1000);

}

void MainFrame::OnTimer(wxTimerEvent & event)
{
    // check to see if stipp playing/codec ok
    int total, curr;

    if (!m_codec)
        return;

    
    total = m_codec->GetTotalTime();
    curr = m_codec->GetCurrentTime();

    if (int(float(curr)/float(total) * 1000.0) == 1000)
    {
        mTimer.Stop();
        return;
    }

    int min = curr / 60;
    int sec = curr % 60;
    wxString time;
    time.Printf("%02d:%02d", min, sec);
    m_songtime->SetValue(time);

    wxSlider * loc = (wxSlider *)FindWindow(ID_LOCATION);

    wxGauge * bufflev = (wxGauge *)FindWindow(ID_BUFF_LEVEL);

    bufflev->SetValue(m_output->mQueue->level() * 100);

    if (total == 0)
        total = 1;

    loc->SetValue(int(float(curr)/float(total) * 1000.0));
    

}

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
        
    wxLogDebug("OnQuit");
    Close(TRUE);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("tehDJ - alpha.\nCopyright 2002 - Jesse Lovelace\nGPL\n")
        _T("%s\nSpecial thanks to:\nMAD, CoolPlayer, PortAudio"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About tehDJ"), wxOK | wxICON_INFORMATION, this);
}

void MainFrame::OnLoad(wxCommandEvent& event)
{
    mFilename = wxFileSelector(wxT("Select a file"),wxT(""), wxT(""), 
        wxT("mp3"), wxT("*.mp3"), wxOPEN | wxFILE_MUST_EXIST, this);

    if (!mFilename.empty())
    {
        m_codec->OpenFile(mFilename.c_str());
    }
    else 
        return;

    wxConfig *config = new wxConfig("tehDJ");

    config->Write("LastFile", mFilename.c_str());

    delete config;
    // get header info...

    mSongname = m_codec->GetArtist().c_str();
    mSongname += " - ";
    mSongname += m_codec->GetTitle().c_str();
    
    if (mSongname.Length() == 3)
    {
        wxLogDebug("ID3 is empty, using filename...");
        mSongname = mFilename.AfterLast('\\');
        mSongname = mSongname.BeforeLast('.');
    }
    m_songname->SetValue(mSongname);

    if (m_producer == NULL)
    {
        m_producer = new ProducerThread(m_output, m_codec, this);
    
        if ( m_producer->Create() != wxTHREAD_NO_ERROR )
        {
            wxLogError(wxT("Can't create thread!\n"));
        }
        m_producer->Run();
    }

}

void MainFrame::OnStart(wxCommandEvent& event)
{
    wxLogDebug("OnStart");

    if (mFilename.IsEmpty())
        return;

    if (m_producer == NULL)
    {
        m_producer = new ProducerThread(m_output, m_codec, this);
    
        if ( m_producer->Create() != wxTHREAD_NO_ERROR )
        {
            wxLogError(wxT("Can't create thread!\n"));
        }
        m_producer->Run();
    }

    mTimer.Start(300);

    m_output->Play(true);
   // m_visual->Run();

}

void MainFrame::OnStop(wxCommandEvent& event)
{
    if (!m_producer)
        return;


    mTimer.Stop();
    m_output->Stop();

    wxSlider * loc = (wxSlider *)FindWindow(ID_LOCATION);
    loc->SetValue(0);

    m_producer->Delete();
    m_producer = NULL;

    m_output->Flush();

    m_codec->Seek(0,1);
    //m_visual->Stop();



}



void MainFrame::OnUIUpdate(wxUpdateUIEvent & event)
{


}

BEGIN_EVENT_TABLE(EQFrame, wxFrame)
    EVT_COMMAND_SCROLL_THUMBRELEASE(-1, EQFrame::OnScroll)
END_EVENT_TABLE()

EQFrame::EQFrame(wxWindow * parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(parent, -1, title, pos, size, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION)
{
    wxPanel * panel = new wxPanel(this);

    for (int i = 0; i < 9; i++)
        mBands[i] = 50;

    EQ(panel, true, true);

}
void EQFrame::OnScroll(wxScrollEvent& event)
{
    int eq = event.GetId() - ID_BAND0;
    int newpos = event.GetPosition();

    if (mBands[eq] < newpos)
        wxLogDebug("Band %d down to %d", eq, 100-newpos);
    else
        if (mBands[eq] > newpos)
            wxLogDebug("Band %d up to %d", eq, 100-newpos);
    mBands[eq] = newpos;
    
#if 0
    switch (eq) {
        case ID_BAND0: wxLogDebug("Unknown EQ Band?!?"); break;
        case ID_BAND1: wxLogDebug("Unknown EQ Band?!?"); break;
        case ID_BAND2: wxLogDebug("Unknown EQ Band?!?"); break;
        case ID_BAND3: wxLogDebug("Unknown EQ Band?!?"); break;
        case ID_BAND4: wxLogDebug("Unknown EQ Band?!?"); break;
        case ID_BAND5: wxLogDebug("Unknown EQ Band?!?"); break;
        case ID_BAND6: wxLogDebug("Unknown EQ Band?!?"); break;
        case ID_BAND7: wxLogDebug("Unknown EQ Band?!?"); break;
        case ID_BAND8: wxLogDebug("Unknown EQ Band?!?"); break;
        default: wxLogDebug("Unknown EQ Band?!?");
    }
#endif

}
