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
class wxListCtrl;

class guiLog: public wxLog, public wxFrame
{
public:
    guiLog(const wxString& title, int x, int y, int w, int h, wxWindow * parent);
    ~guiLog();

    void OnCloseWindow(wxCloseEvent& event);

protected:

    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);

private:
    wxLog *m_logOld;
    wxListCtrl * m_listctrl;
    DECLARE_EVENT_TABLE()

};


