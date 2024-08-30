#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include "main.h"
#include "json.hpp"
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

Main::Main(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    this->m_WorkingDir = wxGetCwd();
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    this->SetTitle(this->m_WorkingDir);

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

    m_TreeCtrl_Projects = new wxTreeCtrl(m_Panel_Projects_Tree, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxTR_EDIT_LABELS);
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

    m_TreeCtrl_Blog = new wxTreeCtrl(m_Panel_Blog_Tree, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxTR_EDIT_LABELS);
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

    wxMenuItem* m_MenuItem_Save;
    m_MenuItem_Save = new wxMenuItem(m_Menu_File, wxID_ANY, wxString(wxT("Save Changes")) + wxT('\t') + wxT("CTRL+S"), wxEmptyString, wxITEM_NORMAL);
    m_Menu_File->Append(m_MenuItem_Save);

    m_Menubar_Main->Append(m_Menu_File, wxT("File"));

    this->SetMenuBar(m_Menubar_Main);

    this->Centre(wxBOTH);

    m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit), NULL, this);
    m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeBeginDrag), NULL, this);
    m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndDrag), NULL, this);
    m_TreeCtrl_Blog->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_Blog_OnTreeEndLabelEdit), NULL, this);
    m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_OpenDir_OnMenuSelection), this, m_MenuItem_OpenDir->GetId());
    m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Save_OnMenuSelection), this, m_MenuItem_Save->GetId());

    this->UpdateTree();
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
        this->UpdateTree();
    }
}

void Main::m_MenuItem_Save_OnMenuSelection(wxCommandEvent& event)
{
    this->Save();
}

void Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();

    for (Category* cat : this->m_Category_Projects)
    {
        if (cat->treeid == id)
        {
            cat->displayname = event.GetLabel();
            break;
        }
    }
}

void Main::m_TreeCtrl_Blog_OnTreeEndLabelEdit(wxTreeEvent& event)
{
    // TODO: Do this
}
void Main::m_TreeCtrl_Projects_OnTreeBeginDrag(wxTreeEvent& event)
{
    this->m_DraggedItem = event.GetItem();
    event.Allow();
}
void Main::m_TreeCtrl_Projects_OnTreeEndDrag(wxTreeEvent& event)
{
    int index = 0;
    wxTreeItemId src = this->m_DraggedItem;
    wxTreeItemId dest = event.GetItem();
    Category* src_elem = NULL;
    Category* dest_elem = NULL;
    this->m_DraggedItem = NULL;

    // Ensure we have a valid destination node
    if (!dest.IsOk() || src == dest)
        return;

    // Ignore placing the source directly above the destination, since it won't move at all.
    if (this->m_TreeCtrl_Projects->GetPrevSibling(src) == dest)
        return;

    // Get the category elements themselves
    for (Category* cat : this->m_Category_Projects)
    {
        if (cat->treeid == src)
            src_elem = cat;
        if (cat->treeid == dest)
            dest_elem = cat;
    }

    // Perform a swap of the data structures
    this->m_Category_Projects.erase(this->m_Category_Projects.begin() + src_elem->index);
    this->m_Category_Projects.insert(this->m_Category_Projects.begin() + dest_elem->index, src_elem);

    // Correct the index values
    for (Category* cat : this->m_Category_Projects)
        cat->index = index++;

    // Move the elements
    this->m_TreeCtrl_Projects->Delete(src);
    src_elem->treeid = this->m_TreeCtrl_Projects->InsertItem(this->m_TreeCtrl_Projects->GetRootItem(), dest, src_elem->displayname);
}

void Main::UpdateTree()
{
    int index;
    wxTreeItemId root;
    wxString filename;
    nlohmann::json projectjson = {};
    //nlohmann::json blogjson = {};
    wxDir projectspath(this->m_WorkingDir + wxString("/projects"));
    //wxDir blogpath(this->m_WorkingDir + wxString("/blog"));

    // Initialize the trees
    this->m_Category_Projects.clear();
    this->m_TreeCtrl_Projects->DeleteAllItems();
    //this->m_TreeCtrl_Blog->DeleteAllItems();
    root = this->m_TreeCtrl_Projects->AddRoot("Root");
    //root = this->m_TreeCtrl_Blog->AddRoot("Root");

    // If the projects folder doesn't exist, create it
    if (!wxDirExists(projectspath.GetName()))
        wxMkDir(projectspath.GetName());

    // If the blogs folder doesn't exist, create it
    //if (!wxDirExists(blogpath.GetName()))
    //	wxMkDir(blogpath.GetName());

    // Open the projects json file
    if (wxFileExists(this->m_WorkingDir + wxString("/projects/projects.json")))
        projectjson = nlohmann::json::parse(std::ifstream(wxString(this->m_WorkingDir + wxString("/projects/projects.json")).ToStdString()));

    // First, add all the folders which are already included in the JSON
    for (nlohmann::json::iterator it = projectjson["Categories"].begin(); it != projectjson["Categories"].end(); ++it)
    {
        // TODO: Check folder still exists before doing this
        Category* cat = new Category();
        cat->foldername = wxString(it.key());
        cat->index = (*it)["Index"];
        cat->displayname = wxString((*it)["DisplayName"]);
        cat->treeid = NULL;
        cat->projects.clear();
        this->m_Category_Projects.push_back(cat);
    }

    // Correct the indices of categories, to make sure they're in proper order
    index = 0;
    std::sort(this->m_Category_Projects.begin(), this->m_Category_Projects.end(), &category_sorter);
    for (Category* cat : this->m_Category_Projects)
        cat->index = index++;

    // Now, add all folders which aren't in the JSON
    bool cont = projectspath.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
    while (cont)
    {
        bool hasfile = false;

        // Check if the folder already exists in our list
        for (Category* cat : this->m_Category_Projects)
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
            cat->treeid = NULL;
            cat->projects.clear();
            this->m_Category_Projects.push_back(cat);
        }

        // Check the next folder
        cont = projectspath.GetNext(&filename);
    }

    // Finally, generate the tree
    for (Category* cat : this->m_Category_Projects)
        cat->treeid = this->m_TreeCtrl_Projects->AppendItem(root, cat->displayname);

    // TODO: Handle blog
}

void Main::Save()
{
    wxTextFile out(this->m_WorkingDir + wxString("/projects/projects.json"));
    nlohmann::json projectjson = {};
    projectjson["Categories"] = {};
    for (Category* cat : this->m_Category_Projects)
    {
        projectjson["Categories"][cat->foldername.ToStdString()] = {};
        projectjson["Categories"][cat->foldername.ToStdString()]["Index"] = cat->index;
        projectjson["Categories"][cat->foldername.ToStdString()]["DisplayName"] = cat->displayname;
    }

    // TODO: Handle blog

    // Dump the JSON to a text file
    if (!out.Exists())
        out.Create();
    out.Clear();
    out.AddLine(wxString(projectjson.dump()));
    out.Write();
    out.Close();
}