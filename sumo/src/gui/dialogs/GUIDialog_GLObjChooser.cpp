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
// Revision 1.9  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/07/12 11:55:37  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
// Revision 1.6  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.5  2004/11/23 10:00:08  dkrajzew
// new class hierarchy for windows applied
//
// Revision 1.4  2004/08/02 11:28:57  dkrajzew
// ported to fox 1.2
//
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <gui/GUISUMOViewParent.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <microsim/MSJunction.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUINet.h>
#include "GUIDialog_GLObjChooser.h"
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/windows/GUIAppGlobals.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,         GUIDialog_GLObjChooser::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MID_CHOOSER_TEXT,   GUIDialog_GLObjChooser::onCmdTextChanged),

};

FXIMPLEMENT(GUIDialog_GLObjChooser, FXMainWindow, GUIDialog_GLObjChooserMap, ARRAYNUMBER(GUIDialog_GLObjChooserMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_GLObjChooser::GUIDialog_GLObjChooser(GUISUMOViewParent *parent,
                                               GUIGlObjectType type,
                                               GUIGlObjectStorage &glStorage)
    : FXMainWindow(gFXApp, "Instance Action Chooser", NULL, NULL, DECOR_ALL, 20,20,300, 300),
    myObjectType(type), myParent(parent), mySelected(0)
{
    FXHorizontalFrame *hbox =
        new FXHorizontalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,
        0,0,0,0);
    // build the list
    FXVerticalFrame *layout1 = new FXVerticalFrame( hbox,
        LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP,0,0,0,0,  4,4,4,4);
    myTextEntry =
        new FXTextField(layout1, 0, this, MID_CHOOSER_TEXT,
            LAYOUT_FILL_X|FRAME_THICK|FRAME_SUNKEN);
    FXVerticalFrame *style1 =
        new FXVerticalFrame( layout1,
            LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|FRAME_THICK|FRAME_SUNKEN,
            0,0,0,0,  0, 0, 0, 0);
    myList =
        new FXList(style1, 0, 0,
            LAYOUT_FILL_X|LAYOUT_FILL_Y|LIST_SINGLESELECT|FRAME_SUNKEN|FRAME_THICK);
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
    case GLO_ADDITIONAL:
        ids = GUIGlObject_AbstractAdd::getIDList();
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
            myList->appendItem(name.c_str(), GUIIconSubSys::getIcon(ICON_FLAG), (void*) o);
        } else {
            myList->appendItem(name.c_str(), 0, (void*) o);
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
    new FXButton(layout, "Close\t\t",
        GUIIconSubSys::getIcon(ICON_NO),
        this, MID_CANCEL,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 4, 4);

    setIcon( GUIIconSubSys::getIcon(ICON_APP_FINDER) );
}


GUIDialog_GLObjChooser::~GUIDialog_GLObjChooser()
{
}


long
GUIDialog_GLObjChooser::onCmdCenter(FXObject*,FXSelector,void*)
{
    int selected = myList->getCurrentItem();
    if(selected>=0) {
        mySelected = static_cast<GUIGlObject*>(myList->getItemData(selected));
    } else {
        mySelected = 0;
    }
    myParent->setView(mySelected);
    return 1;
}


long
GUIDialog_GLObjChooser::onCmdClose(FXObject*,FXSelector,void*)
{
    close(true);
    return 1;
}


long
GUIDialog_GLObjChooser::onCmdTextChanged(FXObject*,FXSelector,void*)
{
    int id = myList->findItem(myTextEntry->getText(), -1, SEARCH_PREFIX);
    if(id<0) {
        return 1;
    }
    myList->deselectItem(myList->getCurrentItem());
    myList->makeItemVisible(id);
    myList->selectItem(id);
    myList->setCurrentItem(id, true);
    return 1;
}


GUIGlObject *
GUIDialog_GLObjChooser::getObject() const
{
    return static_cast<GUIGlObject*>(mySelected);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


