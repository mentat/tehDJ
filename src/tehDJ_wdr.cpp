//------------------------------------------------------------------------------
// Source code generated by wxDesigner from file: tehDJ.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "tehDJ_wdr.cpp"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Include private header
#include "tehDJ_wdr.h"


// Implement window functions

wxSizer *djMain( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );
    item1->AddGrowableCol( 0 );

    wxTextCtrl *item2 = new wxTextCtrl( parent, ID_FILENAME, "", wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item3 = new wxButton( parent, ID_LOAD, "Load File", wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item5 = new wxStaticBox( parent, -1, "Location" );
    wxStaticBoxSizer *item4 = new wxStaticBoxSizer( item5, wxVERTICAL );

    wxSlider *item6 = new wxSlider( parent, ID_LOCATION, 0, 0, 1000, wxDefaultPosition, wxSize(100,-1), wxSL_HORIZONTAL );
    item4->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item0->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxFlexGridSizer *item7 = new wxFlexGridSizer( 2, 0, 0 );
    item7->AddGrowableCol( 1 );
    item7->AddGrowableRow( 0 );

    wxWindow *item8 = parent->FindWindow( ID_SPINBIT );
    wxASSERT( item8 );
    item7->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxTextCtrl *item9 = new wxTextCtrl( parent, ID_TEXTCTRL, "", wxDefaultPosition, wxSize(80,120), wxTE_MULTILINE );
    item7->Add( item9, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxFlexGridSizer *item10 = new wxFlexGridSizer( 2, 0, 0 );

    wxButton *item11 = new wxButton( parent, ID_START, "Start", wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item11, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxButton *item12 = new wxButton( parent, ID_STOP, "Stop", wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item12, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}

wxSizer *djPrefsAudio( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );
    item1->AddGrowableCol( 1 );

    wxStaticText *item2 = new wxStaticText( parent, ID_TEXT, "Device:", wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs3[] = 
    {
        "ComboItem"
    };
    wxComboBox *item3 = new wxComboBox( parent, ID_PREF_AUDIO_DEVICE, "", wxDefaultPosition, wxSize(100,-1), 1, strs3, wxCB_DROPDOWN|wxCB_READONLY );
    item1->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxFlexGridSizer *item4 = new wxFlexGridSizer( 2, 0, 0 );
    item4->AddGrowableCol( 1 );

    wxStaticText *item5 = new wxStaticText( parent, ID_TEXT, "Sampling Rate:", wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs6[] = 
    {
        "ComboItem"
    };
    wxComboBox *item6 = new wxComboBox( parent, ID_PREF_AUDIO_RATE, "", wxDefaultPosition, wxSize(100,-1), 1, strs6, wxCB_DROPDOWN );
    item4->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxFlexGridSizer *item7 = new wxFlexGridSizer( 3, 0, 0 );

    wxCheckBox *item8 = new wxCheckBox( parent, ID_PREF_AUDIO_ASIO, "Use ASIO", wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxString strs9[] = 
    {
        "Stereo", 
        "Mono"
    };
    wxRadioBox *item9 = new wxRadioBox( parent, ID_PREF_AUDIO_STEREO, "Channels", wxDefaultPosition, wxDefaultSize, 2, strs9, 1, wxRA_SPECIFY_COLS );
    item7->Add( item9, 0, wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    wxStaticBox *item11 = new wxStaticBox( parent, -1, "Bit Resolution" );
    wxStaticBoxSizer *item10 = new wxStaticBoxSizer( item11, wxVERTICAL );

    wxString strs12[] = 
    {
        "8/sample", 
        "16/sample", 
        "24/sample", 
        "32/sample"
    };
    wxComboBox *item12 = new wxComboBox( parent, ID_PREF_AUDIO_BITS, "", wxDefaultPosition, wxSize(100,-1), 4, strs12, wxCB_DROPDOWN|wxCB_READONLY );
    item10->Add( item12, 0, wxALIGN_CENTRE|wxALL, 5 );

    item7->Add( item10, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    item0->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item14 = new wxStaticBox( parent, -1, "Buffers" );
    wxStaticBoxSizer *item13 = new wxStaticBoxSizer( item14, wxVERTICAL );

    wxFlexGridSizer *item15 = new wxFlexGridSizer( 2, 0, 0 );
    item15->AddGrowableCol( 0 );

    wxSlider *item16 = new wxSlider( parent, ID_PREF_AUDIO_BUFFERS, 4, 1, 200, wxDefaultPosition, wxSize(100,-1), wxSL_HORIZONTAL );
    item15->Add( item16, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item17 = new wxTextCtrl( parent, ID_PREF_AUDIO_BUFFERS_CTRL, "", wxDefaultPosition, wxSize(80,-1), 0 );
    item15->Add( item17, 0, wxALIGN_CENTRE|wxALL, 5 );

    item13->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxFlexGridSizer *item18 = new wxFlexGridSizer( 3, 0, 0 );

    wxStaticBox *item20 = new wxStaticBox( parent, -1, "Samples per buffer" );
    wxStaticBoxSizer *item19 = new wxStaticBoxSizer( item20, wxVERTICAL );

    wxSpinCtrl *item21 = new wxSpinCtrl( parent, ID_PREF_AUDIO_SAMPLES, "0", wxDefaultPosition, wxSize(100,-1), wxSP_WRAP, 1, 10000, 0 );
    item19->Add( item21, 0, wxALIGN_CENTRE|wxALL, 5 );

    item18->Add( item19, 0, wxALIGN_CENTRE|wxRIGHT|wxTOP|wxBOTTOM, 5 );

    wxStaticText *item22 = new wxStaticText( parent, ID_TEXT, "Nominal latency:", wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item22, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxTextCtrl *item23 = new wxTextCtrl( parent, ID_PREF_AUDIO_LATENCY, "10ms", wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item18->Add( item23, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item18, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}

wxSizer *EQ( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 9, 0, 0 );
    item0->AddGrowableRow( 1 );

    wxStaticText *item1 = new wxStaticText( parent, ID_TEXT, "60", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item2 = new wxStaticText( parent, ID_TEXT, "170", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item3 = new wxStaticText( parent, ID_TEXT, "310", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_TEXT, "600", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_TEXT, "1K", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, "3K", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_TEXT, "6K", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item8 = new wxStaticText( parent, ID_TEXT, "12K", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_TEXT, "16K", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxSlider *item10 = new wxSlider( parent, ID_BAND0, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL );
    item0->Add( item10, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxSlider *item11 = new wxSlider( parent, ID_BAND1, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL );
    item0->Add( item11, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxSlider *item12 = new wxSlider( parent, ID_BAND2, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL );
    item0->Add( item12, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxSlider *item13 = new wxSlider( parent, ID_BAND3, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL );
    item0->Add( item13, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxSlider *item14 = new wxSlider( parent, ID_BAND4, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL );
    item0->Add( item14, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxSlider *item15 = new wxSlider( parent, ID_BAND5, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL );
    item0->Add( item15, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxSlider *item16 = new wxSlider( parent, ID_BAND6, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL );
    item0->Add( item16, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxSlider *item17 = new wxSlider( parent, ID_BAND7, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL );
    item0->Add( item17, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxSlider *item18 = new wxSlider( parent, ID_BAND8, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL );
    item0->Add( item18, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}

wxSizer *djMainSmall( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );
    item1->AddGrowableCol( 1 );

    wxTextCtrl *item2 = new wxTextCtrl( parent, ID_SONGTIME, "", wxDefaultPosition, wxSize(60,-1), wxTE_READONLY );
    item1->Add( item2, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxTOP, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_SONGNAME, "", wxDefaultPosition, wxSize(100,-1), wxTE_READONLY );
    item1->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxFlexGridSizer *item4 = new wxFlexGridSizer( 3, 0, 0 );

    wxWindow *item5 = parent->FindWindow( ID_VISUAL );
    wxASSERT( item5 );
    item4->Add( item5, 0, wxALIGN_CENTRE|wxLEFT, 5 );

    wxSlider *item6 = new wxSlider( parent, ID_VOLUME, 70, 0, 100, wxDefaultPosition, wxSize(70,-1), wxSL_HORIZONTAL );
    item4->Add( item6, 0, wxALIGN_CENTRE, 5 );

    wxButton *item7 = new wxButton( parent, ID_EQ_BUTT, "EQ", wxDefaultPosition, wxSize(20,20), 0 );
    item7->SetFont( wxFont( 8, wxROMAN, wxNORMAL, wxNORMAL ) );
    item4->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    item0->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxSlider *item8 = new wxSlider( parent, ID_LOCATION, 0, 0, 1000, wxDefaultPosition, wxSize(100,-1), wxSL_HORIZONTAL );
    item0->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxFlexGridSizer *item9 = new wxFlexGridSizer( 7, 0, 0 );

    wxButton *item10 = new wxButton( parent, ID_BACK, "|<", wxDefaultPosition, wxSize(20,-1), 0 );
    item9->Add( item10, 0, wxALIGN_CENTRE, 5 );

    wxButton *item11 = new wxButton( parent, ID_START, ">", wxDefaultPosition, wxSize(20,-1), 0 );
    item9->Add( item11, 0, wxALIGN_CENTRE, 5 );

    wxButton *item12 = new wxButton( parent, ID_PAUSE, "||", wxDefaultPosition, wxSize(20,-1), 0 );
    item9->Add( item12, 0, wxALIGN_CENTRE, 5 );

    wxButton *item13 = new wxButton( parent, ID_STOP, "[]", wxDefaultPosition, wxSize(20,-1), 0 );
    item9->Add( item13, 0, wxALIGN_CENTRE, 5 );

    wxButton *item14 = new wxButton( parent, ID_FORWARD, ">|", wxDefaultPosition, wxSize(20,-1), 0 );
    item9->Add( item14, 0, wxALIGN_CENTRE, 5 );

    wxButton *item15 = new wxButton( parent, ID_LOAD, "/\\", wxDefaultPosition, wxSize(20,-1), 0 );
    item9->Add( item15, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5 );

    wxGauge *item16 = new wxGauge( parent, ID_BUFF_LEVEL, 100, wxDefaultPosition, wxSize(-1,20), 0 );
    item9->Add( item16, 0, wxALIGN_CENTRE, 5 );

    item0->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    if (set_sizer)
    {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}

wxSizer *EQView( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 9, 0, 0 );
    item0->AddGrowableRow( 0 );

    wxGauge *item1 = new wxGauge( parent, ID_GAUGE0, 100, wxDefaultPosition, wxSize(-1,20), 0 );
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxGauge *item2 = new wxGauge( parent, ID_GAUGE1, 100, wxDefaultPosition, wxSize(-1,20), 0 );
    item0->Add( item2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxGauge *item3 = new wxGauge( parent, ID_GAUGE2, 100, wxDefaultPosition, wxSize(-1,20), 0 );
    item0->Add( item3, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxGauge *item4 = new wxGauge( parent, ID_GAUGE3, 100, wxDefaultPosition, wxSize(-1,20), 0 );
    item0->Add( item4, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxGauge *item5 = new wxGauge( parent, ID_GAUGE4, 100, wxDefaultPosition, wxSize(-1,20), 0 );
    item0->Add( item5, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxGauge *item6 = new wxGauge( parent, ID_GAUGE5, 100, wxDefaultPosition, wxSize(-1,20), 0 );
    item0->Add( item6, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxGauge *item7 = new wxGauge( parent, ID_GAUGE6, 100, wxDefaultPosition, wxSize(-1,20), 0 );
    item0->Add( item7, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxGauge *item8 = new wxGauge( parent, ID_GAUGE7, 100, wxDefaultPosition, wxSize(-1,20), 0 );
    item0->Add( item8, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}

wxSizer *djPrefsMP3( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item1 = new wxStaticText( parent, ID_TEXT, "Decoding Engine:", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs2[] = 
    {
        "MAD 0.14.2 BETA"
    };
    wxChoice *item2 = new wxChoice( parent, ID_PREF_MP3_ENGINE, wxDefaultPosition, wxSize(100,-1), 1, strs2, 0 );
    item0->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item3 = new wxStaticText( parent, ID_TEXT, "Thread Priority:", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs4[] = 
    {
        "Normal", 
        "High", 
        "Low"
    };
    wxChoice *item4 = new wxChoice( parent, ID_PREF_MP3_THREAD, wxDefaultPosition, wxSize(100,-1), 3, strs4, 0 );
    item0->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_TEXT, "Dithering Technique:", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs6[] = 
    {
        "Linear"
    };
    wxChoice *item6 = new wxChoice( parent, ID_PREF_MP3_DITHER, wxDefaultPosition, wxSize(100,-1), 1, strs6, 0 );
    item0->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}

wxSizer *djPrefs( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxNotebook *item2 = new wxNotebook( parent, ID_NOTEBOOK, wxDefaultPosition, wxSize(200,160), wxNB_LEFT );
    wxNotebookSizer *item1 = new wxNotebookSizer( item2 );

    wxPanel *item3 = new wxPanel( item2, -1 );
    djPrefsAudio( item3, FALSE );
    item2->AddPage( item3, "Audio" );

    wxPanel *item4 = new wxPanel( item2, -1 );
    djPrefsMP3( item4, FALSE );
    item2->AddPage( item4, "Layer III" );

    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}

// Implement menubar functions

// Implement toolbar functions

// Implement bitmap functions


// End of generated file