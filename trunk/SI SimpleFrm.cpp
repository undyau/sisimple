///-----------------------------------------------------------------
///
/// @file      SI SimpleFrm.cpp
/// @author    Andy Simpson
/// Created:   13/02/2010 4:39:56 PM
/// @section   DESCRIPTION
///            SI_SimpleFrm class implementation
///
///------------------------------------------------------------------

#include "SI SimpleFrm.h"
#include "CEvent.h"
#include "Utils.h"
#include "Notifications.h"
#include "CResult.h"
#include <wx/textdlg.h>
#include <wx/choicdlg.h>
#include <wx/arrstr.h>

//Do not add custom headers between
//Header Include Start and Header Include End
//wxDev-C++ designer will remove them
////Header Include Start
#include "Images/SI_SimpleFrm_ID_FILE_OPEN_XPM.xpm"
#include "Images/SI_SimpleFrm_wxID_SAVE_XPM.xpm"
#include "Images/SI_SimpleFrm_ID_MNU_RECALC_1022_XPM.xpm"

#include "Images/SI_SimpleFrm_m_ToolButton_Save_XPM.xpm"
#include "Images/SI_SimpleFrm_m_ToolButton_Recalc_XPM.xpm"
#include "Images/SI_SimpleFrm_m_ToolButton_Open_XPM.xpm"
////Header Include End

#define ID_REINSTATE 3000
#define ID_RENAME 3001
#define ID_SETCOURSE 3002
#define ID_DNF 3003

//----------------------------------------------------------------------------
// SI_SimpleFrm
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(SI_SimpleFrm,wxFrame)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(SI_SimpleFrm::OnClose)
	EVT_MENU(ID_FILE_OPEN, SI_SimpleFrm::OnFileOpen)
	EVT_MENU(wxID_SAVE, SI_SimpleFrm::OnSaveClick)
	EVT_UPDATE_UI(wxID_SAVE, SI_SimpleFrm::OnSaveUpdateUI)
	EVT_MENU(ID_MNU_EXPORTTOIOFXML_1020, SI_SimpleFrm::OnExportIofXml)
	EVT_UPDATE_UI(ID_MNU_EXPORTTOIOFXML_1020, SI_SimpleFrm::OnExportIofXmlUpdateUI)
	EVT_MENU(ID_FILE_EXIT, SI_SimpleFrm::OnFileExit)
	EVT_MENU(ID_MNU_RECALC_1022, SI_SimpleFrm::MnuRecalcClick)
	EVT_UPDATE_UI(ID_MNU_RECALC_1022, SI_SimpleFrm::MnuRecalcUpdateUI)
	EVT_MENU(ID_MENUABOUT, SI_SimpleFrm::MenuaboutClick)
	
	EVT_TEXT(ID_M_EVENTNAME,SI_SimpleFrm::m_EventNameUpdated)
	EVT_CHECKBOX(ID_M_CHECKSPLITS,SI_SimpleFrm::m_CheckSplitsClick)
	EVT_CHECKBOX(ID_M_CHECKHTML,SI_SimpleFrm::m_CheckHTMLClick)
	EVT_MENU(ID_TB_SAVE,SI_SimpleFrm::OnSaveClick)
	EVT_MENU(ID_TB_RECALC,SI_SimpleFrm::MnuRecalcClick)
	EVT_MENU(ID_TB_OPEN,SI_SimpleFrm::FileopenClick)
END_EVENT_TABLE()
////Event Table End

SI_SimpleFrm::SI_SimpleFrm(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	CreateGUIControls();
	CEvent::Event()->Attach(this);
}

SI_SimpleFrm::~SI_SimpleFrm()
{
	CEvent::Event()->Detach(this);
}

