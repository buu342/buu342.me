/***************************************************************
                            json.cpp

JSON handling functions. This file was created to isolate the 
bulky JSON header only library. 
***************************************************************/

#include <algorithm>
#include <fstream>
#include "json.h"
#include "helper.h"
#include "include/json.hpp"
#include <wx/textfile.h>


/*==============================
    json_loadcategories
    Load a list of categories from a JSON file
    @param  The filepath of the JSON file
    @param  A pointer to the category list
    @return The number of categories loaded
==============================*/

int json_loadcategories(wxString filepath, std::vector<Category*>* categorylist)
{
    int index = 0;
    nlohmann::json pagejson = {};

    // Open the projects json file
    if (wxFileExists(filepath))
        pagejson = nlohmann::json::parse(std::ifstream(filepath.ToStdString()));

    // Add all the folders which are already included in the JSON
    for (nlohmann::json::iterator it = pagejson["Categories"].begin(); it != pagejson["Categories"].end(); ++it)
    {
        Category* cat;
        wxString catfoldername = wxString(it.key());

        // If the category folder doesn't exist anymore, skip this category
        if (!wxDirExists(wxFileName(filepath).GetPath() + wxString("/") + catfoldername))
            continue;

        // Create the category object
        cat = new Category();
        cat->foldername = catfoldername;
        cat->index = (*it)["Index"];
        cat->displayname = wxString(std::string((*it)["DisplayName"]));
        cat->description = wxString(std::string((*it)["Description"]));
        cat->wasmodified = false;
        cat->treeid = NULL;
        cat->pages.clear();
        categorylist->push_back(cat);
    }

    // Correct the indices of categories, to make sure they're in proper order
    std::sort(categorylist->begin(), categorylist->end(), &category_sorter);
    for (Category* cat : *categorylist)
        cat->index = index++;

    // Return the number of category elements we found
    return index;
}


/*==============================
    json_loadprojects
    Load a list of projects from a JSON file and populate the
    project tree
    @param  The working directory of the program
    @param  A pointer to the category list
    @param  A pointer to tree to populate
==============================*/

void json_loadprojects(wxString workingdir, std::vector<Category*>* categorylist, wxTreeCtrl* tree)
{
    nlohmann::json pagejson = {};

    // Open the projects json file
    if (wxFileExists(workingdir + wxString("/projects/projects.json")))
        pagejson = nlohmann::json::parse(std::ifstream(wxString(workingdir + wxString("/projects/projects.json")).ToStdString()));

    // Add the projects from the JSON file
    for (nlohmann::json::iterator itcat = pagejson["Categories"].begin(); itcat != pagejson["Categories"].end(); ++itcat)
    {
        int index = 0;
        wxTreeItemId cat_id;
        Category* cat_elem = NULL;
        std::vector<Project*> projects;

        // Find the category tree index
        for (Category* cat : *categorylist)
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
            wxString taglist = wxString("");
            Project* proj = new Project();
            proj->index = (*itproj)["Index"];
            proj->filename = wxString(itproj.key());
            proj->displayname = wxString(std::string((*itproj)["DisplayName"]));
            proj->icon = wxString(std::string((*itproj)["Icon"]));
            proj->date = wxString(std::string((*itproj)["Date"]));
            proj->tooltip = wxString(std::string((*itproj)["ToolTip"]));
            proj->wasmodified = false;
            proj->description = string_fromfile(workingdir + wxString("/projects/") + cat_elem->foldername + wxString("/markdown/") + proj->filename + wxString(".md"));
            proj->images.clear();
            for (nlohmann::json::iterator it = (*itproj)["Images"].begin(); it != (*itproj)["Images"].end(); ++it)
                proj->images.push_back(wxString(std::string(*it)));
            proj->urls.clear();
            for (nlohmann::json::iterator it = (*itproj)["URLs"].begin(); it != (*itproj)["URLs"].end(); ++it)
                proj->urls.push_back(wxString(std::string(*it)));
            proj->tags.clear();
            for (nlohmann::json::iterator it = (*itproj)["Tags"].begin(); it != (*itproj)["Tags"].end(); ++it)
                proj->tags.push_back(wxString(std::string(*it)));
            proj->category = cat_elem;
            proj->treeid = NULL;
            projects.push_back(proj);
        }

        // Correct the indices of the projects, to make sure they're in proper order
        if (projects.size() > 0)
        {
            index = 0;
            std::sort(projects.begin(), projects.end(), &project_sorter);
            for (Project* proj : projects)
                proj->index = index++;

            // Now add the project to the tree
            for (Project* proj : projects)
            {
                proj->treeid = tree->AppendItem(cat_id, proj->displayname);
                cat_elem->pages.push_back(proj);
            }
        }
    }
}


