#pragma warning(disable:4267)
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
#error Please set wxUSE_GLCANVAS to 1 in setup.h.
#endif

#include "glbutton.h"

#ifdef __WXMAC__
#  ifdef __DARWIN__
#    include <OpenGL/glu.h>
#  else
#    include <glu.h>
#  endif
#else
#  include <GL/glu.h>
#endif

#define VIEW_ASPECT 1.3

BEGIN_EVENT_TABLE(wxGLControl, wxGLCanvas)
    EVT_SIZE(wxGLControl::OnSize)
    EVT_PAINT(wxGLControl::OnPaint)
    EVT_ERASE_BACKGROUND(wxGLControl::OnEraseBackground)
    EVT_MOUSE_EVENTS(wxGLControl::OnMouse)
END_EVENT_TABLE()

wxGLControl::wxGLControl(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name):
  wxGLCanvas(parent, id, pos, size, style, name)
{
   block = FALSE;
}

wxGLControl::~wxGLControl(void)
{
    /* destroy mesh */
    lw_object_free(info.lwobject);
}

void wxGLControl::OnPaint( wxPaintEvent& event )
{
    /* must always be here */
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();
    
    /* initialize OpenGL */
    if (info.do_init == TRUE) 
    {
        InitGL();
        info.do_init = FALSE;
    }
  
    /* view */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( info.zoom, VIEW_ASPECT, 1, 100 );
    glMatrixMode( GL_MODELVIEW );

    /* clear */
    glClearColor( .3, .4, .6, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* transformations */
    GLfloat m[4][4];
    glLoadIdentity();
    glTranslatef( 0, 0, -30 );
    build_rotmatrix( m,info.quat );
    glMultMatrixf( &m[0][0] );

    /* draw object */
    lw_object_show( info.lwobject );
    
    /* flush */
    glFlush();

    /* swap */
    SwapBuffers();
}

void wxGLControl::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);
    
    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);
#ifndef __WXMOTIF__
    if (GetContext())
#endif
    {
        SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }
}

void wxGLControl::OnEraseBackground(wxEraseEvent& event)
{
    /* Do nothing, to avoid flashing on MSW */
}

void wxGLControl::LoadLWO(const wxString &filename)
{
    /* test if lightwave object */
    if (!lw_is_lwobject(filename)) return;
  
    /* read lightwave object */
    lwObject *lwobject = lw_object_read(filename);
    
    /* scale */
    lw_object_scale(lwobject, 10.0 / lw_object_radius(lwobject));
    
    /* set up mesh info */
    info.do_init = TRUE;
    info.lwobject = lwobject;
    info.beginx = 0;
    info.beginy = 0;
    info.zoom   = 35;
    trackball( info.quat, 0.0, 0.0, 0.0, 0.0 );
}

void wxGLControl::OnMouse( wxMouseEvent& event )
{
    wxSize sz(GetClientSize());
    if (event.Dragging())
    {
        /* drag in progress, simulate trackball */
        float spin_quat[4];
        trackball(spin_quat,
          (2.0*info.beginx -       sz.x) / sz.x,
          (     sz.y - 2.0*info.beginy) / sz.y,
          (     2.0*event.GetX() - sz.x) / sz.x,
          (    sz.y - 2.0*event.GetY()) / sz.y);
          
        add_quats( spin_quat, info.quat, info.quat );

        /* orientation has changed, redraw mesh */
    Refresh(FALSE);
    }

    info.beginx = event.GetX();
    info.beginy = event.GetY();
}

void wxGLControl::InitGL(void)
{
    GLfloat light0_pos[4]   = { -50.0, 50.0, 0.0, 0.0 };
    GLfloat light0_color[4] = { .6, .6, .6, 1.0 }; /* white light */
    GLfloat light1_pos[4]   = {  50.0, 50.0, 0.0, 0.0 };
    GLfloat light1_color[4] = { .4, .4, 1, 1.0 };  /* cold blue light */

    /* remove back faces */
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
  
    /* speedups */
    glEnable(GL_DITHER);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

    /* light */
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_color);  
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_color);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);  
}