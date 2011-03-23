/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/spinctlg.h
// Purpose:     generic wxSpinCtrl class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     28.10.99
// RCS-ID:      $Id: spinctlg.h,v 1.14 2002/08/31 22:52:24 VZ Exp $
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_SPINBIT_H_
#define _WX_GENERIC_SPINBIT_H_



#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "spinbitg.h"
#endif

// ----------------------------------------------------------------------------
// wxSpinCtrl is a combination of wxTextCtrl and wxSpinButton
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSpinBitmap : public wxWindow
{
public:
    wxSpinBitmap() { Init(); }

    wxSpinBitmap(wxWindow *parent,
               wxWindowID id = -1,
               wxBitmap& bitmap = wxNullBitmap,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               long min = 0, long max = 100, long initial = 0)
    {
        Init();
        Create(parent, id, bitmap, pos, size, style, min, max, initial);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                wxBitmap& bitmap = wxNullBitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                long min = 0, long max = 100, long initial = 0);

    virtual ~wxSpinBitmap();

    // accessors
    long GetValue() const { return m_val; }
    long GetMin() const { return m_min; }
    long GetMax() const { return m_max; }

    // operations
    void SetValue(long val);
    void SetRange(long minVal, long maxVal)
    {
        m_min = minVal;
        m_max = maxVal;
    }
    void SetInc(long incVal)
    {
        m_inc = incVal;
    }
    void SetWrap(bool wrap)
    {
        m_wrap = wrap;
    }

    void SetBitmap(wxBitmap& bitmap);

    // implementation from now on

    // forward these functions to all subcontrols
  //  virtual bool Enable(bool enable = TRUE);
  //  virtual bool Show(bool show = TRUE);

    void OnLMouseDown(wxMouseEvent& event);
    void OnLMouseUp(wxMouseEvent & event);
    void OnMouseLeave(wxMouseEvent & event);
    void OnMouseScroll(wxMouseEvent & event);
    void OnMouseMotion(wxMouseEvent & event);
    void OnChar(wxKeyEvent & event);

    void OnPaint(wxPaintEvent & event);

protected:
   
    void RotatedBitmap2(float radians);
    void Resize(int x, int y);
    // override the base class virtuals involved into geometry calculations
   // virtual wxSize DoGetBestSize() const;
    //virtual void DoMoveWindow(int x, int y, int width, int height);

    // common part of all ctors
    void Init();

    void Rotate(bool clockwise = true);

private:


    bool m_wrap;
    bool m_ldown;
    
    long m_val;
    long m_min;
    long m_max;
    long m_inc;

    wxBitmap mBitmap;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxSpinBitmap)
};



#endif // _WX_GENERIC_SPINCTRL_H_