/*==============================
    json_loadblogentries
    Load a list of blog entries from a JSON file and populate
    the project tree
    @param  The working directory of the program
    @param  A pointer to the category list
    @param  A pointer to tree to populate
==============================*/

void json_loadblogentries(wxString workingdir, std::vector<Category*>* categorylist, wxTreeCtrl* tree)
{
    nlohmann::json pagejson = {};

    // Open the blog json file
    if (wxFileExists(workingdir + wxString("/blog/blog.json")))
        pagejson = nlohmann::json::parse(std::ifstream(wxString(workingdir + wxString("/blog/blog.json")).ToStdString()));

    // Add the blog entries from the JSON file
    for (nlohmann::json::iterator itcat = pagejson["Categories"].begin(); itcat != pagejson["Categories"].end(); ++itcat)
    {
        int index = 0;
        wxTreeItemId cat_id;
        Category* cat_elem = NULL;
        std::vector<Blog*> blogs;

        // Find the category tree index
        for (Category* cat : *categorylist)
        {
            if (wxString(itcat.key()) == cat->foldername)
            {
                cat_elem = cat;
                cat_id = cat->treeid;
                break;
            }
        }

        // Read the blog data from the JSON
        for (nlohmann::json::iterator itblog = (*itcat)["Pages"].begin(); itblog != (*itcat)["Pages"].end(); ++itblog)
        {
            wxString taglist = wxString("");
            Blog* bentry = new Blog();
            bentry->index = (*itblog)["Index"];
            bentry->filename = wxString(itblog.key());
            bentry->displayname = wxString(std::string((*itblog)["DisplayName"]));
            bentry->icon = wxString(std::string((*itblog)["Icon"]));
            bentry->date = wxString(std::string((*itblog)["Date"]));
            bentry->tooltip = wxString(std::string((*itblog)["ToolTip"]));
            bentry->wasmodified = false;
            bentry->content = string_fromfile(workingdir + wxString("/blog/") + cat_elem->foldername + wxString("/markdown/") + bentry->filename + wxString(".md"));
            bentry->tags.clear();
            for (nlohmann::json::iterator it = (*itblog)["Tags"].begin(); it != (*itblog)["Tags"].end(); ++it)
                bentry->tags.push_back(wxString(std::string(*it)));
            bentry->category = cat_elem;
            bentry->treeid = NULL;
            blogs.push_back(bentry);
        }

        // Correct the blog entry indices, to make sure they're in proper order
        if (blogs.size() > 0)
        {
            index = 0;
            std::sort(blogs.begin(), blogs.end(), &blog_sorter);
            for (Blog* bentry : blogs)
                bentry->index = index++;

            // Now add the blog entry to the tree
            for (Blog* bentry : blogs)
            {
                bentry->treeid = tree->AppendItem(cat_id, bentry->displayname);
                cat_elem->pages.push_back(bentry);
            }
        }
    }
}

