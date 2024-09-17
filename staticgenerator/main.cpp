#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include "main.h"
#include "include/json.hpp"
#include "include/maddy/parser.h"
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/treelist.h>
#include <wx/dir.h>
#include <wx/textfile.h>
#include <wx/dirdlg.h>

bool category_sorter(Category* lhs, Category* rhs)
{
    return lhs->index < rhs->index;
}

bool project_sorter(Project* lhs, Project* rhs)
{
    return lhs->index < rhs->index;
}

bool treeitem_iscategory(wxTreeCtrl* tree, wxTreeItemId item)
{
    return tree->GetRootItem() == tree->GetItemParent(item);
}

wxString string_fromfile(wxString path)
{
    wxTextFile file;
    wxString str = wxString("");

    // Ensure we managed to open the file
    file.Open(path);
    if (!file.IsOpened())
        return wxString("");

    // Read the file into the string, and return it
    str += file.GetFirstLine();
    while(!file.Eof())
        str += wxString("\r\n") + file.GetNextLine();
    return str;
}

Main::Main(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    this->m_WorkingDir = wxGetCwd();
    this->m_SelectedItem = NULL;
    this->MarkModified(false);

    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* m_Sizer_Main;
    m_Sizer_Main = new wxBoxSizer( wxVERTICAL );

    m_ChoiceBook_PageSelection = new wxChoicebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
    m_Panel_Projects = new wxPanel( m_ChoiceBook_PageSelection, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* m_Sizer_Projects;
    m_Sizer_Projects = new wxBoxSizer( wxVERTICAL );

    m_Splitter_Projects = new wxSplitterWindow( m_Panel_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
    m_Splitter_Projects->SetSashGravity( 1 );
    m_Splitter_Projects->Connect( wxEVT_IDLE, wxIdleEventHandler( Main::m_Splitter_ProjectsOnIdle ), NULL, this );

    m_Panel_Projects_Tree = new wxPanel( m_Splitter_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer* m_Sizer_Projects_Tree;
    m_Sizer_Projects_Tree = new wxFlexGridSizer( 0, 0, 0, 0 );
    m_Sizer_Projects_Tree->AddGrowableCol( 0 );
    m_Sizer_Projects_Tree->AddGrowableRow( 0 );
    m_Sizer_Projects_Tree->SetFlexibleDirection( wxBOTH );
    m_Sizer_Projects_Tree->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_TreeCtrl_Projects = new wxTreeCtrl( m_Panel_Projects_Tree, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_HIDE_ROOT );
    m_Sizer_Projects_Tree->Add( m_TreeCtrl_Projects, 0, wxALL|wxEXPAND, 5 );


    m_Panel_Projects_Tree->SetSizer( m_Sizer_Projects_Tree );
    m_Panel_Projects_Tree->Layout();
    m_Sizer_Projects_Tree->Fit( m_Panel_Projects_Tree );
    m_Panel_Projects_Editor = new wxPanel( m_Splitter_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer* m_Sizer_Projects_Editor;
    m_Sizer_Projects_Editor = new wxFlexGridSizer( 3, 1, 0, 0 );
    m_Sizer_Projects_Editor->AddGrowableCol( 0 );
    m_Sizer_Projects_Editor->AddGrowableRow( 0 );
    m_Sizer_Projects_Editor->AddGrowableRow( 1 );
    m_Sizer_Projects_Editor->SetFlexibleDirection( wxBOTH );
    m_Sizer_Projects_Editor->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );

    m_ScrolledWindow_Project_Editor = new wxScrolledWindow( m_Panel_Projects_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
    m_ScrolledWindow_Project_Editor->SetScrollRate( 5, 5 );
    m_ScrolledWindow_Project_Editor->Hide();
    m_ScrolledWindow_Project_Editor->SetMinSize( wxSize( 0,0 ) );

    wxFlexGridSizer* m_Sizer_ScrolledWindow_Editor;
    m_Sizer_ScrolledWindow_Editor = new wxFlexGridSizer( 0, 1, 0, 0 );
    m_Sizer_ScrolledWindow_Editor->AddGrowableCol( 0 );
    m_Sizer_ScrolledWindow_Editor->AddGrowableRow( 1 );
    m_Sizer_ScrolledWindow_Editor->SetFlexibleDirection( wxBOTH );
    m_Sizer_ScrolledWindow_Editor->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_Sizer_ScrolledWindow_Editor->SetMinSize( wxSize( 0,0 ) );
    wxFlexGridSizer* m_Sizer_ScrolledWindow_Editor_Main;
    m_Sizer_ScrolledWindow_Editor_Main = new wxFlexGridSizer( 0, 2, 0, 0 );
    m_Sizer_ScrolledWindow_Editor_Main->AddGrowableCol( 1 );
    m_Sizer_ScrolledWindow_Editor_Main->SetFlexibleDirection( wxBOTH );
    m_Sizer_ScrolledWindow_Editor_Main->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_Sizer_ScrolledWindow_Editor_Main->SetMinSize( wxSize( 0,0 ) );
    m_Label_Projects_File = new wxStaticText( m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("File:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_Projects_File->Wrap( -1 );
    m_Sizer_ScrolledWindow_Editor_Main->Add( m_Label_Projects_File, 0, wxALL, 5 );

    m_TextCtrl_Projects_File = new wxTextCtrl( m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_Projects_File->SetToolTip( wxT("The filename of the project") );

    m_Sizer_ScrolledWindow_Editor_Main->Add( m_TextCtrl_Projects_File, 0, wxALL|wxEXPAND, 5 );

    m_Label_Projects_Name = new wxStaticText( m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Display Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_Projects_Name->Wrap( -1 );
    m_Sizer_ScrolledWindow_Editor_Main->Add( m_Label_Projects_Name, 0, wxALL, 5 );

    m_TextCtrl_Projects_Name = new wxTextCtrl( m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_Projects_Name->SetToolTip( wxT("The display name of the project") );

    m_Sizer_ScrolledWindow_Editor_Main->Add( m_TextCtrl_Projects_Name, 0, wxALL|wxEXPAND, 5 );

    m_Label_Projects_Icon = new wxStaticText( m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Icon:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_Projects_Icon->Wrap( -1 );
    m_Sizer_ScrolledWindow_Editor_Main->Add( m_Label_Projects_Icon, 0, wxALL, 5 );

    m_TextCtrl_Projects_Icon = new wxTextCtrl( m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_Projects_Icon->SetToolTip( wxT("The icon used to display the project in the list of projects. Requires a relative path (to the working directory) to a 240x125 png.") );

    m_Sizer_ScrolledWindow_Editor_Main->Add( m_TextCtrl_Projects_Icon, 0, wxALL|wxEXPAND, 5 );

    m_Label_Projects_Tags = new wxStaticText( m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Tags:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_Projects_Tags->Wrap( -1 );
    m_Sizer_ScrolledWindow_Editor_Main->Add( m_Label_Projects_Tags, 0, wxALL, 5 );

    m_TextCtrl_Projects_Tags = new wxTextCtrl( m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_Projects_Tags->SetToolTip( wxT("Project tags, comma separated.") );

    m_Sizer_ScrolledWindow_Editor_Main->Add( m_TextCtrl_Projects_Tags, 0, wxALL|wxEXPAND, 5 );

    m_Label_Projects_Images = new wxStaticText( m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Images:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_Projects_Images->Wrap( -1 );
    m_Sizer_ScrolledWindow_Editor_Main->Add( m_Label_Projects_Images, 0, wxALL, 5 );

    m_TextCtrl_Projects_Images = new wxTextCtrl( m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_Projects_Images->SetToolTip( wxT("Images for this project, comma separated. Paths should be relative to the working directory.") );

    m_Sizer_ScrolledWindow_Editor_Main->Add( m_TextCtrl_Projects_Images, 0, wxALL|wxEXPAND, 5 );

    m_Label_Projects_Date = new wxStaticText( m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Date:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_Projects_Date->Wrap( -1 );
    m_Sizer_ScrolledWindow_Editor_Main->Add( m_Label_Projects_Date, 0, wxALL|wxEXPAND, 5 );

    m_TextCtrl_Projects_Date = new wxTextCtrl( m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_Projects_Date->SetToolTip( wxT("The date of this project's release.") );

    m_Sizer_ScrolledWindow_Editor_Main->Add( m_TextCtrl_Projects_Date, 0, wxALL|wxEXPAND, 5 );

    m_Label_Projects_URLs = new wxStaticText( m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("URLs:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_Projects_URLs->Wrap( -1 );
    m_Sizer_ScrolledWindow_Editor_Main->Add( m_Label_Projects_URLs, 0, wxALL, 5 );

    m_TextCtrl_Projects_URLs = new wxTextCtrl( m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_Projects_URLs->SetToolTip( wxT("URLs for this project, comma separated.") );

    m_Sizer_ScrolledWindow_Editor_Main->Add( m_TextCtrl_Projects_URLs, 0, wxALL|wxEXPAND, 5 );

    m_Label_Projects_ToolTip = new wxStaticText( m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Subtitle:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_Projects_ToolTip->Wrap( -1 );
    m_Sizer_ScrolledWindow_Editor_Main->Add( m_Label_Projects_ToolTip, 0, wxALL, 5 );

    m_TextCtrl_Projects_ToolTip = new wxTextCtrl( m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_Projects_ToolTip->SetToolTip( wxT("Brief description that shows up when hovering over a project with the mouse.") );

    m_Sizer_ScrolledWindow_Editor_Main->Add( m_TextCtrl_Projects_ToolTip, 0, wxALL|wxEXPAND, 5 );


    m_Sizer_ScrolledWindow_Editor->Add( m_Sizer_ScrolledWindow_Editor_Main, 1, wxEXPAND, 5 );

    m_TextCtrl_Projects_Description = new wxTextCtrl( m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_MULTILINE );
    m_TextCtrl_Projects_Description->SetToolTip( wxT("The project description. Supports markdown.") );
    m_TextCtrl_Projects_Description->SetMinSize( wxSize( -1,200 ) );

    m_Sizer_ScrolledWindow_Editor->Add( m_TextCtrl_Projects_Description, 0, wxALL|wxEXPAND, 5 );


    m_ScrolledWindow_Project_Editor->SetSizer( m_Sizer_ScrolledWindow_Editor );
    m_ScrolledWindow_Project_Editor->Layout();
    m_Sizer_ScrolledWindow_Editor->Fit( m_ScrolledWindow_Project_Editor );
    m_Sizer_Projects_Editor->Add( m_ScrolledWindow_Project_Editor, 0, wxALL|wxEXPAND, 5 );

    m_ScrolledWindow_ProjectCategory_Editor = new wxScrolledWindow( m_Panel_Projects_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
    m_ScrolledWindow_ProjectCategory_Editor->SetScrollRate( 5, 5 );
    m_ScrolledWindow_ProjectCategory_Editor->Hide();
    m_ScrolledWindow_ProjectCategory_Editor->SetMinSize( wxSize( 0,0 ) );

    wxFlexGridSizer* m_Sizer_ScrolledWindow_ProjectCategory_Editor;
    m_Sizer_ScrolledWindow_ProjectCategory_Editor = new wxFlexGridSizer( 0, 1, 0, 0 );
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->AddGrowableCol( 0 );
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->AddGrowableRow( 1 );
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->SetFlexibleDirection( wxBOTH );
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_Sizer_ScrolledWindow_ProjectCategory_Editor->SetMinSize( wxSize( 0,0 ) );
    wxFlexGridSizer* m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main;
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main = new wxFlexGridSizer( 0, 2, 0, 0 );
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->AddGrowableCol( 1 );
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->SetFlexibleDirection( wxBOTH );
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->SetMinSize( wxSize( 0,0 ) );
    m_Label_ProjectsCategory_Folder = new wxStaticText( m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxT("Folder:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_ProjectsCategory_Folder->Wrap( -1 );
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add( m_Label_ProjectsCategory_Folder, 0, wxALL, 5 );

    m_TextCtrl_ProjectsCategory_Folder = new wxTextCtrl( m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_ProjectsCategory_Folder->Enable( false );
    m_TextCtrl_ProjectsCategory_Folder->SetToolTip( wxT("The folder that represents this category") );

    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add( m_TextCtrl_ProjectsCategory_Folder, 0, wxALL|wxEXPAND, 5 );

    m_Label_ProjectsCategory_DisplayName = new wxStaticText( m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxT("Display Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_ProjectsCategory_DisplayName->Wrap( -1 );
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add( m_Label_ProjectsCategory_DisplayName, 0, wxALL, 5 );

    m_TextCtrl_ProjectsCategory_DisplayName = new wxTextCtrl( m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_ProjectsCategory_DisplayName->SetToolTip( wxT("The display name of this category") );

    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add( m_TextCtrl_ProjectsCategory_DisplayName, 0, wxALL|wxEXPAND, 5 );

    m_Label_ProjectsCategory_Description = new wxStaticText( m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Label_ProjectsCategory_Description->Wrap( -1 );
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add( m_Label_ProjectsCategory_Description, 0, wxALL, 5 );


    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add( 0, 0, 1, wxEXPAND, 5 );


    m_Sizer_ScrolledWindow_ProjectCategory_Editor->Add( m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main, 1, wxEXPAND, 5 );

    m_TextCtrl_ProjectsCategory_Description = new wxTextCtrl( m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_TextCtrl_ProjectsCategory_Description->SetToolTip( wxT("The filename of the project") );
    m_TextCtrl_ProjectsCategory_Description->SetMinSize( wxSize( -1,200 ) );

    m_Sizer_ScrolledWindow_ProjectCategory_Editor->Add( m_TextCtrl_ProjectsCategory_Description, 0, wxALL|wxEXPAND, 5 );


    m_ScrolledWindow_ProjectCategory_Editor->SetSizer( m_Sizer_ScrolledWindow_ProjectCategory_Editor );
    m_ScrolledWindow_ProjectCategory_Editor->Layout();
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->Fit( m_ScrolledWindow_ProjectCategory_Editor );
    m_Sizer_Projects_Editor->Add( m_ScrolledWindow_ProjectCategory_Editor, 0, wxEXPAND | wxALL, 5 );

    m_Button_Projects_Preview = new wxButton( m_Panel_Projects_Editor, wxID_ANY, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Button_Projects_Preview->Hide();
    m_Button_Projects_Preview->SetMinSize( wxSize( -1,24 ) );

    m_Sizer_Projects_Editor->Add( m_Button_Projects_Preview, 0, wxALIGN_RIGHT|wxALL, 5 );


    m_Panel_Projects_Editor->SetSizer( m_Sizer_Projects_Editor );
    m_Panel_Projects_Editor->Layout();
    m_Sizer_Projects_Editor->Fit( m_Panel_Projects_Editor );
    m_Splitter_Projects->SplitVertically( m_Panel_Projects_Tree, m_Panel_Projects_Editor, 188 );
    m_Sizer_Projects->Add( m_Splitter_Projects, 1, wxEXPAND, 5 );


    m_Panel_Projects->SetSizer( m_Sizer_Projects );
    m_Panel_Projects->Layout();
    m_Sizer_Projects->Fit( m_Panel_Projects );
    m_ChoiceBook_PageSelection->AddPage( m_Panel_Projects, wxT("Projects"), true );
    m_Panel_Blog = new wxPanel( m_ChoiceBook_PageSelection, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* m_Sizer_Blog;
    m_Sizer_Blog = new wxBoxSizer( wxVERTICAL );

    m_Splitter_Blog = new wxSplitterWindow( m_Panel_Blog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
    m_Splitter_Blog->SetSashGravity( 1 );
    m_Splitter_Blog->Connect( wxEVT_IDLE, wxIdleEventHandler( Main::m_Splitter_BlogOnIdle ), NULL, this );

    m_Panel_Blog_Tree = new wxPanel( m_Splitter_Blog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer* m_Sizer_Blog_Tree;
    m_Sizer_Blog_Tree = new wxFlexGridSizer( 0, 0, 0, 0 );
    m_Sizer_Blog_Tree->AddGrowableCol( 0 );
    m_Sizer_Blog_Tree->AddGrowableRow( 0 );
    m_Sizer_Blog_Tree->SetFlexibleDirection( wxBOTH );
    m_Sizer_Blog_Tree->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_TreeCtrl_Blog = new wxTreeCtrl( m_Panel_Blog_Tree, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_HIDE_ROOT );
    m_Sizer_Blog_Tree->Add( m_TreeCtrl_Blog, 0, wxALL|wxEXPAND, 5 );


    m_Panel_Blog_Tree->SetSizer( m_Sizer_Blog_Tree );
    m_Panel_Blog_Tree->Layout();
    m_Sizer_Blog_Tree->Fit( m_Panel_Blog_Tree );
    m_Panel_Blog_TextCtrl = new wxPanel( m_Splitter_Blog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_Panel_Blog_TextCtrl->Enable( false );
    m_Panel_Blog_TextCtrl->Hide();

    wxFlexGridSizer* m_Sizer_Blog_TextCtrl;
    m_Sizer_Blog_TextCtrl = new wxFlexGridSizer( 0, 1, 0, 0 );
    m_Sizer_Blog_TextCtrl->AddGrowableCol( 0 );
    m_Sizer_Blog_TextCtrl->AddGrowableRow( 0 );
    m_Sizer_Blog_TextCtrl->SetFlexibleDirection( wxBOTH );
    m_Sizer_Blog_TextCtrl->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_TextCtrl_Blog = new wxTextCtrl( m_Panel_Blog_TextCtrl, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,200 ), wxTE_MULTILINE );
    m_TextCtrl_Blog->SetToolTip( wxT("The project description. Supports markdown.") );

    m_Sizer_Blog_TextCtrl->Add( m_TextCtrl_Blog, 0, wxALL|wxEXPAND, 5 );

    m_Button_Blog_Preview = new wxButton( m_Panel_Blog_TextCtrl, wxID_ANY, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Blog_TextCtrl->Add( m_Button_Blog_Preview, 0, wxALIGN_RIGHT|wxALL, 5 );


    m_Panel_Blog_TextCtrl->SetSizer( m_Sizer_Blog_TextCtrl );
    m_Panel_Blog_TextCtrl->Layout();
    m_Sizer_Blog_TextCtrl->Fit( m_Panel_Blog_TextCtrl );
    m_Splitter_Blog->SplitVertically( m_Panel_Blog_Tree, m_Panel_Blog_TextCtrl, 188 );
    m_Sizer_Blog->Add( m_Splitter_Blog, 1, wxEXPAND, 5 );


    m_Panel_Blog->SetSizer( m_Sizer_Blog );
    m_Panel_Blog->Layout();
    m_Sizer_Blog->Fit( m_Panel_Blog );
    m_ChoiceBook_PageSelection->AddPage( m_Panel_Blog, wxT("Blog"), false );
    m_Sizer_Main->Add( m_ChoiceBook_PageSelection, 1, wxEXPAND | wxALL, 5 );


    this->SetSizer( m_Sizer_Main );
    this->Layout();
    m_Menubar_Main = new wxMenuBar( 0 );
    m_Menu_File = new wxMenu();
    wxMenuItem* m_MenuItem_OpenDir;
    m_MenuItem_OpenDir = new wxMenuItem( m_Menu_File, wxID_ANY, wxString( wxT("Open Working Directory") ) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL );
    m_Menu_File->Append( m_MenuItem_OpenDir );

    wxMenuItem* m_MenuItem_Save;
    m_MenuItem_Save = new wxMenuItem( m_Menu_File, wxID_ANY, wxString( wxT("Save Changes") ) + wxT('\t') + wxT("CTRL+S"), wxEmptyString, wxITEM_NORMAL );
    m_Menu_File->Append( m_MenuItem_Save );

    m_Menubar_Main->Append( m_Menu_File, wxT("File") );

    this->SetMenuBar( m_Menubar_Main );


    this->Centre( wxBOTH );

    // Connect Events
    m_TreeCtrl_Projects->Connect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( Main::m_TreeCtrl_Projects_OnTreeBeginDrag ), NULL, this );
    m_TreeCtrl_Projects->Connect( wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( Main::m_TreeCtrl_Projects_OnTreeEndDrag ), NULL, this );
    m_TreeCtrl_Projects->Connect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit ), NULL, this );
    m_TreeCtrl_Projects->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( Main::m_TreeCtrl_Projects_OnTreeItemMenu ), NULL, this );
    m_TreeCtrl_Projects->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( Main::m_TreeCtrl_Projects_OnTreeSelChanged ), NULL, this );
    m_TextCtrl_Projects_File->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_Projects_File_OnText ), NULL, this );
    m_TextCtrl_Projects_Name->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_Projects_Name_OnText ), NULL, this );
    m_TextCtrl_Projects_Icon->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_Projects_Icon_OnText ), NULL, this );
    m_TextCtrl_Projects_Tags->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_Projects_Tags_OnText ), NULL, this );
    m_TextCtrl_Projects_Images->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_Projects_Images_OnText ), NULL, this );
    m_TextCtrl_Projects_Date->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_Projects_Date_OnText ), NULL, this );
    m_TextCtrl_Projects_URLs->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_Projects_URLs_OnText ), NULL, this );
    m_TextCtrl_Projects_ToolTip->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_Projects_ToolTip_OnText ), NULL, this );
    m_TextCtrl_Projects_Description->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_Projects_Description_OnText ), NULL, this );
    m_TextCtrl_ProjectsCategory_DisplayName->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_ProjectsCategory_DisplayName_OnText ), NULL, this );
    m_TextCtrl_ProjectsCategory_Description->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Main::m_TextCtrl_ProjectsCategory_Description_OnText ), NULL, this );
    m_TreeCtrl_Blog->Connect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( Main::m_TreeCtrl_Blog_OnTreeBeginDrag ), NULL, this );
    m_TreeCtrl_Blog->Connect( wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( Main::m_TreeCtrl_Blog_OnTreeEndDrag ), NULL, this );
    m_TreeCtrl_Blog->Connect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( Main::m_TreeCtrl_Blog_OnTreeEndLabelEdit ), NULL, this );
    m_TreeCtrl_Blog->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( Main::m_TreeCtrl_Blog_OnTreeItemMenu ), NULL, this );
    m_TreeCtrl_Blog->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( Main::m_TreeCtrl_Blog_OnTreeSelChanged ), NULL, this );
    m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Main::m_MenuItem_OpenDir_OnMenuSelection ), this, m_MenuItem_OpenDir->GetId());
    m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Main::m_MenuItem_Save_OnMenuSelection ), this, m_MenuItem_Save->GetId());

    this->m_ChoiceBook_PageSelection->SetSelection(0);
    this->UpdateTree(this->m_TreeCtrl_Projects, "projects", &this->m_Category_Projects);
    this->LoadProjects();
}

Main::~Main()
{
    m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit), NULL, this);
    m_TreeCtrl_Blog->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeEndLabelEdit), NULL, this);
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

