/***************************************************************
                           helper.cpp

This file contains a bunch of helper functions for transforming,
sorting, and finding data types
***************************************************************/

#include <wx/textfile.h>
#include "helper.h"


/*==============================
    category_sorter
    Sorts two category objects by their index
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool category_sorter(Category* lhs, Category* rhs)
{
    return lhs->index < rhs->index;
}


/*==============================
    project_sorter
    Sorts two project pages by their index
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool project_sorter(Project* lhs, Project* rhs)
{
    return lhs->index < rhs->index;
}


/*==============================
    blog_sorter
    Sorts two blog entries by their index
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool blog_sorter(Blog* lhs, Blog* rhs)
{
    return lhs->index < rhs->index;
}


/*==============================
    date_fromstring
    Retrieves a date from a string
    @param  The input string
    @param  A pointer to the date to output
    @return Whether the conversion was successful
==============================*/

bool date_fromstring(wxString input, wxDateTime* output)
{
    wxString::const_iterator end;
    if (!output->ParseDate(input, &end))
        return false;
    return true;
}


/*==============================
    project_sorter_date
    Sorts two project pages by their date
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool project_sorter_date(Project* lhs, Project* rhs)
{
    wxDateTime date_lhs, date_rhs;
    if (!date_fromstring(lhs->date, &date_lhs))
        return false;
    if (!date_fromstring(rhs->date, &date_rhs))
        return true;
    return date_lhs.IsLaterThan(date_rhs);
}


/*==============================
    blog_sorter_date
    Sorts two blog entries by their date
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool blog_sorter_date(Blog* lhs, Blog* rhs)
{
    wxDateTime date_lhs, date_rhs;
    if (!date_fromstring(lhs->date, &date_lhs))
        return false;
    if (!date_fromstring(rhs->date, &date_rhs))
        return true;
    return date_lhs.IsLaterThan(date_rhs);
}


/*==============================
    taggedpage_sorter
    Sorts two tagged pages by their name
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool taggedpage_sorter(TaggedPage lhs, TaggedPage rhs)
{
    wxString lhs_name, rhs_name;
    if (lhs.type == PAGETYPE_PROJECT)
        lhs_name = ((Project*)lhs.page)->displayname;
    else if (lhs.type == PAGETYPE_BLOG)
        lhs_name = ((Blog*)lhs.page)->displayname;
    if (rhs.type == PAGETYPE_PROJECT)
        rhs_name = ((Project*)rhs.page)->displayname;
    else if (rhs.type == PAGETYPE_BLOG)
        rhs_name = ((Blog*)rhs.page)->displayname;
    return lhs_name < rhs_name;
}


/*==============================
    treeitem_iscategory
    Checks if the item in the given tree is a category object
    @param  The tree to search
    @param  The item to check
    @return Whether the item is a category object
==============================*/

bool treeitem_iscategory(wxTreeCtrl* tree, wxTreeItemId item)
{
    return tree->GetRootItem() == tree->GetItemParent(item);
}


/*==============================
    string_fromfile
    Opens a text file and exports it as a string
    @param  The path to the file to open
    @return The file contents as a string, or ""
==============================*/

wxString string_fromfile(wxString path)
{
    wxTextFile file;
    wxString str = wxString("");
    size_t linecount;

    // Ensure we managed to open the file
    file.Open(path);
    if (!file.IsOpened())
        return wxString("");

    // Read the file into the string, and return it
    linecount = file.GetLineCount();
    str += file.GetFirstLine();
    while (!file.Eof())
    {
        if (file.GetCurrentLine() < linecount-1)
            str += wxString("\r\n");
        str += file.GetNextLine();
    }
    return str;
}


/*==============================
    sanitize_tagname
    Sanitizes a tag's name so it's a valid file name
    @param  The tag name to sanitize
    @return The sanitized tag name
==============================*/

wxString sanitize_tagname(wxString tagname)
{
    tagname.Replace("#", "s");
    tagname.Replace("+", "p");
    tagname.Replace("-", "");
    tagname.Replace("/", "");
    tagname.Replace(".", "");
    tagname.Replace(" ", "");
    return tagname;
}


/*==============================
    md4c_funcptr_handlestr
    A helper function for appending strings during html 
    generation from markdown
    @param  The input markdown text
    @param  The size of the input markdown text
    @param  A pointer to the outputted content
==============================*/