void json_save(wxString workingdir, std::vector<Category*>* categorylist_projects, std::vector<Category*>* categorylist_blog)
{
    wxTextFile out_proj(workingdir + wxString("/projects/projects.json"));
    wxTextFile out_blog(workingdir + wxString("/blog/blog.json"));
    nlohmann::json projectjson = {};
    nlohmann::json blogjson = {};
    projectjson["Categories"] = {};

    // Handle projects
    for (Category* cat : *categorylist_projects)
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
            wxTextFile projmd(workingdir + wxString("/projects/") + cat->foldername + wxString("/markdown/") + proj->filename + wxString(".md"));
            projectjson["Categories"][catstr]["Pages"][projstr] = {};
            projectjson["Categories"][catstr]["Pages"][projstr]["Index"] = proj->index;
            projectjson["Categories"][catstr]["Pages"][projstr]["DisplayName"] = proj->displayname;
            projectjson["Categories"][catstr]["Pages"][projstr]["Icon"] = proj->icon;
            projectjson["Categories"][catstr]["Pages"][projstr]["Date"] = proj->date;
            projectjson["Categories"][catstr]["Pages"][projstr]["ToolTip"] = proj->tooltip;
            projectjson["Categories"][catstr]["Pages"][projstr]["Images"] = {};
            for (wxString str :  proj->images)
                projectjson["Categories"][catstr]["Pages"][projstr]["Images"].push_back(str);
            projectjson["Categories"][catstr]["Pages"][projstr]["URLs"] = {};
            for (wxString str :  proj->urls)
                projectjson["Categories"][catstr]["Pages"][projstr]["URLs"].push_back(str);
            projectjson["Categories"][catstr]["Pages"][projstr]["Tags"] = {};
            for (wxString str :  proj->tags)
                projectjson["Categories"][catstr]["Pages"][projstr]["Tags"].push_back(str);
            if (!wxDirExists(workingdir + wxString("/projects/") + cat->foldername + wxString("/markdown/")))
                wxFileName::Mkdir(workingdir + wxString("/projects/") + cat->foldername + wxString("/markdown/"));
            if (!projmd.Exists())
                projmd.Create();
            projmd.Clear();
            projmd.AddLine(proj->description);
            projmd.Write();
            projmd.Close();
        }
    }

    // Handle blog
    blogjson["Categories"] = {};
    for (Category* cat : *categorylist_blog)
    {
        std::string catstr = cat->foldername.ToStdString();
        blogjson["Categories"][catstr] = {};
        blogjson["Categories"][catstr]["Index"] = cat->index;
        blogjson["Categories"][catstr]["DisplayName"] = cat->displayname;
        blogjson["Categories"][catstr]["Description"] = cat->description;
        blogjson["Categories"][catstr]["Pages"] = {};
        for (void* child : cat->pages)
        {
            Blog* bentry = (Blog*)child;
            std::string blogstr = bentry->filename.ToStdString();
            wxTextFile blogmd(workingdir + wxString("/blog/") + cat->foldername + wxString("/markdown/") + bentry->filename + wxString(".md"));
            blogjson["Categories"][catstr]["Pages"][blogstr] = {};
            blogjson["Categories"][catstr]["Pages"][blogstr]["Index"] = bentry->index;
            blogjson["Categories"][catstr]["Pages"][blogstr]["DisplayName"] = bentry->displayname;
            blogjson["Categories"][catstr]["Pages"][blogstr]["Icon"] = bentry->icon;
            blogjson["Categories"][catstr]["Pages"][blogstr]["Date"] = bentry->date;
            blogjson["Categories"][catstr]["Pages"][blogstr]["ToolTip"] = bentry->tooltip;
            blogjson["Categories"][catstr]["Pages"][blogstr]["Tags"] = {};
            for (wxString str : bentry->tags)
                blogjson["Categories"][catstr]["Pages"][blogstr]["Tags"].push_back(str);
            if (!wxDirExists(workingdir + wxString("/blog/") + cat->foldername + wxString("/markdown/")))
                wxFileName::Mkdir(workingdir + wxString("/blog/") + cat->foldername + wxString("/markdown/"));
            if (!blogmd.Exists())
                blogmd.Create();
            blogmd.Clear();
            blogmd.AddLine(bentry->content);
            blogmd.Write();
            blogmd.Close();
        }
    }

    // Dump the JSONs to text files
    if (!out_proj.Exists())
        out_proj.Create();
    out_proj.Clear();
    out_proj.AddLine(wxString(projectjson.dump(4)));
    out_proj.Write();
    out_proj.Close();
    if (!out_blog.Exists())
        out_blog.Create();
    out_blog.Clear();
    out_blog.AddLine(wxString(blogjson.dump(4)));
    out_blog.Write();
    out_blog.Close();
}