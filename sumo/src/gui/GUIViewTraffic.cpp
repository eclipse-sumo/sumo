/****************************************************************************/
/// @file    GUIViewTraffic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A view on the simulation; this view is a microscopic one
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
#include <microsim/MSJunctionControl.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include "GUISUMOViewParent.h"
#include "GUIViewTraffic.h"
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <foreign/rtree/SUMORTree.h>
#include <utils/gui/div/GLHelper.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUIViewTraffic::GUIViewTraffic(FXComposite *p,
                               GUIMainWindow &app,
                               GUISUMOViewParent *parent,
                               GUINet &net, FXGLVisual *glVis,
                               FXGLCanvas *share)
        : GUISUMOAbstractView(p, app, parent, *net.myGrid, glVis, share),
        myTrackedID(-1) {
}


GUIViewTraffic::~GUIViewTraffic() {
}


void
GUIViewTraffic::buildViewToolBars(GUIGlChildWindow &v) {
    // build coloring tools
    {
        const std::vector<std::string> &names = gSchemeStorage.getNames();
        for (std::vector<std::string>::const_iterator i=names.begin(); i!=names.end(); ++i) {
            v.getColoringSchemesCombo().appendItem((*i).c_str());
            if ((*i) == myVisualizationSettings->name) {
                v.getColoringSchemesCombo().setCurrentItem(v.getColoringSchemesCombo().getNumItems()-1);
            }
        }
        v.getColoringSchemesCombo().setNumVisible(5);
    }
    // for junctions
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Junction\tLocate a junction within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEJUNCTION), &v, MID_LOCATEJUNCTION,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    // for edges
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Street\tLocate a street within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEEDGE), &v, MID_LOCATEEDGE,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    // for vehicles
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Vehicle\tLocate a vehicle within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEVEHICLE), &v, MID_LOCATEVEHICLE,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    // for tls
    new FXButton(v.getLocatorPopup(),
                 "\tLocate TLS\tLocate a tls within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATETLS), &v, MID_LOCATETLS,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    // for additional stuff
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Additional\tLocate an additional structure within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEADD), &v, MID_LOCATEADD,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
    // for shapes
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Shape\tLocate a shape within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATESHAPE), &v, MID_LOCATESHAPE,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
}


void
GUIViewTraffic::setColorScheme(const std::string &name) {
    if (myVisualizationChanger!=0) {
        if (myVisualizationChanger->getCurrentScheme()!=name) {
            myVisualizationChanger->setCurrentScheme(name);
        }
    }
    myVisualizationSettings = &gSchemeStorage.get(name.c_str());
    update();
}


int
GUIViewTraffic::doPaintGL(int mode, SUMOReal scale) {
    // init view settings
    glRenderMode(mode);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    GLdouble sxmin = myCX - myX1;
    GLdouble sxmax = myCX + myX1;
    GLdouble symin = myCY - myY1;
    GLdouble symax = myCY + myY1;

    // compute lane width
    SUMOReal lw = m2p(3.0) * scale;
    // draw decals (if not in grabbing mode)
    if (!myUseToolTips) {
        drawDecals();
        if (myVisualizationSettings->showGrid) {
            paintGLGrid();
        }
    }

    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    float minB[2];
    float maxB[2];
    minB[0] = sxmin;
    minB[1] = symin;
    maxB[0] = sxmax;
    maxB[1] = symax;
    myVisualizationSettings->needsGlID = myUseToolTips;
    myVisualizationSettings->scale = lw;
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_POLYGON_OFFSET_LINE);
    int hits2 = myGrid->Search(minB, maxB, *myVisualizationSettings);
    //
    glTranslated(0, 0, -.01);
    for (std::vector<VehicleOps>::iterator i=myVehicleOps.begin(); i!=myVehicleOps.end(); ++i) {
        const VehicleOps &vo = *i;
        switch (vo.type) {
        case VO_SHOW_ROUTE: {
            if (vo.routeNo>=0) {
                drawRoute(vo, vo.routeNo, 0.25);
            } else {
                if (vo.vehicle->hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)) {
                    int noReroutePlus1 = vo.vehicle->getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE) + 1;
                    for (int i=noReroutePlus1-1; i>=0; i--) {
                        SUMOReal darken = SUMOReal(0.4) / SUMOReal(noReroutePlus1) * SUMOReal(i);
                        drawRoute(vo, i, darken);
                    }
                } else {
                    drawRoute(vo, 0, 0.25);
                }
            }
        }
        break;
        case VO_SHOW_BEST_LANES: {
            drawBestLanes(vo);
            break;
        }
        break;
        default:
            break;
        }
    }
    glTranslated(0, 0, .01);
    glPopMatrix();
    /*
    // draw legends
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(1.-.2, 1.-.5, 0.);
    glScaled(.2, .5, 1.);
    GUIColoringSchemesMap<GUILaneWrapper> &sm = GUIViewTraffic::getLaneSchemesMap(); //!!!
    sm.getColorer(myVisualizationSettings->laneEdgeMode)->drawLegend();
    */
    return hits2;
}