void Main::m_MenuItem_OpenDir_OnMenuSelection(wxCommandEvent& event)
{
    wxDirDialog dir(this, wxString("Select Working Directory"), this->m_WorkingDir);
    if (dir.ShowModal() == wxID_OK)
    {
        this->m_WorkingDir = dir.GetPath();
        this->MarkModified(false);
        this->UpdateTree(this->m_TreeCtrl_Projects, "projects", &this->m_Category_Projects);
        this->LoadProjects();
        
        //this->UpdateTree(this->m_TreeCtrl_Blog, "blog", &this->m_Category_Blog);
    }
}

void Main::m_MenuItem_Save_OnMenuSelection(wxCommandEvent& event)
{
    this->Save();
}

void Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit(wxTreeEvent& event)
{
    if (treeitem_iscategory(this->m_TreeCtrl_Projects, event.GetItem()))
    {
        wxTreeItemId id = event.GetItem();
        Category* cat = this->FindCategory(id);
        if (cat == NULL)
            return;
        cat->displayname = event.GetLabel();
        this->MarkModified();
        return;
    }
    else
    {
        Project* proj = this->FindProject(event.GetItem());
        if (proj != NULL)
        {
            proj->displayname = event.GetLabel();
            if (this->m_SelectedItem == event.GetItem())
                this->m_TextCtrl_Projects_Name->SetValue(proj->displayname);
            this->MarkModified();
            return;
        }
    }
    event.Skip();
}

