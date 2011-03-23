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
#ifndef DJ_STREAM_H
#define DJ_STREAM_H

#include "wx/file.h"

class InStream
{
public:
// Public functions
    InStream(const char * url) {}
    virtual ~InStream() {}

	virtual bool Read(void* pDestBuffer, const unsigned int iBytesToRead, unsigned int& piBytesRead) = 0;
	virtual bool Seek(const unsigned int iNewOffset) = 0;
    virtual bool SeekEnd(const int iNewOffset) = 0;
	virtual unsigned int GetLength() = 0;
    virtual off_t GetPosition() = 0;
	virtual bool IsSeekable() = 0;
    virtual bool Error() = 0;
    virtual bool Eof() = 0;

};

class FileStream: public InStream
{
public:
    FileStream(const char * filename);
    virtual ~FileStream();

	virtual bool Read(void* pDestBuffer, const unsigned int iBytesToRead, unsigned int& piBytesRead);
	virtual bool Seek(const unsigned int iNewOffset);
    virtual bool SeekEnd(const int iNewOffset);
	virtual unsigned int GetLength();
    virtual off_t GetPosition();
	virtual bool IsSeekable();
    virtual bool Error();
    virtual bool Eof();

private:
    wxFile m_file;
};

#endif
