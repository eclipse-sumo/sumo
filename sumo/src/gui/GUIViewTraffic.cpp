//---------------------------------------------------------------------------//
//                        GUIViewTraffic.cpp -
//  A view on the simulation; this view is a microscopic one
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
// Revision 1.29  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.28  2004/02/16 13:56:27  dkrajzew
// renamed some buttons and toolitips
//
// Revision 1.27  2004/02/05 16:29:31  dkrajzew
// memory leaks removed
//
// Revision 1.26  2003/12/09 11:23:07  dkrajzew
// some memory leaks removed
//
// Revision 1.25  2003/11/12 14:07:46  dkrajzew
// clean up after recent changes
//
// Revision 1.24  2003/11/11 08:40:03  dkrajzew
// consequent position2D instead of two doubles implemented
//
// Revision 1.23  2003/10/15 11:37:50  dkrajzew
// old row-drawer replaced by new ones; setting of name information seems to
//  be necessary
//
// Revision 1.22  2003/10/02 14:55:56  dkrajzew
// visualisation of E2-detectors implemented
//
// Revision 1.21  2003/09/23 14:25:13  dkrajzew
// possibility to visualise detectors using different geometry complexities
//  added
//
// Revision 1.20  2003/09/05 14:55:11  dkrajzew
// lighter drawer implementations
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
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include "GUISUMOViewParent.h"
#include "drawerimpl/GUIVehicleDrawer_FGwTasTriangle.h"
#include "drawerimpl/GUIVehicleDrawer_FGnTasTriangle.h"
#include "drawerimpl/GUIVehicleDrawer_SGwTasTriangle.h"
#include "drawerimpl/GUIVehicleDrawer_SGnTasTriangle.h"
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
#include "GUIPerspectiveChanger.h"
#include "GUIViewTraffic.h"
#include "GUIApplicationWindow.h"
#include "GUIAppEnum.h"
#include <utils/foxtools/MFXCheckableButton.h>
#include "icons/GUIIcons.h"
#include "icons/GUIIconSubSys.h"

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
FXDEFMAP(GUIViewTraffic) GUIViewTrafficMap[]={
    FXMAPFUNC(SEL_COMMAND,  MID_COLOURVEHICLES, GUIViewTraffic::onCmdColourVehicles),
    FXMAPFUNC(SEL_COMMAND,  MID_COLOURLANES,    GUIViewTraffic::onCmdColourLanes),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWTOOLTIPS,   GUIViewTraffic::onCmdShowToolTips),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWGRID,       GUIViewTraffic::onCmdShowGrid),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWFULLGEOM,   GUIViewTraffic::onCmdShowFullGeom),
};

FXIMPLEMENT(GUIViewTraffic,GUISUMOAbstractView,GUIViewTrafficMap,ARRAYNUMBER(GUIViewTrafficMap))


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIViewTraffic::GUIViewTraffic(FXComposite *p,
                               GUIApplicationWindow &app,
                               GUISUMOViewParent *parent,
                               GUINet &net, FXGLVisual *glVis)
    : GUISUMOAbstractView(p, app, parent, net, glVis),
    _vehicleColScheme(VCS_BY_SPEED), _laneColScheme(LCS_BLACK),
    myTrackedID(-1), myUseFullGeom(true),
    _edges2Show(0), _junctions2Show(0), _detectors2Show(0)
{
    init(net);
}


GUIViewTraffic::GUIViewTraffic(FXComposite *p,
                               GUIApplicationWindow &app,
                               GUISUMOViewParent *parent,
                               GUINet &net, FXGLVisual *glVis,
                               FXGLCanvas *share)
    : GUISUMOAbstractView(p, app, parent, net, glVis, share),
    _vehicleColScheme(VCS_BY_SPEED), _laneColScheme(LCS_BLACK),
    myTrackedID(-1), myUseFullGeom(true),
    _edges2Show(0), _junctions2Show(0), _detectors2Show(0)
{
    init(net);
}