void Main::m_TreeCtrl_Projects_OnTreeBeginDrag(wxTreeEvent& event)
{
    this->m_DraggedItem = event.GetItem();
    event.Allow();
}
void Main::m_TreeCtrl_Projects_OnTreeEndDrag(wxTreeEvent& event)
{
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, this->m_SelectedItem))
        this->EndDrag_Project(event);
    else
        this->EndDrag(event, this->m_TreeCtrl_Projects, &this->m_Category_Projects);
}

void Main::m_TreeCtrl_Projects_OnTreeItemMenu( wxTreeEvent& event )
{
    wxMenu menu;
    this->m_SelectedItem = event.GetItem();
    menu.Append(wxID_NEW, wxT("Create project"));
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, event.GetItem()))
        menu.Append(wxID_DELETE, wxT("Delete project"));
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::OnPopupClick_Projects), NULL, this);
    PopupMenu(&menu, event.GetPoint());
}

void Main::m_TreeCtrl_Projects_OnTreeSelChanged( wxTreeEvent& event )
{
    bool modifiedbeforechange = this->m_Modified;
    wxTreeItemId item = event.GetItem();
    this->m_SelectedItem = item;
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, item))
    {
        wxString wip;
        Project* proj_elem = FindProject(item);
        if (proj_elem == NULL)
            return;
        this->m_TextCtrl_Projects_File->SetValue(proj_elem->filename);
        this->m_TextCtrl_Projects_Name->SetValue(proj_elem->displayname);
        this->m_TextCtrl_Projects_Icon->SetValue(proj_elem->icon);
        this->m_TextCtrl_Projects_ToolTip->SetValue(proj_elem->tooltip);
        this->m_TextCtrl_Projects_Description->SetValue(proj_elem->description);
        
        wip = wxString("");
        for (Tag* tag : proj_elem->tags)
            wip += tag->name + wxString(", ");
        this->m_TextCtrl_Projects_Tags->SetValue(wip);
        
        wip = wxString("");
        for (wxString str : proj_elem->images)
            wip += str + wxString(", ");
        this->m_TextCtrl_Projects_Images->SetValue(wip);
        this->m_TextCtrl_Projects_Date->SetValue(proj_elem->date);
        
        wip = wxString("");
        for (wxString str : proj_elem->urls)
            wip += str + wxString(", ");
        this->m_TextCtrl_Projects_URLs->SetValue(wip);
        this->m_TextCtrl_Projects_ToolTip->SetValue(proj_elem->tooltip);
        this->m_TextCtrl_Projects_Description->SetValue(proj_elem->description);
        this->ShowProjectEditor();
        if (!modifiedbeforechange)
            this->MarkModified(false);
    }
    else if (treeitem_iscategory(this->m_TreeCtrl_Projects, item))
    {
        Category* cat_elem = FindCategory(item);
        if (cat_elem == NULL)
            return;
        this->m_TextCtrl_ProjectsCategory_Folder->SetValue(cat_elem->foldername);
        this->m_TextCtrl_ProjectsCategory_DisplayName->SetValue(cat_elem->displayname);
        this->m_TextCtrl_ProjectsCategory_Description->SetValue(cat_elem->description);
        this->ShowProjectCategoryEditor(true);
        if (!modifiedbeforechange)
            this->MarkModified(false);
    }
    else
        this->ShowProjectEditor(false);
}

