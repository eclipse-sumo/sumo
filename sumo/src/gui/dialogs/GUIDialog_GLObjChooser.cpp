//---------------------------------------------------------------------------//
//                        GUIDialog_GLObjChooser.cpp -
//  Class for the window that allows to choose a street, junction or vehicle
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2004/03/19 12:33:36  dkrajzew
// porting to FOX
//
// Revision 1.1  2004/03/19 12:32:26  dkrajzew
// porting to FOX
//
// Revision 1.5  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.4  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.3  2003/03/12 16:55:16  dkrajzew
// centering of objects debugged
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <gui/GUISUMOViewParent.h>
#include <gui/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include "GUIDialog_GLObjChooser.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIDialog_GLObjChooser) GUIDialog_GLObjChooserMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_OK,      GUIDialog_GLObjChooser::onCmdOK),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,  GUIDialog_GLObjChooser::onCmdCancel),
};

FXIMPLEMENT(GUIDialog_GLObjChooser, FXMainWindow, GUIDialog_GLObjChooserMap, ARRAYNUMBER(GUIDialog_GLObjChooserMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_GLObjChooser::GUIDialog_GLObjChooser(
        GUISUMOViewParent *parent, GUIGlObjectType type,
        std::vector<std::string> &names)
    : FXMainWindow(gFXApp, "choose", NULL, NULL, DECOR_ALL, 0, 0, 300, 300),
    myObjectType(type), myParent(parent)
{
    FXHorizontalFrame *hbox =
        new FXHorizontalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,
        0,0,0,0);
    // build the list
    myList = new FXList(hbox, 0, 0,
        LAYOUT_FILL_X|LAYOUT_FILL_Y|LIST_SINGLESELECT);
    for(std::vector<std::string>::iterator i=names.begin(); i!=names.end(); i++) {
        myList->appendItem((*i).c_str());
    }
    // build the layout
    FXVerticalFrame *layout = new FXVerticalFrame( hbox, LAYOUT_TOP,0,0,0,0,
        0,0,0,0);
    // build the "OK"-button
    new FXButton(layout, "OK\t\t", 0, this, MID_OK,
        LAYOUT_FIX_WIDTH|LAYOUT_CENTER_X|JUSTIFY_CENTER_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 50, 30);
    new FXButton(layout, "Cancel\t\t", 0, this, MID_CANCEL,
        LAYOUT_FIX_WIDTH|LAYOUT_CENTER_X|JUSTIFY_CENTER_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 50, 30);
}


GUIDialog_GLObjChooser::~GUIDialog_GLObjChooser()
{
}


long
GUIDialog_GLObjChooser::onCmdOK(FXObject*,FXSelector,void*)
{
    int selected = myList->getCurrentItem();
    if(selected>=0) {
        mySelectedID = string(myList->getItemText(selected).text());
    } else {
        mySelectedID = "";
    }
    close(true);
    return 1;
}


long
GUIDialog_GLObjChooser::onCmdCancel(FXObject*,FXSelector,void*)
{
    mySelectedID = "";
    close(true);
    return 1;
}


FXbool
GUIDialog_GLObjChooser::close(FXbool notify)
{
    if(mySelectedID.length()!=0) {
        myParent->setView(myObjectType, mySelectedID);
    }
    return FXMainWindow::close(notify);
}


std::string
GUIDialog_GLObjChooser::getID() const
{
    return mySelectedID;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


