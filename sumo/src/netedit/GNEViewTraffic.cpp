//---------------------------------------------------------------------------//
//                        GNEViewTraffic.cpp -
//  A view on the simulation; this view is a microscopic one
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 15 Dec 2004
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
// Revision 1.4  2005/05/04 08:37:26  dkrajzew
// ported to fox1.4
//
// Revision 1.3  2005/01/31 09:27:35  dkrajzew
// added the possibility to save nodes and edges or the build network to netedit
//
// Revision 1.2  2005/01/05 23:07:04  miguelliebe
// debugging
//
// Revision 1.1  2004/12/15 09:20:19  dkrajzew
// made guisim independent of giant/netedit
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utility>
#include <cmath>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSCORN.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Polygon2D.h>
#include <utils/gui/drawer/GUILaneDrawer_SGwT.h>
#include <utils/gui/drawer/GUILaneDrawer_SGnT.h>
#include <utils/gui/drawer/GUILaneDrawer_FGwT.h>
#include <utils/gui/drawer/GUILaneDrawer_FGnT.h>
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
#include <utils/gui/div/GUIExcp_VehicleIsInvisible.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include "GNEViewParent.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GNEViewTraffic) GNEViewTrafficMap[]={
// new Andreas begin
    FXMAPFUNC(SEL_COMMAND,  MID_EDIT_GRAPH,     GNEViewTraffic::onCmdEditGraph),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,   0,       GUIViewTraffic::onLeftBtnRelease),
// new Andreas end

};

FXIMPLEMENT(GNEViewTraffic,GUIViewTraffic,GNEViewTrafficMap,ARRAYNUMBER(GNEViewTrafficMap))


/* =========================================================================
 * member method definitions
 * ======================================================================= */
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
{
}



long
GNEViewTraffic::onCmdEditGraph(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    _inEditMode = button->amChecked();
    if(_inEditMode) {
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

    FXToolBar &toolbar = v.getToolBar(*this);

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
    if(/*e->state&&*/_inEditMode){
        _lock.lock();
        if(makeCurrent())
    {
        // initialise the select mode
        unsigned int id = getObjectUnderCursor();
        GUIGlObject *o = 0;
        if(id!=0) {
            o = gIDStorage.getObjectBlocking(id);
        } else {
            o = gNetWrapper;
        }

        if(o!=0) {
            GUIParameterTableWindow *w =
            o->getParameterWindow(*myApp, *this);
        }
        makeNonCurrent();
    }
    _lock.unlock();
    }

    //new Andreas
    return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


