#pragma once

typedef struct IUnknown IUnknown; 

#include "main.h"

bool treeitem_iscategory(wxTreeCtrl* tree, wxTreeItemId item);
wxString string_fromfile(wxString path);

bool category_sorter(Category* lhs, Category* rhs);
bool project_sorter(Project* lhs, Project* rhs);
bool blog_sorter(Blog* lhs, Blog* rhs);
bool project_sorter_date(Project* lhs, Project* rhs);
bool blog_sorter_date(Blog* lhs, Blog* rhs);

bool date_fromstring(wxString input, wxDateTime* output);

void md4c_funcptr_handlestr(const MD_CHAR* input, MD_SIZE inputsize, void* output);
wxString* md_sanitize(wxString* input);
wxString* md_unsanitize(wxString* input);