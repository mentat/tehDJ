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

#ifndef _WX_GL_BUTTON_H_
#define _WX_GL_BUTTON_H_



#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "glbutton.h"
#endif

#include "wx/glcanvas.h"

extern "C" {
#include "lw.h"
#include "trackball.h"
}

/* information needed to display lightwave mesh */
typedef struct 
{
//  gint do_init;         /* true if initgl not yet called */
  int do_init;
  lwObject *lwobject;   /* lightwave object mesh */
  float beginx,beginy;  /* position of mouse */
  float quat[4];        /* orientation of object */
  float zoom;           /* field of view in degrees */
} mesh_info;

// ----------------------------------------------------------------------------
// wxSpinCtrl is a combination of wxTextCtrl and wxSpinButton
// ----------------------------------------------------------------------------
class wxGLControl: public wxGLCanvas
{
public:
   wxGLControl(wxWindow *parent, const wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "TestGLCanvas");
   
   ~wxGLControl();

   void OnPaint(wxPaintEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnEraseBackground(wxEraseEvent& event);
   void LoadLWO( const wxString &filename);
   void OnMouse( wxMouseEvent& event );
   void InitGL(void);
   
   mesh_info  info;
   bool       block;

DECLARE_EVENT_TABLE()
};


#endif // _WX_GENERIC_SPINCTRL_H_

