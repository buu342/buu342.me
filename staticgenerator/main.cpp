/***************************************************************
                            main.cpp

The main code of the static site generator
***************************************************************/

#include "main.h"
#include "helper.h"
#include "json.h"
#include "include/md4c/md4c-html.h"
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/treelist.h>
#include <wx/dir.h>
#include <wx/textfile.h>
#include <wx/dirdlg.h>


/*==============================
    Main (Constructor)
    Initializes the class
==============================*/

Main::Main(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    // Initialize the program state
    this->m_WorkingDir = wxGetCwd();
    this->m_SelectedItem = NULL;
    this->MarkModified(false);

    // Create the main frame elements
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    wxBoxSizer* m_Sizer_Main;
    m_Sizer_Main = new wxBoxSizer(wxVERTICAL);
    this->m_ChoiceBook_PageSelection = new wxChoicebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT);

    // Create the main project panels
    this->m_Panel_Projects = new wxPanel(this->m_ChoiceBook_PageSelection, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* m_Sizer_Projects;
    m_Sizer_Projects = new wxBoxSizer(wxVERTICAL);
    this->m_Splitter_Projects = new wxSplitterWindow(this->m_Panel_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    this->m_Splitter_Projects->SetSashGravity(0);
    this->m_Splitter_Projects->Connect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_ProjectsOnIdle), NULL, this);

    // Create the project tree
    this->m_Panel_Projects_Tree = new wxPanel(this->m_Splitter_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_Projects_Tree;
    m_Sizer_Projects_Tree = new wxFlexGridSizer(0, 0, 0, 0);
    m_Sizer_Projects_Tree->AddGrowableCol(0);
    m_Sizer_Projects_Tree->AddGrowableRow(0);
    m_Sizer_Projects_Tree->SetFlexibleDirection(wxBOTH);
    m_Sizer_Projects_Tree->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_TreeCtrl_Projects = new wxTreeCtrl(this->m_Panel_Projects_Tree, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_HIDE_ROOT);
    m_Sizer_Projects_Tree->Add(this->m_TreeCtrl_Projects, 0, wxALL|wxEXPAND, 5);
    this->m_Panel_Projects_Tree->SetSizer(m_Sizer_Projects_Tree);
    this->m_Panel_Projects_Tree->Layout();
    m_Sizer_Projects_Tree->Fit(this->m_Panel_Projects_Tree);

    // Create the primary project editor
    this->m_Panel_Projects_Editor = new wxPanel(this->m_Splitter_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_Projects_Editor;
    m_Sizer_Projects_Editor = new wxFlexGridSizer(3, 1, 0, 0);
    m_Sizer_Projects_Editor->AddGrowableCol(0);
    m_Sizer_Projects_Editor->AddGrowableRow(0);
    m_Sizer_Projects_Editor->AddGrowableRow(1);
    m_Sizer_Projects_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_Projects_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
    this->m_ScrolledWindow_Project_Editor = new wxScrolledWindow(this->m_Panel_Projects_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    this->m_ScrolledWindow_Project_Editor->SetScrollRate(5, 5);
    this->m_ScrolledWindow_Project_Editor->Hide();
    this->m_ScrolledWindow_Project_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_Editor;
    m_Sizer_ScrolledWindow_Editor = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_ScrolledWindow_Editor->AddGrowableCol(0);
    m_Sizer_ScrolledWindow_Editor->AddGrowableRow(1);
    m_Sizer_ScrolledWindow_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_Editor_Main;
    m_Sizer_ScrolledWindow_Editor_Main = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ScrolledWindow_Editor_Main->AddGrowableCol(1);
    m_Sizer_ScrolledWindow_Editor_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_Editor_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_Editor_Main->SetMinSize(wxSize(0,0));

    // Create the project page editor controls 
    this->m_Label_Projects_File = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("File:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_File->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_File, 0, wxALL, 5);
    this->m_TextCtrl_Projects_File = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_File->SetToolTip(wxT("The filename of the project"));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_File, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_Name = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Display Name:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_Name->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_Name, 0, wxALL, 5);
    this->m_TextCtrl_Projects_Name = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_Name->SetToolTip(wxT("The display name of the project"));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_Name, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_Icon = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Icon:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_Icon->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_Icon, 0, wxALL, 5);
    this->m_TextCtrl_Projects_Icon = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_Icon->SetToolTip(wxT("The icon used to display the project in the list of projects. Requires a relative path (to the working directory) to a 240x125 png."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_Icon, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_Tags = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Tags:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_Tags->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_Tags, 0, wxALL, 5);
    this->m_TextCtrl_Projects_Tags = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_Tags->SetToolTip(wxT("Project tags, comma separated."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_Tags, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_Images = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Images:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_Images->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_Images, 0, wxALL, 5);
    this->m_TextCtrl_Projects_Images = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_Images->SetToolTip(wxT("Images for this project, comma separated. Paths should be relative to the working directory."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_Images, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_Date = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Date:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_Date->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_Date, 0, wxALL|wxEXPAND, 5);
    this->m_TextCtrl_Projects_Date = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_Date->SetToolTip(wxT("The date of this project's release."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_Date, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_URLs = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("URLs:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_URLs->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_URLs, 0, wxALL, 5);
    this->m_TextCtrl_Projects_URLs = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_URLs->SetToolTip(wxT("URLs for this project, comma separated."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_URLs, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_ToolTip = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Subtitle:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_ToolTip->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_ToolTip, 0, wxALL, 5);
    this->m_TextCtrl_Projects_ToolTip = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_ToolTip->SetToolTip(wxT("Brief description that shows up when hovering over a project with the mouse."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_ToolTip, 0, wxALL|wxEXPAND, 5);
    m_Sizer_ScrolledWindow_Editor->Add(m_Sizer_ScrolledWindow_Editor_Main, 1, wxEXPAND, 5);
    this->m_TextCtrl_Projects_Description = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1,-1), wxTE_MULTILINE);
    this->m_TextCtrl_Projects_Description->SetToolTip(wxT("The project description. Supports markdown."));
    this->m_TextCtrl_Projects_Description->SetMinSize(wxSize(-1,200));
    m_Sizer_ScrolledWindow_Editor->Add(this->m_TextCtrl_Projects_Description, 0, wxALL|wxEXPAND, 5);
    this->m_ScrolledWindow_Project_Editor->SetSizer(m_Sizer_ScrolledWindow_Editor);
    this->m_ScrolledWindow_Project_Editor->Layout();
    m_Sizer_ScrolledWindow_Editor->Fit(this->m_ScrolledWindow_Project_Editor);
    m_Sizer_Projects_Editor->Add(this->m_ScrolledWindow_Project_Editor, 0, wxALL|wxEXPAND, 5);

    // Create the project category editor 
    this->m_ScrolledWindow_ProjectCategory_Editor = new wxScrolledWindow(this->m_Panel_Projects_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    this->m_ScrolledWindow_ProjectCategory_Editor->SetScrollRate(5, 5);
    this->m_ScrolledWindow_ProjectCategory_Editor->Hide();
    this->m_ScrolledWindow_ProjectCategory_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_ProjectCategory_Editor;
    m_Sizer_ScrolledWindow_ProjectCategory_Editor = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->AddGrowableCol(0);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->AddGrowableRow(1);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main;
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->AddGrowableCol(1);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->SetMinSize(wxSize(0,0));

    // Create the project category editor controls 
    this->m_Label_ProjectsCategory_Folder = new wxStaticText(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxT("Folder:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_ProjectsCategory_Folder->Wrap(-1);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(this->m_Label_ProjectsCategory_Folder, 0, wxALL, 5);
    this->m_TextCtrl_ProjectsCategory_Folder = new wxTextCtrl(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ProjectsCategory_Folder->Enable(false);
    this->m_TextCtrl_ProjectsCategory_Folder->SetToolTip(wxT("The folder that represents this category"));
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(this->m_TextCtrl_ProjectsCategory_Folder, 0, wxALL|wxEXPAND, 5);
    this->m_Label_ProjectsCategory_DisplayName = new wxStaticText(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxT("Display Name:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_ProjectsCategory_DisplayName->Wrap(-1);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(this->m_Label_ProjectsCategory_DisplayName, 0, wxALL, 5);
    this->m_TextCtrl_ProjectsCategory_DisplayName = new wxTextCtrl(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ProjectsCategory_DisplayName->SetToolTip(wxT("The display name of this category"));
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(this->m_TextCtrl_ProjectsCategory_DisplayName, 0, wxALL|wxEXPAND, 5);
    this->m_Label_ProjectsCategory_Description = new wxStaticText(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_ProjectsCategory_Description->Wrap(-1);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(this->m_Label_ProjectsCategory_Description, 0, wxALL, 5);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(0, 0, 1, wxEXPAND, 5);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->Add(m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main, 1, wxEXPAND, 5);
    this->m_TextCtrl_ProjectsCategory_Description = new wxTextCtrl(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    this->m_TextCtrl_ProjectsCategory_Description->SetToolTip(wxT("The filename of the project"));
    this->m_TextCtrl_ProjectsCategory_Description->SetMinSize(wxSize(-1,200));
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->Add(this->m_TextCtrl_ProjectsCategory_Description, 0, wxALL|wxEXPAND, 5);
    this->m_ScrolledWindow_ProjectCategory_Editor->SetSizer(m_Sizer_ScrolledWindow_ProjectCategory_Editor);
    this->m_ScrolledWindow_ProjectCategory_Editor->Layout();
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->Fit(this->m_ScrolledWindow_ProjectCategory_Editor);
    m_Sizer_Projects_Editor->Add(this->m_ScrolledWindow_ProjectCategory_Editor, 0, wxEXPAND | wxALL, 5);

    // Create the project preview button
    this->m_Button_Projects_Preview = new wxButton(this->m_Panel_Projects_Editor, wxID_ANY, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Button_Projects_Preview->Hide();
    this->m_Button_Projects_Preview->SetMinSize(wxSize(-1,24));
    m_Sizer_Projects_Editor->Add(this->m_Button_Projects_Preview, 0, wxALIGN_RIGHT|wxALL, 5);

    // Add everything to the projects panel
    this->m_Panel_Projects_Editor->SetSizer(m_Sizer_Projects_Editor);
    this->m_Panel_Projects_Editor->Layout();
    m_Sizer_Projects_Editor->Fit(this->m_Panel_Projects_Editor);
    this->m_Splitter_Projects->SplitVertically(this->m_Panel_Projects_Tree, this->m_Panel_Projects_Editor, 200);
    m_Sizer_Projects->Add(this->m_Splitter_Projects, 1, wxEXPAND, 5);
    this->m_Panel_Projects->SetSizer(m_Sizer_Projects);
    this->m_Panel_Projects->Layout();
    m_Sizer_Projects->Fit(this->m_Panel_Projects);
    this->m_ChoiceBook_PageSelection->AddPage(this->m_Panel_Projects, wxT("Projects"), false);

    // Create the main blog panels
    this->m_Panel_Blog = new wxPanel(this->m_ChoiceBook_PageSelection, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* m_Sizer_Blog;
    m_Sizer_Blog = new wxBoxSizer(wxVERTICAL);
    this->m_Splitter_Blog = new wxSplitterWindow(this->m_Panel_Blog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    this->m_Splitter_Blog->SetSashGravity(0);
    this->m_Splitter_Blog->Connect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_BlogOnIdle), NULL, this);

    // Create the blog tree
    this->m_Panel_Blog_Tree = new wxPanel(this->m_Splitter_Blog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_Blog_Tree;
    m_Sizer_Blog_Tree = new wxFlexGridSizer(0, 0, 0, 0);
    m_Sizer_Blog_Tree->AddGrowableCol(0);
    m_Sizer_Blog_Tree->AddGrowableRow(0);
    m_Sizer_Blog_Tree->SetFlexibleDirection(wxBOTH);
    m_Sizer_Blog_Tree->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_TreeCtrl_Blog = new wxTreeCtrl(this->m_Panel_Blog_Tree, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_HIDE_ROOT);
    m_Sizer_Blog_Tree->Add(this->m_TreeCtrl_Blog, 0, wxALL|wxEXPAND, 5);
    this->m_Panel_Blog_Tree->SetSizer(m_Sizer_Blog_Tree);
    this->m_Panel_Blog_Tree->Layout();
    m_Sizer_Blog_Tree->Fit(this->m_Panel_Blog_Tree);

    // Create the primary blog editor
    this->m_Panel_Blog_Editor = new wxPanel(this->m_Splitter_Blog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_Blog_Editor;
    m_Sizer_Blog_Editor = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_Blog_Editor->AddGrowableCol(0);
    m_Sizer_Blog_Editor->AddGrowableRow(0);
    m_Sizer_Blog_Editor->AddGrowableRow(1);
    m_Sizer_Blog_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_Blog_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_ScrolledWindow_Blog_Editor = new wxScrolledWindow(this->m_Panel_Blog_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    this->m_ScrolledWindow_Blog_Editor->SetScrollRate(5, 5);
    this->m_ScrolledWindow_Blog_Editor->Hide();
    this->m_ScrolledWindow_Blog_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_Blog_Editor;
    m_Sizer_ScrolledWindow_Blog_Editor = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_ScrolledWindow_Blog_Editor->AddGrowableCol(0);
    m_Sizer_ScrolledWindow_Blog_Editor->AddGrowableRow(1);
    m_Sizer_ScrolledWindow_Blog_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_Blog_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_Blog_Editor->SetMinSize(wxSize(0,0));

    // Create the blog page editor controls 
    wxFlexGridSizer* m_Sizer_Blog_Editor_Details;
    m_Sizer_Blog_Editor_Details = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_Blog_Editor_Details->AddGrowableCol(1);
    m_Sizer_Blog_Editor_Details->SetFlexibleDirection(wxBOTH);
    m_Sizer_Blog_Editor_Details->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_Label_Blog_File = new wxStaticText(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxT("File:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Blog_File->Wrap(-1);
    m_Sizer_Blog_Editor_Details->Add(this->m_Label_Blog_File, 0, wxALL, 5);
    this->m_TextCtrl_Blog_File = new wxTextCtrl(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Blog_File->SetToolTip(wxT("The filename of the project"));
    m_Sizer_Blog_Editor_Details->Add(this->m_TextCtrl_Blog_File, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Blog_Name = new wxStaticText(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxT("Display Name:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Blog_Name->Wrap(-1);
    m_Sizer_Blog_Editor_Details->Add(this->m_Label_Blog_Name, 0, wxALL, 5);
    this->m_TextCtrl_Blog_Name = new wxTextCtrl(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Blog_Name->SetToolTip(wxT("The display name of the project"));
    m_Sizer_Blog_Editor_Details->Add(this->m_TextCtrl_Blog_Name, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Blog_Icon = new wxStaticText(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxT("Icon:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Blog_Icon->Wrap(-1);
    m_Sizer_Blog_Editor_Details->Add(this->m_Label_Blog_Icon, 0, wxALL, 5);
    this->m_TextCtrl_Blog_Icon = new wxTextCtrl(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Blog_Icon->SetToolTip(wxT("The icon used to display the project in the list of projects. Requires a relative path (to the working directory) to a 240x125 png."));
    m_Sizer_Blog_Editor_Details->Add(this->m_TextCtrl_Blog_Icon, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Blog_ToolTip = new wxStaticText(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxT("Subtitle:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Blog_ToolTip->Wrap(-1);
    m_Sizer_Blog_Editor_Details->Add(this->m_Label_Blog_ToolTip, 0, wxALL, 5);
    this->m_TextCtrl_Blog_ToolTip = new wxTextCtrl(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Blog_ToolTip->SetToolTip(wxT("Brief description that shows up when hovering over a project with the mouse."));
    m_Sizer_Blog_Editor_Details->Add(this->m_TextCtrl_Blog_ToolTip, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Blog_Date = new wxStaticText(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxT("Date:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Blog_Date->Wrap(-1);
    m_Sizer_Blog_Editor_Details->Add(this->m_Label_Blog_Date, 0, wxALL, 5);
    this->m_TextCtrl_Blog_Date = new wxTextCtrl(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Blog_Date->SetToolTip(wxT("Brief description that shows up when hovering over a project with the mouse."));
    m_Sizer_Blog_Editor_Details->Add(this->m_TextCtrl_Blog_Date, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Blog_Tags = new wxStaticText(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxT("Tags:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Blog_Tags->Wrap(-1);
    m_Sizer_Blog_Editor_Details->Add(this->m_Label_Blog_Tags, 0, wxALL, 5);
    this->m_TextCtrl_Blog_Tags = new wxTextCtrl(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Blog_Tags->SetToolTip(wxT("Project tags, comma separated."));
    m_Sizer_Blog_Editor_Details->Add(this->m_TextCtrl_Blog_Tags, 0, wxALL|wxEXPAND, 5);
    m_Sizer_ScrolledWindow_Blog_Editor->Add(m_Sizer_Blog_Editor_Details, 1, wxEXPAND, 5);
    this->m_TextCtrl_Blog = new wxTextCtrl(this->m_ScrolledWindow_Blog_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1,200), wxTE_MULTILINE);
    this->m_TextCtrl_Blog->SetToolTip(wxT("The project description. Supports markdown."));
    m_Sizer_ScrolledWindow_Blog_Editor->Add(this->m_TextCtrl_Blog, 0, wxALL|wxEXPAND, 5);
    this->m_ScrolledWindow_Blog_Editor->SetSizer(m_Sizer_ScrolledWindow_Blog_Editor);
    this->m_ScrolledWindow_Blog_Editor->Layout();
    m_Sizer_ScrolledWindow_Blog_Editor->Fit(this->m_ScrolledWindow_Blog_Editor);
    m_Sizer_Blog_Editor->Add(this->m_ScrolledWindow_Blog_Editor, 0, wxEXPAND | wxALL, 5);

    // Create the blog page category editor controls 
    this->m_ScrolledWindow_BlogCategory_Editor = new wxScrolledWindow(this->m_Panel_Blog_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    this->m_ScrolledWindow_BlogCategory_Editor->SetScrollRate(5, 5);
    this->m_ScrolledWindow_BlogCategory_Editor->Hide();
    this->m_ScrolledWindow_BlogCategory_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_BlogCategory_Editor;
    m_Sizer_ScrolledWindow_BlogCategory_Editor = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_ScrolledWindow_BlogCategory_Editor->AddGrowableCol(0);
    m_Sizer_ScrolledWindow_BlogCategory_Editor->AddGrowableRow(1);
    m_Sizer_ScrolledWindow_BlogCategory_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_BlogCategory_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_BlogCategory_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_BlogCategory_Editor_Main;
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main->AddGrowableCol(1);
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main->SetMinSize(wxSize(0,0));
    this->m_Label_BlogCategory_Folder = new wxStaticText(this->m_ScrolledWindow_BlogCategory_Editor, wxID_ANY, wxT("Folder:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_BlogCategory_Folder->Wrap(-1);
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main->Add(this->m_Label_BlogCategory_Folder, 0, wxALL, 5);
    this->m_TextCtrl_BlogCategory_Folder = new wxTextCtrl(this->m_ScrolledWindow_BlogCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_BlogCategory_Folder->Enable(false);
    this->m_TextCtrl_BlogCategory_Folder->SetToolTip(wxT("The folder that represents this category"));
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main->Add(this->m_TextCtrl_BlogCategory_Folder, 0, wxALL|wxEXPAND, 5);
    this->m_Label_BlogCategory_DisplayName = new wxStaticText(this->m_ScrolledWindow_BlogCategory_Editor, wxID_ANY, wxT("Display Name:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_BlogCategory_DisplayName->Wrap(-1);
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main->Add(this->m_Label_BlogCategory_DisplayName, 0, wxALL, 5);
    this->m_TextCtrl_BlogCategory_DisplayName = new wxTextCtrl(this->m_ScrolledWindow_BlogCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_BlogCategory_DisplayName->SetToolTip(wxT("The display name of this category"));
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main->Add(this->m_TextCtrl_BlogCategory_DisplayName, 0, wxALL|wxEXPAND, 5);
    m_Label_BlogCategory_Description = new wxStaticText(this->m_ScrolledWindow_BlogCategory_Editor, wxID_ANY, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_BlogCategory_Description->Wrap(-1);
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main->Add(m_Label_BlogCategory_Description, 0, wxALL, 5);
    m_Sizer_ScrolledWindow_BlogCategory_Editor_Main->Add(0, 0, 1, wxEXPAND, 5);
    m_Sizer_ScrolledWindow_BlogCategory_Editor->Add(m_Sizer_ScrolledWindow_BlogCategory_Editor_Main, 1, wxEXPAND, 5);
    this->m_TextCtrl_BlogCategory_Description = new wxTextCtrl(this->m_ScrolledWindow_BlogCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    this->m_TextCtrl_BlogCategory_Description->SetToolTip(wxT("The filename of the project"));
    this->m_TextCtrl_BlogCategory_Description->SetMinSize(wxSize(-1,200));
    m_Sizer_ScrolledWindow_BlogCategory_Editor->Add(this->m_TextCtrl_BlogCategory_Description, 0, wxALL|wxEXPAND, 5);
    this->m_ScrolledWindow_BlogCategory_Editor->SetSizer(m_Sizer_ScrolledWindow_BlogCategory_Editor);
    this->m_ScrolledWindow_BlogCategory_Editor->Layout();
    m_Sizer_ScrolledWindow_BlogCategory_Editor->Fit(this->m_ScrolledWindow_BlogCategory_Editor);
    m_Sizer_Blog_Editor->Add(this->m_ScrolledWindow_BlogCategory_Editor, 0, wxEXPAND | wxALL, 5);

    // Create the blog preview button
    this->m_Button_Blog_Preview = new wxButton(this->m_Panel_Blog_Editor, wxID_ANY, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Button_Blog_Preview->Hide();
    m_Sizer_Blog_Editor->Add(this->m_Button_Blog_Preview, 0, wxALIGN_RIGHT|wxALL, 5);

    // Layout the window
    this->m_Panel_Blog_Editor->SetSizer(m_Sizer_Blog_Editor);
    this->m_Panel_Blog_Editor->Layout();
    m_Sizer_Blog_Editor->Fit(this->m_Panel_Blog_Editor);
    this->m_Splitter_Blog->SplitVertically(this->m_Panel_Blog_Tree, this->m_Panel_Blog_Editor, 200);
    m_Sizer_Blog->Add(this->m_Splitter_Blog, 1, wxEXPAND, 5);
    this->m_Panel_Blog->SetSizer(m_Sizer_Blog);
    this->m_Panel_Blog->Layout();
    m_Sizer_Blog->Fit(this->m_Panel_Blog);
    this->m_ChoiceBook_PageSelection->AddPage(this->m_Panel_Blog, wxT("Blog"), true);
    m_Sizer_Main->Add(this->m_ChoiceBook_PageSelection, 1, wxEXPAND | wxALL, 5);
    this->SetSizer(m_Sizer_Main);
    this->Layout();

    // Add the menubar
    this->m_Menubar_Main = new wxMenuBar(0);
    this->m_Menu_File = new wxMenu();
    wxMenuItem* m_MenuItem_OpenDir;
    m_MenuItem_OpenDir = new wxMenuItem(this->m_Menu_File, wxID_ANY, wxString(wxT("Open Working Directory")) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_File->Append(m_MenuItem_OpenDir);
    wxMenuItem* m_MenuItem_Save;
    m_MenuItem_Save = new wxMenuItem(this->m_Menu_File, wxID_ANY, wxString(wxT("Save Changes")) + wxT('\t') + wxT("CTRL+S"), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_File->Append(m_MenuItem_Save);
    this->m_Menubar_Main->Append(this->m_Menu_File, wxT("File"));
    this->SetMenuBar(m_Menubar_Main);

    // Start a timer to fix the splitter location
    this->m_Timer = new wxTimer();
    this->m_Timer->Start(100, wxTIMER_ONE_SHOT);

    // Center the final window
    this->Centre(wxBOTH);

    // Connect Events
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Main::OnClose));
    this->m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeBeginDrag), NULL, this);
    this->m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndDrag), NULL, this);
    this->m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit), NULL, this);
    this->m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeItemMenu), NULL, this);
    this->m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeSelChanged), NULL, this);
    this->m_TextCtrl_Projects_File->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_File_OnText), NULL, this);
    this->m_TextCtrl_Projects_Name->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Name_OnText), NULL, this);
    this->m_TextCtrl_Projects_Icon->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Icon_OnText), NULL, this);
    this->m_TextCtrl_Projects_Tags->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Tags_OnText), NULL, this);
    this->m_TextCtrl_Projects_Images->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Images_OnText), NULL, this);
    this->m_TextCtrl_Projects_Date->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Date_OnText), NULL, this);
    this->m_TextCtrl_Projects_URLs->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_URLs_OnText), NULL, this);
    this->m_TextCtrl_Projects_ToolTip->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_ToolTip_OnText), NULL, this);
    this->m_TextCtrl_Projects_Description->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Description_OnText), NULL, this);
    this->m_TextCtrl_ProjectsCategory_DisplayName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ProjectsCategory_DisplayName_OnText), NULL, this);
    this->m_TextCtrl_ProjectsCategory_Description->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ProjectsCategory_Description_OnText), NULL, this);
    this->m_Button_Projects_Preview->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Projects_Preview_OnButtonClick), NULL, this);
    this->m_TreeCtrl_Blog->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeBeginDrag), NULL, this);
    this->m_TreeCtrl_Blog->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeEndDrag), NULL, this);
    this->m_TreeCtrl_Blog->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeEndLabelEdit), NULL, this);
    this->m_TreeCtrl_Blog->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeItemMenu), NULL, this);
    this->m_TreeCtrl_Blog->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeSelChanged), NULL, this);
    this->m_TextCtrl_Blog_File->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_File_OnText), NULL, this);
    this->m_TextCtrl_Blog_Name->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_Name_OnText), NULL, this);
    this->m_TextCtrl_Blog_Icon->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_Icon_OnText), NULL, this);
    this->m_TextCtrl_Blog_ToolTip->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_ToolTip_OnText), NULL, this);
    this->m_TextCtrl_Blog_Date->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_Date_OnText), NULL, this);
    this->m_TextCtrl_Blog_Tags->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_Tags_OnText), NULL, this);
    this->m_TextCtrl_Blog->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_OnText), NULL, this);
    this->m_TextCtrl_BlogCategory_DisplayName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_BlogCategory_DisplayName_OnText), NULL, this);
    this->m_TextCtrl_BlogCategory_Description->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_BlogCategory_Description_OnText), NULL, this);
    this->m_Button_Blog_Preview->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Blog_Preview_OnButtonClick), NULL, this);
    this->m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_OpenDir_OnMenuSelection), this, m_MenuItem_OpenDir->GetId());
    this->m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Save_OnMenuSelection), this, m_MenuItem_Save->GetId());
    this->m_Timer->Bind(wxEVT_TIMER, wxTimerEventHandler(Main::m_Timer_OnTimer), this, this->m_Timer->GetId());

    // Load the projects and blog from the current working directory
    this->m_ChoiceBook_PageSelection->SetSelection(0);
    this->UpdateTree(this->m_TreeCtrl_Projects, "projects", &this->m_Category_Projects);
    this->UpdateTree(this->m_TreeCtrl_Blog, "blog", &this->m_Category_Blog);
    this->LoadProjects();
    this->LoadBlog();
}


