/*
 * tehDJ - A realtime mp3 dj-ing system
 * (C) 2002 - Jesse Lovelace
*/

#include "wx/wx.h"
#include "wx/timer.h"

#include <vector>
#include "djVisual.h"
#include "math.h"
#include <rfftw.h>
#include <fftw.h>
#include "inlines.h"

using namespace std;

enum
{
    MONOSCOPE_TIMER = 9999,
    STEREOSCOPE_TIMER
};

//wxVisual::wxVisual(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
//    : wxWindow(parent, id, pos, size, style), m_right(NULL), m_left(NULL)
//{}

void wxVisual::Init()
{
    m_right = NULL;
    m_left = NULL;

}

bool wxVisual::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if ( !wxWindow::Create(parent, id, pos, size, style) )
    {
        wxLogDebug("Could not create!");
        return false;
    }

    return true;

}

wxVisual::~wxVisual()
{
    delete [] m_right;
    delete [] m_left;
}


void wxVisual::SetBufferSize(unsigned long sz)
{
    if (m_right)
        delete m_right;
    if (m_left)
        delete m_left;

    m_right = new short[sz];
    m_left = new short[sz];
    m_length = sz;
    m_actual = 0;

}

void wxVisual::OnTimer(wxTimerEvent & event)
{
    wxClientDC dc(this);
    DoPaint(&dc);
}

void wxVisual::OnPaint(wxPaintEvent & event)
{
    wxPaintDC dc(this);
    DoPaint(&dc);
}

void wxVisual::Eat(short * buffer, unsigned int length, int p, int c)
{
    long len = length, cnt;
    short *l = 0, *r = 0;

    len /= c;
    len /= (p / 8);
    if (len > 512)
	len = 512;
    cnt = len;

    if (c == 2) {
	l = new short[len];
	r = new short[len];

	if (p == 8)
	    stereo16_from_stereopcm8(l, r, (unsigned char *)buffer, cnt);
	else if (p == 16)
	    stereo16_from_stereopcm16(l, r, buffer, cnt);
    } else if (c == 1) {
	l = new short[len];

	if (p == 8)
	    mono16_from_monopcm8(l, (unsigned char *)buffer, cnt);
	else if (p == 16)
	    mono16_from_monopcm16(l, buffer, cnt);
    } else
	len = 0;

    Process(l, r, len);
    delete [] l;
    delete [] r;
}

BEGIN_EVENT_TABLE(wxMonoScope, wxVisual)
    EVT_SIZE(wxMonoScope::OnSize)
    EVT_TIMER(MONOSCOPE_TIMER, wxMonoScope::OnTimer)
    EVT_PAINT(wxMonoScope::OnPaint)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxMonoScope, wxWindow)

bool wxMonoScope::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if (!wxVisual::Create(parent, id, pos, size, style))
    {
        wxLogDebug("Could not create wxMonoScope");
        return false;
    }
    return true;
    //wxMutexLocker lock(m_lock);
    
}

void wxMonoScope::Init()
{
    m_startColor = *wxGREEN;
    m_targetColor = *wxRED;
    m_timer.SetOwner(this, MONOSCOPE_TIMER);
    plan =  rfftw_create_plan(512, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);

    m_scaleFactor = 1.0;
    m_falloff = 1.0;
    m_analyzerBarWidth = 4;
    m_fps = 20;

    SetBackgroundColour(*wxBLACK);
}

wxMonoScope::~wxMonoScope()
{
    rfftw_destroy_plan(plan);

}