void SI_SimpleFrm::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	wxInitAllImageHandlers();   //Initialize graphic format handlers

	WxBoxSizer1 = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(WxBoxSizer1);
	this->SetAutoLayout(true);

	WxSplitterWindow1 = new wxSplitterWindow(this, ID_WXSPLITTERWINDOW1, wxPoint(0, 0), wxSize(854, 317));
	WxBoxSizer1->Add(WxSplitterWindow1,1,wxALIGN_CENTER | wxEXPAND | wxALL,0);

	wxArrayString arrayStringFor_m_ListLog;
	m_ListLog = new wxListBox(WxSplitterWindow1, ID_M_LISTLOG, wxPoint(366, 5), wxSize(121, 97), arrayStringFor_m_ListLog, wxLB_SINGLE | wxLB_HSCROLL);
	m_ListLog->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Lucida Console")));

	m_DisplayEdit = new wxTextCtrl(WxSplitterWindow1, ID_M_DISPLAYEDIT, wxT("The raw data and results are displayed here"), wxPoint(5, 112), wxSize(844, 200), wxTE_RICH | wxTE_DONTWRAP | wxTE_MULTILINE, wxDefaultValidator, wxT("m_DisplayEdit"));
	m_DisplayEdit->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Courier New")));

	m_StatusBar = new wxStatusBar(this, ID_M_STATUSBAR);
	m_StatusBar->SetFieldsCount(3);
	m_StatusBar->SetStatusText(wxT(""),0);
	m_StatusBar->SetStatusText(wxT("Global SI Numbers:"),1);
	m_StatusBar->SetStatusText(wxT("Event Data:"),2);
	int m_StatusBar_Widths[3];
	m_StatusBar_Widths[0] = 150;
	m_StatusBar_Widths[1] = 400;
	m_StatusBar_Widths[2] = -1;
	m_StatusBar->SetStatusWidths(3,m_StatusBar_Widths);

	m_ToolBar = new wxToolBar(this, ID_M_TOOLBAR, wxPoint(0, 0), wxSize(888, 42));

	wxBitmap m_ToolButton_Open_BITMAP (SI_SimpleFrm_m_ToolButton_Open_XPM);
	wxBitmap m_ToolButton_Open_DISABLE_BITMAP (wxNullBitmap);
	m_ToolBar->AddTool(ID_TB_OPEN, wxT("Open"), m_ToolButton_Open_BITMAP, m_ToolButton_Open_DISABLE_BITMAP, wxITEM_NORMAL, wxT("Open"), wxT(""));

	wxBitmap m_ToolButton_Recalc_BITMAP (SI_SimpleFrm_m_ToolButton_Recalc_XPM);
	wxBitmap m_ToolButton_Recalc_DISABLE_BITMAP (wxNullBitmap);
	m_ToolBar->AddTool(ID_TB_RECALC, wxT("Recalculate"), m_ToolButton_Recalc_BITMAP, m_ToolButton_Recalc_DISABLE_BITMAP, wxITEM_NORMAL, wxT("Recalculate"), wxT(""));

	wxBitmap m_ToolButton_Save_BITMAP (SI_SimpleFrm_m_ToolButton_Save_XPM);
	wxBitmap m_ToolButton_Save_DISABLE_BITMAP (wxNullBitmap);
	m_ToolBar->AddTool(ID_TB_SAVE, wxT("Save"), m_ToolButton_Save_BITMAP, m_ToolButton_Save_DISABLE_BITMAP, wxITEM_NORMAL, wxT(""), wxT(""));

	m_ToolBar->AddSeparator();

	m_CheckHTML = new wxCheckBox(m_ToolBar, ID_M_CHECKHTML, wxT("HTML"), wxPoint(91, 0), wxSize(47, 19), wxALIGN_RIGHT, wxDefaultValidator, wxT("m_CheckHTML"));
	m_ToolBar->AddControl(m_CheckHTML);

	m_CheckSplits = new wxCheckBox(m_ToolBar, ID_M_CHECKSPLITS, wxT("Splits"), wxPoint(138, 0), wxSize(62, 19), wxALIGN_RIGHT, wxDefaultValidator, wxT("m_CheckSplits"));
	m_ToolBar->AddControl(m_CheckSplits);

	WxStaticText1 = new wxStaticText(m_ToolBar, ID_WXSTATICTEXT1, wxT("      Event: "), wxPoint(200, 0), wxDefaultSize, 0, wxT("WxStaticText1"));
	m_ToolBar->AddControl(WxStaticText1);

	m_EventName = new wxTextCtrl(m_ToolBar, ID_M_EVENTNAME, wxT(""), wxPoint(257, 0), wxSize(227, 19), 0, wxDefaultValidator, wxT("m_EventName"));
	m_ToolBar->AddControl(m_EventName);

	WxMenuBar1 = new wxMenuBar();
	wxMenu *ID_MENU_FILE_Mnu_Obj = new wxMenu(0);
	wxMenuItem * ID_FILE_OPEN_mnuItem_obj = new wxMenuItem (ID_MENU_FILE_Mnu_Obj, ID_FILE_OPEN, wxT("&Open"), wxT("Open Event"), wxITEM_NORMAL);
	wxBitmap ID_FILE_OPEN_mnuItem_obj_BMP(SI_SimpleFrm_ID_FILE_OPEN_XPM);
	ID_FILE_OPEN_mnuItem_obj->SetBitmap(ID_FILE_OPEN_mnuItem_obj_BMP);
	ID_MENU_FILE_Mnu_Obj->Append(ID_FILE_OPEN_mnuItem_obj);
	wxMenuItem * wxID_SAVE_mnuItem_obj = new wxMenuItem (ID_MENU_FILE_Mnu_Obj, wxID_SAVE, wxT("Save\tCtrl+S"), wxT("Save Results"), wxITEM_NORMAL);
	wxBitmap wxID_SAVE_mnuItem_obj_BMP(SI_SimpleFrm_wxID_SAVE_XPM);
	wxID_SAVE_mnuItem_obj->SetBitmap(wxID_SAVE_mnuItem_obj_BMP);
	ID_MENU_FILE_Mnu_Obj->Append(wxID_SAVE_mnuItem_obj);
	ID_MENU_FILE_Mnu_Obj->Append(ID_MNU_EXPORTTOIOFXML_1020, wxT("Export to IOF XML..."), wxT("Export to IOF XML"), wxITEM_NORMAL);
	ID_MENU_FILE_Mnu_Obj->Append(ID_FILE_EXIT, wxT("E&xit\tCtrl+Q"), wxT("Quit"), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MENU_FILE_Mnu_Obj, wxT("&File"));
	
	wxMenu *ID_MNU_RESULTS_1021_Mnu_Obj = new wxMenu(0);
	wxMenuItem * ID_MNU_RECALC_1022_mnuItem_obj = new wxMenuItem (ID_MNU_RESULTS_1021_Mnu_Obj, ID_MNU_RECALC_1022, wxT("Recalc"), wxT("Recalculate Results"), wxITEM_NORMAL);
	wxBitmap ID_MNU_RECALC_1022_mnuItem_obj_BMP(SI_SimpleFrm_ID_MNU_RECALC_1022_XPM);
	ID_MNU_RECALC_1022_mnuItem_obj->SetBitmap(ID_MNU_RECALC_1022_mnuItem_obj_BMP);
	ID_MNU_RESULTS_1021_Mnu_Obj->Append(ID_MNU_RECALC_1022_mnuItem_obj);
	WxMenuBar1->Append(ID_MNU_RESULTS_1021_Mnu_Obj, wxT("&Results"));
	
	wxMenu *ID_MNU_HELP_1017_Mnu_Obj = new wxMenu(0);
	ID_MNU_HELP_1017_Mnu_Obj->Append(ID_MENUABOUT, wxT("&About"), wxT("Show about dialog"), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MNU_HELP_1017_Mnu_Obj, wxT("&Help"));
	SetMenuBar(WxMenuBar1);

	WxSplitterWindow1->SplitHorizontally(m_ListLog,m_DisplayEdit,839);

	m_ToolBar->SetToolBitmapSize(wxSize(16,16));
	m_ToolBar->SetToolSeparation(19);
	m_ToolBar->Realize();
	SetToolBar(m_ToolBar);
	SetStatusBar(m_StatusBar);
	SetTitle(wxT("SI Simple"));
	SetIcon(wxNullIcon);
	
	GetSizer()->Layout();
	GetSizer()->Fit(this);
	Center();
	
	////GUI Items Creation End
	m_ListLog->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(SI_SimpleFrm::OnListLogRightClick),NULL,this);
	m_CheckHTML->SetValue(CEvent::Event()->GetShowHTML());
    m_CheckSplits->SetValue(CEvent::Event()->GetShowSplits());	
    m_EventName->SetValue(CEvent::Event()->GetEventName());
}

