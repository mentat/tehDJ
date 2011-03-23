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

#ifndef __DJ_FILTER_H__
#define __DJ_FILTER_H__

#include <string>
using namespace std;
#if 0
class FilterOptions
{
public:
    FilterOptions() {}

    string GetName() { return m_name; }

protected:
    string m_name;

};

class SamplerOptions: public FilterOptions
{
public:
    SamplerOptions(): FilterOptions() { m_name = "SamplerOptions"; }
    SamplerOptions &operator=(const SamplerOptions &other);

    int mSamplingRate;
    bool mMode; // fast/accurate
};

class Filter
{
public:
    Filter(const string& name, Filter * next = NULL, Filter * prev = NULL)
        : m_name(name), m_next(next), m_prev(prev), m_stop(false) {}

    virtual ~Filter() { delete m_next; }

    void Add(Filter * flt);

    // returns non-null only if it was head node and new node becomes head
    Filter * Remove(const string& name);

    // returns true if filter exists and options were set
    bool SetOptions(const string& name, FilterOptions * options);
    
    virtual bool IsOk(const string& name);
    virtual bool Start(const string& name);
    virtual bool Stop(const string& name);

    // calls Process() and forwards to next filter
    void * ProcessBlock(void * inBlock, int &numsamples, int samplebits, int channels);
                

protected:

    bool m_stop;
    virtual bool DoIsOk() = 0;
    virtual void * DoProcessBlock(void * inBlock, int &numsamples, int samplebits, int channels) = 0;
    virtual bool DoSetOptions(FilterOptions * options) = 0;

    Filter * m_next;
    Filter * m_prev;

    string m_name;

};
#endif

#define IBUFFSIZE 4096
class Sampler
{
public:
    Sampler();

    ~Sampler();

    void * ProcessBlock(void * inBlock, double factor, int &numsamples, int samplebits, int channels);

private:

    double m_factor;

    unsigned int Time, Time2;          /* Current time/pos in input sample */
    unsigned short Xp, Ncreep, Xoff, Xread;
    int OBUFFSIZE;
    short X1[IBUFFSIZE], *Y1; /* I/O buffers */
    short X2[IBUFFSIZE], *Y2; /* I/O buffers */
    unsigned short Nout, Nx;
    int i, Ycount, last;

};

#endif
#endif
