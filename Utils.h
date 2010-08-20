/* Utility functons */

#ifndef __UTILS_H__
#define __UTILS_H__
#include <wx/string.h>
#include <wx/window.h>
#include <wx/datetime.h>

int SIMessageBox(const wxString&  message,
		const wxString&  caption = "SI Simple",
		long style = wxOK|wxCENTRE,
		wxWindow*  parent = NULL);
		
wxString SimplifyPath(wxString a_Path);		

long ToLong(const wxString& a_Value);
wxDateTime ToDateTime(const wxString& a_Value);
int DOWToInt(const wxString& a_DOW);
wxString FormatTimeTaken(wxTimeSpan a_TimeSpan, bool a_NullOK = false);
#endif
