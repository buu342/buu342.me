#pragma once

typedef struct IUnknown IUnknown; 

#include "main.h"
#include "include/json.hpp"
#include "include/md4c/md4c-html.h"

bool treeitem_iscategory(wxTreeCtrl* tree, wxTreeItemId item);
wxString string_fromfile(wxString path);

bool category_sorter(Category* lhs, Category* rhs);
bool project_sorter(Project* lhs, Project* rhs);
bool blog_sorter(Blog* lhs, Blog* rhs);
bool project_sorter_date(Project* lhs, Project* rhs);
bool blog_sorter_date(Blog* lhs, Blog* rhs);
bool taggedpage_sorter(TaggedPage lhs, TaggedPage rhs);

bool date_fromstring(wxString input, wxDateTime* output);

wxString sanitize_tagname(wxString tagname);
void md4c_funcptr_handlestr(const MD_CHAR* input, MD_SIZE inputsize, void* output);
wxString* md_sanitize(wxString* input);
wxString* md_unsanitize(wxString* input);