/***************************************************************
 * Name:      reversi_guiApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Rick (1300012743@pku.edu.cn)
 * Created:   2016-04-16
 * Copyright: Rick ()
 * License:
 **************************************************************/

#include "reversi_guiApp.h"

//(*AppHeaders
#include "reversi_guiMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(reversi_guiApp);

bool reversi_guiApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    reversi_guiFrame* Frame = new reversi_guiFrame(0);
    Frame->Show();
    SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