void Main::m_TextCtrl_Projects_File_OnText( wxCommandEvent& event )
{
    Project* proj = FindProject(this->m_SelectedItem);
    proj->filename = event.GetString();
    this->MarkModified();
}

void Main::m_TextCtrl_Projects_Name_OnText( wxCommandEvent& event )
{
    Project* proj = FindProject(this->m_SelectedItem);
    proj->displayname = event.GetString();
    this->m_TreeCtrl_Projects->SetItemText(proj->treeid, proj->displayname);
    this->MarkModified();
}

void Main::m_TextCtrl_Projects_Icon_OnText( wxCommandEvent& event )
{
    Project* proj = FindProject(this->m_SelectedItem);
    proj->icon = event.GetString();
    this->MarkModified();
}

void Main::m_TextCtrl_Projects_Tags_OnText( wxCommandEvent& event )
{
    // TODO: Handle tags
}

void Main::m_TextCtrl_Projects_Images_OnText( wxCommandEvent& event )
{
    Project* proj = FindProject(this->m_SelectedItem);
    wxArrayString strarray = wxSplit(event.GetString(), ',');
    proj->images.clear();
    for (wxString str : strarray)
    {
        str.Trim(true);
        str.Trim(false);
        if (str != "")
            proj->images.push_back(str);
    }
    this->MarkModified();
}