void SI_SimpleFrm::OnClose(wxCloseEvent& event)
{
	Destroy();
}

/*
 * OnFileExit
 */
void SI_SimpleFrm::OnFileExit(wxCommandEvent& event)
{
	Close();
}

void SI_SimpleFrm::Update(ISubject* a_ChangedSubject, int a_Hint)
{
	switch(a_Hint)
	{
		case NFY_ALLFILES:
            m_ListLog->Clear();
            m_DisplayEdit->Clear();
			SetStatusText("Event: " + SimplifyPath(CEvent::Event()->Directory()),2);		 
			SetStatusText("Global SI data: " + SimplifyPath(CEvent::Event()->SINamesGlobalFile()),1);		 
			break;
			
		case NFY_LOGMSG:
            m_ListLog->Append(CEvent::Event()->LastLogMsg() + '\n');
            break;
            
		case NFY_DISPLAYMSG:
            m_DisplayEdit->AppendText(CEvent::Event()->LastDisplayMsg() + '\n');
            break;    
            
        case NFY_CLEARDISPLAY:
            m_DisplayEdit->Clear();
            break;        
	}
		
}

/*
 * FileopenClick
 */
void SI_SimpleFrm::FileopenClick(wxCommandEvent& event)
{
	OnFileOpen(event);
}

