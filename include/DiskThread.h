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

#ifndef DJ_DISK_THREAD_H
#define DJ_DISK_TREAD_H

#include "mad.h"

class InStream;

class CoDecThread : public wxThread
{
public:
    CoDecThread(MainFrame *frame, const wxString& filename, 
        const unsigned short buffsize, const unsigned short waveblock );
    
    ~CoDecThread() { delete [] mInputBuffer; delete [] mWaveBuff; }

    virtual void *Entry();

    virtual void OnExit();

    void WriteText(const wxString& text);

    void PrintFrameInfo(mad_header *Header);

private:

    // file information
    InStream * mStream;

    // read buffer from disk
    unsigned char *     mInputBuffer;  
    short *             mWaveBuff;

    unsigned short mWaveSize;
    unsigned short mBuffSize;

    MainFrame *m_frame;


};

#endif