/*==============================
    App (Destructor)
    Cleans up the class before deletion
==============================*/

Main::~Main()
{
    this->m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeBeginDrag), NULL, this);
    this->m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndDrag), NULL, this);
    this->m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit), NULL, this);
    this->m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeItemMenu), NULL, this);
    this->m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeSelChanged), NULL, this);
    this->m_TextCtrl_Projects_File->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_File_OnText), NULL, this);
    this->m_TextCtrl_Projects_Name->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Name_OnText), NULL, this);
    this->m_TextCtrl_Projects_Icon->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Icon_OnText), NULL, this);
    this->m_TextCtrl_Projects_Tags->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Tags_OnText), NULL, this);
    this->m_TextCtrl_Projects_Images->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Images_OnText), NULL, this);
    this->m_TextCtrl_Projects_Date->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Date_OnText), NULL, this);
    this->m_TextCtrl_Projects_URLs->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_URLs_OnText), NULL, this);
    this->m_TextCtrl_Projects_ToolTip->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_ToolTip_OnText), NULL, this);
    this->m_TextCtrl_Projects_Description->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Description_OnText), NULL, this);
    this->m_TextCtrl_ProjectsCategory_DisplayName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ProjectsCategory_DisplayName_OnText), NULL, this);
    this->m_TextCtrl_ProjectsCategory_Description->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ProjectsCategory_Description_OnText), NULL, this);
    this->m_Button_Projects_Preview->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Projects_Preview_OnButtonClick), NULL, this);
    this->m_TreeCtrl_Blog->Disconnect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeBeginDrag), NULL, this);
    this->m_TreeCtrl_Blog->Disconnect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeEndDrag), NULL, this);
    this->m_TreeCtrl_Blog->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeEndLabelEdit), NULL, this);
    this->m_TreeCtrl_Blog->Disconnect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeItemMenu), NULL, this);
    this->m_TreeCtrl_Blog->Disconnect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeSelChanged), NULL, this);
    this->m_TextCtrl_Blog_File->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_File_OnText), NULL, this);
    this->m_TextCtrl_Blog_Name->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_Name_OnText), NULL, this);
    this->m_TextCtrl_Blog_Icon->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_Icon_OnText), NULL, this);
    this->m_TextCtrl_Blog_ToolTip->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_ToolTip_OnText), NULL, this);
    this->m_TextCtrl_Blog_Date->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_Date_OnText), NULL, this);
    this->m_TextCtrl_Blog_Tags->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_Tags_OnText), NULL, this);
    this->m_TextCtrl_Blog->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Blog_OnText), NULL, this);
    this->m_TextCtrl_BlogCategory_DisplayName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_BlogCategory_DisplayName_OnText), NULL, this);
    this->m_TextCtrl_BlogCategory_Description->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_BlogCategory_Description_OnText), NULL, this);
    this->m_Button_Blog_Preview->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Blog_Preview_OnButtonClick), NULL, this);
    this->m_Timer->Disconnect(wxEVT_TIMER, wxTimerEventHandler(Main::m_Timer_OnTimer), NULL, this);
}


