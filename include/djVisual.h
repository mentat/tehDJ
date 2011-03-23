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
#ifndef DJ_VISUAL_H
#define DJ_VISUAL_H

#include <vector>
#include <rfftw.h>
#include <fftw.h>
#include "LogScale.h"
#include "CircQueue.h"

using namespace std;

class VisualNode
{
public:
    VisualNode(short *l, short *r, unsigned long n, unsigned long o)
	: left(l), right(r), length(n), offset(o)
    {
	// left and right are allocated and then passed to this class
	// the code that allocated left and right should give up all ownership
    }

    ~VisualNode()
    {
	delete [] left;
	delete [] right;
    }

    short *left, *right;
    long length, offset;
};


class wxVisual: public wxWindow
{
public:

    wxVisual() { Init(); }
    virtual ~wxVisual();

    wxVisual(wxWindow *parent, const wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize, long style = 0)
    {
        Init();
        Create(parent, id, pos, size, style);
    }

    virtual void Init();

    virtual bool Create(wxWindow *parent, const wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize, long style = 0);
  
    virtual void Process(short * left, short * right, unsigned int length) = 0;
    virtual void Run() = 0;
    virtual void Stop() = 0;

    void SetBufferSize(unsigned long sz);
    void Eat( short * buffer, unsigned int length, int bits, int channels);

    void OnPaint(wxPaintEvent& event);
    virtual void OnSize(wxSizeEvent& event) = 0;
    void OnTimer(wxTimerEvent & event);

protected:

    virtual void DoPaint(wxDC * dc) = 0;

    //wxThread m_vis;
    //CircQueue<wxBitmap> m_queue;

    wxMutex m_lock;
    short * m_left;
    short * m_right;
    bool m_mono;
    unsigned long m_length;
    unsigned long m_actual;


};

class wxMonoScope: public wxVisual
{
public:
    wxMonoScope(wxWindow *parent, const wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize, long style = 0)
    {
        Init();
        Create(parent, id, pos, size, style);
    }


    wxMonoScope() { Init(); }

    virtual bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    virtual void Init();

    virtual ~wxMonoScope();

    virtual void Stop();
    virtual void Run();
    virtual void Process(short * left, short * right, unsigned int length);

    //void OnPaint(wxPaintEvent& event);
    virtual void OnSize(wxSizeEvent& event);
    //void OnTimer(wxTimerEvent & event);

private:
    virtual void DoPaint(wxDC * dc);


    wxColor m_startColor, m_targetColor;
    wxTimer m_timer;
    vector<double> m_mags;
    vector<wxRect> m_rects;

    double  m_scaleFactor, 
            m_falloff;
    int     m_analyzerBarWidth, 
            m_fps;

    LogScale m_scale;

    rfftw_plan plan;
    fftw_real lin[512], rin[512], lout[1024], rout[1024];
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxMonoScope)

};

class wxStereoScope : public wxVisual
{
public:
    wxStereoScope(wxWindow *parent, const wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize, long style = 0)
    {
        Init();
        Create(parent, id, pos, size, style);
    }


    wxStereoScope() { Init(); }

    virtual bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    virtual void Init();

    virtual ~wxStereoScope();

    virtual void Stop();
    virtual void Run();
    virtual void Process(short * left, short * right, unsigned int length);

    void OnSize(wxSizeEvent& event);

protected:

    void DoPaint(wxDC * dc);

    wxColor m_startColor, m_targetColor;
    wxTimer m_timer;
    vector<double> m_mags;
    vector<wxRect> m_rects;
    bool m_rubberband;
    double m_falloff;
    int m_fps;
    
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxStereoScope)
};

#endif