void md4c_funcptr_handlestr(const MD_CHAR* input, MD_SIZE inputsize, void* output)
{
    ((wxString*)output)->Append(input, inputsize);
}


/*==============================
    md_sanitize
    Sanitizes UTF-8 symbols for markdown parsing
    @param  The input text to sanitize
    @return The input text pointer, but sanitized
==============================*/

wxString* md_sanitize(wxString* input)
{
    input->Replace(wxString::FromUTF8("&"), "_AMPERSAND_SYMBOL");
    input->Replace(wxString::FromUTF8("€"), "_EURO_SYMBOL");
    input->Replace(wxString::FromUTF8("£"), "_POUND_SYMBOL");
    input->Replace(wxString::FromUTF8("¥"), "_YEN_SYMBOL");
    input->Replace(wxString::FromUTF8("§"), "_SECTION_SYMBOL");
    input->Replace(wxString::FromUTF8("™"), "_TRADEMARK_SYMBOL");
    input->Replace(wxString::FromUTF8("®"), "_REGISTERED_SYMBOL");
    input->Replace(wxString::FromUTF8("©"), "_COPYRIGHT_SYMBOL");
    input->Replace(wxString::FromUTF8("º"), "_DEGREE_SYMBOL");
    input->Replace(wxString::FromUTF8("ª"), "_SUBA_SYMBOL");
    input->Replace(wxString::FromUTF8("á"), "_AACUTE_SYMBOL");
    input->Replace(wxString::FromUTF8("Á"), "_AACUTEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ã"), "_ATILDE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ã"), "_ATILDEC_SYMBOL");
    input->Replace(wxString::FromUTF8("â"), "_ACIRCUM_SYMBOL");
    input->Replace(wxString::FromUTF8("Â"), "_ACIRCUMC_SYMBOL");
    input->Replace(wxString::FromUTF8("à"), "_AGRAVE_SYMBOL");
    input->Replace(wxString::FromUTF8("À"), "_AGRAVEC_SYMBOL");
    input->Replace(wxString::FromUTF8("é"), "_EACUTE_SYMBOL");
    input->Replace(wxString::FromUTF8("É"), "_EACUTEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ẽ"), "_ETILDE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ẽ"), "_ETILDEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ê"), "_ECIRCUM_SYMBOL");
    input->Replace(wxString::FromUTF8("Ê"), "_ECIRCUMC_SYMBOL");
    input->Replace(wxString::FromUTF8("è"), "_EGRAVE_SYMBOL");
    input->Replace(wxString::FromUTF8("È"), "_EGRAVEC_SYMBOL");
    input->Replace(wxString::FromUTF8("í"), "_IACUTE_SYMBOL");
    input->Replace(wxString::FromUTF8("Í"), "_IACUTEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ĩ"), "_ITILDE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ĩ"), "_ITILDEC_SYMBOL");
    input->Replace(wxString::FromUTF8("î"), "_ICIRCUM_SYMBOL");
    input->Replace(wxString::FromUTF8("Î"), "_ICIRCUMC_SYMBOL");
    input->Replace(wxString::FromUTF8("ì"), "_IGRAVE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ì"), "_IGRAVEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ó"), "_OACUTE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ó"), "_OACUTEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ô"), "_OTILDE_SYMBOL");
    input->Replace(wxString::FromUTF8("Õ"), "_OTILDEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ô"), "_OCIRCUM_SYMBOL");
    input->Replace(wxString::FromUTF8("Ô"), "_OCIRCUMC_SYMBOL");
    input->Replace(wxString::FromUTF8("ò"), "_OGRAVE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ò"), "_OGRAVEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ú"), "_UACUTE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ú"), "_UACUTEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ũ"), "_UTILDE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ũ"), "_UTILDEC_SYMBOL");
    input->Replace(wxString::FromUTF8("û"), "_UCIRCUM_SYMBOL");
    input->Replace(wxString::FromUTF8("Û"), "_UCIRCUMC_SYMBOL");
    input->Replace(wxString::FromUTF8("ù"), "_UGRAVE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ù"), "_UGRAVEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ç"), "_CCEDILHA_SYMBOL");
    input->Replace(wxString::FromUTF8("Ç"), "_CCEDILHAC_SYMBOL");
    return input;
}


