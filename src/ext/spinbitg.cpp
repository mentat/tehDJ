///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/spinctlg.cpp
// Purpose:     implements wxSpinCtrl as a composite control
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.01.01
// RCS-ID:      $Id: spinctlg.cpp,v 1.19 2002/08/31 22:52:24 VZ Exp $
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "spinbitg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
#endif //WX_PRECOMP


#include "spinbitg.h"
#include "math.h"

IMPLEMENT_DYNAMIC_CLASS(wxSpinBitmap, wxWindow)

BEGIN_EVENT_TABLE(wxSpinBitmap, wxWindow)
    EVT_PAINT(wxSpinBitmap::OnPaint)
    EVT_LEFT_DOWN(wxSpinBitmap::OnLMouseDown)
    EVT_LEFT_UP(wxSpinBitmap::OnLMouseUp)
    EVT_MOTION(wxSpinBitmap::OnMouseMotion)
    EVT_LEAVE_WINDOW(wxSpinBitmap::OnMouseLeave)
    EVT_MOUSEWHEEL(wxSpinBitmap::OnMouseScroll)
    EVT_CHAR(wxSpinBitmap::OnChar)
END_EVENT_TABLE()

void wxSpinBitmap::Init()
{
    m_min = 0;
    m_max = 100;
    m_inc = 1;
    m_ldown = false;
    m_wrap = false;
   
}

bool wxSpinBitmap::Create(wxWindow *parent,
                        wxWindowID id,
                        wxBitmap& bitmap,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        long min,
                        long max,
                        long initial)
{
    if ( !wxWindow::Create(parent, id, pos, size, style) )

    {
        wxLogDebug("Could not create!");
        return FALSE;
    }

    mBitmap = bitmap;

    wxLogDebug("In constructor");
    Resize(mBitmap.GetWidth(), mBitmap.GetHeight());
   
    return TRUE;
}

void wxSpinBitmap::Resize(int x, int y)
{
    SetSize(x, y);
    SetSizeHints(x, y, x, y);
    SetAutoLayout(TRUE);
    Layout();

}
#if 0
bool wxSpinBitmap::Show(bool show)
{
    if (m_visible && !show)
    {
        m_visible = false;
        return true;
    }
    if (m_visible && show)
        return false;

    if (!m_visible && !show)
        return false;

    m_visible = true;

    return true;

}

bool wxSpinBitmap::Enable(bool enable)
{
    if (m_enabled && !enable)
    {
        m_enabled = false;
        return true;
    }
    if (m_enabled && enable)
        return false;

    if (!m_enabled && !enable)
        return false;

    m_enabled = true;
    return true;



}
#endif

wxSpinBitmap::~wxSpinBitmap()
{

}

void wxSpinBitmap::OnPaint(wxPaintEvent & event)
{
    wxPaintDC dc(this);
    dc.DrawBitmap(mBitmap, 0,0,true);
#if 0
    float pi = 3.1415926535897932;
    wxLogDebug("OnPaint");
    int w, h;
    GetClientSize(&w, &h);
    
    wxPaintDC dc(this);
    dc.SetBrush(*wxBLACK_BRUSH);
    wxPen knoby(*wxBLACK, 1, wxSHORT_DASH);
    dc.SetPen(knoby);
    dc.DrawCircle(w/2,
		         h/2,
		         w/2);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    wxPen mypen(*wxWHITE, 2, wxSOLID);
    dc.SetPen(mypen);
    dc.DrawLine(w/2, h/2, w, h/2); 
#endif

}

