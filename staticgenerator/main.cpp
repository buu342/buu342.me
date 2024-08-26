#include <wx/event.h>
#include "main.h"

Main::Main(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* m_Sizer_Main;
	m_Sizer_Main = new wxBoxSizer(wxVERTICAL);

	m_ChoiceBook_PageSelection = new wxChoicebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT);
	m_Panel_Projects = new wxPanel(m_ChoiceBook_PageSelection, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* m_Sizer_Projects;
	m_Sizer_Projects = new wxBoxSizer(wxVERTICAL);

	m_Splitter_Projects = new wxSplitterWindow(m_Panel_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
	m_Splitter_Projects->SetSashGravity(0);
	m_Splitter_Projects->Connect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_ProjectsOnIdle), NULL, this);

	m_Panel_Projects_Tree = new wxPanel(m_Splitter_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* m_Sizer_Projects_Tree;
	m_Sizer_Projects_Tree = new wxFlexGridSizer(0, 0, 0, 0);
	m_Sizer_Projects_Tree->AddGrowableCol(0);
	m_Sizer_Projects_Tree->AddGrowableRow(0);
	m_Sizer_Projects_Tree->SetFlexibleDirection(wxBOTH);
	m_Sizer_Projects_Tree->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_TreeCtrl_Projects = new wxTreeCtrl(m_Panel_Projects_Tree, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
	m_Sizer_Projects_Tree->Add(m_TreeCtrl_Projects, 0, wxALL|wxEXPAND, 5);

	m_Panel_Projects_Tree->SetSizer(m_Sizer_Projects_Tree);
	m_Panel_Projects_Tree->Layout();
	m_Sizer_Projects_Tree->Fit(m_Panel_Projects_Tree);
	m_Panel_Projects_TextCtrl = new wxPanel(m_Splitter_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* m_Sizer_Projects_TextCtrl;
	m_Sizer_Projects_TextCtrl = new wxFlexGridSizer(0, 1, 0, 0);
	m_Sizer_Projects_TextCtrl->AddGrowableCol(0);
	m_Sizer_Projects_TextCtrl->AddGrowableRow(0);
	m_Sizer_Projects_TextCtrl->SetFlexibleDirection(wxBOTH);
	m_Sizer_Projects_TextCtrl->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_TextCtrl_Projects = new wxRichTextCtrl(m_Panel_Projects_TextCtrl, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS);
	m_Sizer_Projects_TextCtrl->Add(m_TextCtrl_Projects, 1, wxEXPAND | wxALL, 5);

	m_Button_Projects_Preview = new wxButton(m_Panel_Projects_TextCtrl, wxID_ANY, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Projects_TextCtrl->Add(m_Button_Projects_Preview, 0, wxALIGN_RIGHT|wxALL, 5);

	m_Panel_Projects_TextCtrl->SetSizer(m_Sizer_Projects_TextCtrl);
	m_Panel_Projects_TextCtrl->Layout();
	m_Sizer_Projects_TextCtrl->Fit(m_Panel_Projects_TextCtrl);
	m_Splitter_Projects->SplitVertically(m_Panel_Projects_Tree, m_Panel_Projects_TextCtrl, 0);
	m_Sizer_Projects->Add(m_Splitter_Projects, 1, wxEXPAND, 5);

	m_Panel_Projects->SetSizer(m_Sizer_Projects);
	m_Panel_Projects->Layout();
	m_Sizer_Projects->Fit(m_Panel_Projects);
	m_ChoiceBook_PageSelection->AddPage(m_Panel_Projects, wxT("Projects"), false);
	m_Panel_Blog = new wxPanel(m_ChoiceBook_PageSelection, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* m_Sizer_Blog;
	m_Sizer_Blog = new wxBoxSizer(wxVERTICAL);

	m_Splitter_Blog = new wxSplitterWindow(m_Panel_Blog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
	m_Splitter_Blog->Connect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_BlogOnIdle), NULL, this);

	m_Panel_Blog_Tree = new wxPanel(m_Splitter_Blog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* m_Sizer_Blog_Tree;
	m_Sizer_Blog_Tree = new wxFlexGridSizer(0, 0, 0, 0);
	m_Sizer_Blog_Tree->AddGrowableCol(0);
	m_Sizer_Blog_Tree->AddGrowableRow(0);
	m_Sizer_Blog_Tree->SetFlexibleDirection(wxBOTH);
	m_Sizer_Blog_Tree->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_TreeCtrl_Blog = new wxTreeCtrl(m_Panel_Blog_Tree, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
	m_Sizer_Blog_Tree->Add(m_TreeCtrl_Blog, 0, wxALL|wxEXPAND, 5);

	m_Panel_Blog_Tree->SetSizer(m_Sizer_Blog_Tree);
	m_Panel_Blog_Tree->Layout();
	m_Sizer_Blog_Tree->Fit(m_Panel_Blog_Tree);
	m_Panel_Blog_TextCtrl = new wxPanel(m_Splitter_Blog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* m_Sizer_Blog_TextCtrl;
	m_Sizer_Blog_TextCtrl = new wxFlexGridSizer(0, 1, 0, 0);
	m_Sizer_Blog_TextCtrl->AddGrowableCol(0);
	m_Sizer_Blog_TextCtrl->AddGrowableRow(0);
	m_Sizer_Blog_TextCtrl->SetFlexibleDirection(wxBOTH);
	m_Sizer_Blog_TextCtrl->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_TextCtrl_Blog = new wxRichTextCtrl(m_Panel_Blog_TextCtrl, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS);
	m_Sizer_Blog_TextCtrl->Add(m_TextCtrl_Blog, 1, wxEXPAND | wxALL, 5);

	m_Button_Blog_Preview = new wxButton(m_Panel_Blog_TextCtrl, wxID_ANY, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Blog_TextCtrl->Add(m_Button_Blog_Preview, 0, wxALIGN_RIGHT|wxALL, 5);

	m_Panel_Blog_TextCtrl->SetSizer(m_Sizer_Blog_TextCtrl);
	m_Panel_Blog_TextCtrl->Layout();
	m_Sizer_Blog_TextCtrl->Fit(m_Panel_Blog_TextCtrl);
	m_Splitter_Blog->SplitVertically(m_Panel_Blog_Tree, m_Panel_Blog_TextCtrl, 0);
	m_Sizer_Blog->Add(m_Splitter_Blog, 1, wxEXPAND, 5);

	m_Panel_Blog->SetSizer(m_Sizer_Blog);
	m_Panel_Blog->Layout();
	m_Sizer_Blog->Fit(m_Panel_Blog);
	m_ChoiceBook_PageSelection->AddPage(m_Panel_Blog, wxT("Blog"), false);
	m_Sizer_Main->Add(m_ChoiceBook_PageSelection, 1, wxEXPAND | wxALL, 5);

	this->SetSizer(m_Sizer_Main);
	this->Layout();

	m_Menubar_Main = new wxMenuBar(0);
	m_Menu_File = new wxMenu();
	wxMenuItem* m_MenuItem_OpenDir;
	m_MenuItem_OpenDir = new wxMenuItem(m_Menu_File, wxID_ANY, wxString(wxT("Open Working Directory")) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL);
	m_Menu_File->Append(m_MenuItem_OpenDir);

	m_Menubar_Main->Append(m_Menu_File, wxT("File"));

	this->SetMenuBar(m_Menubar_Main);

	this->Centre(wxBOTH);

	m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_File_OnMenuSelection), this, m_MenuItem_OpenDir->GetId());
}

Main::~Main()
{
    
}

void Main::m_Splitter_ProjectsOnIdle(wxIdleEvent&)
{
    this->m_Splitter_Projects->SetSashPosition(0);
	this->m_Splitter_Projects->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_ProjectsOnIdle), NULL, this);
}

void Main::m_Splitter_BlogOnIdle(wxIdleEvent&)
{
	this->m_Splitter_Blog->SetSashPosition(0);
	this->m_Splitter_Blog->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_BlogOnIdle), NULL, this);
}

void Main::m_MenuItem_File_OnMenuSelection(wxCommandEvent& event)
{

}