/*==============================
    md_sanitize
    Unsanitizes UTF-8 symbols from markdown parsing
    @param  The input text to unsanitize
    @return The input text pointer, but unsanitized
==============================*/

wxString* md_unsanitize(wxString* input)
{
    input->Replace("_AMPERSAND_SYMBOL", "&amp;");
    input->Replace("_EURO_SYMBOL", "&euro;");
    input->Replace("_POUND_SYMBOL", "&pound;");
    input->Replace("_YEN_SYMBOL", "&yen;");
    input->Replace("_SECTION_SYMBOL", "&sect;");
    input->Replace("_TRADEMARK_SYMBOL", "&trade;");
    input->Replace("_REGISTERED_SYMBOL", "&reg");
    input->Replace("_COPYRIGHT_SYMBOL", "&copy;");
    input->Replace("_DEGREE_SYMBOL", "&ordm;");
    input->Replace("_SUBA_SYMBOL", "&ordf;");
    input->Replace("_AACUTE_SYMBOL", "&aacute;");
    input->Replace("_AACUTEC_SYMBOL", "&Aacute;");
    input->Replace("_ATILDE_SYMBOL", "&atilde;");
    input->Replace("_ATILDEC_SYMBOL", "&Atilde;");
    input->Replace("_ACIRCUM_SYMBOL", "&acirc;");
    input->Replace("_ACIRCUMC_SYMBOL", "&Acirc;");
    input->Replace("_AGRAVE_SYMBOL", "&agrave;");
    input->Replace("_AGRAVEC_SYMBOL", "&Agrave;");
    input->Replace("_EACUTE_SYMBOL", "&eacute;");
    input->Replace("_EACUTEC_SYMBOL", "&Eacute;");
    input->Replace("_ETILDE_SYMBOL", "&etilde;");
    input->Replace("_ETILDEC_SYMBOL", "&Etilde;");
    input->Replace("_ECIRCUM_SYMBOL", "&ecirc;");
    input->Replace("_ECIRCUMC_SYMBOL", "&Ecirc;");
    input->Replace("_EGRAVE_SYMBOL", "&egrave;");
    input->Replace("_EGRAVEC_SYMBOL", "&Egrave;");
    input->Replace("_IACUTE_SYMBOL", "&iacute;");
    input->Replace("_IACUTEC_SYMBOL", "&Iacute;");
    input->Replace("_ITILDE_SYMBOL", "&itilde;");
    input->Replace("_ITILDEC_SYMBOL", "&Itilde;");
    input->Replace("_ICIRCUM_SYMBOL", "&icirc;");
    input->Replace("_ICIRCUMC_SYMBOL", "&Icirc;");
    input->Replace("_IGRAVE_SYMBOL", "&igrave;");
    input->Replace("_IGRAVEC_SYMBOL", "&Igrave;");
    input->Replace("_OACUTE_SYMBOL", "&oacute;");
    input->Replace("_OACUTEC_SYMBOL", "&Oacute;");
    input->Replace("_OTILDE_SYMBOL", "&otilde;");
    input->Replace("_OTILDEC_SYMBOL", "&Otilde;");
    input->Replace("_OCIRCUM_SYMBOL", "&ocirc;");
    input->Replace("_OCIRCUMC_SYMBOL", "&Ocirc;");
    input->Replace("_OGRAVE_SYMBOL", "&ograve;");
    input->Replace("_OGRAVEC_SYMBOL", "&Ograve;");
    input->Replace("_UACUTE_SYMBOL", "&uacute;");
    input->Replace("_UACUTEC_SYMBOL", "&Uacute;");
    input->Replace("_UTILDE_SYMBOL", "&utilde;");
    input->Replace("_UTILDEC_SYMBOL", "&Utilde;");
    input->Replace("_UCIRCUM_SYMBOL", "&ucirc;");
    input->Replace("_UCIRCUMC_SYMBOL", "&Ucirc;");
    input->Replace("_UGRAVE_SYMBOL", "&ugrave;");
    input->Replace("_UGRAVEC_SYMBOL", "&Ugrave;");
    input->Replace("_CCEDILHA_SYMBOL", "&ccedil;");
    input->Replace("_CCEDILHAC_SYMBOL", "&Ccedil;");
    return input;
}