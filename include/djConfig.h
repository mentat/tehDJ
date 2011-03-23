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
#ifndef DJ_CONF_H
#define DJ_CONF_H

#include "wx/string.h"
struct s_audio
{
    long sampleRate;
    long sampleFormat;
    unsigned short channels;
    unsigned short buffers;
    unsigned short bufferSize;
    unsigned short device;
    unsigned short useASIO;
};
struct s_decoder
{
    unsigned short thread_priority;
    wxString engine;
    unsigned short engine_id;
    unsigned short dither;
};

struct config
{
    s_audio audio;
    s_decoder decoder;
    bool loaded;

};

#endif