/*==============================
    m_Splitter_ProjectsOnIdle
    Handles the project splitter event
    @param Unused
==============================*/

void Main::m_Splitter_ProjectsOnIdle(wxIdleEvent&)
{
    this->m_Splitter_Projects->SetSashPosition(0);
    this->m_Splitter_Projects->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_ProjectsOnIdle), NULL, this);
}


/*==============================
    m_Splitter_BlogOnIdle
    Handles the blog splitter event
    @param Unused
==============================*/

void Main::m_Splitter_BlogOnIdle(wxIdleEvent&)
{
    this->m_Splitter_Blog->SetSashPosition(0);
    this->m_Splitter_Blog->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_BlogOnIdle), NULL, this);
}


/*==============================
    m_MenuItem_OpenDir_OnMenuSelection
    Handles the Open Directory menu event
    @param Unused
==============================*/

void Main::m_MenuItem_OpenDir_OnMenuSelection(wxCommandEvent&)
{
    wxDirDialog dir(this, wxString("Select Working Directory"), this->m_WorkingDir);
    if (dir.ShowModal() == wxID_OK)
    {
        this->m_WorkingDir = dir.GetPath();
        this->MarkModified(false);
        this->UpdateTree(this->m_TreeCtrl_Projects, "projects", &this->m_Category_Projects);
        this->LoadProjects();
        this->UpdateTree(this->m_TreeCtrl_Blog, "blog", &this->m_Category_Blog);
        this->LoadBlog();
    }
}


/*==============================
    m_MenuItem_Save_OnMenuSelection
    Handles the Save menu event
    @param Unused
==============================*/

void Main::m_MenuItem_Save_OnMenuSelection(wxCommandEvent&)
{
    this->Save();
}


