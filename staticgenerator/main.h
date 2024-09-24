#pragma once

typedef struct IUnknown IUnknown;

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/scrolwin.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/splitter.h>
#include <wx/choicebk.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/filefn.h>
#include <wx/timer.h>

typedef struct Category_s Category;
typedef struct Project_s Project;
typedef struct Blog_s Blog;

typedef enum
{
    PAGETYPE_PROJECT,
    PAGETYPE_BLOG,
} PageType;

typedef struct TaggedPage_s
{
    PageType type;
    void* page;
} TaggedPage;

typedef struct Category_s {
	int index;
	wxString foldername;
	wxString displayname;
    wxString description;
	wxTreeItemId treeid;
	std::vector<void*> pages;
} Category;

typedef struct Project_s {
	int index;
	wxString filename;
	wxString displayname;
    wxString icon;
    wxString date;
    wxString tooltip;
    wxString description;
    std::vector<wxString> images;
	std::vector<wxString> urls;
    std::vector<wxString> tags;
	Category* category;
    wxTreeItemId treeid;
} Project;

typedef struct Blog_s {
    int index;
    wxString filename;
    wxString displayname;
    wxString icon;
    wxString date;
    wxString tooltip;
    wxString content;
    std::vector<wxString> tags;
    Category* category;
    wxTreeItemId treeid;
} Blog;

class Main : public wxFrame
{
	private:
        bool m_Modified;
		std::vector<Category*> m_Category_Blog;
        std::vector<Category*> m_Category_Projects;
		wxString m_WorkingDir;
		wxTreeItemId m_DraggedItem;
        wxTreeItemId m_SelectedItem;

	protected:
        wxChoicebook* m_ChoiceBook_PageSelection;
        wxPanel* m_Panel_Projects;
        wxSplitterWindow* m_Splitter_Projects;
        wxPanel* m_Panel_Projects_Tree;
        wxTreeCtrl* m_TreeCtrl_Projects;
        wxPanel* m_Panel_Projects_Editor;
        wxScrolledWindow* m_ScrolledWindow_Project_Editor;
        wxStaticText* m_Label_Projects_File;
        wxTextCtrl* m_TextCtrl_Projects_File;
        wxStaticText* m_Label_Projects_Name;
        wxTextCtrl* m_TextCtrl_Projects_Name;
        wxStaticText* m_Label_Projects_Icon;
        wxTextCtrl* m_TextCtrl_Projects_Icon;
        wxStaticText* m_Label_Projects_Tags;
        wxTextCtrl* m_TextCtrl_Projects_Tags;
        wxStaticText* m_Label_Projects_Images;
        wxTextCtrl* m_TextCtrl_Projects_Images;
        wxStaticText* m_Label_Projects_Date;
        wxTextCtrl* m_TextCtrl_Projects_Date;
        wxStaticText* m_Label_Projects_URLs;
        wxTextCtrl* m_TextCtrl_Projects_URLs;
        wxStaticText* m_Label_Projects_ToolTip;
        wxTextCtrl* m_TextCtrl_Projects_ToolTip;
        wxTextCtrl* m_TextCtrl_Projects_Description;
        wxScrolledWindow* m_ScrolledWindow_ProjectCategory_Editor;
        wxStaticText* m_Label_ProjectsCategory_Folder;
        wxTextCtrl* m_TextCtrl_ProjectsCategory_Folder;
        wxStaticText* m_Label_ProjectsCategory_DisplayName;
        wxTextCtrl* m_TextCtrl_ProjectsCategory_DisplayName;
        wxStaticText* m_Label_ProjectsCategory_Description;
        wxTextCtrl* m_TextCtrl_ProjectsCategory_Description;
        wxButton* m_Button_Projects_Preview;
        wxPanel* m_Panel_Blog;
        wxSplitterWindow* m_Splitter_Blog;
        wxPanel* m_Panel_Blog_Tree;
        wxTreeCtrl* m_TreeCtrl_Blog;
        wxPanel* m_Panel_Blog_Editor;
        wxScrolledWindow* m_ScrolledWindow_Blog_Editor;
        wxStaticText* m_Label_Blog_File;
        wxTextCtrl* m_TextCtrl_Blog_File;
        wxStaticText* m_Label_Blog_Name;
        wxTextCtrl* m_TextCtrl_Blog_Name;
        wxStaticText* m_Label_Blog_Icon;
        wxTextCtrl* m_TextCtrl_Blog_Icon;
        wxStaticText* m_Label_Blog_ToolTip;
        wxTextCtrl* m_TextCtrl_Blog_ToolTip;
        wxStaticText* m_Label_Blog_Date;
        wxTextCtrl* m_TextCtrl_Blog_Date;
        wxStaticText* m_Label_Blog_Tags;
        wxTextCtrl* m_TextCtrl_Blog_Tags;
        wxTextCtrl* m_TextCtrl_Blog;
        wxScrolledWindow* m_ScrolledWindow_BlogCategory_Editor;
        wxStaticText* m_Label_BlogCategory_Folder;
        wxTextCtrl* m_TextCtrl_BlogCategory_Folder;
        wxStaticText* m_Label_BlogCategory_DisplayName;
        wxTextCtrl* m_TextCtrl_BlogCategory_DisplayName;
        wxStaticText* m_Label_BlogCategory_Description;
        wxTextCtrl* m_TextCtrl_BlogCategory_Description;
        wxButton* m_Button_Blog_Preview;
        wxMenuBar* m_Menubar_Main;
        wxMenu* m_Menu_File;
        wxTimer* m_Timer;