void
GUIViewTraffic::startTrack(int id) {
    myTrackedID = id;
}


void
GUIViewTraffic::stopTrack() {
    myTrackedID = -1;
}


int
GUIViewTraffic::getTrackedID() const {
    return myTrackedID;
}


void
GUIViewTraffic::drawRoute(const VehicleOps &vo, int routeNo, SUMOReal darken) {
    if (myUseToolTips) {
        glPushName(vo.vehicle->getGlID());
    }
    myVisualizationSettings->vehicleColorer.setGlColor(*(vo.vehicle));
    GLdouble colors[4];
    glGetDoublev(GL_CURRENT_COLOR, colors);
    colors[0] -= darken;
    if (colors[0]<0) colors[0] = 0;
    colors[1] -= darken;
    if (colors[1]<0) colors[1] = 0;
    colors[2] -= darken;
    if (colors[2]<0) colors[2] = 0;
    colors[3] -= darken;
    if (colors[3]<0) colors[3] = 0;
    glColor3dv(colors);
    draw(vo.vehicle->getRoute(routeNo));
    if (myUseToolTips) {
        glPopName();
    }
}


void
GUIViewTraffic::drawBestLanes(const VehicleOps &vo) {
    if (myUseToolTips) {
        glPushName(vo.vehicle->getGlID());
    }
    const std::vector<MSVehicle::LaneQ> &lanes = vo.vehicle->getBestLanes();
    SUMOReal gmax = -1;
    SUMOReal rmax = -1;
    for (std::vector<MSVehicle::LaneQ>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
        gmax = MAX2((*i).length, gmax);
        rmax = MAX2((*i).occupied, rmax);
    }
    for (std::vector<MSVehicle::LaneQ>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
        const Position2DVector &shape = (*i).lane->getShape();
        SUMOReal g = (*i).length / gmax;
        SUMOReal r = (*i).occupied / rmax;
        glColor3d(r, g, 0);
        GLHelper::drawBoxLines(shape, 0.5);

        Position2DVector s1 = shape;
        s1.move2side((SUMOReal) .1);
        glColor3d(r, 0, 0);
        GLHelper::drawLine(s1);
        s1.move2side((SUMOReal) -.2);
        glColor3d(0, g, 0);
        GLHelper::drawLine(s1);

        glColor3d(r, g, 0);
        Position2D lastPos = shape[-1];
        for (std::vector<MSLane*>::const_iterator j=(*i).joined.begin(); j!=(*i).joined.end(); ++j) {
            const Position2DVector &shape = (*j)->getShape();
            GLHelper::drawLine(lastPos, shape[0]);
            GLHelper::drawBoxLines(shape, (SUMOReal) 0.2);
            lastPos = shape[-1];
        }
    }
    if (myUseToolTips) {
        glPopName();
    }
}


void
GUIViewTraffic::draw(const MSRoute &r) {
    MSRouteIterator i = r.begin();
    for (; i!=r.end(); ++i) {
        const MSEdge *e = *i;
        const GUIEdge *ge = static_cast<const GUIEdge*>(e);
        const GUILaneWrapper &lane = ge->getLaneGeometry((size_t) 0);
        GLHelper::drawBoxLines(lane.getShape(), lane.getShapeRotations(), lane.getShapeLengths(), 1.0);
    }
}


void
GUIViewTraffic::showRoute(GUIVehicle * v, int index) throw() {
    VehicleOps vo;
    vo.vehicle = v;
    vo.type = VO_SHOW_ROUTE;
    vo.routeNo = index;
    myVehicleOps.push_back(vo);
    update();
}


