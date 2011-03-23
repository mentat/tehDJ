// --*-c++-*--
/*
 
    GNU Messenger - The secure instant messenger
    Copyright (C) 2001-2002  Jesse Lovelace
 
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

/********************wxWindows****************************/
#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/log.h"
#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/listctrl.h"
#include "wx/bitmap.h"
/********************wxWindows****************************/

#include "bitmaps/stormtroop.xpm"
#include "bitmaps/yoda.xpm"
#include "bitmaps/boba_ship.xpm"
#include "bitmaps/deejay.xpm"

#include "NLog.h"

#define ID_LOG_LIST 34

BEGIN_EVENT_TABLE(guiLog, wxFrame)
    EVT_CLOSE(guiLog::OnCloseWindow)
END_EVENT_TABLE()

wxSizer *LogView( wxWindow *parent, bool call_fit, bool set_sizer );
guiLog::guiLog(const wxString& title, int x, int y, int w, int h, wxWindow * parent)
: wxFrame(parent, -1, title, wxPoint(x, y), wxSize(w, h))
{
    wxIcon appIcon(yoda_xpm);
    SetIcon(appIcon);  // Set the icon

    wxPanel * panel = new wxPanel(this);

    LogView(panel, true, true);

    m_listctrl = (wxListCtrl *)FindWindow(ID_LOG_LIST);

    m_listctrl->InsertColumn(0, wxT("Message"));
    m_listctrl->InsertColumn(1, wxT("Type"));

    // prepare the imagelist
    enum { ICON_SIZE = 16 };
    wxImageList *imageList = new wxImageList(ICON_SIZE, ICON_SIZE, true);

    // order should be the same as in the switch below!

    imageList->Add( wxBitmap( stormtroop_xpm ).ConvertToImage().Rescale(ICON_SIZE, ICON_SIZE) );
    imageList->Add( wxBitmap( boba_ship_xpm ).ConvertToImage().Rescale(ICON_SIZE, ICON_SIZE) );
    imageList->Add( wxBitmap( yoda_xpm ).ConvertToImage().Rescale(ICON_SIZE, ICON_SIZE) );

    m_listctrl->SetImageList(imageList, wxIMAGE_LIST_SMALL);

    Show(TRUE);

    m_logOld = wxLog::SetActiveTarget(this);

}

void guiLog::OnCloseWindow(wxCloseEvent& event)
{

}

guiLog::~guiLog()
{

	wxLog::SetActiveTarget(m_logOld);
	delete m_listctrl->GetImageList(wxIMAGE_LIST_SMALL);
}

void guiLog::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
  int image;

  wxString string_type;

  switch ( level )
  {
  case wxLOG_Error:
    image = 0;
    string_type = wxT("Error");
    break;
  case wxLOG_Warning:
    image = 1;
    string_type = wxT("Warning");
    break;
  case wxLOG_FatalError:
    image = 0;
    string_type = wxT("Fatal error");
    break;
  case wxLOG_Message:
    image = 2;
    string_type = wxT("Message");
    break;
  case wxLOG_Info:
    image = 2;
    string_type = wxT("Info");
    break;
  case wxLOG_Status:
    image = 2;
    string_type = wxT("Status");
    break;
  case wxLOG_Debug:
    image = 2;
    string_type = wxT("Debug");
    break;
  case wxLOG_Trace:
    image = 2;
    string_type = wxT("Trace");
    break;
  case wxLOG_Progress:
    image = 2;
    string_type = wxT("Progress");
    break;
  case wxLOG_User:
    image = 2;
    string_type = wxT("User");
    break;
  default:
    string_type = wxT("Unknown");
    image = 2;
    break;
  }

  m_listctrl->InsertItem(0, szString, image);
  m_listctrl->SetItem(0,1,string_type);

  m_listctrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
  m_listctrl->SetColumnWidth(1, wxLIST_AUTOSIZE);

}

wxSizer *LogView( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxListCtrl *item1 = new wxListCtrl( parent, ID_LOG_LIST, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}

/*
    -----
    $Log: NLog.cpp,v $
    Revision 1.2  2002/07/04 22:45:44  mentat
    Major changes to NNIM's contact interface and wizards.

    Revision 1.1  2002/06/28 18:17:51  mentat
    Merging SF sources.

    Revision 1.4  2002/06/27 14:25:50  thementat
    Fixed GTK debug errors.

    Revision 1.3  2002/06/26 04:27:08  thementat
    Event fixes.

    Revision 1.2  2002/06/20 01:25:00  thementat
    Removed unicode for the time being to fix linux build.

    Revision 1.1  2002/06/19 16:27:18  thementat
    Restructured directories.

    Revision 1.2  2002/06/09 19:45:03  thementat
    Liberal use of auto_ptr.

    Revision 1.1.1.1  2002/06/06 17:21:30  thementat
    Checkin of new sources BETA 2

    Revision 1.9  2002/01/17 20:00:51  mentat
    Moved dirs back to normal.

    Revision 1.1  2001/12/18 04:10:39  mentat
    Restructuring all sources. Beta 1.

    Revision 1.7  2001/12/12 19:25:25  mentat
    Fixes for gcc 2.95.3 and new makefile to build (for now until I sort out automake).
    Builds in FreeBSD4.4 with gcc 2.95.3 and wxWindows 2.3.2

    Revision 1.6  2001/12/05 05:26:58  mentat
    Fixed taskbar crash, fixed logout crash, toc almost works. :) wxNetwork rewritten with events, works well.

    Revision 1.5  2001/12/02 21:40:26  mentat
    Major restructure of all source, preparing for more changes and release.i
*/
