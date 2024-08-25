#include "htmlpreview.h"

HTMLPreview::HTMLPreview( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* m_Sizer_HTML;
	m_Sizer_HTML = new wxFlexGridSizer( 0, 1, 0, 0 );
	m_Sizer_HTML->AddGrowableCol( 0 );
	m_Sizer_HTML->AddGrowableRow( 0 );
	m_Sizer_HTML->SetFlexibleDirection( wxBOTH );
	m_Sizer_HTML->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_HTMLWin_Main = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	m_Sizer_HTML->Add( m_HTMLWin_Main, 0, wxALL|wxEXPAND, 5 );

	m_Button_Refresh = new wxToggleButton( this, wxID_ANY, wxT("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Sizer_HTML->Add( m_Button_Refresh, 0, wxALIGN_RIGHT|wxALL, 5 );


	this->SetSizer( m_Sizer_HTML );
	this->Layout();

	this->Centre( wxBOTH );
}

HTMLPreview::~HTMLPreview()
{
    
}