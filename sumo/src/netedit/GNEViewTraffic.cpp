/****************************************************************************/
/// @file    GNEViewTraffic.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 15 Dec 2004
/// @version $Id$
///
// A view on the simulation; this view is a microscopic one
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

#include <iostream>
#include <utility>
#include <cmath>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSCORN.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include "GNEViewTraffic.h"
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/drawer/GUIColorer_SingleColor.h>
#include <utils/gui/drawer/GUIColorer_LaneBySelection.h>
#include <utils/gui/drawer/GUIColorer_ShadeByFunctionValue.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include "GNEViewParent.h"

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
FXDEFMAP(GNEViewTraffic) GNEViewTrafficMap[]={
            // new Andreas begin
            FXMAPFUNC(SEL_COMMAND,  MID_EDIT_GRAPH,     GNEViewTraffic::onCmdEditGraph),
            FXMAPFUNC(SEL_LEFTBUTTONRELEASE,   0,       GUIViewTraffic::onLeftBtnRelease),
            // new Andreas end

        };

FXIMPLEMENT(GNEViewTraffic,GUIViewTraffic,GNEViewTrafficMap,ARRAYNUMBER(GNEViewTrafficMap))


// ===========================================================================
// member method definitions
// ===========================================================================
GNEViewTraffic::GNEViewTraffic(FXComposite *p,
                               GUIMainWindow &app,
                               GUISUMOViewParent *parent,
                               GUINet &net, FXGLVisual *glVis)
        : GUIViewTraffic(p, app, parent, net, glVis)
{
    par=parent;
}


GNEViewTraffic::GNEViewTraffic(FXComposite *p,
                               GUIMainWindow &app,
                               GUISUMOViewParent *parent,
                               GUINet &net, FXGLVisual *glVis,
                               FXGLCanvas *share)
        : GUIViewTraffic(p, app, parent, net, glVis, share)
{
    par=parent;
}


GNEViewTraffic::~GNEViewTraffic()
{}



long
GNEViewTraffic::onCmdEditGraph(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    _inEditMode = button->amChecked();
    if (_inEditMode) {
        static_cast<GNEViewParent*>(par)->getEditGroupBox()->show();
    } else {
        static_cast<GNEViewParent*>(par)->getEditGroupBox()->hide();
    }
    recalc();
    _widthInPixels = getWidth();
    _heightInPixels = getHeight();
    _changer->otherChange();
    update();
    return 1;
}


void
GNEViewTraffic::buildViewToolBars(GUIGlChildWindow &v)
{
    GUIViewTraffic::buildViewToolBars(v);

    FXToolBar &toolbar = v.getNavigationToolBar(*this);

    new FXToolBarGrip(&toolbar,NULL,0,TOOLBARGRIP_SINGLE);

    new MFXCheckableButton(false,
                           &toolbar,
                           "\tToggle Editor Tool\tToggle Editor Tool Selection.",
                           GUIIconSubSys::getIcon(ICON_EDITGRAPH), this, MID_EDIT_GRAPH,
                           ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
}



long
GNEViewTraffic::onLeftBtnRelease(FXObject*sender,FXSelector selector,void*data)
{
    GUIViewTraffic::onLeftBtnRelease(sender, selector, data);
    FXEvent *e = (FXEvent*) data;
    //new Andreas
    if (/*e->state&&*/_inEditMode) {
        if (makeCurrent()) {
            // initialise the select mode
            unsigned int id = getObjectUnderCursor();
            GUIGlObject *o = 0;
            if (id!=0) {
                o = gIDStorage.getObjectBlocking(id);
            } else {
                o = gNetWrapper;
            }

            if (o!=0) {
                GUIParameterTableWindow *w =
                    o->getParameterWindow(*myApp, *this);
            }
            makeNonCurrent();
        }
    }

    //new Andreas
    return 1;
}



/****************************************************************************/