void wxMonoScope::Process(short * left, short * right, unsigned int length)
{
    wxMutexLocker lock(m_lock);
    if (m_right && m_left)
    {
        m_actual = length;
        memcpy(m_right, right, length);
        memcpy(m_left, left, length);
    }
    else
    {
        wxLogDebug("Buffers not allocated.");
        return;
    }

    wxSize size = GetClientSize();
    bool allZero = TRUE;
    unsigned int i;
    long w = 0, index;
    //QRect *rectsp = rects.data();
    //double *magnitudesp = magnitudes.data();
    double magL, magR, tmp;

    if (m_left) 
    {
	    i = m_actual;
	    fast_real_set_from_short(lin, m_left, m_actual);
    	
        if (m_right)
	        fast_real_set_from_short(rin, m_right, m_actual);
    } 
    else
	    i = 0;

    fast_reals_set(lin + i, rin + i, 0, 512 - i);

    rfftw_one(plan, lin, lout);
    rfftw_one(plan, rin, rout);

    index = 1;
    for (i = 0; i < m_rects.size(); i++, w += m_analyzerBarWidth) 
    {
	    magL = (log(lout[index] * lout[index] +
		        lout[512 - index] * lout[512 - index]) - 22.0) *
	        m_scaleFactor;
	    magR = (log(rout[index] * rout[index] +
		        rout[512 - index] * rout[512 - index]) - 22.0) *
	        m_scaleFactor;

	    if (magL > size.GetHeight() / 2)
	        magL = size.GetHeight() / 2;
	    if (magL < m_mags[i]) 
        {
	        tmp = m_mags[i] - m_falloff;
	        if ( tmp < magL )
		        tmp = magL;
	        magL = tmp;
	    }
	
        if (magL < 1.)
	    magL = 1.;

	    if (magR > size.GetHeight() / 2)
	        magR = size.GetHeight() / 2;
	    if (magR < m_mags[i + m_scale.range()]) 
        {
	        tmp = m_mags[i + m_scale.range()] - m_falloff;
	        if ( tmp < magR )
		    tmp = magR;
	        magR = tmp;
	    }

	    if (magR < 1.)
	        magR = 1.;

	    if (magR != 1 || magL != 1)
	        allZero = FALSE;

	    m_mags[i] = magL;
	    m_mags[i + m_scale.range()] = magR;

	    //m_rects[i].setTop( size.GetHeight() / 2 - int( magL ) );
	    //m_rects[i].setBottom( size.GetHeight() / 2 + int( magR ) );
        m_rects[i].SetHeight( size.GetHeight() / 2 + 2 *int( magR ) );
        m_rects[i].SetY( size.GetHeight() / 2 - int( magL ) );
    	

	    index = m_scale[i];
    }

    //return allZero;

}

void wxMonoScope::DoPaint(wxDC * dc)
{
    wxSize size = GetClientSize();
    //wxDC & p(*dc);
    wxBitmap scope(size.GetWidth(),size.GetHeight()) ;

    wxMemoryDC p;
    p.SelectObject(scope);
    

    wxPen pen(GetBackgroundColour(), 1, wxTRANSPARENT);
    wxBrush brush(GetBackgroundColour(), wxSOLID);

    p.SetPen(pen);
    p.SetBrush(brush);

    p.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
    
    //brush.SetStyle(wxTRANSPARENT);
    pen.SetStyle(wxSOLID);
    pen.SetWidth(0);
    
    //p.SetBrush(brush);
    p.SetPen(pen);


    double r, g, b, per;

    //p->fillRect(0, 0, size.width(), size.height(), back);
    for (unsigned int i = 0; i < m_rects.size(); i++) 
    {
        //wxBrush brush(*wxBLACK, wxSOLID);
	    per = double( m_rects[i].GetHeight() - 2 ) / double( size.GetHeight() );
	    if (per > 1.0)
	        per = 1.0;
	    else if (per < 0.0)
	        per = 0.0;

	    r = m_startColor.Red() + (m_targetColor.Red() -
			        m_startColor.Red()) * (per * per);
	    g = m_startColor.Green() + (m_targetColor.Green() -
			        m_startColor.Green()) * (per * per);
	    b = m_startColor.Blue() + (m_targetColor.Blue() -
			        m_startColor.Blue()) * (per * per);

	    if (r > 255.0)
	        r = 255.0;
	    else if (r < 0.0)
	        r = 0;

	    if (g > 255.0)
	        g = 255.0;
	    else if (g < 0.0)
	        g = 0;

	    if (b > 255.0)
	        b = 255.0;
	    else if (b < 0.0)
	        b = 0;

        brush.SetColour(r, g, b);
        p.SetBrush(brush);
        p.DrawRectangle(m_rects[i]);
	    //p->fillRect(m_rects[i], QColor(int(r), int(g), int(b)));
    }
    p.SelectObject( wxNullBitmap );
    //dc->Blit(0, 0, scope.GetWidth(), scope.GetHeight(), &p, 0, 0);
    dc->DrawBitmap( scope, 0, 0 );
    //Refresh();

}