	public:
		Main(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(800, 600), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
		~Main();
        void m_Splitter_ProjectsOnIdle(wxIdleEvent& event);
        void m_Splitter_BlogOnIdle(wxIdleEvent& event);
        void m_TreeCtrl_Projects_OnTreeBeginDrag(wxTreeEvent& event);
        void m_TreeCtrl_Projects_OnTreeEndDrag(wxTreeEvent& event);
        void m_TreeCtrl_Projects_OnTreeEndLabelEdit(wxTreeEvent& event);
        void m_TreeCtrl_Projects_OnTreeItemMenu(wxTreeEvent& event);
        void m_TreeCtrl_Projects_OnTreeSelChanged(wxTreeEvent& event);
        void m_TextCtrl_Projects_File_OnText(wxCommandEvent& event);
        void m_TextCtrl_Projects_Name_OnText(wxCommandEvent& event);
        void m_TextCtrl_Projects_Icon_OnText(wxCommandEvent& event);
        void m_TextCtrl_Projects_Tags_OnText(wxCommandEvent& event);
        void m_TextCtrl_Projects_Images_OnText(wxCommandEvent& event);
        void m_TextCtrl_Projects_Date_OnText(wxCommandEvent& event);
        void m_TextCtrl_Projects_URLs_OnText(wxCommandEvent& event);
        void m_TextCtrl_Projects_ToolTip_OnText(wxCommandEvent& event);
        void m_TextCtrl_Projects_Description_OnText(wxCommandEvent& event);
        void m_TextCtrl_ProjectsCategory_DisplayName_OnText(wxCommandEvent& event);
        void m_TextCtrl_ProjectsCategory_Description_OnText(wxCommandEvent& event);
        void m_Button_Projects_Preview_OnButtonClick(wxCommandEvent& event);
        void m_TreeCtrl_Blog_OnTreeBeginDrag(wxTreeEvent& event);
        void m_TreeCtrl_Blog_OnTreeEndDrag(wxTreeEvent& event);
        void m_TreeCtrl_Blog_OnTreeEndLabelEdit(wxTreeEvent& event);
        void m_TreeCtrl_Blog_OnTreeItemMenu(wxTreeEvent& event);
        void m_TreeCtrl_Blog_OnTreeSelChanged(wxTreeEvent& event);
        void m_TextCtrl_Blog_File_OnText(wxCommandEvent& event);
        void m_TextCtrl_Blog_Name_OnText(wxCommandEvent& event);
        void m_TextCtrl_Blog_Icon_OnText(wxCommandEvent& event);
        void m_TextCtrl_Blog_ToolTip_OnText(wxCommandEvent& event);
        void m_TextCtrl_Blog_Date_OnText(wxCommandEvent& event);
        void m_TextCtrl_Blog_Tags_OnText(wxCommandEvent& event);
        void m_TextCtrl_Blog_OnText(wxCommandEvent& event);
        void m_TextCtrl_BlogCategory_DisplayName_OnText(wxCommandEvent& event);
        void m_TextCtrl_BlogCategory_Description_OnText(wxCommandEvent& event);
        void m_Button_Blog_Preview_OnButtonClick(wxCommandEvent& event);
        void m_MenuItem_OpenDir_OnMenuSelection(wxCommandEvent& event);
        void m_MenuItem_Save_OnMenuSelection(wxCommandEvent& event);
        void m_Timer_OnTimer(wxTimerEvent& event);
        Category* FindCategory_Projects(wxTreeItemId item);
        Category* FindCategory_Blog(wxTreeItemId item);
        Project* FindProject(wxTreeItemId item);
        Blog* FindBlog(wxTreeItemId item);
        void OnPopupClick_Projects(wxCommandEvent& event);
        void OnPopupClick_Blog(wxCommandEvent& event);
		void UpdateTree(wxTreeCtrl* tree, wxString folder, std::vector<Category*>* categorylist);
        void LoadProjects();
        void LoadBlog();
        void EndDrag(wxTreeEvent& event, wxTreeCtrl* tree, std::vector<Category*>* categorylist);
        void EndDrag_Project(wxTreeEvent& event);
        void EndDrag_Blog(wxTreeEvent& event);
		void Save();
        void CompileProjects();
        void CompileProjects_List();
        void CompileProjects_Project(Project* proj);
        void CompileBlog();
        void CompileBlog_List();
        void CompileBlog_Entry(Blog* bentry);
        void CompileTags();
        void CompileHomePage();
        void MarkModified(bool modified=true);
        void ShowProjectEditor(bool show=true);
        void ShowProjectCategoryEditor(bool show=true);
        void ShowBlogEditor(bool show=true);
        void ShowBlogCategoryEditor(bool show=true);
};