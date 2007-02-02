/****************************************************************************/
/// @file    InfoDialog.h
/// @author  Maik Drozdzynski
/// @date    21.12.2004
/// @version $Id: $
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef InfoDialog_h
#define InfoDialog_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "fx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdlib.h"


#include <iostream>
#include <sstream>
// ===========================================================================
// used namespaces
// ===========================================================================
#include <string>
using namespace std;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class InfoDialog : public FXDialogBox
{
    FXDECLARE(InfoDialog)
protected:
    FXVerticalFrame* contents;
    FXHorizontalFrame* buttonframe;
    FXPopup*           pane;
    FXVerticalFrame*   boxframe1;


public:


// Messages
    enum{
        ID_PANEL=FXMainWindow::ID_LAST,

        ID_OK,
    };

private:
    InfoDialog()
    {}

public:
    InfoDialog(FXWindow* owner);
    virtual ~InfoDialog();

    long onCmdPanel(FXObject*,FXSelector,void* ptr);
    long onCmdOK(FXObject*,FXSelector,void* ptr);






    //void create();
};


#endif

/****************************************************************************/

