//---------------------------------------------------------------------------
//
// Name:        SI SimpleApp.cpp
// Author:      Andy Simpson
// Created:     13/02/2010 4:39:55 PM
// Description: 
//
//---------------------------------------------------------------------------

#include "SI SimpleApp.h"
#include "SI SimpleFrm.h"

IMPLEMENT_APP(SI_SimpleFrmApp)

bool SI_SimpleFrmApp::OnInit()
{
    SI_SimpleFrm* frame = new SI_SimpleFrm(NULL);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
 
int SI_SimpleFrmApp::OnExit()
{
	return 0;
}