void
GUIViewTraffic::init(GUINet &net)
{
    // build the artifact-instances-to-draw - tables
    _edges2ShowSize = (MSEdge::dictSize()>>5) + 1;
    _edges2Show = new size_t[_edges2ShowSize];
    clearUsetable(_edges2Show, _edges2ShowSize);
    _junctions2ShowSize = (MSJunction::dictSize()>>5) + 1;
    _junctions2Show = new size_t[_junctions2ShowSize];
    clearUsetable(_junctions2Show, _junctions2ShowSize);
    _detectors2ShowSize = (net.getDetectorWrapperNo()>>5) + 1;
    _detectors2Show = new size_t[_detectors2ShowSize];
    clearUsetable(_detectors2Show, _detectors2ShowSize);
    // build the drawers
    myVehicleDrawer[0] =
        new GUIVehicleDrawer_SGnTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[1] =
        new GUIVehicleDrawer_SGwTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[2] =
        new GUIVehicleDrawer_FGnTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[3] =
        new GUIVehicleDrawer_FGwTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[4] =
        new GUIVehicleDrawer_SGnTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[5] =
        new GUIVehicleDrawer_SGwTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[6] =
        new GUIVehicleDrawer_FGnTasTriangle(_net->myEdgeWrapper);
    myVehicleDrawer[7] =
        new GUIVehicleDrawer_FGwTasTriangle(_net->myEdgeWrapper);
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
    myDetectorDrawer[0] = new GUIDetectorDrawer_SGnT(_net->myDetectorWrapper);
    myDetectorDrawer[1] = new GUIDetectorDrawer_SGwT(_net->myDetectorWrapper);
    myDetectorDrawer[2] = new GUIDetectorDrawer_FGnT(_net->myDetectorWrapper);
    myDetectorDrawer[3] = new GUIDetectorDrawer_FGwT(_net->myDetectorWrapper);
    myDetectorDrawer[4] = new GUIDetectorDrawer_SGnT(_net->myDetectorWrapper);
    myDetectorDrawer[5] = new GUIDetectorDrawer_SGwT(_net->myDetectorWrapper);
    myDetectorDrawer[6] = new GUIDetectorDrawer_FGnT(_net->myDetectorWrapper);
    myDetectorDrawer[7] = new GUIDetectorDrawer_FGwT(_net->myDetectorWrapper);
    myROWDrawer[0] = new GUIROWDrawer_SGnT(_net->myEdgeWrapper);
    myROWDrawer[1] = new GUIROWDrawer_SGwT(_net->myEdgeWrapper);
    myROWDrawer[2] = new GUIROWDrawer_FGnT(_net->myEdgeWrapper);
    myROWDrawer[3] = new GUIROWDrawer_FGwT(_net->myEdgeWrapper);
    myROWDrawer[4] = new GUIROWDrawer_SGnT(_net->myEdgeWrapper);
    myROWDrawer[5] = new GUIROWDrawer_SGwT(_net->myEdgeWrapper);
    myROWDrawer[6] = new GUIROWDrawer_FGnT(_net->myEdgeWrapper);
    myROWDrawer[7] = new GUIROWDrawer_FGwT(_net->myEdgeWrapper);
}


GUIViewTraffic::~GUIViewTraffic()
{
    for(size_t i=0; i<8; i++) {
        delete myVehicleDrawer[i];
        delete myLaneDrawer[i];
        delete myJunctionDrawer[i];
        delete myDetectorDrawer[i];
        delete myROWDrawer[i];
    }
    delete _edges2Show;
    delete _junctions2Show;
    delete _detectors2Show;
}