void Main::m_TextCtrl_Projects_Date_OnText( wxCommandEvent& event )
{
    Project* proj = FindProject(this->m_SelectedItem);
    proj->date = event.GetString();
    this->MarkModified();
}

void Main::m_TextCtrl_Projects_URLs_OnText( wxCommandEvent& event )
{
    Project* proj = FindProject(this->m_SelectedItem);
    wxArrayString strarray = wxSplit(event.GetString(), ',');
    proj->urls.clear();
    for (wxString str : strarray)
    {
        str.Trim(true);
        str.Trim(false);
        if (str != "")
            proj->urls.push_back(str);
    }
    this->MarkModified();
}

void Main::m_TextCtrl_Projects_Description_OnText( wxCommandEvent& event )
{
    Project* proj = FindProject(this->m_SelectedItem);
    proj->description = event.GetString();
    this->MarkModified();
}

void Main::m_TextCtrl_Projects_ToolTip_OnText( wxCommandEvent& event )
{
    Project* proj = FindProject(this->m_SelectedItem);
    proj->tooltip = event.GetString();
    this->MarkModified();
}

void Main::m_TextCtrl_ProjectsCategory_DisplayName_OnText( wxCommandEvent& event )
{
    Category* cat = FindCategory(this->m_SelectedItem);
    cat->displayname = event.GetString();
    this->m_TreeCtrl_Projects->SetItemText(cat->treeid, cat->displayname);
    this->MarkModified();
}

