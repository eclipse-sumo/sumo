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
// Revision 1.3  2004/07/02 08:08:32  dkrajzew
// global object selection added
//
// Revision 1.2  2004/04/02 10:58:27  dkrajzew
// visualisation whether an item is selected added
//
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
#include <gui/GUIGlObject.h>
#include <gui/GUIGlObjectStorage.h>
#include <gui/icons/GUIIconSubSys.h>
#include <microsim/MSJunction.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUINet.h>
#include "GUIDialog_GLObjChooser.h"
#include <gui/GUIGlobalSelection.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIDialog_GLObjChooser) GUIDialog_GLObjChooserMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_CENTER, GUIDialog_GLObjChooser::onCmdCenter),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,         GUIDialog_GLObjChooser::onCmdCancel),
};

FXIMPLEMENT(GUIDialog_GLObjChooser, FXMainWindow, GUIDialog_GLObjChooserMap, ARRAYNUMBER(GUIDialog_GLObjChooserMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_GLObjChooser::GUIDialog_GLObjChooser(GUISUMOViewParent *parent,
                                               GUIGlObjectType type,
                                               GUIGlObjectStorage &glStorage)
    : FXMainWindow(gFXApp, "Instance Action Chooser", NULL, NULL, DECOR_ALL, 0, 0, 300, 300),
    myObjectType(type), myParent(parent)
{
    FXHorizontalFrame *hbox =
        new FXHorizontalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,
        0,0,0,0);
    // build the list
    myList = new FXList(hbox, 0, 0,
        LAYOUT_FILL_X|LAYOUT_FILL_Y|LIST_SINGLESELECT);
    std::vector<size_t> ids;
        // get the ids
    switch(type) {
    case GLO_JUNCTION:
        ids = static_cast<GUINet*>(GUINet::getInstance())->getJunctionIDs();
        break;
    case GLO_EDGE:
        ids = GUIEdge::getIDs();
        break;
    case GLO_VEHICLE:
        ids = GUIVehicle::getIDs();
        break;
    default:
        break;
    }
    for(std::vector<size_t>::iterator i=ids.begin(); i!=ids.end(); i++) {
        GUIGlObject *o = glStorage.getObjectBlocking(*i);
        if(o==0) {
            continue;
        }
        const std::string &name = o->microsimID();
        bool selected = false;
        if(type==GLO_EDGE) {
            for(int j=static_cast<GUIEdge*>(o)->nLanes()-1; j>=0; j--) {
                const GUILaneWrapper &l =
                    static_cast<GUIEdge*>(o)->getLaneGeometry(j);
                if(gSelected.isSelected(GLO_LANE, l.getGlID())) {
                    selected = true;
                }
            }
        } else {
            selected = gSelected.isSelected(type, *i);
        }
        if(selected) {
            myList->appendItem(name.c_str(), GUIIconSubSys::getIcon(ICON_FLAG));
        } else {
            myList->appendItem(name.c_str());
        }
        glStorage.unblockObject(*i);
    }
    // build the buttons
    FXVerticalFrame *layout = new FXVerticalFrame( hbox, LAYOUT_TOP,0,0,0,0,
        4,4,4,4);
    new FXButton(layout, "Center\t\t",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW),
        this, MID_CHOOSER_CENTER,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 4, 4);

    new FXHorizontalSeparator(layout,SEPARATOR_GROOVE|LAYOUT_FILL_X);
    new FXButton(layout, "Cancel\t\t",
        GUIIconSubSys::getIcon(ICON_NO),
        this, MID_CANCEL,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 4, 4);
}


GUIDialog_GLObjChooser::~GUIDialog_GLObjChooser()
{
}


long
GUIDialog_GLObjChooser::onCmdCenter(FXObject*,FXSelector,void*)
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
        myParent->setView(myObjectType, mySelectedID, "");
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


