//---------------------------------------------------------------------------//
//                        GUIViewAggregatedLanes.cpp -
//  A view on the simulation; this views is a microscopic one
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
// Revision 1.9  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.8  2004/02/16 13:56:27  dkrajzew
// renamed some buttons and toolitips
//
// Revision 1.7  2003/11/20 13:17:33  dkrajzew
// further work on aggregated views
//
// Revision 1.6  2003/11/12 14:07:46  dkrajzew
// clean up after recent changes
//
// Revision 1.5  2003/11/11 08:40:03  dkrajzew
// consequent position2D instead of two doubles implemented
//
// Revision 1.4  2003/10/30 08:57:53  dkrajzew
// first implementation of aggregated views using E2-detectors
//
// Revision 1.3  2003/10/15 11:37:50  dkrajzew
// old row-drawer replaced by new ones; setting of name information seems to
//  be necessary
//
// Revision 1.2  2003/09/23 14:25:13  dkrajzew
// possibility to visualise detectors using different geometry complexities
//  added
//
// Revision 1.1  2003/09/05 14:45:44  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.19  2003/08/14 13:44:14  dkrajzew
// tls/row - drawer added
//
// Revision 1.18  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.17  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.16  2003/07/16 15:18:23  dkrajzew
// new interfaces for drawing classes; junction drawer interface added
//
// Revision 1.15  2003/06/06 10:33:47  dkrajzew
// changes due to moving the popup-menus into a subfolder
//
// Revision 1.14  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.13  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.12  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged;
// additional parameter of maximum display size added
//
// Revision 1.11  2003/04/14 08:24:57  dkrajzew
// unneeded display switch and zooming option removed;
// new gl-object concept implemented; comments added
//
// Revision 1.10  2003/04/07 10:15:16  dkrajzew
// glut reinserted
//
// Revision 1.9  2003/04/04 15:13:20  roessel
// Commented out #include <glut.h>
// Added #include <qcursor.h>
//
// Revision 1.8  2003/04/04 08:37:51  dkrajzew
// view centering now applies net size; closing problems debugged;
// comments added; tootip button added
//
// Revision 1.7  2003/04/02 11:50:28  dkrajzew
// a working tool tip implemented
//
// Revision 1.6  2003/03/20 16:17:52  dkrajzew
// windows eol removed
//
// Revision 1.5  2003/03/12 16:55:19  dkrajzew
// centering of objects debugged
//
// Revision 1.3  2003/03/03 15:10:20  dkrajzew
// debugging
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

#include <iostream>
#include <utility>
#include <cmath>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUIVehicle.h>
#include "GUIGlobals.h"
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include "GUISUMOViewParent.h"
#include "drawerimpl/GUIJunctionDrawer_nT.h"
#include "drawerimpl/GUIJunctionDrawer_wT.h"
#include "drawerimpl/GUIDetectorDrawer_SGnT.h"
#include "drawerimpl/GUIDetectorDrawer_SGwT.h"
#include "drawerimpl/GUIDetectorDrawer_FGnT.h"
#include "drawerimpl/GUIDetectorDrawer_FGwT.h"
#include "drawerimpl/GUIROWDrawer_SGnT.h"
#include "drawerimpl/GUIROWDrawer_FGnT.h"
#include "drawerimpl/GUIROWDrawer_SGwT.h"
#include "drawerimpl/GUIROWDrawer_FGwT.h"
#include "drawerimpl/GUILaneDrawer_SGwT.h"
#include "drawerimpl/GUILaneDrawer_SGnT.h"
#include "drawerimpl/GUILaneDrawer_FGwT.h"
#include "drawerimpl/GUILaneDrawer_FGnT.h"
#include "GUIDanielPerspectiveChanger.h"
#include "GUIViewAggregatedLanes.h"
#include "GUIApplicationWindow.h"
#include "GUIGlobals.h"
#include "icons/GUIIconSubSys.h"
#include "GUIAppEnum.h"
#include <utils/foxtools/MFXCheckableButton.h>

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
FXDEFMAP(GUIViewAggregatedLanes) GUIViewAggregatedLanesMap[]={
    FXMAPFUNC(SEL_COMMAND,  MID_LANEAGGRMEM,    GUIViewAggregatedLanes::onCmdAggMemory),
    FXMAPFUNC(SEL_COMMAND,  MID_LANEAGGTIME,    GUIViewAggregatedLanes::onCmdAggChoose),
    FXMAPFUNC(SEL_COMMAND,  MID_COLOURLANES,    GUIViewAggregatedLanes::onCmdColourLanes),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWTOOLTIPS,   GUIViewAggregatedLanes::onCmdShowToolTips),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWGRID,       GUIViewAggregatedLanes::onCmdShowGrid),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWFULLGEOM,   GUIViewAggregatedLanes::onCmdShowFullGeom),
};