void
GUIViewTraffic::buildViewToolBars(GUISUMOViewParent &v)
{
    FXToolBar &toolbar = v.getToolBar(*this);
    new FXToolBarGrip(&toolbar,NULL,0,TOOLBARGRIP_SEPARATOR);
    // build coloring tools
        // vehicle colors
    new FXButton(&toolbar,
        "\tChange Vehicle Coloring\tAllows you to change the vehicle coloring Scheme.",
        GUIIconSubSys::getIcon(ICON_COLOURVEHICLES), this, 0,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|LAYOUT_TOP|LAYOUT_LEFT);
    myVehicleColoring =
        new FXComboBox(&toolbar, 12, this, MID_COLOURVEHICLES,
            FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP);
    myVehicleColoring->appendItem("by speed");
    myVehicleColoring->appendItem("specified");
    myVehicleColoring->appendItem("random#1");
    myVehicleColoring->appendItem("random#2");
    myVehicleColoring->appendItem("lanechange#1");
    myVehicleColoring->appendItem("lanechange#2");
    myVehicleColoring->appendItem("lanechange#3");
    myVehicleColoring->appendItem("waiting#1");
    myVehicleColoring->setNumVisible(8);
        // lane colors
    new FXButton(&toolbar,
        "\tChange Lane Coloring\tAllows you to change the lane coloring Scheme.",
        GUIIconSubSys::getIcon(ICON_COLOURLANES), this, 0,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|LAYOUT_TOP|LAYOUT_LEFT);
    myLaneColoring =
        new FXComboBox(&toolbar, 10, this, MID_COLOURLANES,
            FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP);
    myLaneColoring->appendItem("black");
    myLaneColoring->appendItem("by purpose");
    myLaneColoring->appendItem("by speed");
    myLaneColoring->appendItem("by selection");
    myLaneColoring->setNumVisible(4);

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
        // for vehicles
    new FXButton(&toolbar,
        "\tLocate Vehicle\tLocate a Vehicle within the Network.",
        GUIIconSubSys::getIcon(ICON_LOCATEVEHICLE), &v, MID_LOCATEVEHICLE,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);

    new FXToolBarGrip(&toolbar,NULL,0,TOOLBARGRIP_SEPARATOR);

    // add toggle button for grid on/off
    new MFXCheckableButton(false,
        &toolbar,
        "\tToggles Net Grid\tToggle whether the Grid shall be visualised.",
        GUIIconSubSys::getIcon(ICON_SHOWGRID), this, MID_SHOWGRID,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // add toggle button for tool-tips on/off
    new MFXCheckableButton(false,
        &toolbar,
        "\tToggles Tool Tips\tToggle whether Tool Tips shall be shown.",
        GUIIconSubSys::getIcon(ICON_SHOWTOOLTIPS), this, MID_SHOWTOOLTIPS,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // add toggle button for full geometry-tips on/off
    new MFXCheckableButton(true,
        &toolbar,
        "\tToggles Geometry\tToggle whether full or simple Geometry shall be used.",
        GUIIconSubSys::getIcon(ICON_SHOWFULLGEOM), this, MID_SHOWFULLGEOM,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
}


long
GUIViewTraffic::onCmdColourVehicles(FXObject*,FXSelector,void*)
{
    int index = myVehicleColoring->getCurrentItem();
    // set the vehicle coloring scheme in dependec to
    //  the chosen item
    switch(index) {
    case 0:
        _vehicleColScheme = VCS_BY_SPEED;
        break;
    case 1:
        _vehicleColScheme = VCS_SPECIFIED;
        break;
    case 2:
        _vehicleColScheme = VCS_RANDOM1;
        break;
    case 3:
        _vehicleColScheme = VCS_RANDOM2;
        break;
    case 4:
        _vehicleColScheme = VCS_LANECHANGE1;
        break;
    case 5:
        _vehicleColScheme = VCS_LANECHANGE2;
        break;
    case 6:
        _vehicleColScheme = VCS_LANECHANGE3;
        break;
    case 7:
        _vehicleColScheme = VCS_WAITING1;
        break;
    default:
        _vehicleColScheme = VCS_BY_SPEED;
        break;
    }
    update();
    return 1;
}


long
GUIViewTraffic::onCmdColourLanes(FXObject*,FXSelector,void*)
{
    int index = myLaneColoring->getCurrentItem();
    // set the lane coloring scheme in dependec to
    //  the chosen item
    switch(index) {
    case 0:
        _laneColScheme = LCS_BLACK;
        break;
    case 1:
        _laneColScheme = LCS_BY_PURPOSE;
        break;
    case 2:
        _laneColScheme = LCS_BY_SPEED;
        break;
    case 3:
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
GUIViewTraffic::onCmdShowFullGeom(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    myUseFullGeom = button->amChecked();
    update();
    return 1;
}


void
GUIViewTraffic::doPaintGL(int mode, double scale)
{
    // init view settings
    glRenderMode(mode);
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    // get the viewport settings
	const Boundery &nb = _net->getBoundery();
    double x = (nb.getCenter().x() - _changer->getXPos()); // center of view
    double xoff = 50.0 / _changer->getZoom() * _netScale
        / _addScl; // offset to right
    double y = (nb.getCenter().y() - _changer->getYPos()); // center of view
    double yoff = 50.0 / _changer->getZoom() * _netScale
        / _addScl; // offset to top
    // reset the tables of things to show if the viewport has changed
    if(myViewSettings.differ(x, y, xoff, yoff)) {
        clearUsetable(_edges2Show, _edges2ShowSize);
        clearUsetable(_junctions2Show, _junctions2ShowSize);
        _net->_grid.get(GLO_LANE|GLO_JUNCTION|GLO_DETECTOR, x, y, xoff, yoff,
            _edges2Show, _junctions2Show, _detectors2Show, 0);
        myViewSettings.set(x, y, xoff, yoff);
    }
    // compute lane width
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
    myDetectorDrawer[drawerToUse]->drawGLDetectors(_detectors2Show,
        _detectors2ShowSize, scale);
    myROWDrawer[drawerToUse]->drawGLROWs(*_net,
        _edges2Show, _edges2ShowSize, width);

/*
	Position2DVector tmp;
	tmp.push_front(Position2D(nb.getCenter().first/2.0, nb.ymin()));
	tmp.push_front(Position2D(nb.xmax(), nb.getCenter().second/2.0));
	tmp.push_front(Position2D(nb.getCenter().first/2.0, nb.ymax()));
	tmp.push_front(Position2D(nb.xmin(), nb.getCenter().second/2.0));
	tmp.push_front(Position2D(nb.getCenter().first/2.0, nb.ymin()));
	tmp.push_front(Position2D(nb.xmax(), nb.getCenter().second/2.0));
	tmp.push_front(Position2D(nb.getCenter().first/2.0, nb.ymax()));

	drawPolygon(tmp, 20, true);
*/
    // draw vehicles only when they're visible
    if(scale*m2p(3)>1) {
        myVehicleDrawer[drawerToUse]->drawGLVehicles(_edges2Show,
            _edges2ShowSize, _vehicleColScheme);
    }
    glPopMatrix();
}


/*
void
GUIViewTraffic::drawPolygon(const Position2DVector &v, const Position2D &center, bool close)
{
	glBegin(GL_POLYGON);
	const Position2DVector::ContType &l = v.getCont();
	for(Position2DVector::ContType ::const_iterator i=l.begin(); i!=l.end(); i++) {
		const Position2D &p = *i;
		glVertex2f(p.x(), p.y());
	}
	glEnd();
}
*/


void
GUIViewTraffic::drawPolygon(const Position2DVector &v, double lineWidth, bool close)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLE_STRIP);
	const Position2DVector::ContType &l = v.getCont();
    Position2DVector::ContType ::const_iterator i = l.begin();
    Position2D p1 = *i++;
    Position2D p2 = *i++;
    size_t pos = 0;
	for(; i!=l.end()-1; i++) {
        Position2D p3 = *i;
        double x1, x2, y1, y2;
        if(pos!=0) {
            double alpha1 = atan2(p1.x()-p2.x(), p1.y()-p2.y());
            double alpha2 = atan2(p2.x()-p3.x(), p2.y()-p3.y());
            x1 = p1.x()-cos(alpha1)*lineWidth+cos((alpha1+alpha2)/2.0)*lineWidth;
            y1 = p1.y()+sin(alpha1)*lineWidth-sin((alpha1+alpha2)/2.0)*lineWidth;
            x2 = p1.x()+cos(alpha1)*lineWidth-cos((alpha1+alpha2)/2.0)*lineWidth;
            y2 = p1.y()-sin(alpha1)*lineWidth+sin((alpha1+alpha2)/2.0)*lineWidth;
        } else {
            double alpha = atan2(p1.x()-p2.x(), p1.y()-p2.y());
            x1 = p1.x()-cos(alpha)*lineWidth;
            y1 = p1.y()+sin(alpha)*lineWidth;
            x2 = p1.x()+cos(alpha)*lineWidth;
            y2 = p1.y()-sin(alpha)*lineWidth;
        }
        glColor3f(
            pos==0||pos==3 ? 0 : 1,
            pos==1||pos==4 ? 0 : 1,
            pos==2||pos==5 ? 0 : 1);
        pos++;
		glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        p1 = p2;
        p2 = p3;
	}
	glEnd();
}


RGBColor
GUIViewTraffic::getEdgeColor(GUIEdge *edge) const
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
GUIViewTraffic::track(int id)
{
    myTrackedID = id;
}


void
GUIViewTraffic::doInit()
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