/*
 * OnFileOpen
 */
void SI_SimpleFrm::OnFileOpen(wxCommandEvent& event)
{
	CEvent* oevent = CEvent::Event();
	wxDirDialog dlg(this, "Choose event directory", oevent->Directory(),
                        wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() != wxID_OK)
    	return;
	 
    // Test that each required file exists
    if (oevent->FindRawDataFile(dlg.GetPath()).IsEmpty())
        {
        SIMessageBox("No raw data file in specified directory !", "Specify Event Directory");
        return;
        }
     if (oevent->FindCourseFile(dlg.GetPath()).IsEmpty())
     	{
        SIMessageBox("No courses file in specified directory !", "Specify Event Directory");
        return;
		}
            
    if (oevent->FindSINamesFile(dlg.GetPath()).IsEmpty())
       	{	
        if (SIMessageBox("No SI Names file in specified directory. Do you want to continue, using any available global SI names ?", "Specify Event Directory", wxYES_NO|wxCENTRE ) == wxID_NO)
			return;            
        }
        
    oevent->SetDirectory(dlg.GetPath());    

}


/*
 * MenuaboutClick
 */
void SI_SimpleFrm::MenuaboutClick(wxCommandEvent& event)
{

    wxString msg;

    msg.Printf( _T("SI Simple.\nweb: http://sourceforge.net/projects/sisimple/\n\n")
                  _T("Built with %s"),
                        wxVERSION_STRING);

    wxMessageDialog dlg(this, msg, _T("About SI Simple"),
                         wxOK | wxICON_INFORMATION);
    dlg.ShowModal();
}

void SI_SimpleFrm::OnListLogRightClick(wxMouseEvent& event)
{
    int item = m_ListLog->HitTest(event.GetPosition());
    if (item != wxNOT_FOUND)
        {
        m_ListLog->Select(item);
        wxString text = m_ListLog->GetString(item);
        int start = text.First('(');
        int end = text.First(')');
        if (start >= 0 && end > start)
            {
            long ref;
            text.Mid(start+1, end-start).ToLong(&ref);
            CResult* res = CEvent::Event()->GetResult(ref);
            
            if (res != NULL)
                {                   
                // show pop-up menu
                void *data = reinterpret_cast<void *>(res);
            	wxMenu mnu;
            	mnu.SetClientData( data );
            	if (!res->GetFinished() && !res->GetInvalid())
                	mnu.Append(ID_REINSTATE, "Reinstate");
                if (res->GetFinished())
                	mnu.Append(ID_DNF, 	"DNF");    	
            	mnu.Append(ID_RENAME, 	"Name...");   
                if (CEvent::Event()->GetCourseCount() > 1)	    	
                	mnu.Append(ID_SETCOURSE, 	"Course...");    	    	    	
            	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SI_SimpleFrm::OnPopupClick, NULL, this);
            	PopupMenu(&mnu);
                }
            }
        }
}