void wxSpinBitmap::RotatedBitmap2(float radians)
{
    //CDC Bitmap_DC;
    wxMemoryDC srcDC;
    srcDC.SelectObject(mBitmap);

    wxBitmap dstBit;
    dstBit.SetHeight(mBitmap.GetHeight());
    dstBit.SetWidth(mBitmap.GetWidth());
    dstBit.SetDepth(mBitmap.GetDepth());

    wxMemoryDC dstDC;
    dstDC.SelectObject(dstBit);

    wxPoint Center(mBitmap.GetWidth()/2, mBitmap.GetHeight()/2);

    //Bitmap_DC.CreateCompatibleDC( NULL );
    //Bitmap_DC.SelectObject(Source);

    int Height = mBitmap.GetHeight();
    int Width = mBitmap.GetWidth();
    int XX = 0;
    int YY = 0;
    
    //COLORREF C[4];
    wxColor C[4];
    double Pi = 3.14159265359;
    int c1x;
    int c1y;
    float a=0;
    int r;
    int p1x;
    int p1y;
    int p2x;
    int p2y;
    int n;
    float cosine = (float)cos(radians);
    float sine = (float)sin(radians);
    int modX = (int)(Center.x*cosine + Center.y*sine);
    int modY = (int)(Center.y*cosine - Center.x*sine);

    c1x = Width / 2;
    c1y = Height / 2;
    n = sqrt((c1x * c1x) + (c1y * c1y));
    for(p2x = 0;p2x<n;p2x++)
    {
        for(p2y=0;p2y<n;p2y++)
        {
            if(p2x==0)
                a=Pi/2.0;
                else 
                    a=tanh(p2y/(p2x*1.0));

            r = sqrt(p2x*p2x + p2y*p2y);
            p1x = (int)(p2x*cosine - p2y*sine);
            p1y = (int)(p2y*cosine + p2x*sine);
            //C[0] = Bitmap_DC.GetPixel(c1x + p1x,c1y + p1y);
            //C[1] = Bitmap_DC.GetPixel(c1x - p1x,c1y - p1y);
            //C[2] = Bitmap_DC.GetPixel(c1x + p1y,c1y - p1x);
            //C[3] = Bitmap_DC.GetPixel(c1x - p1y,c1y + p1x);
            srcDC.GetPixel(c1x + p1x,c1y + p1y, &C[0]);
            srcDC.GetPixel(c1x - p1x,c1y - p1y, &C[1]);
            srcDC.GetPixel(c1x + p1y,c1y - p1x, &C[2]);
            srcDC.GetPixel(c1x - p1y,c1y + p1x, &C[3]);
            wxPen newPix;

            dstDC.SetPen(newPix);

            newPix.SetColour(C[0]);
            dstDC.DrawPoint(c1x + p2x+XX+modX, c1y + p2y+YY+modY);
            
            newPix.SetColour(C[1]);
            dstDC.DrawPoint(c1x - p2x+XX+modX, c1y - p2y+YY+modY);

            newPix.SetColour(C[2]);
            dstDC.DrawPoint(c1x + p2y+XX+modX, c1y - p2x+YY+modY);

            newPix.SetColour(C[3]);
            dstDC.DrawPoint(c1x - p2y+XX+modX, c1y + p2x+YY+modY);


            //if(C[0] !=clrBack&&C[0] !=-1) 
                //SetPixel(theHDC,c1x + p2x+XX+modX, c1y + p2y+YY+modY, C[0]);
            //if(C[1] !=clrBack&&C[1] !=-1) 
                //SetPixel(theHDC,c1x - p2x+XX+modX, c1y - p2y+YY+modY, C[1]);
            //if(C[2] !=clrBack&&C[2] !=-1) 
                //SetPixel(theHDC,c1x + p2y+XX+modX, c1y - p2x+YY+modY, C[2]);
            //if(C[3] !=clrBack&&C[3] !=-1) 
                //SetPixel(theHDC,c1x - p2y+XX+modX, c1y + p2x+YY+modY, C[3]);
        }
    }

    mBitmap = dstBit;
} 



// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------
#if 0
wxSize wxSpinBitmap::DoGetBestSize() const
{
    wxSize sz(mBitmap.GetWidth(), mBitmap.GetHeight());

    return sz;
}

#endif


void wxSpinBitmap::SetValue(long val)
{
 

}

void wxSpinBitmap::OnLMouseDown(wxMouseEvent& event)
{
    wxLogDebug("oh");
    if (!m_ldown)
        m_ldown = true;
    RotatedBitmap2(1); 
}

void wxSpinBitmap::OnLMouseUp(wxMouseEvent & event)
{

    m_ldown = false;
}

void wxSpinBitmap::OnMouseLeave(wxMouseEvent & event)
{

    m_ldown = false;
}

void wxSpinBitmap::OnMouseScroll(wxMouseEvent & event)
{

}

void wxSpinBitmap::OnMouseMotion(wxMouseEvent & event)
{
    

}

void wxSpinBitmap::OnChar(wxKeyEvent & event)
{


    int key = event.GetKeyCode();

    switch (key){

        case(WXK_DOWN): Rotate(false); break;
        case(WXK_UP): Rotate(); break;
        default: break;
    }
}

void wxSpinBitmap::Rotate(bool clockwise)
{
    if (clockwise)
        SetValue(++m_val);
    else
        SetValue(--m_val);

}