/*==============================
    m_TreeCtrl_Projects_OnTreeEndLabelEdit
    Handles the project tree label editing event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit(wxTreeEvent& event)
{
    // Prevent blank display names
    if (event.GetLabel() == "")
    {
        event.Veto();
        return;
    }

    // Handle label editing based on whether it's a category or not
    if (treeitem_iscategory(this->m_TreeCtrl_Projects, event.GetItem())) // Handle category label editing
    {
        wxTreeItemId id = event.GetItem();
        Category* cat = this->FindCategory_Projects(id);
        if (cat != NULL)
        {
            cat->displayname = event.GetLabel();
            if (this->m_SelectedItem == event.GetItem())
                this->m_TextCtrl_ProjectsCategory_DisplayName->ChangeValue(cat->displayname);
            cat->wasmodified = true;
            this->MarkModified();
            return;
        }
    }
    else // Handle project label editing
    {
        Project* proj = this->FindProject(event.GetItem());
        if (proj != NULL)
        {
            proj->displayname = event.GetLabel();
            if (this->m_SelectedItem == event.GetItem())
                this->m_TextCtrl_Projects_Name->ChangeValue(proj->displayname);
            proj->category->wasmodified = true;
            proj->wasmodified = true;
            this->MarkModified();
            return;
        }
    }
    event.Veto();
}


/*==============================
    m_TreeCtrl_Projects_OnTreeBeginDrag
    Handles the project tree drag event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeBeginDrag(wxTreeEvent& event)
{
    this->m_DraggedItem = event.GetItem();
    event.Allow();
}


/*==============================
    m_TreeCtrl_Projects_OnTreeEndDrag
    Handles the project tree drag ending event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeEndDrag(wxTreeEvent& event)
{
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, this->m_SelectedItem))
        this->EndDrag_Project(event);
    else
        this->EndDrag(event, this->m_TreeCtrl_Projects, &this->m_Category_Projects);
}


/*==============================
    m_TreeCtrl_Projects_OnTreeItemMenu
    Handles the project tree right click event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeItemMenu(wxTreeEvent& event)
{
    wxMenu menu;
    this->m_SelectedItem = event.GetItem();
    menu.Append(wxID_NEW, wxT("Create project"));
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, event.GetItem()))
        menu.Append(wxID_DELETE, wxT("Delete project"));
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::OnPopupClick_Projects), NULL, this);
    PopupMenu(&menu, event.GetPoint());
}


/*==============================
    m_TreeCtrl_Projects_OnTreeSelChanged
    Handles the project tree selection event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeSelChanged(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    wxTreeItemId olditem = this->m_SelectedItem;

    // Handle the item selection
    this->m_SelectedItem = item;
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, item)) // Handle project selection
    {
        wxString wip;
        Project* proj = FindProject(item);

        // Ensure the project exists
        if (proj == NULL)
            return;

        // Fill in the simple text controls
        this->m_TextCtrl_Projects_File->ChangeValue(proj->filename);
        this->m_TextCtrl_Projects_Name->ChangeValue(proj->displayname);
        this->m_TextCtrl_Projects_Icon->ChangeValue(proj->icon);
        this->m_TextCtrl_Projects_ToolTip->ChangeValue(proj->tooltip);
        this->m_TextCtrl_Projects_Description->ChangeValue(proj->description);
        this->m_TextCtrl_Projects_Date->ChangeValue(proj->date);

        // Fill in the tags text control
        wip = wxString("");
        for (wxString str : proj->tags)
            wip += str + wxString(", ");
        this->m_TextCtrl_Projects_Tags->ChangeValue(wip);
        
        // Fill in the images text control
        wip = wxString("");
        for (wxString str : proj->images)
            wip += str + wxString(", ");
        this->m_TextCtrl_Projects_Images->ChangeValue(wip);
        
        // Fill in the url text control
        wip = wxString("");
        for (wxString str : proj->urls)
            wip += str + wxString(", ");
        this->m_TextCtrl_Projects_URLs->ChangeValue(wip);

        // Show the project editor
        this->ShowProjectEditor();
    }
    else if (treeitem_iscategory(this->m_TreeCtrl_Projects, item)) // Handle category selection
    {
        Category* cat = this->FindCategory_Projects(item);

        // Ensure the category exists
        if (cat == NULL)
            return;

        // Fill in the simple text controls
        this->m_TextCtrl_ProjectsCategory_Folder->ChangeValue(cat->foldername);
        this->m_TextCtrl_ProjectsCategory_DisplayName->ChangeValue(cat->displayname);
        this->m_TextCtrl_ProjectsCategory_Description->ChangeValue(cat->description);

        // Show the project category editor
        this->ShowProjectCategoryEditor(true);
    }
    else
        this->ShowProjectEditor(false);
}


/*==============================
    m_TextCtrl_Projects_File_OnText
    Handles the project filename text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_File_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->filename = event.GetString();
    proj->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Projects_Name_OnText
    Handles the project name text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Name_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->displayname = event.GetString();
    this->m_TreeCtrl_Projects->SetItemText(proj->treeid, proj->displayname);
    proj->wasmodified = true;
    proj->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Projects_Icon_OnText
    Handles the project icon text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Icon_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->icon = event.GetString();
    proj->wasmodified = true;
    proj->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Projects_Tags_OnText
    Handles the project tags text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Tags_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj != NULL)
    {
        wxArrayString strarray = wxSplit(event.GetString(), ',');
        proj->tags.clear();

        // Split the elements by comma
        for (wxString str : strarray)
        {
            str.Trim(true);
            str.Trim(false);
            if (str != "")
                proj->tags.push_back(str);
        }
        proj->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    m_TextCtrl_Projects_Images_OnText
    Handles the project images text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Images_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj != NULL)
    {
        wxArrayString strarray = wxSplit(event.GetString(), ',');
        proj->images.clear();

        // Split the elements by comma
        for (wxString str : strarray)
        {
            str.Trim(true);
            str.Trim(false);
            if (str != "")
                proj->images.push_back(str);
        }
        proj->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    m_TextCtrl_Projects_Date_OnText
    Handles the project date text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Date_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->date = event.GetString();
    proj->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Projects_URLs_OnText
    Handles the project date text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_URLs_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj != NULL)
    {
        wxArrayString strarray = wxSplit(event.GetString(), ',');
        proj->urls.clear();

        // Split the elements by comma
        for (wxString str : strarray)
        {
            str.Trim(true);
            str.Trim(false);
            if (str != "")
                proj->urls.push_back(str);
        }
        proj->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    m_TextCtrl_Projects_ToolTip_OnText
    Handles the project tooltip text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_ToolTip_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->tooltip = event.GetString();
    proj->wasmodified = true;
    proj->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Projects_Description_OnText
    Handles the project description text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Description_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->description = event.GetString();
    proj->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_ProjectsCategory_DisplayName_OnText
    Handles the project category name text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ProjectsCategory_DisplayName_OnText(wxCommandEvent& event)
{
    Category* cat = this->FindCategory_Projects(this->m_SelectedItem);
    if (cat == NULL)
        return;
    cat->displayname = event.GetString();
    this->m_TreeCtrl_Projects->SetItemText(cat->treeid, cat->displayname);
    cat->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_ProjectsCategory_Description_OnText
    Handles the project category description text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ProjectsCategory_Description_OnText(wxCommandEvent& event)
{
    Category* cat = this->FindCategory_Projects(this->m_SelectedItem);
    if (cat == NULL)
        return;
    cat->description = event.GetString();
    cat->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_Button_Projects_Preview_OnButtonClick
    Handles the project preview button event
    @param Unused
==============================*/

void Main::m_Button_Projects_Preview_OnButtonClick(wxCommandEvent&)
{
    wxString content;
    wxString url = wxString("");

    // Compile the relevant page(s)
    if (treeitem_iscategory(this->m_TreeCtrl_Projects, this->m_SelectedItem)) // Handle category preview
    {
        Category* cat = this->FindCategory_Projects(this->m_SelectedItem);
        if (cat != NULL)
        {
            CompileProjects_List();
            url = this->m_WorkingDir + wxString("/projects.html") + wxString("#") + cat->foldername;
        }
    }
    else // Handle project preview
    {
        Project* proj = this->FindProject(this->m_SelectedItem);
        if (proj != NULL)
        {
            CompileProjects_Project(proj);
            url = this->m_WorkingDir + wxString("/projects/") + proj->category->foldername + wxString("/") + proj->filename + wxString(".html");
        }
    }

    // Launch the generated page in a browser window
    wxLaunchDefaultBrowser(wxString("file:") + url);
}


/*==============================
    m_TreeCtrl_Blog_OnTreeEndLabelEdit
    Handles the blog tree label editing event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Blog_OnTreeEndLabelEdit(wxTreeEvent& event)
{
    // Prevent blank display names
    if (event.GetLabel() == "")
    {
        event.Veto();
        return;
    }

    // Handle label editing based on whether it's a category or not
    if (treeitem_iscategory(this->m_TreeCtrl_Blog, event.GetItem())) // Handle category label editing
    {
        wxTreeItemId id = event.GetItem();
        Category* cat = this->FindCategory_Blog(id);
        if (cat != NULL)
        {
            cat->displayname = event.GetLabel();
            if (this->m_SelectedItem == event.GetItem())
                this->m_TextCtrl_BlogCategory_DisplayName->ChangeValue(cat->displayname);
            cat->wasmodified = true;
            this->MarkModified();
            return;
        }
    }
    else // Handle blog entry label editing
    {
        Blog* bentry = this->FindBlog(event.GetItem());
        if (bentry != NULL)
        {
            bentry->displayname = event.GetLabel();
            if (this->m_SelectedItem == event.GetItem())
                this->m_TextCtrl_Blog_Name->ChangeValue(bentry->displayname);
            bentry->wasmodified = true;
            bentry->category->wasmodified = true;
            this->MarkModified();
            return;
        }
    }
    event.Veto();
}


/*==============================
    m_TreeCtrl_Blog_OnTreeBeginDrag
    Handles the blog tree drag event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Blog_OnTreeBeginDrag(wxTreeEvent& event)
{
    this->m_DraggedItem = event.GetItem();
    event.Allow();
}


/*==============================
    m_TreeCtrl_Blog_OnTreeEndDrag
    Handles the blog tree drag ending event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Blog_OnTreeEndDrag(wxTreeEvent& event)
{
    if (!treeitem_iscategory(this->m_TreeCtrl_Blog, this->m_SelectedItem))
        this->EndDrag_Blog(event);
    else
        this->EndDrag(event, this->m_TreeCtrl_Blog, &this->m_Category_Blog);
}


/*==============================
    m_TreeCtrl_Blog_OnTreeItemMenu
    Handles the blog tree right click event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Blog_OnTreeItemMenu(wxTreeEvent& event)
{
    wxMenu menu;
    this->m_SelectedItem = event.GetItem();
    menu.Append(wxID_NEW, wxT("Create blog entry"));
    if (!treeitem_iscategory(this->m_TreeCtrl_Blog, event.GetItem()))
        menu.Append(wxID_DELETE, wxT("Delete blog entry"));
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::OnPopupClick_Blog), NULL, this);
    PopupMenu(&menu, event.GetPoint());
}


/*==============================
    m_TreeCtrl_Blog_OnTreeSelChanged
    Handles the blog tree selection event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Blog_OnTreeSelChanged(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    wxTreeItemId olditem = this->m_SelectedItem;

    // Handle the item selection
    if (!treeitem_iscategory(this->m_TreeCtrl_Blog, item))  // Handle blog entry selection
    {
        wxString wip;
        Blog* bentry = this->FindBlog(item);

        // Ensure the blog entry exists
        if (bentry == NULL)
            return;

        // Fill in the simple text controls
        this->m_TextCtrl_Blog_File->ChangeValue(bentry->filename);
        this->m_TextCtrl_Blog_Name->ChangeValue(bentry->displayname);
        this->m_TextCtrl_Blog_Icon->ChangeValue(bentry->icon);
        this->m_TextCtrl_Blog_ToolTip->ChangeValue(bentry->tooltip);
        this->m_TextCtrl_Blog_Date->ChangeValue(bentry->date);
        this->m_TextCtrl_Blog->ChangeValue(bentry->content);
        
        // Fill in the tags text control
        wip = wxString("");
        for (wxString str : bentry->tags)
            wip += str + wxString(", ");
        this->m_TextCtrl_Blog_Tags->ChangeValue(wip);

        // Show the blog editor
        this->ShowBlogEditor();
    }
    else if (treeitem_iscategory(this->m_TreeCtrl_Blog, item))
    {
        Category* cat = this->FindCategory_Blog(item);

        // Ensure the category exists
        if (cat == NULL)
            return;

        // Fill in the simple text controls
        this->m_TextCtrl_BlogCategory_Folder->ChangeValue(cat->foldername);
        this->m_TextCtrl_BlogCategory_DisplayName->ChangeValue(cat->displayname);
        this->m_TextCtrl_BlogCategory_Description->ChangeValue(cat->description);

        // Show the blog category editor
        this->ShowBlogCategoryEditor(true);
    }
    else
        this->ShowBlogEditor(false);
}


/*==============================
    m_TextCtrl_Blog_File_OnText
    Handles the blog entry filename text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Blog_File_OnText(wxCommandEvent& event)
{
    Blog* bentry = this->FindBlog(this->m_SelectedItem);
    if (bentry == NULL)
        return;
    bentry->filename = event.GetString();
    bentry->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Blog_Name_OnText
    Handles the blog entry name text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Blog_Name_OnText(wxCommandEvent& event)
{
    Blog* bentry = this->FindBlog(this->m_SelectedItem);
    if (bentry == NULL)
        return;
    bentry->displayname = event.GetString();
    this->m_TreeCtrl_Blog->SetItemText(bentry->treeid, bentry->displayname);
    bentry->wasmodified = true;
    bentry->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Blog_Name_OnText
    Handles the blog entry icon text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Blog_Icon_OnText(wxCommandEvent& event)
{
    Blog* bentry = this->FindBlog(this->m_SelectedItem);
    if (bentry == NULL)
        return;
    bentry->icon = event.GetString();
    bentry->wasmodified = true;
    bentry->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Blog_Date_OnText
    Handles the blog entry date text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Blog_Date_OnText(wxCommandEvent& event)
{
    Blog* bentry = this->FindBlog(this->m_SelectedItem);
    if (bentry == NULL)
        return;
    bentry->date = event.GetString();
    bentry->wasmodified= true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Blog_Tags_OnText
    Handles the blog entry tags text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Blog_Tags_OnText(wxCommandEvent& event)
{
    Blog* bentry = this->FindBlog(this->m_SelectedItem);
    if (bentry != NULL)
    {
        wxArrayString strarray = wxSplit(event.GetString(), ',');
        bentry->tags.clear();

        // Split the elements by comma
        for (wxString str : strarray)
        {
            str.Trim(true);
            str.Trim(false);
            if (str != "")
                bentry->tags.push_back(str);
        }
        bentry->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    m_TextCtrl_Blog_ToolTip_OnText
    Handles the blog entry tooltip text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Blog_ToolTip_OnText(wxCommandEvent& event)
{
    Blog* bentry = this->FindBlog(this->m_SelectedItem);
    if (bentry == NULL)
        return;
    bentry->tooltip = event.GetString();
    bentry->wasmodified = true;
    bentry->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Blog_OnText
    Handles the blog entry description text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Blog_OnText(wxCommandEvent& event)
{
    Blog* bentry = this->FindBlog(this->m_SelectedItem);
    if (bentry == NULL)
        return;
    bentry->content = event.GetString();
    bentry->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_BlogCategory_DisplayName_OnText
    Handles the blog category name text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_BlogCategory_DisplayName_OnText(wxCommandEvent& event)
{
    Category* cat = this->FindCategory_Blog(this->m_SelectedItem);
    if (cat == NULL)
        return;
    cat->displayname = event.GetString();
    this->m_TreeCtrl_Blog->SetItemText(cat->treeid, cat->displayname);
    cat->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_BlogCategory_Description_OnText
    Handles the blog category description text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_BlogCategory_Description_OnText(wxCommandEvent& event)
{
    Category* cat = this->FindCategory_Blog(this->m_SelectedItem);
    if (cat == NULL)
        return;
    cat->description = event.GetString();
    cat->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_Button_Blog_Preview_OnButtonClick
    Handles the blog preview button event
    @param Unused
==============================*/

