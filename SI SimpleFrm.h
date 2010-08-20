///-----------------------------------------------------------------
///
/// @file      SI SimpleFrm.h
/// @author    Andy Simpson
/// Created:   13/02/2010 4:39:56 PM
/// @section   DESCRIPTION
///            SI_SimpleFrm class declaration
///
///------------------------------------------------------------------

#ifndef __SI_SIMPLEFRM_H__
#define __SI_SIMPLEFRM_H__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/frame.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/menu.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/toolbar.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/sizer.h>
////Header Include End
#include "IObserver.h"

////Dialog Style Start
#undef SI_SimpleFrm_STYLE
#define SI_SimpleFrm_STYLE wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

class SI_SimpleFrm : public wxFrame, public IObserver
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		SI_SimpleFrm(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("SI Simple"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = SI_SimpleFrm_STYLE);
		void FileopenClick(wxCommandEvent& event);
		virtual ~SI_SimpleFrm();
		void OnFileExit(wxCommandEvent& event);
		void OnFileOpen(wxCommandEvent& event);
		void MenuaboutClick(wxCommandEvent& event);
		void OnListLogRightClick(wxMouseEvent& event);
		
		void Update(ISubject* a_ChangedSubject, int a_Hint);
		void OnExportIofXml(wxCommandEvent& event);
		void OnExportIofXmlUpdateUI(wxUpdateUIEvent& event);
		void OnPopupClick(wxCommandEvent &evt);
		void MnuRecalcClick(wxCommandEvent& event);
		void MnuRecalcUpdateUI(wxUpdateUIEvent& event);
		void m_CheckHTMLClick(wxCommandEvent& event);
		void m_CheckSplitsClick(wxCommandEvent& event);
		void m_EventNameUpdated(wxCommandEvent& event);
		void OnSaveClick(wxCommandEvent& event);
		void OnSaveUpdateUI(wxUpdateUIEvent& event);
		
	private:
		//Do not add custom control declarations between
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxMenuBar *WxMenuBar1;
		wxTextCtrl *m_EventName;
		wxStaticText *WxStaticText1;
		wxCheckBox *m_CheckSplits;
		wxCheckBox *m_CheckHTML;
		wxToolBar *m_ToolBar;
		wxStatusBar *m_StatusBar;
		wxTextCtrl *m_DisplayEdit;
		wxListBox *m_ListLog;
		wxSplitterWindow *WxSplitterWindow1;
		wxBoxSizer *WxBoxSizer1;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_MENU_FILE = 1003,
			ID_FILE_OPEN = 1007,
			ID_MNU_EXPORTTOIOFXML_1020 = 1020,
			ID_FILE_EXIT = 1004,
			ID_MNU_RESULTS_1021 = 1021,
			ID_MNU_RECALC_1022 = 1022,
			ID_MNU_HELP_1017 = 1017,
			ID_MENUABOUT = 1018,
			
			ID_M_EVENTNAME = 1027,
			ID_WXSTATICTEXT1 = 1026,
			ID_M_CHECKSPLITS = 1025,
			ID_M_CHECKHTML = 1024,
			ID_WXSEPARATOR1 = 1023,
			ID_TB_SAVE = 1028,
			ID_TB_RECALC = 1021,
			ID_TB_OPEN = 1003,
			ID_M_TOOLBAR = 1005,
			ID_M_STATUSBAR = 1002,
			ID_M_DISPLAYEDIT = 1016,
			ID_M_LISTLOG = 1019,
			ID_WXSPLITTERWINDOW1 = 1014,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
