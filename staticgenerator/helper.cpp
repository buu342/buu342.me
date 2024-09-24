#include <wx/textfile.h>
#include "helper.h"

bool category_sorter(Category* lhs, Category* rhs)
{
    return lhs->index < rhs->index;
}

bool project_sorter(Project* lhs, Project* rhs)
{
    return lhs->index < rhs->index;
}

bool blog_sorter(Blog* lhs, Blog* rhs)
{
    return lhs->index < rhs->index;
}

bool date_fromstring(wxString input, wxDateTime* output)
{
    wxString::const_iterator end;
    if (!output->ParseDate(input, &end))
        return false;
    return true;
}

bool project_sorter_date(Project* lhs, Project* rhs)
{
    wxDateTime date_lhs, date_rhs;
    if (!date_fromstring(lhs->date, &date_lhs))
        return false;
    if (!date_fromstring(rhs->date, &date_rhs))
        return true;
    return date_lhs.IsLaterThan(date_rhs);
}

bool blog_sorter_date(Blog* lhs, Blog* rhs)
{
    wxDateTime date_lhs, date_rhs;
    if (!date_fromstring(lhs->date, &date_lhs))
        return false;
    if (!date_fromstring(rhs->date, &date_rhs))
        return true;
    return date_lhs.IsLaterThan(date_rhs);
}

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

bool treeitem_iscategory(wxTreeCtrl* tree, wxTreeItemId item)
{
    return tree->GetRootItem() == tree->GetItemParent(item);
}

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

void md4c_funcptr_handlestr(const MD_CHAR* input, MD_SIZE inputsize, void* output)
{
    ((wxString*)output)->Append(input, inputsize);
}

wxString* md_sanitize(wxString* input)
{
    input->Replace(wxString::FromUTF8("&"), "AMPERSAND_SYMBOL");
    input->Replace(wxString::FromUTF8("€"), "EURO_SYMBOL");
    input->Replace(wxString::FromUTF8("£"), "POUND_SYMBOL");
    input->Replace(wxString::FromUTF8("¥"), "YEN_SYMBOL");
    input->Replace(wxString::FromUTF8("§"), "SECTION_SYMBOL");
    input->Replace(wxString::FromUTF8("™"), "TRADEMARK_SYMBOL");
    input->Replace(wxString::FromUTF8("®"), "REGISTERED_SYMBOL");
    input->Replace(wxString::FromUTF8("©"), "COPYRIGHT_SYMBOL");
    input->Replace(wxString::FromUTF8("á"), "AACUTE_SYMBOL");
    input->Replace(wxString::FromUTF8("Á"), "AACUTEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ã"), "ATILDE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ã"), "ATILDEC_SYMBOL");
    input->Replace(wxString::FromUTF8("â"), "ACIRCUM_SYMBOL");
    input->Replace(wxString::FromUTF8("Â"), "ACIRCUMC_SYMBOL");
    input->Replace(wxString::FromUTF8("à"), "AGRAVE_SYMBOL");
    input->Replace(wxString::FromUTF8("À"), "AGRAVEC_SYMBOL");
    input->Replace(wxString::FromUTF8("é"), "EACUTE_SYMBOL");
    input->Replace(wxString::FromUTF8("É"), "EACUTEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ẽ"), "ETILDE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ẽ"), "ETILDEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ê"), "ECIRCUM_SYMBOL");
    input->Replace(wxString::FromUTF8("Ê"), "ECIRCUMC_SYMBOL");
    input->Replace(wxString::FromUTF8("è"), "EGRAVE_SYMBOL");
    input->Replace(wxString::FromUTF8("È"), "EGRAVEC_SYMBOL");
    input->Replace(wxString::FromUTF8("í"), "IACUTE_SYMBOL");
    input->Replace(wxString::FromUTF8("Í"), "IACUTEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ĩ"), "ITILDE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ĩ"), "ITILDEC_SYMBOL");
    input->Replace(wxString::FromUTF8("î"), "ICIRCUM_SYMBOL");
    input->Replace(wxString::FromUTF8("Î"), "ICIRCUMC_SYMBOL");
    input->Replace(wxString::FromUTF8("ì"), "IGRAVE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ì"), "IGRAVEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ó"), "OACUTE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ó"), "OACUTEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ô"), "OTILDE_SYMBOL");
    input->Replace(wxString::FromUTF8("Õ"), "OTILDEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ô"), "OCIRCUM_SYMBOL");
    input->Replace(wxString::FromUTF8("Ô"), "OCIRCUMC_SYMBOL");
    input->Replace(wxString::FromUTF8("ò"), "OGRAVE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ò"), "OGRAVEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ú"), "UACUTE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ú"), "UACUTEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ũ"), "UTILDE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ũ"), "UTILDEC_SYMBOL");
    input->Replace(wxString::FromUTF8("û"), "UCIRCUM_SYMBOL");
    input->Replace(wxString::FromUTF8("Û"), "UCIRCUMC_SYMBOL");
    input->Replace(wxString::FromUTF8("ù"), "UGRAVE_SYMBOL");
    input->Replace(wxString::FromUTF8("Ù"), "UGRAVEC_SYMBOL");
    input->Replace(wxString::FromUTF8("ç"), "CCEDILHA_SYMBOL");
    input->Replace(wxString::FromUTF8("Ç"), "CCEDILHAC_SYMBOL");
    return input;
}