void Main::m_Button_Blog_Preview_OnButtonClick(wxCommandEvent&)
{
    wxString content;
    wxString url = wxString("");

    // Compile the relevant page(s)
    if (treeitem_iscategory(this->m_TreeCtrl_Blog, this->m_SelectedItem)) // Handle category preview
    {
        Category* cat = this->FindCategory_Blog(this->m_SelectedItem);
        if (cat != NULL)
        {
            this->CompileBlog_List();
            url = this->m_WorkingDir + wxString("/blogs.html") + wxString("#") + cat->foldername;
        }
    }
    else // Handle blog entry preview
    {
        Blog* bentry = this->FindBlog(this->m_SelectedItem);
        if (bentry != NULL)
        {
            this->CompileBlog_Entry(bentry);
            url = this->m_WorkingDir + wxString("/blog/") + bentry->category->foldername + wxString("/") + bentry->filename + wxString(".html");
        }
    }

    // Launch the generated page in a browser window
    wxLaunchDefaultBrowser(wxString("file:") + url);
}


/*==============================
    m_Timer_OnTimer
    Handles the timer that starts with the program to fix the
    initial sash positions
    @param Unused
==============================*/

void Main::m_Timer_OnTimer(wxTimerEvent&)
{
    this->m_Splitter_Projects->SetSashPosition(200);
    this->m_Splitter_Blog->SetSashPosition(200);
}


/*==============================
    m_Timer_OnTimer
    Handles the clicking of a project popup
    @param The event that was generated
==============================*/

void Main::OnPopupClick_Projects(wxCommandEvent& event)
{
    Project* proj;
    Category* cat;
    int index = 0;
    wxTreeItemId cat_id = this->m_SelectedItem;

    // Find the category of the element
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, cat_id))
        cat_id = this->m_TreeCtrl_Projects->GetItemParent(cat_id);
    cat = this->FindCategory_Projects(cat_id);

    // If the category doesn't exist, stop
    if (cat == NULL)
        return;

    // Decide what to do based on the event ID
    switch (event.GetId())
    {
        case wxID_NEW: // Create a new project
            proj = new Project();
            proj->index = cat->pages.size();
            proj->filename = "new";
            proj->displayname = "New Project";
            proj->icon = "";
            proj->date = "";
            proj->tooltip = "";
            proj->description = "";
            proj->wasmodified = true;
            proj->images.clear();
            proj->urls.clear();
            proj->tags.clear();
            proj->category = cat;
            proj->treeid = this->m_TreeCtrl_Projects->AppendItem(cat_id, proj->displayname);
            cat->pages.push_back(proj);
            this->m_TreeCtrl_Projects->Expand(cat->treeid);
            this->m_TreeCtrl_Projects->SelectItem(proj->treeid);
            this->m_SelectedItem = proj->treeid;
            cat->wasmodified = true;
            this->MarkModified();
            break;
        case wxID_DELETE: // Delete an existing project
            proj = this->FindProject(this->m_SelectedItem);
            if (proj != NULL)
            {
                cat->pages.erase(cat->pages.begin() + proj->index);
                this->m_TreeCtrl_Projects->Delete(proj->treeid);
                if (wxFileExists(this->m_WorkingDir + wxString("/projects/") + cat->foldername + wxString("/") + proj->filename + wxString(".html")))
                    wxRemoveFile(this->m_WorkingDir + wxString("/projects/") + cat->foldername + wxString("/") + proj->filename + wxString(".html"));
                if (wxFileExists(this->m_WorkingDir + wxString("/projects/") + cat->foldername + wxString("/markdown/") + proj->filename + wxString(".md")))
                    wxRemoveFile(this->m_WorkingDir + wxString("/projects/") + cat->foldername + wxString("/markdown/") + proj->filename + wxString(".md"));
                delete proj;
                for (void* projptr : cat->pages)
                {
                    proj = (Project*)projptr;
                    proj->index = index++;
                }
                cat->wasmodified = true;
                this->MarkModified();
            }
            break;
    }
}


/*==============================
    m_Timer_OnTimer
    Handles the clicking of a blog entry popup
    @param The event that was generated
==============================*/

void Main::OnPopupClick_Blog(wxCommandEvent& event)
{
    Blog* bentry;
    Category* cat;
    int index = 0;
    wxTreeItemId cat_id = this->m_SelectedItem;

    // Find the category of the element
    if (!treeitem_iscategory(this->m_TreeCtrl_Blog, cat_id))
        cat_id = this->m_TreeCtrl_Blog->GetItemParent(cat_id);
    cat = this->FindCategory_Blog(cat_id);

    // If the category doesn't exist, stop
    if (cat == NULL)
        return;

    // Decide what to do based on the event ID
    switch (event.GetId())
    {
        case wxID_NEW: // Create a new blog entry 
            bentry = new Blog();
            bentry->index = cat->pages.size();
            bentry->filename = "new";
            bentry->displayname = "New Blog Entry";
            bentry->icon = "";
            bentry->date = "";
            bentry->tooltip = "";
            bentry->content = "";
            bentry->wasmodified = true;
            bentry->tags.clear();
            bentry->category = cat;
            bentry->treeid = this->m_TreeCtrl_Blog->AppendItem(cat_id, bentry->displayname);
            cat->pages.push_back(bentry);
            this->m_TreeCtrl_Blog->Expand(cat->treeid);
            this->m_TreeCtrl_Blog->SelectItem(bentry->treeid);
            this->m_SelectedItem = bentry->treeid;
            cat->wasmodified = true;
            this->MarkModified();
            break;
        case wxID_DELETE: // Delete an existing blog entry
            bentry = this->FindBlog(this->m_SelectedItem);
            if (bentry != NULL)
            {
                cat->pages.erase(cat->pages.begin() + bentry->index);
                this->m_TreeCtrl_Blog->Delete(bentry->treeid);
                if (wxFileExists(this->m_WorkingDir + wxString("/blog/") + cat->foldername + wxString("/") + bentry->filename + wxString(".html")))
                    wxRemoveFile(this->m_WorkingDir + wxString("/blog/") + cat->foldername + wxString("/") + bentry->filename + wxString(".html"));
                if (wxFileExists(this->m_WorkingDir + wxString("/blog/") + cat->foldername + wxString("/markdown/") + bentry->filename + wxString(".md")))
                    wxRemoveFile(this->m_WorkingDir + wxString("/blog/") + cat->foldername + wxString("/markdown/") + bentry->filename + wxString(".md"));
                delete bentry;
                for (void* blogptr : cat->pages)
                {
                    bentry = (Blog*)blogptr;
                    bentry->index = index++;
                }
                cat->wasmodified = true;
                this->MarkModified();
            }
            break;
    }
}


/*==============================
    OnClose
    Handles the frame close event
    @param  The event that was generated
==============================*/

void Main::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto() && this->m_Modified)
    {
        if (wxMessageBox("You have unsaved changes. Continue?", "Unsaved changes", wxICON_QUESTION | wxYES_NO) != wxYES)
        {
            event.Veto();
            return;
        }
    }
    event.Skip();
}


/*==============================
    FindCategory_Projects
    Finds a project category object using a tree item id
    @param  The tree item ID to find the category of
    @return The category that was found, or NULL
==============================*/

Category* Main::FindCategory_Projects(wxTreeItemId item)
{
    for (Category* cat : this->m_Category_Projects)
        if (cat->treeid == item)
            return cat;
    return NULL;
}


/*==============================
    FindCategory_Blog
    Finds a blog entry category object using a tree item id
    @param  The tree item ID to find the category of
    @return The category that was found, or NULL
==============================*/
    
Category* Main::FindCategory_Blog(wxTreeItemId item)
{
    for (Category* cat : this->m_Category_Blog)
        if (cat->treeid == item)
            return cat;
    return NULL;
}


/*==============================
    FindProject
    Finds a project object using a tree item id
    @param  The tree item ID to find the project of
    @return The project that was found, or NULL
==============================*/

Project* Main::FindProject(wxTreeItemId item)
{
    wxTreeItemId cat_id;
    Category* cat_elem;

    if (treeitem_iscategory(this->m_TreeCtrl_Projects, item))
        return NULL;

    cat_id = this->m_TreeCtrl_Projects->GetItemParent(item);
    cat_elem = this->FindCategory_Projects(cat_id);
    if (cat_elem == NULL)
        return NULL;

    for (void* proj : cat_elem->pages)
        if (((Project*)proj)->treeid == item)
            return (Project*)proj;
    return NULL;
}


/*==============================
    FindBlog
    Finds a blog entry object using a tree item id
    @param  The tree item ID to find the blog entry of
    @return The blog entry that was found, or NULL
==============================*/

Blog* Main::FindBlog(wxTreeItemId item)
{
    wxTreeItemId cat_id;
    Category* cat_elem;

    if (treeitem_iscategory(this->m_TreeCtrl_Blog, item))
        return NULL;

    cat_id = this->m_TreeCtrl_Blog->GetItemParent(item);
    cat_elem = this->FindCategory_Blog(cat_id);
    if (cat_elem == NULL)
        return NULL;

    for (void* bentry : cat_elem->pages)
        if (((Blog*)bentry)->treeid == item)
            return (Blog*)bentry;
    return NULL;
}


/*==============================
    UpdateTree
    Updates a given tree list
    @param  A pointer to the tree to update
    @param  The folder of the tree we're updating
    @param  A pointer of the category list to update
==============================*/

void Main::UpdateTree(wxTreeCtrl* tree, wxString folder, std::vector<Category*>* categorylist)
{
    bool cont;
    int index;
    wxTreeItemId root;
    wxString filename;
    wxDir pagepath;

    // First, free all memory
    for (Category* cat : *categorylist)
    {
        for (void* child : cat->pages)
        {
            if (folder == "projects")
                delete (Project*)child;
            if (folder == "blog")
                delete (Blog*)child;
        }
        delete cat;
    }

    // Initialize the trees
    categorylist->clear();
    tree->DeleteAllItems();
    root = tree->AddRoot("Root");

    // If the folder doesn't exist, create it
    if (!wxDirExists(pagepath.GetName()))
        wxMkDir(pagepath.GetName(), wxS_DIR_DEFAULT);
    pagepath.Open(this->m_WorkingDir + wxString("/")+folder);
    if (!pagepath.IsOpened())
        return;

    // First, open the relevant JSON file and add folders that are registered in the JSON file
    index = json_loadcategories(this->m_WorkingDir + wxString("/") + folder + wxString("/") + folder + wxString(".json"), categorylist);

    // Now, add all folders which aren't in the JSON
    cont = pagepath.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
    while (cont)
    {
        bool hasfile = false;

        // Check if the folder already exists in our list
        for (Category* cat : *categorylist)
        {
            if (cat->foldername == filename)
            {
                hasfile = true;
                break;
            }
        }

        // If it doesn't, create it
        if (!hasfile)
        {
            Category* cat = new Category();
            cat->foldername = filename;
            cat->index = index++;
            cat->displayname = filename;
            cat->description = wxString("");
            cat->treeid = NULL;
            cat->pages.clear();
            categorylist->push_back(cat);
        }

        // Check the next folder
        cont = pagepath.GetNext(&filename);
    }

    // Finally, generate the tree
    for (Category* cat : *categorylist)
        cat->treeid = tree->AppendItem(root, cat->displayname);
}


