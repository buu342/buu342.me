#pragma once

typedef struct IUnknown IUnknown; 

#include <wx/string.h>
#include <vector>

typedef enum {
    PAGETYPE_PROJECT,
    PAGETYPE_BLOG,
} PageType;

typedef struct Tag_s {
    wxString name;
    PageType type;
    std::vector<void*> pages;
} Tag;