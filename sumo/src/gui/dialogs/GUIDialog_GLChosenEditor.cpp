//---------------------------------------------------------------------------//
//                        GUIDialog_GLChosenEditor.cpp -
//  Editor for the  list of chosen objects
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 11.03.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIGlObject.h>
#include <guisim/GUINet.h>
#include "GUIDialog_GLChosenEditor.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIDialog_GLChosenEditor) GUIDialog_GLChosenEditorMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_OK,      GUIDialog_GLChosenEditor::onCmdOK),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,  GUIDialog_GLChosenEditor::onCmdCancel),
};

FXIMPLEMENT(GUIDialog_GLChosenEditor, FXMainWindow, GUIDialog_GLChosenEditorMap, ARRAYNUMBER(GUIDialog_GLChosenEditorMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_GLChosenEditor::GUIDialog_GLChosenEditor(GUIApplicationWindow *parent)
    : FXMainWindow(gFXApp, "choose", NULL, NULL, DECOR_ALL, 0, 0, 300, 300),
    myParent(parent)
{
    FXHorizontalFrame *hbox =
        new FXHorizontalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,
        0,0,0,0);
    // build the list
    myList = new FXList(hbox, 0, 0,
        LAYOUT_FILL_X|LAYOUT_FILL_Y|LIST_SINGLESELECT);
    for(std::vector<size_t>::iterator i=gChosenObjects.begin(); i!=gChosenObjects.end(); i++) {
        GUIGlObject *object = gSimInfo->net.getIDStorage().getObjectBlocking(*i);
        if(object!=0) {
            std::string name = object->getFullName();
            myList->appendItem(name.c_str());
            gSimInfo->net.getIDStorage().unblockObject(*i);
        }
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


GUIDialog_GLChosenEditor::~GUIDialog_GLChosenEditor()
{
}


long
GUIDialog_GLChosenEditor::onCmdOK(FXObject*,FXSelector,void*)
{
    close(true);
    return 1;
}


long
GUIDialog_GLChosenEditor::onCmdCancel(FXObject*,FXSelector,void*)
{
    close(true);
    return 1;
}


FXbool
GUIDialog_GLChosenEditor::close(FXbool notify)
{
    return FXMainWindow::close(notify);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