/*==============================
    LoadProjects
    Load all projects stored in the JSON file
==============================*/

void Main::LoadProjects()
{
    json_loadprojects(this->m_WorkingDir, &this->m_Category_Projects, this->m_TreeCtrl_Projects);
    this->ShowProjectEditor(false);
}


/*==============================
    LoadBlog
    Load all blog entries stored in the JSON file
==============================*/

void Main::LoadBlog()
{
    json_loadblogentries(this->m_WorkingDir, &this->m_Category_Blog, this->m_TreeCtrl_Blog);
    this->ShowBlogEditor(false);
}


/*==============================
    EndDrag
    Handle the drag end event properly
    @param The generated event
    @param The affected tree
    @param The affected category list
==============================*/

void Main::EndDrag(wxTreeEvent& event, wxTreeCtrl* tree, std::vector<Category*>* categorylist)
{
    wxTreeItemId src = this->m_DraggedItem;
    wxTreeItemId dest = event.GetItem();
    Category* src_elem = NULL;
    Category* dest_elem = NULL;
    this->m_DraggedItem = NULL;

    // If the destination is not a category item, then get the parent
    if (!treeitem_iscategory(tree, dest))
        dest = tree->GetItemParent(dest);

    // Ensure we have a valid destination node
    if (!dest.IsOk() || src == dest)
        return;

    // Ignore placing the source directly above the destination, since it won't move at all.
    if (tree->GetPrevSibling(src) == dest)
        return;

    // Only allow moving categories into categories
    if (treeitem_iscategory(tree, src) && treeitem_iscategory(tree, dest))
    {
        int index = 0;

        // Get the category elements themselves
        for (Category* cat : *categorylist)
        {
            if (cat->treeid == src)
                src_elem = cat;
            if (cat->treeid == dest)
                dest_elem = cat;
        }

        // Move the elements
        if (dest_elem->index < src_elem->index)
        {
            categorylist->erase(categorylist->begin() + src_elem->index);
            categorylist->insert(categorylist->begin() + dest_elem->index + 1, src_elem);
        }
        else
        {
            categorylist->erase(categorylist->begin() + src_elem->index);
            categorylist->insert(categorylist->begin() + dest_elem->index, src_elem);
        }
        tree->Delete(src);
        src_elem->treeid = tree->InsertItem(tree->GetRootItem(), dest, src_elem->displayname);

        // Reinsert the children
        for (void* child : src_elem->pages)
        {
            if (tree == this->m_TreeCtrl_Projects)
            {
                Project* proj = (Project*)child;
                proj->treeid = tree->InsertItem(src_elem->treeid, proj->index, proj->displayname);
                proj->category->wasmodified = true;
            }
            else
            {
                Blog* bentry = (Blog*)child;
                bentry->treeid = tree->InsertItem(src_elem->treeid, bentry->index, bentry->displayname);
                bentry->category->wasmodified = true;
            }
        }

        // Correct the index values
        for (Category* cat : *categorylist)
        {
            cat->index = index++;
            cat->wasmodified = true;
        }

        // Mark the program as modified
        this->MarkModified();
    }
}


/*==============================
    EndDrag_Project
    Handle the drag end event for projects properly
    @param The generated event
==============================*/