void wxMonoScope::OnSize(wxSizeEvent & event)
{

    wxSize size = event.GetSize();

    m_scale.setMax(192, size.GetWidth() / m_analyzerBarWidth);

    m_rects.resize(  m_scale.range() );
    int i = 0, w = 0;
    for (; (unsigned) i < m_rects.size(); i++, w += m_analyzerBarWidth)
	m_rects[i]= wxRect(w, size.GetHeight() / 2, m_analyzerBarWidth - 1, 1);

    int os =  m_mags.size();
    m_mags.resize( m_scale.range() * 2 );
    for (; (unsigned) os < m_mags.size(); os++)
	m_mags[os] = 0.0;

    m_scaleFactor = double( size.GetHeight() / 2 ) / log( 512.0 );

    event.Skip();

}

void wxMonoScope::Run()
{
    wxMutexLocker lock(m_lock);
    m_timer.Start(1000/m_fps);

}

void wxMonoScope::Stop()
{
    wxMutexLocker lock(m_lock);
    m_timer.Stop();
}


BEGIN_EVENT_TABLE(wxStereoScope, wxVisual)
    EVT_SIZE(wxStereoScope::OnSize)
    EVT_TIMER(STEREOSCOPE_TIMER, wxStereoScope::OnTimer)
    EVT_PAINT(wxStereoScope::OnPaint)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxStereoScope, wxWindow)

bool wxStereoScope::Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style)
{
    if (!wxVisual::Create(parent, id, pos, size, style))
    {
        wxLogDebug("Could not create wxStereoScope");
        return false;
    }
    return true;
}

void wxStereoScope::Init()
{
    m_startColor = *wxGREEN;
    m_targetColor = *wxRED;
    m_timer.SetOwner(this, STEREOSCOPE_TIMER);
   
    m_rubberband = true; 
    m_falloff = 1.0;
    m_fps = 20;

    SetBackgroundColour(*wxBLACK);
}

wxStereoScope::~wxStereoScope()
{
}

void wxStereoScope::Stop()
{
    wxMutexLocker lock(m_lock);
    m_timer.Stop();
}

void wxStereoScope::Run()
{
    wxMutexLocker lock(m_lock);
    m_timer.Start(1000/m_fps);
}

void wxStereoScope::Process(short * left, short * right, unsigned int length)
{
    //wxMutexLocker lock(m_lock);
    if (m_right && m_left)
    {
        m_actual = length;
        memcpy(m_right, right, length);
        memcpy(m_left, left, length);
    }
    else
    {
        wxLogDebug("Buffers not allocated.");
        return;
    }

    wxSize size = GetClientSize();
    bool allZero = true;
    int i;
    long s, index, indexTo, step = 512 / size.GetWidth();

    double valL, valR, tmpL, tmpR;

    if (m_left && m_right) {
	index = 0;
	for ( i = 0; i < size.GetWidth(); i++) {
	    indexTo = index + step;

	    if ( m_rubberband ) {
		valL = m_mags[ i ];
		valR = m_mags[ i + size.GetWidth() ];
		if (valL < 0.) {
		    valL += m_falloff;
		    if ( valL > 0. )
			valL = 0.;
		} else {
		    valL -= m_falloff;
		    if ( valL < 0. )
			valL = 0.;
		}
		if (valR < 0.) {
		    valR += m_falloff;
		    if ( valR > 0. )
			valR = 0.;
		} else {
		    valR -= m_falloff;
		    if ( valR < 0. )
			valR = 0.;
		}
	    } else
		valL = valR = 0.;

	    for (s = index; s < indexTo && s < m_actual; s++) {
		tmpL = ( ( m_left ?
			   double( m_left[s] ) : 0.) *
			 double( size.GetHeight() / 4 ) ) / 32768.;
		tmpR = ( ( m_right ?
			   double( m_right[s]) : 0.) *
			 double( size.GetHeight() / 4 ) ) / 32768.;
		if (tmpL > 0)
		    valL = (tmpL > valL) ? tmpL : valL;
		else
		    valL = (tmpL < valL) ? tmpL : valL;
		if (tmpR > 0)
		    valR = (tmpR > valR) ? tmpR : valR;
		else
		    valR = (tmpR < valR) ? tmpR : valR;
	    }

	    if (valL != 0. || valR != 0.)
		allZero = FALSE;

	    m_mags[ i ] = valL;
	    m_mags[ i + size.GetWidth() ] = valR;

	    index = indexTo;
	}
    } else if (m_rubberband) {
	for ( i = 0; i < size.GetWidth(); i++) {
	    valL = m_mags[ i ];
	    if (valL < 0) {
		valL += 2;
		if (valL > 0.)
		    valL = 0.;
	    } else {
		valL -= 2;
		if (valL < 0.)
		    valL = 0.;
	    }

	    valR = m_mags[ i + size.GetWidth() ];
	    if (valR < 0.) {
		valR += m_falloff;
		if (valR > 0.)
		    valR = 0.;
	    } else {
		valR -= m_falloff;
		if (valR < 0.)
		    valR = 0.;
	    }

	    if (valL != 0. || valR != 0.)
		allZero = FALSE;

	    m_mags[ i ] = valL;
	    m_mags[ i + size.GetWidth() ] = valR;
	}
    } else {
	for ( i = 0; (unsigned) i < m_mags.size(); i++ )
	    m_mags[ i ] = 0.;
    }
}