void Main::m_TextCtrl_ProjectsCategory_Description_OnText( wxCommandEvent& event )
{
    Category* cat = FindCategory(this->m_SelectedItem);
    cat->description = event.GetString();
    this->MarkModified();
}

void Main::m_TreeCtrl_Blog_OnTreeEndLabelEdit(wxTreeEvent& event)
{

}

void Main::m_TreeCtrl_Blog_OnTreeBeginDrag( wxTreeEvent& event )
{

}

void Main::m_TreeCtrl_Blog_OnTreeEndDrag( wxTreeEvent& event )
{

}

void Main::m_TreeCtrl_Blog_OnTreeItemMenu( wxTreeEvent& event )
{

}

void Main::m_TreeCtrl_Blog_OnTreeSelChanged( wxTreeEvent& event )
{

}

Category* Main::FindCategory(wxTreeItemId item)
{
    for (Category* cat : this->m_Category_Projects)
        if (cat->treeid == item)
            return cat;
    return NULL;
}

Project* Main::FindProject(wxTreeItemId item)
{
    wxTreeItemId cat_id;
    Category* cat_elem;

    if (treeitem_iscategory(this->m_TreeCtrl_Projects, item))
        return NULL;

    cat_id = this->m_TreeCtrl_Projects->GetItemParent(item);
    cat_elem = FindCategory(cat_id);
    if (cat_elem == NULL)
        return NULL;

    for (void* proj : cat_elem->pages)
        if (((Project*)proj)->treeid == item)
            return (Project*)proj;
    return NULL;
}

void Main::OnPopupClick_Projects(wxCommandEvent& event)
{
    int index = 0;
    wxTreeItemId cat = this->m_SelectedItem;
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, cat))
        cat = this->m_TreeCtrl_Projects->GetItemParent(cat);
    Category* cat_elem = this->FindCategory(cat);
    Project* proj;

    if (cat_elem == NULL)
        return;

    switch (event.GetId())
    {
        case wxID_NEW:
            proj = new Project();
            proj->index = cat_elem->pages.size();
            proj->filename = "new";
            proj->displayname = "New Project";
            proj->icon = "";
            proj->date = "";
            proj->tooltip = "";
            proj->description = "";
            proj->images.clear();
            proj->urls.clear();
            proj->tags.clear();
            proj->category = cat_elem;
            proj->treeid = this->m_TreeCtrl_Projects->AppendItem(cat, proj->displayname);
            cat_elem->pages.push_back(proj);
            this->m_TreeCtrl_Projects->Expand(cat_elem->treeid);
            this->m_TreeCtrl_Projects->SelectItem(proj->treeid);
            this->m_SelectedItem = proj->treeid;
            break;
        case wxID_DELETE:
            proj = this->FindProject(this->m_SelectedItem);
            cat_elem->pages.erase(cat_elem->pages.begin() + proj->index);
            this->m_TreeCtrl_Projects->Delete(proj->treeid);
            if (wxFileExists(this->m_WorkingDir + wxString("/") + wxString("projects/") + cat_elem->foldername + wxString("/") + proj->filename + wxString(".html")))
                wxRemoveFile(this->m_WorkingDir + wxString("/") + wxString("projects/") + cat_elem->foldername + wxString("/") + proj->filename + wxString(".html"));
            delete proj;
            for (void* projptr : cat_elem->pages)
            {
                proj = (Project*)projptr;
                proj->index = index++;
            }
            break;
    }
}

