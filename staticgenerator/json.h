#pragma once

#include <vector>
#include "main.h"


/*********************************
        Function Prototypes
*********************************/

int json_loadcategories(wxString filepath, std::vector<Category*>* categorylist);
void json_loadprojects(wxString workingdir, std::vector<Category*>* categorylist, wxTreeCtrl* tree);
void json_loadblogentries(wxString workingdir, std::vector<Category*>* categorylist, wxTreeCtrl* tree);
void json_save(wxString workingdir, std::vector<Category*>* categorylist_projects, std::vector<Category*>* categorylist_blog);