void SI_SimpleFrm::OnPopupClick(wxCommandEvent &evt)
{
    void *data=static_cast<wxMenu *>(evt.GetEventObject())->GetClientData();
    CResult* res = static_cast<CResult*>(data);
    switch (evt.GetId()) 
    {
        case ID_REINSTATE: res->SetFinishedOverride(true); break;
        case ID_DNF: res->SetFinishedOverride(false); break;
        case ID_RENAME:
            {
            wxTextEntryDialog dlg(this, "Enter name of runner", res->GetDisplayName());
            if (dlg.ShowModal() == wxID_OK)
                res->SetName(dlg.GetValue());
            break;
            }
        case ID_SETCOURSE:
            {
            wxArrayString courses;
            CEvent::Event()->GetCourseNames(courses);
            wxSingleChoiceDialog dlg(this, "Select course", "Set course", courses);
            if (dlg.ShowModal() == wxID_OK)
                CEvent::Event()->SetResultCourse(res, dlg.GetStringSelection());
            break;
            }
    }
}
		
/*
 * OnExportIofXml
 */
void SI_SimpleFrm::OnExportIofXml(wxCommandEvent& event)
{
	CEvent* oevent = CEvent::Event();
	wxFileDialog dlg(this, "Export results to", oevent->Directory(), wxEmptyString, "XML files (*.xml)|*.xml",
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() == wxID_OK)
        CEvent::Event()->ExportXML(dlg.GetPath());
}

/*
 * OnExportIofXmlUpdateUI
 */
void SI_SimpleFrm::OnExportIofXmlUpdateUI(wxUpdateUIEvent& event)
{
	// insert your code here
}

/*
 * MnuRecalcClick
 */
void SI_SimpleFrm::MnuRecalcClick(wxCommandEvent& event)
{
	CEvent::Event()->RecalcResults();
}

/*
 * MnuRecalcUpdateUI
 */
void SI_SimpleFrm::MnuRecalcUpdateUI(wxUpdateUIEvent& event)
{
	// insert your code here
}

/*
 * m_CheckHTMLClick
 */
void SI_SimpleFrm::m_CheckHTMLClick(wxCommandEvent& event)
{
	CEvent::Event()->SetShowHTML(event.IsChecked());
}

/*
 * m_CheckSplitsClick
 */
void SI_SimpleFrm::m_CheckSplitsClick(wxCommandEvent& event)
{
	CEvent::Event()->SetShowSplits(event.IsChecked());
}

/*
 * m_EventNameUpdated
 */
void SI_SimpleFrm::m_EventNameUpdated(wxCommandEvent& event)
{
	CEvent::Event()->SetEventName(event.GetString());
}

/*
 * OnSaveClick
 */
void SI_SimpleFrm::OnSaveClick(wxCommandEvent& event)
{
	CEvent* oevent = CEvent::Event();
	wxString filter;
	if (oevent->GetShowHTML())
	   filter = "HTML files (*.htm;*.html)|*.htm;*.html";
	else
	   filter = "Text files (*.txt)|*.txt";
	   
	wxFileDialog dlg(this, "Save results to", oevent->Directory(), wxEmptyString, filter,
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() == wxID_OK)
        CEvent::Event()->SaveResults(dlg.GetPath());
}

/*
 * OnSaveUpdateUI
 */
void SI_SimpleFrm::OnSaveUpdateUI(wxUpdateUIEvent& event)
{
	// insert your code here
}