void Main::UpdateTree(wxTreeCtrl* tree, wxString folder, std::vector<Category*>* categorylist)
{
    int index;
    wxTreeItemId root;
    wxString filename;
    nlohmann::json pagejson = {};
    wxDir pagepath;

    // First, free all memory
    for (Category* cat : *categorylist)
    {
        for (void* child : cat->pages)
        {
            if (folder == "projects")
                delete (Project*)child;
            // TODO: Handle blogs
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

    // Open the projects json file
    if (wxFileExists(this->m_WorkingDir + wxString("/") + folder + wxString("/") + folder + wxString(".json")))
        pagejson = nlohmann::json::parse(std::ifstream(wxString(this->m_WorkingDir + wxString("/") + folder + wxString("/") + folder + wxString(".json")).ToStdString()));

    // First, add all the folders which are already included in the JSON
    for (nlohmann::json::iterator it = pagejson["Categories"].begin(); it != pagejson["Categories"].end(); ++it)
    {
        // TODO: Check project folder still exists before doing this
        Category* cat = new Category();
        cat->foldername = wxString(it.key());
        cat->index = (*it)["Index"];
        cat->displayname = wxString((*it)["DisplayName"]);
        cat->description = wxString((*it)["Description"]);
        cat->treeid = NULL;
        cat->pages.clear();
        categorylist->push_back(cat);
    }

    // Correct the indices of categories, to make sure they're in proper order
    index = 0;
    std::sort(categorylist->begin(), categorylist->end(), &category_sorter);
    for (Category* cat : *categorylist)
        cat->index = index++;

    // Now, add all folders which aren't in the JSON
    bool cont = pagepath.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
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

void Main::LoadProjects()
{
    nlohmann::json pagejson = {};

    // Open the projects json file
    if (wxFileExists(this->m_WorkingDir + wxString("/projects/projects.json")))
        pagejson = nlohmann::json::parse(std::ifstream(wxString(this->m_WorkingDir + wxString("/projects/projects.json"))));

    for (nlohmann::json::iterator itcat = pagejson["Categories"].begin(); itcat != pagejson["Categories"].end(); ++itcat)
    {
        int index = 0;
        wxTreeItemId cat_id;
        Category* cat_elem;
        std::vector<Project*> projects;

        // Find the category tree index
        for (Category* cat : this->m_Category_Projects)
        {
            if (wxString(itcat.key()) == cat->foldername)
            {
                cat_elem = cat;
                cat_id = cat->treeid;
                break;
            }
        }

        // Read the project data from the JSON
        for (nlohmann::json::iterator itproj = (*itcat)["Pages"].begin(); itproj != (*itcat)["Pages"].end(); ++itproj)
        {
            Project* proj = new Project();
            proj->index = (*itproj)["Index"];
            proj->filename = wxString(itproj.key());
            proj->displayname = wxString((*itproj)["DisplayName"]);
            proj->icon = wxString((*itproj)["Icon"]);
            proj->date = wxString((*itproj)["Date"]);
            proj->tooltip = wxString((*itproj)["ToolTip"]);
            proj->description = wxString((*itproj)["Description"]);
            proj->images.clear();
            for (nlohmann::json::iterator it = (*itproj)["Images"].begin(); it != (*itproj)["Images"].end(); ++it)
                proj->images.push_back(wxString(*it));
            proj->urls.clear();
            for (nlohmann::json::iterator it = (*itproj)["URLs"].begin(); it != (*itproj)["URLs"].end(); ++it)
                proj->urls.push_back(wxString(*it));
            proj->tags.clear();
            // TODO: Handle tags
            proj->category = cat_elem;
            proj->treeid = NULL;
            projects.push_back(proj);
        }

        // Correct the indices of projects, to make sure they're in proper order
        if (projects.size() > 0)
        {
            index = 0;
            std::sort(projects.begin(), projects.end(), &project_sorter);
            for (Project* proj : projects)
                proj->index = index++;

            // Now add the project to the tree
            for (Project* proj : projects)
            {
                proj->treeid = this->m_TreeCtrl_Projects->AppendItem(cat_id, proj->displayname);
                cat_elem->pages.push_back(proj);
            }
        }
    }
    this->ShowProjectEditor(false);
}

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
            Project* proj = (Project*)child;
            proj->treeid = tree->InsertItem(src_elem->treeid, proj->index, proj->displayname);
        }

        // Correct the index values
        for (Category* cat : *categorylist)
            cat->index = index++;

        // Mark the project as modified
        this->MarkModified();
    }
}

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

    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, dest))
    {
        int index = 0;
        Project* proj_src = this->FindProject(src);
        Project* proj_dest = this->FindProject(dest);
        Category* cat = proj_src->category;
        std::vector<void*>* pages = &cat->pages;

        // Ensure both projects are in the same category
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

        // Mark the project as modified
        this->MarkModified();
    }
}

