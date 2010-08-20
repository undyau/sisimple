#include "Utils.h"
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>


int SIMessageBox(const wxString &  	message,
		const wxString& caption,
		long style,
		wxWindow* parent)
{
    wxMessageDialog dlg(parent, message, caption, style);
    return dlg.ShowModal();
}

wxString SimplifyPath(wxString a_Path)
{
    wxString docdir =  wxStandardPaths::Get().GetDocumentsDir();
    if (a_Path.Left(docdir.length() + 1) ==  docdir + wxFileName::GetPathSeparator())
        return a_Path.Mid(docdir.length() + 1);
    else
        return a_Path;  
}

long ToLong(const wxString& a_Value)
{
    long result;
    if (a_Value.ToLong(&result))
        return result;
    else
        return 0;
}

wxDateTime ToDateTime(const wxString& a_Value)
{
    wxDateTime result;
    
    if (!a_Value.IsEmpty()) 
        result.ParseTime(a_Value);
    return result;
}

int DOWToInt(const wxString& a_DOW)
{
    if (a_DOW.IsEmpty())
        return -1;
    if (a_DOW == "Su") return 0;
    if (a_DOW == "Mo") return 1;
    if (a_DOW == "Tu") return 2;
    if (a_DOW == "We") return 3;
    if (a_DOW == "Th") return 4;
    if (a_DOW == "Fr") return 5;
    if (a_DOW == "Sa") return 6;
    return -2;
}

wxString FormatTimeTaken(wxTimeSpan a_TimeSpan, bool a_NullOK)
{
    if ((!a_NullOK && a_TimeSpan.IsNull()) || a_TimeSpan.IsNegative())
        return "";
        
    wxLongLong minutes, seconds;
    seconds = a_TimeSpan.GetSeconds();
    minutes = seconds /60;
    seconds = seconds - (minutes * 60);

    wxString s;
    s.Printf("%01ld:%02ld", minutes.ToLong(), seconds.ToLong());
    return s;
}
