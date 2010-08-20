//---------------------------------------------------------------------------
//
// Name:        SI SimpleApp.h
// Author:      Andy Simpson
// Created:     13/02/2010 4:39:55 PM
// Description: 
//
//---------------------------------------------------------------------------

#ifndef __SI_SIMPLEFRMApp_h__
#define __SI_SIMPLEFRMApp_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif

class SI_SimpleFrmApp : public wxApp
{
	public:
		bool OnInit();
		int OnExit();
};

#endif