wxString* md_unsanitize(wxString* input)
{
    input->Replace("AMPERSAND_SYMBOL", "&amp;");
    input->Replace("EURO_SYMBOL", "&euro;");
    input->Replace("POUND_SYMBOL", "&pound;");
    input->Replace("YEN_SYMBOL", "&yen;");
    input->Replace("SECTION_SYMBOL", "&sect;");
    input->Replace("TRADEMARK_SYMBOL", "&trade;");
    input->Replace("REGISTERED_SYMBOL", "&reg");
    input->Replace("COPYRIGHT_SYMBOL", "&copy;");
    input->Replace("AACUTE_SYMBOL", "&aacute;");
    input->Replace("AACUTEC_SYMBOL", "&Aacute;");
    input->Replace("ATILDE_SYMBOL", "&atilde;");
    input->Replace("ATILDEC_SYMBOL", "&Atilde;");
    input->Replace("ACIRCUM_SYMBOL", "&acirc;");
    input->Replace("ACIRCUMC_SYMBOL", "&Acirc;");
    input->Replace("AGRAVE_SYMBOL", "&agrave;");
    input->Replace("AGRAVEC_SYMBOL", "&Agrave;");
    input->Replace("EACUTE_SYMBOL", "&eacute;");
    input->Replace("EACUTEC_SYMBOL", "&Eacute;");
    input->Replace("ETILDE_SYMBOL", "&etilde;");
    input->Replace("ETILDEC_SYMBOL", "&Etilde;");
    input->Replace("ECIRCUM_SYMBOL", "&ecirc;");
    input->Replace("ECIRCUMC_SYMBOL", "&Ecirc;");
    input->Replace("EGRAVE_SYMBOL", "&egrave;");
    input->Replace("EGRAVEC_SYMBOL", "&Egrave;");
    input->Replace("IACUTE_SYMBOL", "&iacute;");
    input->Replace("IACUTEC_SYMBOL", "&Iacute;");
    input->Replace("ITILDE_SYMBOL", "&itilde;");
    input->Replace("ITILDEC_SYMBOL", "&Itilde;");
    input->Replace("ICIRCUM_SYMBOL", "&icirc;");
    input->Replace("ICIRCUMC_SYMBOL", "&Icirc;");
    input->Replace("IGRAVE_SYMBOL", "&igrave;");
    input->Replace("IGRAVEC_SYMBOL", "&Igrave;");
    input->Replace("OACUTE_SYMBOL", "&oacute;");
    input->Replace("OACUTEC_SYMBOL", "&Oacute;");
    input->Replace("OTILDE_SYMBOL", "&otilde;");
    input->Replace("OTILDEC_SYMBOL", "&Otilde;");
    input->Replace("OCIRCUM_SYMBOL", "&ocirc;");
    input->Replace("OCIRCUMC_SYMBOL", "&Ocirc;");
    input->Replace("OGRAVE_SYMBOL", "&ograve;");
    input->Replace("OGRAVEC_SYMBOL", "&Ograve;");
    input->Replace("UACUTE_SYMBOL", "&uacute;");
    input->Replace("UACUTEC_SYMBOL", "&Uacute;");
    input->Replace("UTILDE_SYMBOL", "&utilde;");
    input->Replace("UTILDEC_SYMBOL", "&Utilde;");
    input->Replace("UCIRCUM_SYMBOL", "&ucirc;");
    input->Replace("UCIRCUMC_SYMBOL", "&Ucirc;");
    input->Replace("UGRAVE_SYMBOL", "&ugrave;");
    input->Replace("UGRAVEC_SYMBOL", "&Ugrave;");
    input->Replace("CCEDILHA_SYMBOL", "&ccedil;");
    input->Replace("CCEDILHAC_SYMBOL", "&Ccedil;");
    return input;
}