void Main::Save()
{
    wxTextFile out(this->m_WorkingDir + wxString("/projects/projects.json"));
    nlohmann::json projectjson = {};
    projectjson["Categories"] = {};
    for (Category* cat : this->m_Category_Projects)
    {
        std::string catstr = cat->foldername.ToStdString();
        projectjson["Categories"][catstr] = {};
        projectjson["Categories"][catstr]["Index"] = cat->index;
        projectjson["Categories"][catstr]["DisplayName"] = cat->displayname;
        projectjson["Categories"][catstr]["Description"] = cat->description;
        projectjson["Categories"][catstr]["Pages"] = {};
        for (void* child : cat->pages)
        {
            Project* proj = (Project*)child;
            std::string projstr = proj->filename.ToStdString();
            projectjson["Categories"][catstr]["Pages"][projstr] = {};
            projectjson["Categories"][catstr]["Pages"][projstr]["Index"] = proj->index;
            projectjson["Categories"][catstr]["Pages"][projstr]["DisplayName"] = proj->displayname;
            projectjson["Categories"][catstr]["Pages"][projstr]["Icon"] = proj->icon;
            projectjson["Categories"][catstr]["Pages"][projstr]["Date"] = proj->date;
            projectjson["Categories"][catstr]["Pages"][projstr]["ToolTip"] = proj->tooltip;
            projectjson["Categories"][catstr]["Pages"][projstr]["Description"] = proj->description;
            projectjson["Categories"][catstr]["Pages"][projstr]["Images"] = {};
            for (wxString str :  proj->images)
                projectjson["Categories"][catstr]["Pages"][projstr]["Images"].push_back(str);
            projectjson["Categories"][catstr]["Pages"][projstr]["URLs"] = {};
            for (wxString str :  proj->urls)
                projectjson["Categories"][catstr]["Pages"][projstr]["URLs"].push_back(str);
            projectjson["Categories"][catstr]["Pages"][projstr]["Tags"] = {};
            for (Tag* tag :  proj->tags)
                projectjson["Categories"][catstr]["Pages"][projstr]["Tags"].push_back(tag->name);
        }
    }

    // TODO: Handle blog

    // Compile the website
    this->CompileProjects();

    // Dump the JSON to a text file
    if (!out.Exists())
        out.Create();
    out.Clear();
    out.AddLine(wxString(projectjson.dump()));
    out.Write();
    out.Close();

    // Mark the program as no longer modified
    this->MarkModified(false);
}

void Main::CompileProjects()
{
    wxTextFile out(this->m_WorkingDir + wxString("/projects.html"));
    wxDateTime today = wxDateTime::Today();
    wxString html_final = wxString("");
    wxString html_categories = wxString("");

    // Read the projects page template
    html_final = string_fromfile(this->m_WorkingDir + "/templates/projects.html");

    // Generate the page from the section + project templates
    for (Category* cat : this->m_Category_Projects)
    {
        wxString html_projects = wxString("");
        wxString relativepath = wxString("projects/") + cat->foldername + wxString("/");
        std::stringstream mdinput(cat->description.ToStdString());
        std::shared_ptr<maddy::ParserConfig> config = std::make_shared<maddy::ParserConfig>();
        config->enabledParsers |= maddy::types::HTML_PARSER; 
        std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>(config);
        wxString desc;

        // Generate the project blocks
        for (void* page : cat->pages)
        {
            Project* proj = (Project*)page;
            html_projects += string_fromfile(this->m_WorkingDir + "/templates/projects_project.html");
            html_projects.Replace("_TEMPLATE_PROJECT_URL_", relativepath + proj->filename + wxString(".html"));
            html_projects.Replace("_TEMPLATE_PROJECT_TITLE_", proj->displayname);
            html_projects.Replace("_TEMPLATE_PROJECT_IMAGE_", relativepath + proj->icon);
            html_projects.Replace("_TEMPLATE_PROJECT_TOOLTIP_", proj->tooltip);
        }

        // Generate the section blocks
        html_categories += string_fromfile(this->m_WorkingDir + "/templates/projects_section.html");
        html_categories.Replace("_TEMPLATE_TITLE_", cat->displayname);
        html_categories.Replace("_TEMPLATE_HREF_", cat->foldername);
        desc = wxString(parser->Parse(mdinput));
        desc.Replace("<p>", "<p align=\"left\">");
        html_categories.Replace("_TEMPLATE_SECTION_DESCRIPTION_", desc);
        html_categories.Replace("_TEMPLATE_PROJECT_LIST_", html_projects);
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

    // Now create the page for each project
    for (Category* cat : this->m_Category_Projects)
    {
        wxString relativepath = wxString("projects/") + cat->foldername + wxString("/");
        for (void* page : cat->pages)
        {
            Project* proj = (Project*)page;
            wxString projoutpath = this->m_WorkingDir + wxString("/") + relativepath + proj->filename + wxString(".html");
            wxTextFile projout(projoutpath);
            std::stringstream mdinput(proj->description.ToStdString());
            std::shared_ptr<maddy::ParserConfig> config = std::make_shared<maddy::ParserConfig>();
            std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>(config);
            std::vector<wxString> images;
            std::vector<wxString> youtubes;

            // Replace most of the basic page info
            html_final = string_fromfile(this->m_WorkingDir + "/templates/project.html");
            html_final.Replace("_TEMPLATE_PROJECTS_TITLE_", proj->displayname);
            html_final.Replace("_TEMPLATE_PROJECTS_DATE_", proj->date);
            html_final.Replace("_TEMPLATE_PROJECTS_DESCRIPTION_", wxString(parser->Parse(mdinput)));

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

            // TODO: Handle page URLS

            // Generate the page itself
            if (!projout.Exists())
                projout.Create();
            projout.Open();
            projout.Clear();
            projout.AddLine(html_final);
            projout.Write();
            projout.Close();
        }
    }
}

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