void wxStereoScope::OnSize(wxSizeEvent& event)
{
    wxSize size = GetClientSize();

    unsigned int os = m_mags.size();
    m_mags.resize( size.GetWidth() * 2 );
    for ( ; os < m_mags.size(); os++ )
	    m_mags[os] = 0.0;
}

void wxStereoScope::DoPaint(wxDC * dc)
{
    wxDC & p(*dc);
    wxSize size = GetClientSize();
    wxPen pen(GetBackgroundColour(), 1, wxTRANSPARENT);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    
    p.SetPen(pen);
    p.SetBrush(brush);

    double r, g, b, per;

    p.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
    
    brush.SetStyle(wxTRANSPARENT);
    pen.SetStyle(wxSOLID);
    
    p.SetBrush(brush);
    p.SetPen(pen);

    for ( int i = 1; i < size.GetWidth(); i++ ) {
	// left
	per = double( m_mags[ i ] * 2 ) /
	      double( size.GetHeight() / 4 );
	if (per < 0.0)
	    per = -per;
	if (per > 1.0)
	    per = 1.0;
	else if (per < 0.0)
	    per = 0.0;

	r = m_startColor.Red() + (m_targetColor.Red() -
				m_startColor.Red()) * (per * per);
	g = m_startColor.Green() + (m_targetColor.Green() -
				  m_startColor.Green()) * (per * per);
	b = m_startColor.Blue() + (m_targetColor.Blue() -
				 m_startColor.Blue()) * (per * per);

	if (r > 255.0)
	    r = 255.0;
	else if (r < 0.0)
	    r = 0;

	if (g > 255.0)
	    g = 255.0;
	else if (g < 0.0)
	    g = 0;

	if (b > 255.0)
	    b = 255.0;
	else if (b < 0.0)
	    b = 0;

    pen.SetColour(r, g, b);
	p.SetPen(pen );
	p.DrawLine( i - 1, ( size.GetHeight() / 4 ) + m_mags[ i - 1 ],
		     i, ( size.GetHeight() / 4 ) + m_mags[ i ] );

	// right
	per = double( m_mags[ i + size.GetWidth() ] * 2 ) /
	      double( size.GetHeight() / 4 );
	if (per < 0.0)
	    per = -per;
	if (per > 1.0)
	    per = 1.0;
	else if (per < 0.0)
	    per = 0.0;

    r = m_startColor.Red() + (m_targetColor.Red() -
				m_startColor.Red()) * (per * per);
	g = m_startColor.Green() + (m_targetColor.Green() -
				  m_startColor.Green()) * (per * per);
	b = m_startColor.Blue() + (m_targetColor.Blue() -
				 m_startColor.Blue()) * (per * per);

	if (r > 255.0)
	    r = 255.0;
	else if (r < 0.0)
	    r = 0;

	if (g > 255.0)
	    g = 255.0;
	else if (g < 0.0)
	    g = 0;

	if (b > 255.0)
	    b = 255.0;
	else if (b < 0.0)
	    b = 0;

	pen.SetColour(r, g, b);
	p.SetPen(pen );
	p.DrawLine( i - 1, ( size.GetHeight() * 3 / 4 ) +
		     m_mags[ i + size.GetWidth() - 1 ],
		     i, ( size.GetHeight() * 3 / 4 ) + m_mags[ i + size.GetWidth() ] );
    }

}