FXIMPLEMENT(GUIViewAggregatedLanes,GUISUMOAbstractView,GUIViewAggregatedLanesMap,ARRAYNUMBER(GUIViewAggregatedLanesMap))


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIViewAggregatedLanes::GUIViewAggregatedLanes(FXComposite *p,
                               GUIApplicationWindow &app,
                               GUISUMOViewParent *parent,
                               GUINet &net, FXGLVisual *glVis)
    : GUISUMOAbstractView(p, app, parent, net, glVis),
    _laneColScheme(LCS_BY_DENSITY)
{
    init(net);
}


GUIViewAggregatedLanes::GUIViewAggregatedLanes(FXComposite *p,
                               GUIApplicationWindow &app,
                               GUISUMOViewParent *parent,
                               GUINet &net, FXGLVisual *glVis,
                               FXGLCanvas *share)
    : GUISUMOAbstractView(p, app, parent, net, glVis, share),
    _laneColScheme(LCS_BY_DENSITY)
{
    init(net);
}


void
GUIViewAggregatedLanes::init(GUINet &net)
{
    _edges2ShowSize = (MSEdge::dictSize()>>5) + 1;
    _edges2Show = new size_t[_edges2ShowSize];
    clearUsetable(_edges2Show, _edges2ShowSize);
    _junctions2ShowSize = (MSJunction::dictSize()>>5) + 1;
    _junctions2Show = new size_t[_junctions2ShowSize];
    clearUsetable(_junctions2Show, _junctions2ShowSize);
    _detectors2ShowSize = (net.getDetectorWrapperNo()>>5) + 1;
    _detectors2Show = new size_t[_detectors2ShowSize];
    clearUsetable(_detectors2Show, _detectors2ShowSize);
    myLaneDrawer[0] = new GUILaneDrawer_SGnT(_net->myEdgeWrapper);
    myLaneDrawer[1] = new GUILaneDrawer_SGwT(_net->myEdgeWrapper);
    myLaneDrawer[2] = new GUILaneDrawer_FGnT(_net->myEdgeWrapper);
    myLaneDrawer[3] = new GUILaneDrawer_FGwT(_net->myEdgeWrapper);
    myLaneDrawer[4] = new GUILaneDrawer_SGnT(_net->myEdgeWrapper);
    myLaneDrawer[5] = new GUILaneDrawer_SGwT(_net->myEdgeWrapper);
    myLaneDrawer[6] = new GUILaneDrawer_FGnT(_net->myEdgeWrapper);
    myLaneDrawer[7] = new GUILaneDrawer_FGwT(_net->myEdgeWrapper);
    myJunctionDrawer[0] = new GUIJunctionDrawer_nT(_net->myJunctionWrapper);
    myJunctionDrawer[1] = new GUIJunctionDrawer_wT(_net->myJunctionWrapper);
    myJunctionDrawer[2] = new GUIJunctionDrawer_nT(_net->myJunctionWrapper);
    myJunctionDrawer[3] = new GUIJunctionDrawer_wT(_net->myJunctionWrapper);
    myJunctionDrawer[4] = new GUIJunctionDrawer_nT(_net->myJunctionWrapper);
    myJunctionDrawer[5] = new GUIJunctionDrawer_wT(_net->myJunctionWrapper);
    myJunctionDrawer[6] = new GUIJunctionDrawer_nT(_net->myJunctionWrapper);
    myJunctionDrawer[7] = new GUIJunctionDrawer_wT(_net->myJunctionWrapper);
    myROWDrawer[0] = new GUIROWDrawer_SGnT(_net->myEdgeWrapper);
    myROWDrawer[1] = new GUIROWDrawer_SGwT(_net->myEdgeWrapper);
    myROWDrawer[2] = new GUIROWDrawer_FGnT(_net->myEdgeWrapper);
    myROWDrawer[3] = new GUIROWDrawer_FGwT(_net->myEdgeWrapper);
    myROWDrawer[4] = new GUIROWDrawer_SGnT(_net->myEdgeWrapper);
    myROWDrawer[5] = new GUIROWDrawer_SGwT(_net->myEdgeWrapper);
    myROWDrawer[6] = new GUIROWDrawer_FGnT(_net->myEdgeWrapper);
    myROWDrawer[7] = new GUIROWDrawer_FGwT(_net->myEdgeWrapper);
}