void
GUIViewTraffic::showBestLanes(GUIVehicle *v) {
    VehicleOps vo;
    vo.vehicle = v;
    vo.type = VO_SHOW_BEST_LANES;
    myVehicleOps.push_back(vo);
    update();
}


void
GUIViewTraffic::hideRoute(GUIVehicle * v, int index) throw() {
    std::vector<VehicleOps>::iterator i =
        find_if(myVehicleOps.begin(), myVehicleOps.end(), vehicle_in_ops_finder(v));
    while (i!=myVehicleOps.end()) {
        if ((*i).type==VO_SHOW_ROUTE&&(*i).routeNo==index) {
            i = myVehicleOps.erase(i);
            update();
            return;
        }
        i = find_if(i+1, myVehicleOps.end(), vehicle_in_ops_finder(v));
    }
    update();
}


void
GUIViewTraffic::hideBestLanes(GUIVehicle *v) {
    std::vector<VehicleOps>::iterator i =
        find_if(myVehicleOps.begin(), myVehicleOps.end(), vehicle_in_ops_finder(v));
    while (i!=myVehicleOps.end()) {
        if ((*i).type==VO_SHOW_BEST_LANES) {
            i = myVehicleOps.erase(i);
            update();
            return;
        }
        i = find_if(i+1, myVehicleOps.end(), vehicle_in_ops_finder(v));
    }
    update();
}


bool
GUIViewTraffic::amShowingRouteFor(GUIVehicle * v, int index) throw() {
    std::vector<VehicleOps>::iterator i =
        find_if(myVehicleOps.begin(), myVehicleOps.end(), vehicle_in_ops_finder(v));
    while (i!=myVehicleOps.end()) {
        if ((*i).type==VO_SHOW_ROUTE&&(*i).routeNo==index) {
            return true;
        }
        i = find_if(i+1, myVehicleOps.end(), vehicle_in_ops_finder(v));
    }
    return false;
}


bool
GUIViewTraffic::amShowingBestLanesFor(GUIVehicle *v) {
    std::vector<VehicleOps>::iterator i =
        find_if(myVehicleOps.begin(), myVehicleOps.end(), vehicle_in_ops_finder(v));
    while (i!=myVehicleOps.end()) {
        if ((*i).type==VO_SHOW_BEST_LANES) {
            return true;
        }
        i = find_if(i+1, myVehicleOps.end(), vehicle_in_ops_finder(v));
    }
    return false;
}



void
GUIViewTraffic::showViewschemeEditor() {
    if (myVisualizationChanger==0) {
        myVisualizationChanger =
            new GUIDialog_ViewSettings(
            this, myVisualizationSettings,
            &myDecals, &myDecalsLock);
        myVisualizationChanger->create();
    } else {
        myVisualizationChanger->setCurrent(myVisualizationSettings);
    }
    myVisualizationChanger->show();
}


void
GUIViewTraffic::onGamingClick(Position2D pos) {
    const std::vector<MSTrafficLightLogic*> &logics = MSNet::getInstance()->getTLSControl().getAllLogics();
    MSTrafficLightLogic *minTll = 0;
    SUMOReal minDist = 1e400;
    for (std::vector<MSTrafficLightLogic*>::const_iterator i=logics.begin(); i!=logics.end(); ++i) {
        // get the logic
        MSTrafficLightLogic *tll = (*i);
        // get the links
        const MSTrafficLightLogic::LaneVector &lanes = tll->getLanesAt(0);
        if (lanes.size()>0) {
            const Position2D &endPos = lanes[0]->getShape().getEnd();
            if (endPos.distanceTo(pos) < minDist) {
                minDist = endPos.distanceTo(pos);
                minTll = tll;
            }
        }
    }
    if (minTll != 0) {
        const MSTLLogicControl::TLSLogicVariants &vars = MSNet::getInstance()->getTLSControl().get(minTll->getID());
        const std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
        if (logics.size() > 1) {
            if (minTll->getSubID() == logics[0]->getSubID()) {
                MSNet::getInstance()->getTLSControl().switchTo(minTll->getID(), logics[1]->getSubID());
            } else {
                MSNet::getInstance()->getTLSControl().switchTo(minTll->getID(), logics[0]->getSubID());
            }
            update();
        }
    }
}


/****************************************************************************/
