/****************************************************************************/
/// @file    GUIDialog_GLObjChooser.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fxkeys.h>
#include <gui/GUISUMOViewParent.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <microsim/MSJunction.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUIVehicleControl.h>
#include "GUIDialog_GLObjChooser.h"
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/windows/GUIAppGlobals.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_GLObjChooser) GUIDialog_GLObjChooserMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_CENTER, GUIDialog_GLObjChooser::onCmdCenter),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,         GUIDialog_GLObjChooser::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MID_CHOOSER_TEXT,   GUIDialog_GLObjChooser::onChgText),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_TEXT,   GUIDialog_GLObjChooser::onCmdText),
    FXMAPFUNC(SEL_KEYPRESS,  MID_CHOOSER_LIST,   GUIDialog_GLObjChooser::onListKeyPress),
};

FXIMPLEMENT(GUIDialog_GLObjChooser, FXMainWindow, GUIDialog_GLObjChooserMap, ARRAYNUMBER(GUIDialog_GLObjChooserMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_GLObjChooser::GUIDialog_GLObjChooser(GUISUMOViewParent *parent,
        FXIcon *icon, const FXString &title,
        GUIGlObjectType type,
        GUIGlObjectStorage &glStorage)
        : FXMainWindow(parent->getApp(), title, icon, NULL, DECOR_ALL, 20,20,300, 300),
        myObjectType(type), myParent(parent), mySelected(0) {
    FXHorizontalFrame *hbox = new FXHorizontalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
    // build the list
    FXVerticalFrame *layout1 = new FXVerticalFrame(hbox, LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP, 0,0,0,0, 4,4,4,4);
    myTextEntry = new FXTextField(layout1, 0, this, MID_CHOOSER_TEXT, LAYOUT_FILL_X|FRAME_THICK|FRAME_SUNKEN);
    FXVerticalFrame *style1 = new FXVerticalFrame(layout1, LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|FRAME_THICK|FRAME_SUNKEN, 0,0,0,0, 0,0,0,0);
    myList = new FXList(style1, this, MID_CHOOSER_LIST, LAYOUT_FILL_X|LAYOUT_FILL_Y|LIST_SINGLESELECT|FRAME_SUNKEN|FRAME_THICK);
    // get the ids
    std::vector<GLuint> ids;
    switch (type) {
    case GLO_JUNCTION:
        ids = static_cast<GUINet*>(GUINet::getInstance())->getJunctionIDs();
        break;
    case GLO_EDGE:
        ids = GUIEdge::getIDs();
        break;
    case GLO_VEHICLE:
        static_cast<GUIVehicleControl&>(MSNet::getInstance()->getVehicleControl()).insertVehicleIDs(ids);
        break;
    case GLO_TLLOGIC:
        ids = static_cast<GUINet*>(GUINet::getInstance())->getTLSIDs();
        break;
    case GLO_ADDITIONAL:
        ids = GUIGlObject_AbstractAdd::getIDList();
        break;
    case GLO_SHAPE:
        ids = static_cast<GUINet*>(GUINet::getInstance())->getShapeIDs();
        break;
    default:
        break;
    }
    for (std::vector<GLuint>::iterator i=ids.begin(); i!=ids.end(); ++i) {
        GUIGlObject *o = glStorage.getObjectBlocking(*i);
        if (o==0) {
            continue;
        }
        const std::string &name = o->getMicrosimID();
        bool selected = false;
        if (type==GLO_EDGE) {
            size_t noLanes = static_cast<GUIEdge*>(o)->getLanes().size();
            for (size_t j=0; j<noLanes; ++j) {
                const GUILaneWrapper &l = static_cast<GUIEdge*>(o)->getLaneGeometry(j);
                if (gSelected.isSelected(GLO_LANE, l.getGlID())) {
                    selected = true;
                }
            }
        } else {
            selected = gSelected.isSelected(type, *i);
        }
        if (selected) {
            myList->appendItem(name.c_str(), GUIIconSubSys::getIcon(ICON_FLAG), (void*) o);
        } else {
            myList->appendItem(name.c_str(), 0, (void*) o);
        }
        glStorage.unblockObject(*i);
    }
    // build the buttons
    FXVerticalFrame *layout = new FXVerticalFrame(hbox, LAYOUT_TOP, 0,0,0,0, 4,4,4,4);
    new FXButton(layout, "Center\t\t", GUIIconSubSys::getIcon(ICON_RECENTERVIEW),
                 this, MID_CHOOSER_CENTER, ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 4, 4);
    new FXHorizontalSeparator(layout,SEPARATOR_GROOVE|LAYOUT_FILL_X);
    new FXButton(layout, "Close\t\t", GUIIconSubSys::getIcon(ICON_NO),
                 this, MID_CANCEL, ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 4, 4);

    myParent->getParent()->addChild(this);
    myTextEntry->setFocus();
}


GUIDialog_GLObjChooser::~GUIDialog_GLObjChooser() {
    myParent->getParent()->removeChild(this);
}


long
GUIDialog_GLObjChooser::onCmdCenter(FXObject*,FXSelector,void*) {
    int selected = myList->getCurrentItem();
    if (selected>=0) {
        mySelected = static_cast<GUIGlObject*>(myList->getItemData(selected));
    } else {
        mySelected = 0;
    }
    myParent->setView(mySelected);
    return 1;
}


long
GUIDialog_GLObjChooser::onCmdClose(FXObject*,FXSelector,void*) {
    close(true);
    return 1;
}


long
GUIDialog_GLObjChooser::onChgText(FXObject*,FXSelector,void*) {
    int id = myList->findItem(myTextEntry->getText(), -1, SEARCH_PREFIX);
    if (id<0) {
        return 1;
    }
    myList->deselectItem(myList->getCurrentItem());
    myList->makeItemVisible(id);
    myList->selectItem(id);
    myList->setCurrentItem(id, true);
    return 1;
}


long
GUIDialog_GLObjChooser::onCmdText(FXObject*,FXSelector,void*) {
    int selected = myList->getCurrentItem();
    if (selected>=0) {
        mySelected = static_cast<GUIGlObject*>(myList->getItemData(selected));
        myParent->setView(mySelected);
    }
    return 1;
}


long
GUIDialog_GLObjChooser::onListKeyPress(FXObject*,FXSelector,void*ptr) {
    FXEvent* event=(FXEvent*)ptr;
    switch (event->code) {
    case KEY_Return: {
        int current = myList->getCurrentItem();
        if (current>=0&&myList->isItemSelected(current)) {
            mySelected = static_cast<GUIGlObject*>(myList->getItemData(current));
            myParent->setView(mySelected);
        }
    }
    }
    return 1;
}



/****************************************************************************/