GUIViewAggregatedLanes::~GUIViewAggregatedLanes()
{
}


void
GUIViewAggregatedLanes::buildViewToolBars(GUISUMOViewParent &v)
{
    FXToolBar &toolbar = v.getToolBar(*this);
    new FXToolBarGrip(&toolbar,NULL,0,TOOLBARGRIP_SEPARATOR);
    // build coloring tools
        // lane colors
    new FXButton(&toolbar,"\tChange Lane Coloring\tAllows you to change the Lane coloring Scheme.",
        GUIIconSubSys::getIcon(ICON_COLOURLANES), this, 0,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|LAYOUT_TOP|LAYOUT_LEFT);
    myLaneColoring =
        new FXComboBox(&toolbar, 13, this, MID_COLOURLANES,
            FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP);
    myLaneColoring->appendItem("by density");
    myLaneColoring->appendItem("by mean speed");
//    myLaneColoring->appendItem("by mean halts");
    myLaneColoring->appendItem("black");
    myLaneColoring->appendItem("by purpose");
    myLaneColoring->appendItem("by allowed speed");
    myLaneColoring->appendItem("by selection");
    myLaneColoring->setNumVisible(6);

    myAggregationLength =
        new FXComboBox(&toolbar, 10, this, MID_LANEAGGTIME,
            FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP);
    myAggregationLength->appendItem("exponential");
    myAggregationLength->appendItem("60s");
//    myAggregationLength->appendItem("5min");
//    myAggregationLength->appendItem("15min");
//    myAggregationLength->appendItem("30min");
//    myAggregationLength->appendItem("60min");
    myAggregationLength->setNumVisible(2);
    if(!gAllowAggregatedFloating) {
        myAggregationLength->disable();
    }

    myRememberingFactor =
        new FXRealSpinDial(&toolbar, 6, this, MID_LANEAGGRMEM,
        LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
    myRememberingFactor->setFormatString("%.2f");
    myRememberingFactor->setIncrements(0.01,.10,.10);
    myRememberingFactor->setRange(0.01,0.99);
    myRememberingFactor->setValue(0.5);
    gAggregationRememberingFactor = 0.5;

    new FXToolBarGrip(&toolbar,NULL,0,TOOLBARGRIP_SEPARATOR);

    // build the locator buttons
        // for junctions
    new FXButton(&toolbar,
        "\tLocate Junction\tLocate a Junction within the Network.",
        GUIIconSubSys::getIcon(ICON_LOCATEJUNCTION), &v, MID_LOCATEJUNCTION,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        // for edges
    new FXButton(&toolbar,
        "\tLocate Street\tLocate a Street within the Network.",
        GUIIconSubSys::getIcon(ICON_LOCATEEDGE), &v, MID_LOCATEEDGE,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);

    new FXToolBarGrip(&toolbar,NULL,0,TOOLBARGRIP_SEPARATOR);

    // add toggle button for grid on/off
    new MFXCheckableButton(false,
        &toolbar,
        "\tToggles Net Grid\tToggles whether the Grid shall be visualised.",
        GUIIconSubSys::getIcon(ICON_SHOWGRID), this, MID_SHOWGRID,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // add toggle button for tool-tips on/off
    new MFXCheckableButton(false,
        &toolbar,"\tToggles Tool Tips\tToggles whether Tool Tips shall be shown.",
        GUIIconSubSys::getIcon(ICON_SHOWTOOLTIPS), this, MID_SHOWTOOLTIPS,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // add toggle button for full geometry-tips on/off
    new MFXCheckableButton(true,
        &toolbar,
        "\tToggles Geometry\tToggles whether full or simple Geometry shall be used.",
        GUIIconSubSys::getIcon(ICON_SHOWFULLGEOM), this, MID_SHOWFULLGEOM,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
}


long
GUIViewAggregatedLanes::onCmdColourLanes(FXObject*,FXSelector,void*)
{
    int index = myLaneColoring->getCurrentItem();
    // set the lane coloring scheme in dependec to
    //  the chosen item
    switch(index) {
    case 0:
        _laneColScheme = LCS_BY_DENSITY;
        break;
    case 1:
        _laneColScheme = LCS_BY_MEAN_SPEED;
        break;
    case 2:
        _laneColScheme = LCS_BY_MEAN_HALTS;
        break;
    case 3:
        _laneColScheme = LCS_BLACK;
        break;
    case 4:
        _laneColScheme = LCS_BY_PURPOSE;
        break;
    case 5:
        _laneColScheme = LCS_BY_SPEED;
        break;
    case 6:
        _laneColScheme = LCS_BY_SELECTION;
        break;
    default:
        _laneColScheme = LCS_BLACK;
        break;
    }
    update();
    return 1;
}


long
GUIViewAggregatedLanes::onCmdShowFullGeom(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    myUseFullGeom = button->amChecked();
    update();
    return 1;
}


long
GUIViewAggregatedLanes::onCmdAggMemory(FXObject*,FXSelector,void*)
{
    gAggregationRememberingFactor = myRememberingFactor->getValue();
    return 1;
}


long
GUIViewAggregatedLanes::onCmdAggChoose(FXObject*,FXSelector,void*)
{
    int index = myAggregationLength->getCurrentItem();
    for(size_t i=0; i<8; i++) {
        myLaneDrawer[i]->setUseExponential(index==0);
    }
    return 1;
}


void
GUIViewAggregatedLanes::doPaintGL(int mode, double scale)
{
    glRenderMode(mode);
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

	const Boundery &nb = _net->getBoundery();
    double x = (nb.getCenter().x() - _changer->getXPos()); // center of view
    double xoff = 50.0 / _changer->getZoom() * _netScale
        / _addScl; // offset to right
    double y = (nb.getCenter().y() - _changer->getYPos()); // center of view
    double yoff = 50.0 / _changer->getZoom() * _netScale
        / _addScl; // offset to top
    if(myViewSettings.differ(x, y, xoff, yoff)) {
        clearUsetable(_edges2Show, _edges2ShowSize);
        clearUsetable(_junctions2Show, _junctions2ShowSize);
        _net->_grid.get(GLO_LANE|GLO_JUNCTION|GLO_DETECTOR, x, y, xoff, yoff,
            _edges2Show, _junctions2Show, _detectors2Show, 0);
        myViewSettings.set(x, y, xoff, yoff);
    }
    double width = m2p(3.0) * scale;
    size_t drawerToUse = 0;
    // compute which drawer shall be used
    if(myUseFullGeom) {
        drawerToUse = 2;
    }
    if(_useToolTips) {
        drawerToUse += 1;
    }
    // draw
    myJunctionDrawer[drawerToUse]->drawGLJunctions(_junctions2Show,
        _junctions2ShowSize, _junctionColScheme);
    myLaneDrawer[drawerToUse]->drawGLLanes(_edges2Show, _edges2ShowSize,
        width, _laneColScheme);
    glPopMatrix();
}



RGBColor
GUIViewAggregatedLanes::getEdgeColor(GUIEdge *edge) const
{
    switch(edge->getPurpose()) {
    case GUIEdge::EDGEFUNCTION_NORMAL:
        return RGBColor(0, 0, 0);
    case GUIEdge::EDGEFUNCTION_SOURCE:
        return RGBColor(0, 0.2, 0);
    case GUIEdge::EDGEFUNCTION_SINK:
        return RGBColor(0.2, 0, 0);
    case GUIEdge::EDGEFUNCTION_INTERNAL:
        return RGBColor(0, 0, 0.2);
    default:
        break;
    }
    throw 1;
}


void
GUIViewAggregatedLanes::doInit()
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


