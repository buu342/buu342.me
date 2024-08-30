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
#include <wx/richtext/richtextctrl.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/splitter.h>
#include <wx/choicebk.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/filefn.h>

typedef struct Category_s Category;
typedef struct Project_s Project;
typedef struct Tag_s Tag;

typedef struct Category_s {
	int index;
	wxString foldername;
	wxString displayname;
	wxTreeItemId treeid;
	std::vector<Project*> projects;
} Category;

typedef struct Project_s {
	int index;
	wxString filename;
	wxString displayname;
	Category* category;
	std::vector<Tag*> tags;
} Project;

typedef struct Tag_s {
	wxString name;
	std::vector<Project*> projects;
} Tag;

class Main : public wxFrame
{
	private:
		std::vector<Category*> m_Category_Projects;
		wxString m_WorkingDir;
		wxTreeItemId m_DraggedItem;

	protected:
		wxChoicebook* m_ChoiceBook_PageSelection;
		wxPanel* m_Panel_Projects;
		wxSplitterWindow* m_Splitter_Projects;
		wxPanel* m_Panel_Projects_Tree;
		wxTreeCtrl* m_TreeCtrl_Projects;
		wxPanel* m_Panel_Projects_TextCtrl;
		wxRichTextCtrl* m_TextCtrl_Projects;
		wxButton* m_Button_Projects_Preview;
		wxPanel* m_Panel_Blog;
		wxSplitterWindow* m_Splitter_Blog;
		wxPanel* m_Panel_Blog_Tree;
		wxTreeCtrl* m_TreeCtrl_Blog;
		wxPanel* m_Panel_Blog_TextCtrl;
		wxRichTextCtrl* m_TextCtrl_Blog;
		wxButton* m_Button_Blog_Preview;
		wxMenuBar* m_Menubar_Main;
		wxMenu* m_Menu_File;

	public:
		Main(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(576, 419), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
		~Main();
        void m_Splitter_ProjectsOnIdle(wxIdleEvent& event);
        void m_Splitter_BlogOnIdle(wxIdleEvent& event);
		void m_TreeCtrl_Projects_OnTreeEndLabelEdit(wxTreeEvent& event);
		void m_TreeCtrl_Blog_OnTreeEndLabelEdit(wxTreeEvent& event);
		void m_TreeCtrl_Projects_OnTreeBeginDrag(wxTreeEvent& event);
		void m_TreeCtrl_Projects_OnTreeEndDrag(wxTreeEvent& event);
		void m_MenuItem_OpenDir_OnMenuSelection(wxCommandEvent& event);
		void m_MenuItem_Save_OnMenuSelection(wxCommandEvent& event);
		void UpdateTree();
		void Save();

};