void Main::EndDrag_Project(wxTreeEvent& event)
{
    wxTreeItemId src = this->m_DraggedItem;
    wxTreeItemId dest = event.GetItem();
    this->m_DraggedItem = NULL;

    // Ensure we have a valid destination node
    if (!dest.IsOk() || src == dest)
        return;

    // Ignore placing the source directly above the destination, since it won't move at all.
    if (this->m_TreeCtrl_Projects->GetPrevSibling(src) == dest)
        return;

    // Only work on project nodes
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, dest))
    {
        Category* cat;
        std::vector<void*>* pages;
        int index = 0;
        Project* proj_src = this->FindProject(src);
        Project* proj_dest = this->FindProject(dest);

        // Ensure a valid source and destination
        if (proj_src == NULL || proj_dest == NULL)
            return;

        // Ensure both projects are in the same category
        cat = proj_src->category;
        pages = &cat->pages;
        if (cat != proj_dest->category)
            return;

        // Move the elements
        if (proj_dest->index < proj_src->index)
        {
            pages->erase(pages->begin() + proj_src->index);
            pages->insert(pages->begin() + proj_dest->index + 1, proj_src);
        }
        else
        {
            pages->erase(pages->begin() + proj_src->index);
            pages->insert(pages->begin() + proj_dest->index, proj_src);
        }
        this->m_TreeCtrl_Projects->Delete(src);
        proj_src->treeid = this->m_TreeCtrl_Projects->InsertItem(cat->treeid, dest, proj_src->displayname);

        // Correct the index values
        for (void* projptr : *pages)
        {
            Project* proj = (Project*)projptr;
            proj->index = index++;
        }

        // Mark the project and its category as modified
        cat->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    EndDrag_Blog
    Handle the drag end event for blog entries properly
    @param The generated event
==============================*/

void Main::EndDrag_Blog(wxTreeEvent& event)
{
    wxTreeItemId src = this->m_DraggedItem;
    wxTreeItemId dest = event.GetItem();
    this->m_DraggedItem = NULL;

    // Ensure we have a valid destination node
    if (!dest.IsOk() || src == dest)
        return;

    // Ignore placing the source directly above the destination, since it won't move at all.
    if (this->m_TreeCtrl_Blog->GetPrevSibling(src) == dest)
        return;

    // Only work on blog entry nodes
    if (!treeitem_iscategory(this->m_TreeCtrl_Blog, dest))
    {
        Category* cat;
        std::vector<void*>* pages;
        int index = 0;
        Blog* blog_src = this->FindBlog(src);
        Blog* blog_dest = this->FindBlog(dest);

        // Ensure a valid source and destination
        if (blog_src == NULL || blog_dest == NULL)
            return;

        // Ensure both blog entries are in the same category
        cat = blog_src->category;
        pages = &cat->pages;
        if (cat != blog_dest->category)
            return;

        // Move the elements
        if (blog_dest->index < blog_src->index)
        {
            pages->erase(pages->begin() + blog_src->index);
            pages->insert(pages->begin() + blog_dest->index + 1, blog_src);
        }
        else
        {
            pages->erase(pages->begin() + blog_src->index);
            pages->insert(pages->begin() + blog_dest->index, blog_src);
        }
        this->m_TreeCtrl_Blog->Delete(src);
        blog_src->treeid = this->m_TreeCtrl_Blog->InsertItem(cat->treeid, dest, blog_src->displayname);

        // Correct the index values
        for (void* blogptr : *pages)
        {
            Blog* bentry = (Blog*)blogptr;
            bentry->index = index++;
        }

        // Mark the blog entry and its category as modified
        cat->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    Save
    Save the entire website and compile it
==============================*/

void Main::Save()
{
    // Save the json files
    json_save(this->m_WorkingDir, &this->m_Category_Projects, &this->m_Category_Blog);

    // Compile the website
    this->CompileProjects();
    this->CompileBlog();
    this->CompileTags();
    this->CompileHomePage();

    // Mark the program as no longer modified
    this->MarkModified(false);
}


/*==============================
    CompileProjects
    Compile all project-related pages
==============================*/

void Main::CompileProjects()
{
    this->CompileProjects_List();

    // Now create the page for each project
    for (Category* cat : this->m_Category_Projects)
        for (void* page : cat->pages)
            this->CompileProjects_Project((Project*)page);
}


/*==============================
    CompileBlog
    Compile all blog-related pages
==============================*/

void Main::CompileBlog()
{
    this->CompileBlog_List();

    // Now create the page for each blog entry
    for (Category* cat : this->m_Category_Blog)
        for (void* page : cat->pages)
            this->CompileBlog_Entry((Blog*)page);
}


/*==============================
    CompileProjects_List
    Compile the project list page
==============================*/

void Main::CompileProjects_List()
{
    bool modified = false;
    wxTextFile out(this->m_WorkingDir + wxString("/projects.html"));
    wxDateTime today = wxDateTime::Today();
    wxString html_final = wxString("");
    wxString html_categories = wxString("");

    // Check if a category was modified, if not, then skip page generation
    for (Category* cat : this->m_Category_Projects)
    {
        if (cat->wasmodified)
        {
            modified = true;
            break;
        }
    }
    if (!modified)
        return;

    // Read the projects page template
    html_final = string_fromfile(this->m_WorkingDir + "/templates/projects.html");

    // Generate the page from the section + project templates
    for (Category* cat : this->m_Category_Projects)
    {
        wxString html_projects = wxString("");
        wxString relativepath = wxString("projects/") + cat->foldername + wxString("/");
        const char* mdstr = md_sanitize(&cat->description)->mb_str();
        wxString desc;

        // If there's no projects in this category, skip it
        if (cat->pages.size() == 0)
            continue;

        // Generate the project blocks
        for (void* page : cat->pages)
        {
            Project* proj = (Project*)page;
            html_projects += string_fromfile(this->m_WorkingDir + "/templates/projects_project.html");
            html_projects.Replace("_TEMPLATE_PROJECT_URL_", relativepath + proj->filename + wxString(".html"));
            html_projects.Replace("_TEMPLATE_PROJECT_TITLE_", proj->displayname);
            html_projects.Replace("_TEMPLATE_PROJECT_IMAGE_", relativepath + proj->icon);
            html_projects.Replace("_TEMPLATE_PROJECT_TOOLTIP_", proj->tooltip);
            html_projects.Append("\r\n");
        }

        // Generate the section blocks
        html_categories += string_fromfile(this->m_WorkingDir + "/templates/projects_section.html");
        html_categories.Replace("_TEMPLATE_TITLE_", cat->displayname);
        html_categories.Replace("_TEMPLATE_HREF_", cat->foldername);
        md_html(mdstr, strlen(mdstr), md4c_funcptr_handlestr, &desc, MD_FLAG_NOHTMLBLOCKS | MD_FLAG_HEADINGAUTOID, 0, new MD_TOC_OPTIONS());
        if (desc.Length() == 0 && cat->description.Length() > 0)
        {
            wxMessageDialog dialog(this, wxString("Markdown parsing was unsuccessful for '") + cat->displayname + wxString("'! Do you have some unsupported unicode?"), wxString("MD Generation Failure"), wxICON_EXCLAMATION);
            dialog.ShowModal();
        }
        desc.Replace("<p>", "<p align=\"left\">");
        html_categories.Replace("_TEMPLATE_SECTION_DESCRIPTION_", *md_unsanitize(&desc));
        html_categories.Replace("_TEMPLATE_PROJECT_LIST_", html_projects);
        html_categories.Append("\r\n");

        // Mark the category as unmodified
        cat->wasmodified = false;
    }

    // Finalize the projects page
    html_final.Replace("_TEMPLATE_PROJECTS_LIST_", html_categories);
    html_final.Replace("_TEMPLATE_PROJECTS_DATE_", today.Format("%b %d %Y"));

    // Dump the html for the projects page
    if (!out.Exists())
        out.Create();
    out.Open();
    out.Clear();
    out.AddLine(html_final);
    out.Write();
    out.Close();
}


/*==============================
    CompileProjects_Project
    Compile a project page
    @param The project page to compile
==============================*/

void Main::CompileProjects_Project(Project* proj)
{
    wxString html_final = wxString("");
    Category* cat = proj->category;
    wxString relativepath = wxString("projects/") + cat->foldername + wxString("/");
    wxString projoutpath = this->m_WorkingDir + wxString("/") + relativepath + proj->filename + wxString(".html");
    wxTextFile projout(projoutpath);
    wxString html_md = wxString(""), pagetags = wxString("");
    const char* mdstr = proj->description.mb_str();
    std::vector<wxString> images;
    std::vector<wxString> youtubes;

    // Skip this project if it wasn't modified
    if (!proj->wasmodified)
        return;

    // Replace most of the basic page info
    html_final = string_fromfile(this->m_WorkingDir + "/templates/project.html");
    html_final.Replace("_TEMPLATE_PROJECTS_TITLE_", proj->displayname);
    html_final.Replace("_TEMPLATE_PROJECTS_DATE_", proj->date);
    md_html(mdstr, strlen(mdstr), md4c_funcptr_handlestr, &html_md, MD_FLAG_NOHTMLBLOCKS | MD_FLAG_HEADINGAUTOID, 0, new MD_TOC_OPTIONS());
    if (html_md.Length() == 0 && proj->description.Length() > 0)
    {
        wxMessageDialog dialog(this, wxString("Markdown parsing was unsuccessful for '") + proj->displayname + wxString("'! Do you have some unsupported unicode?"), wxString("MD Generation Failure"), wxICON_EXCLAMATION);
        dialog.ShowModal();
    }
    html_final.Replace("_TEMPLATE_PROJECTS_DESCRIPTION_", html_md);
    html_final.Replace("_TEMPLATE_PROJECTS_CATEGORY_", proj->category->foldername);
    html_final.Replace("_TEMPLATE_PROJECT_URL_", relativepath + proj->filename + wxString(".html"));
    html_final.Replace("_TEMPLATE_PROJECT_IMAGE_", relativepath + proj->icon);
    html_final.Replace("_TEMPLATE_PROJECT_TOOLTIP_", proj->tooltip);

    // Handle image carousel
    for (wxString str : proj->images)
    {
        str.Replace("\"", wxString(""));
        if (str.Contains("youtube.com"))
            youtubes.push_back(str.AfterFirst('=').Left(11));
        else if (str.Contains("youtu.be"))
            youtubes.push_back(str.Mid(str.Find('/', true), 11));
        else
            images.push_back(str);
    }
    if (images.size() > 0 || youtubes.size() > 0)
    {
        int i;
        wxString carousel = string_fromfile(this->m_WorkingDir + "/templates/project_carousel.html");
        wxString carousel_object = wxString("");
        wxString carousel_list = wxString("");
        if (images.size() > 0)
        {
            carousel_object += string_fromfile(this->m_WorkingDir + "/templates/project_carousel_object_img.html");
            carousel_object.Replace("_TEMPLATE_PROJECTS_CAROUSEL_IMAGE_", images[0]);
        }
        if (youtubes.size() > 0)
        {
            carousel_object += string_fromfile(this->m_WorkingDir + "/templates/project_carousel_object_youtube.html");
            carousel_object.Replace("_TEMPLATE_PROJECTS_CAROUSEL_YOUTUBE_", youtubes[0]);
        }
        i=0;
        for (wxString str : youtubes)
        {
            wxString obj = string_fromfile(this->m_WorkingDir + "/templates/project_carousel_list_youtube.html");
            if (i == 0 && images.size() == 0)
                obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_SELECTED_", "selected");
            else
                obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_SELECTED_", "");
            obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_YOUTUBE_", str);
            carousel_list += obj;
            i++;
        }
        i=0;
        for (wxString str : images)
        {
            wxString obj = string_fromfile(this->m_WorkingDir + "/templates/project_carousel_list_img.html");
            if (i == 0)
                obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_SELECTED_", "selected");
            else
                obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_SELECTED_", "");
            obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_IMAGE_", str);
            carousel_list += obj;
            i++;
        }
        carousel.Replace("_TEMPLATE_PROJECTS_CAROUSEL_OBJECTS_", carousel_object);
        carousel.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_", carousel_list);
        html_final.Replace("_TEMPLATE_PROJECTS_IMAGES_", carousel);
    }
    else
        html_final.Replace("_TEMPLATE_PROJECTS_IMAGES_", wxString(""));

    // Handle page tags
    for (wxString tag : proj->tags)
    {
        wxString str = string_fromfile(this->m_WorkingDir + "/templates/project_tag.html");
        str.Replace("_TEMPLATE_TAG_URL_", wxString("../../tags/") + sanitize_tagname(tag) + wxString(".html"));
        str.Replace("_TEMPLATE_TAG_NAME_", tag);
        pagetags += str;
        if (tag != proj->tags.back())
            pagetags += wxString(", ");
    }
    html_final.Replace("_TEMPLATE_PROJECTS_TAGS_", pagetags);

    // Handle page URLS
    if (proj->urls.size() > 0)
    {
        wxString url_list = wxString("");
        for (wxString str : proj->urls)
        {
            wxString obj = string_fromfile(this->m_WorkingDir + "/templates/project_url.html");
            str.Replace("\"", wxString(""));
            if (str.Contains("github.com"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadgh.png");
            else if (str.Contains("gitlab.com"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadgl.png");
            else if (str.Contains("store.steampowered.com"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadst.png");
            else if (str.Contains("steamcommunity.com"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadsw.png");
            else if (str.Contains("youtube.com") || str.Contains("youtu.be"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadyt.png");
            else if (str.Contains("knockout.chat"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadko.png");
            else if (str.Contains("buu342.me/blog") && !str.Contains("/downloads/"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadbp.png");
            else
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadzp.png");
            obj.Replace("_TEMPLATE_PROJECTS_URL_", str);
            url_list += obj;
        }
        html_final.Replace("_TEMPLATE_PROJECTS_LINKS_", url_list);
    }
    else
        html_final.Replace("_TEMPLATE_PROJECTS_LINKS_", wxString(""));

    // Generate the page itself
    if (!projout.Exists())
        projout.Create();
    projout.Open();
    projout.Clear();
    projout.AddLine(html_final);
    projout.Write();
    projout.Close();

    // Mark the project as no longer modified
    proj->wasmodified = false;
}


/*==============================
    CompileBlog_List
    Compile the blog entry list page
==============================*/

void Main::CompileBlog_List()
{
    bool modified = false;
    wxTextFile out(this->m_WorkingDir + wxString("/blog.html"));
    wxDateTime today = wxDateTime::Today();
    wxString html_final = wxString("");
    wxString html_categories = wxString("");

    // Check if a category was modified, if not, then skip page generation
    for (Category* cat : this->m_Category_Blog)
    {
        if (cat->wasmodified)
        {
            modified = true;
            break;
        }
    }
    if (!modified)
        return;

    // Read the blog page template
    html_final = string_fromfile(this->m_WorkingDir + "/templates/blog.html");

    // Generate the page from the section + blog templates
    for (Category* cat : this->m_Category_Blog)
    {
        wxString html_blogentries = wxString("");
        wxString relativepath = wxString("blog/") + cat->foldername + wxString("/");
        const char* mdstr = cat->description.mb_str();
        wxString desc = wxString("");

        // If there's no blog entries in this category, skip it
        if (cat->pages.size() == 0)
            continue;

        // Generate the blog blocks
        for (void* page : cat->pages)
        {
            Blog* bentry = (Blog*)page;
            html_blogentries += string_fromfile(this->m_WorkingDir + "/templates/blog_section_entry.html");
            html_blogentries.Replace("_TEMPLATE_BLOG_URL_", relativepath + bentry->filename + wxString(".html"));
            html_blogentries.Replace("_TEMPLATE_BLOG_TITLE_", bentry->displayname);
            html_blogentries.Replace("_TEMPLATE_BLOG_IMAGE_", relativepath + bentry->icon);
            html_blogentries.Replace("_TEMPLATE_BLOG_TOOLTIP_", bentry->tooltip);
            html_blogentries.Append("\r\n");
        }

        // Generate the section blocks
        html_categories += string_fromfile(this->m_WorkingDir + "/templates/blog_section.html");
        html_categories.Replace("_TEMPLATE_TITLE_", cat->displayname);
        html_categories.Replace("_TEMPLATE_HREF_", cat->foldername);
        md_html(mdstr, strlen(mdstr), md4c_funcptr_handlestr, &desc, MD_FLAG_NOHTMLBLOCKS | MD_FLAG_HEADINGAUTOID, 0, new MD_TOC_OPTIONS());
        if (desc.Length() == 0 && cat->description.Length() > 0)
        {
            wxMessageDialog dialog(this, wxString("Markdown parsing was unsuccessful for '") + cat->displayname + wxString("'! Do you have some unsupported unicode?"), wxString("MD Generation Failure"), wxICON_EXCLAMATION);
            dialog.ShowModal();
        }
        desc.Replace("<p>", "<p align=\"left\">");
        html_categories.Replace("_TEMPLATE_SECTION_DESCRIPTION_", desc);
        html_categories.Replace("_TEMPLATE_BLOG_LIST_", html_blogentries);
        html_categories.Append("\r\n");
        
        // Mark the category as unmodified
        cat->wasmodified = false;
    }

    // Finalize the blog page
    html_final.Replace("_TEMPLATE_BLOG_LIST_", html_categories);
    html_final.Replace("_TEMPLATE_BLOG_DATE_", today.Format("%b %d %Y"));

    // Dump the html for the blog page
    if (!out.Exists())
        out.Create();
    out.Open();
    out.Clear();
    out.AddLine(html_final);
    out.Write();
    out.Close();
}


/*==============================
    CompileBlog_Entry
    Compile a blog entry
    @param The blog entry page to compile
==============================*/

void Main::CompileBlog_Entry(Blog* bentry)
{
    wxString html_final = wxString("");
    Category* cat = bentry->category;
    wxString relativepath = wxString("blog/") + cat->foldername + wxString("/");
    wxString bentryoutpath = this->m_WorkingDir + wxString("/") + relativepath + bentry->filename + wxString(".html");
    wxTextFile bentryout(bentryoutpath);
    wxString html_md = wxString(""), pagetags = wxString("");
    const char* mdstr = md_sanitize(&bentry->content)->mb_str();

    // Skip this blog entry if it wasn't modified
    if (!bentry->wasmodified)
        return;

    // Replace most of the basic page info
    html_final = string_fromfile(this->m_WorkingDir + "/templates/blog_entry.html");
    html_final.Replace("_TEMPLATE_BLOG_TITLE_", bentry->displayname);
    html_final.Replace("_TEMPLATE_BLOG_DATE_", bentry->date);
    md_html(mdstr, strlen(mdstr), md4c_funcptr_handlestr, &html_md, MD_FLAG_NOHTMLBLOCKS | MD_FLAG_HEADINGAUTOID, 0, new MD_TOC_OPTIONS());
    if (html_md.Length() == 0 && bentry->content.Length() > 0)
    {
        wxMessageDialog dialog(this, wxString("Markdown parsing was unsuccessful for '") + bentry->displayname + wxString("'! Do you have some unsupported unicode?"), wxString("MD Generation Failure"), wxICON_EXCLAMATION);
        dialog.ShowModal();
    }
    html_final.Replace("_TEMPLATE_BLOG_CONTENT_", *md_unsanitize(&html_md));
    html_final.Replace("_TEMPLATE_BLOG_CATEGORY_", bentry->category->foldername);
    html_final.Replace("_TEMPLATE_BLOG_URL_", relativepath + bentry->filename + wxString(".html"));
    html_final.Replace("_TEMPLATE_BLOG_IMAGE_", relativepath + bentry->icon);
    html_final.Replace("_TEMPLATE_BLOG_TOOLTIP_", bentry->tooltip);

    // Handle page tags
    for (wxString tag : bentry->tags)
    {
        wxString str = string_fromfile(this->m_WorkingDir + "/templates/blog_entry_tag.html");
        str.Replace("_TEMPLATE_TAG_URL_", wxString("../../tags/") + sanitize_tagname(tag) + wxString(".html"));
        str.Replace("_TEMPLATE_TAG_NAME_", tag);
        pagetags += str;
        if (tag != bentry->tags.back())
            pagetags += wxString(", ");
    }
    html_final.Replace("_TEMPLATE_BLOG_TAGS_", pagetags);
    
    // Generate the page itself
    if (!bentryout.Exists())
        bentryout.Create();
    bentryout.Open();
    bentryout.Clear();
    bentryout.AddLine(html_final);
    bentryout.Write();
    bentryout.Close();

    // Mark the blog entry as no longer modified
    bentry->wasmodified = false;
}


/*==============================
    CompileTags
    Compile the tag pages
==============================*/

void Main::CompileTags()
{
    bool cont;
    wxString filename;
    wxString tagspath = this->m_WorkingDir + wxString("/tags");
    wxDir tagsdir(tagspath);
    std::map<wxString, std::vector<TaggedPage>> tagmap;

    // Go through all projects and add each one to the tag map
    for (Category* cat : this->m_Category_Projects)
    {
        for (void* page : cat->pages)
        {
            for (wxString tag : ((Project*)page)->tags)
            {
                std::map<wxString, std::vector<TaggedPage>>::iterator foundtag = tagmap.find(tag);
                if (foundtag == tagmap.end())
                    foundtag = tagmap.insert(std::pair<wxString, std::vector<TaggedPage>>(tag, std::vector<TaggedPage>())).first;
                foundtag->second.push_back({PAGETYPE_PROJECT, page});
            }
        }
    }

    // Now the blogs
    for (Category* cat : this->m_Category_Blog)
    {
        for (void* page : cat->pages)
        {
            for (wxString tag : ((Blog*)page)->tags)
            {
                std::map<wxString, std::vector<TaggedPage>>::iterator foundtag = tagmap.find(tag);
                if (foundtag == tagmap.end())
                    foundtag = tagmap.insert(std::pair<wxString, std::vector<TaggedPage>>(tag, std::vector<TaggedPage>())).first;
                foundtag->second.push_back({PAGETYPE_BLOG, page});
            }
        }
    }

    // Purge the tags folder of html files
    cont = tagsdir.GetFirst(&filename, "*.html");
    while (cont)
    {
        wxRemoveFile(tagspath + wxString("/") + filename);
        cont = tagsdir.GetNext(&filename);
    }

    // Now, alphabetically sort all the pages in the tags, and make their respective pages
    for (std::pair<wxString, std::vector<TaggedPage>> it : tagmap)
    {
        wxString tagname_page;
        wxString pagelist;
        wxString html_final = string_fromfile(this->m_WorkingDir + "/templates/tag.html");
        std::sort(it.second.begin(), it.second.end(), taggedpage_sorter);

        // For a valid tag, its name needs to be sanitized
        tagname_page = sanitize_tagname(it.first);

        // Fill the tag page
        html_final.Replace("_TEMPLATE_TAG_NAME_", it.first);
        html_final.Replace("_TEMPLATE_TAG_CLEANNAME_", tagspath + wxString("/") + tagname_page + wxString(".html"));
        for (TaggedPage tp : it.second)
        {
            wxString item = string_fromfile(this->m_WorkingDir + "/templates/tag_item.html");
            if (tp.type == PAGETYPE_PROJECT)
            {
                Project* proj = (Project*)tp.page;
                item.Replace("_TEMPLATE_TAG_PAGENAME_", proj->displayname);
                item.Replace("_TEMPLATE_TAG_PAGEURL_", wxString("../projects/") + proj->category->foldername + wxString("/") + proj->filename + wxString(".html"));
            }
            else if (tp.type == PAGETYPE_BLOG)
            {
                Blog* bentry = (Blog*)tp.page;
                item.Replace("_TEMPLATE_TAG_PAGENAME_", bentry->displayname);
                item.Replace("_TEMPLATE_TAG_PAGEURL_", wxString("../blog/") + bentry->category->foldername + wxString("/") + bentry->filename + wxString(".html"));
            }
            pagelist += item + wxString("\r\n");
        }
        html_final.Replace("_TEMPLATE_TAG_PAGELIST_", pagelist);

        // Generate the page itself
        wxTextFile out(tagspath + wxString("/") + tagname_page + wxString(".html"));
        if (!out.Exists())
            out.Create();
        out.Open();
        out.Clear();
        out.AddLine(html_final);
        out.Write();
        out.Close();
    }
}


/*==============================
    CompileTags
    Compile the homepage
==============================*/

void Main::CompileHomePage()
{
    int count;
    wxString homepath = this->m_WorkingDir + wxString("/index.html");
    wxTextFile fileout(homepath);
    wxString html_entry;
    wxString html_final = string_fromfile(this->m_WorkingDir + "/templates/index.html");
    std::vector<Project*> latestprojects;
    std::vector<Blog*> latestblogentries;

    // Get the list of projects with dates
    for (Category* cat : this->m_Category_Projects)
    {
        for (void* page : cat->pages)
        {
            Project* proj = (Project*)page;
            if (proj->date != "")
                latestprojects.push_back(proj);
        }
    }

    // Get the list of blog entries with dates
    for (Category* cat : this->m_Category_Blog)
    {
        for (void* page : cat->pages)
        {
            Blog* bentry = (Blog*)page;
            if (bentry->date != "")
                latestblogentries.push_back(bentry);
        }
    }
    

    // Paste the 5 most recent projects
    count = 0;
    html_entry = wxString("");
    std::sort(latestprojects.begin(), latestprojects.end(), &project_sorter_date);
    for (Project* proj : latestprojects)
    {
        wxDateTime date;
        wxString tableelem;
        if (count == 5)
            break;
        tableelem = string_fromfile(this->m_WorkingDir + "/templates/index_latest.html");
        if (date_fromstring(proj->date, &date))
            tableelem.Replace("_TEMPLATE_LATEST_ENTRY_DATE_", date.Format("%d/%m/%Y"));
        else
            tableelem.Replace("_TEMPLATE_LATEST_ENTRY_DATE_", "??/??/????");
        tableelem.Replace("_TEMPLATE_LATEST_ENTRY_CATEGORY_", proj->category->displayname);
        tableelem.Replace("_TEMPLATE_LATEST_ENTRY_NAME_", proj->displayname);
        tableelem.Replace("_TEMPLATE_ENTRY_CATEGORY_", wxString("projects.html#") + proj->category->foldername);
        tableelem.Replace("_TEMPLATE_ENTRY_PAGE_", wxString("projects/") + proj->category->foldername + wxString("/") + proj->filename + wxString(".html"));
        html_entry += tableelem + wxString("\r\n");
        count++;
    }
    html_final.Replace("_TEMPLATE_LATEST_PROJECTS_", html_entry);

    // Paste the 5 most recent blog entries
    count = 0;
    html_entry = wxString("");
    std::sort(latestblogentries.begin(), latestblogentries.end(), &blog_sorter_date);
    for (Blog* bentry : latestblogentries)
    {
        wxDateTime date;
        wxString tableelem;
        if (count == 5)
            break;
        tableelem = string_fromfile(this->m_WorkingDir + "/templates/index_latest.html");
        if (date_fromstring(bentry->date, &date))
            tableelem.Replace("_TEMPLATE_LATEST_ENTRY_DATE_", date.Format("%d/%m/%Y"));
        else
            tableelem.Replace("_TEMPLATE_LATEST_ENTRY_DATE_", "??/??/????");
        tableelem.Replace("_TEMPLATE_LATEST_ENTRY_CATEGORY_", bentry->category->displayname);
        tableelem.Replace("_TEMPLATE_LATEST_ENTRY_NAME_", bentry->displayname);
        tableelem.Replace("_TEMPLATE_ENTRY_CATEGORY_", wxString("blog.html#") + bentry->category->foldername);
        tableelem.Replace("_TEMPLATE_ENTRY_PAGE_", wxString("blog/") + bentry->category->foldername + wxString("/") + bentry->filename + wxString(".html"));
        html_entry += tableelem + wxString("\r\n");;
        count++;
    }
    html_final.Replace("_TEMPLATE_LATEST_BLOGPOSTS_", html_entry);

    // Generate the page itself
    if (!fileout.Exists())
        fileout.Create();
    fileout.Open();
    fileout.Clear();
    fileout.AddLine(html_final);
    fileout.Write();
    fileout.Close();
}


/*==============================
    MarkModified
    Mark the work as modified
==============================*/

void Main::MarkModified(bool modified)
{
    if (modified)
    {
        if (!this->m_Modified)
        {
            this->SetTitle(this->m_WorkingDir + wxString(" *"));
            this->m_Modified = true;
        }
    }
    else
    {
        this->SetTitle(this->m_WorkingDir);
        this->m_Modified = false;
    }
}


/*==============================
    ShowProjectEditor
    Show or hide the project editor panel
    @param Whether to show the editor panel 
==============================*/

void Main::ShowProjectEditor(bool show)
{
    if (show)
    {
        this->m_ScrolledWindow_Project_Editor->Show();
        this->m_ScrolledWindow_ProjectCategory_Editor->Hide();
        this->m_Button_Projects_Preview->Show();
    }
    else
    {
        this->m_ScrolledWindow_Project_Editor->Hide();
        this->m_ScrolledWindow_ProjectCategory_Editor->Hide();
        this->m_Button_Projects_Preview->Hide();
    }
    this->m_Panel_Projects_Editor->Layout();
}


/*==============================
    ShowProjectCategoryEditor
    Show or hide the project category editor panel
    @param Whether to show the editor panel 
==============================*/

void Main::ShowProjectCategoryEditor(bool show)
{
    if (show)
    {
        this->m_ScrolledWindow_Project_Editor->Hide();
        this->m_ScrolledWindow_ProjectCategory_Editor->Show();
        this->m_Button_Projects_Preview->Show();
    }
    else
    {
        this->m_ScrolledWindow_Project_Editor->Hide();
        this->m_ScrolledWindow_ProjectCategory_Editor->Hide();
        this->m_Button_Projects_Preview->Hide();
    }
    this->m_Panel_Projects_Editor->Layout();
}


/*==============================
    ShowBlogEditor
    Show or hide the blog entry editor panel
    @param Whether to show the editor panel 
==============================*/

void Main::ShowBlogEditor(bool show)
{
    if (show)
    {
        this->m_ScrolledWindow_Blog_Editor->Show();
        this->m_ScrolledWindow_BlogCategory_Editor->Hide();
        this->m_Button_Blog_Preview->Show();
    }
    else
    {
        this->m_ScrolledWindow_Blog_Editor->Hide();
        this->m_ScrolledWindow_BlogCategory_Editor->Hide();
        this->m_Button_Blog_Preview->Hide();
    }
    this->m_Panel_Blog_Editor->Layout();
}


/*==============================
    ShowBlogCategoryEditor
    Show or hide the blog entry category editor panel
    @param Whether to show the editor panel 
==============================*/

void Main::ShowBlogCategoryEditor(bool show)
{
    if (show)
    {
        this->m_ScrolledWindow_Blog_Editor->Hide();
        this->m_ScrolledWindow_BlogCategory_Editor->Show();
        this->m_Button_Blog_Preview->Show();
    }
    else
    {
        this->m_ScrolledWindow_Blog_Editor->Hide();
        this->m_ScrolledWindow_BlogCategory_Editor->Hide();
        this->m_Button_Blog_Preview->Hide();
    }
    this->m_Panel_Blog_Editor->Layout();
}