/****************************************************************************/
/// @file    GUIViewMesoEdges.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: GUIViewMesoEdges.cpp 253 2007-07-20 11:31:46Z behr_mi $
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
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include <gui/GUISUMOViewParent.h>
#include "GUIViewMesoEdges.h"
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
//#include "GUIColorer_LaneByPurpose.h"
#include <utils/glutils/polyfonts.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/gui/drawer/GUICompleteSchemeStorage.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>

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
// static member definitions
// ===========================================================================
GUIColoringSchemesMap<GUIVehicle> GUIViewMesoEdges::myVehicleColoringSchemes;
GUIColoringSchemesMap<GUIEdge> GUIViewMesoEdges::myLaneColoringSchemes;


// ===========================================================================
// member method definitions
// ===========================================================================
GUIViewMesoEdges::GUIViewMesoEdges(FXComposite *p,
                                   GUIMainWindow &app,
                                   GUISUMOViewParent *parent,
                                   GUINet &net, FXGLVisual *glVis)
        : GUISUMOAbstractView(p, app, parent, net.myGrid, glVis),
  //      myVehicleDrawer(net.myEdgeWrapper), 
        myLaneDrawer(net.myEdgeWrapper),
        myJunctionDrawer(net.myJunctionWrapper),
        myDetectorDrawer(GUIGlObject_AbstractAdd::getObjectList()),
//        myROWDrawer(net.myEdgeWrapper),
        myTrackedID(-1),
        _edges2Show(0), _junctions2Show(0), _additional2Show(0), _pointToMove(0),_IdToMove(0),
        _leftButtonPressed(false), _net(&net)
{
    init(net);
}


GUIViewMesoEdges::GUIViewMesoEdges(FXComposite *p,
                                   GUIMainWindow &app,
                                   GUISUMOViewParent *parent,
                                   GUINet &net, FXGLVisual *glVis,
                                   FXGLCanvas *share)
        : GUISUMOAbstractView(p, app, parent, net.myGrid, glVis, share),
    //    myVehicleDrawer(net.myEdgeWrapper), 
        myLaneDrawer(net.myEdgeWrapper),
        myJunctionDrawer(net.myJunctionWrapper),
        myDetectorDrawer(GUIGlObject_AbstractAdd::getObjectList()),
      //  myROWDrawer(net.myEdgeWrapper),
        myTrackedID(-1),
        _edges2Show(0), _junctions2Show(0), _additional2Show(0), _pointToMove(0),
        _net(&net)
{
    init(net);
}


void
GUIViewMesoEdges::init(GUINet &net)
{
    // build the artifact-instances-to-draw - tables
    _edges2ShowSize = (MSEdge::dictSize()>>5) + 1;
    _edges2Show = new size_t[_edges2ShowSize];
    clearUsetable(_edges2Show, _edges2ShowSize);
    _junctions2ShowSize = (MSJunction::dictSize()>>5) + 1;
    _junctions2Show = new size_t[_junctions2ShowSize];
    clearUsetable(_junctions2Show, _junctions2ShowSize);
    _additional2ShowSize = (GUIGlObject_AbstractAdd::getObjectList().size()>>5) + 1;
    _additional2Show = new size_t[_additional2ShowSize];
    clearUsetable(_additional2Show, _additional2ShowSize);
    // initialise default scheme
    myVisualizationSettings = &gSchemeStorage.get(gSchemeStorage.getNames()[0]);
}


GUIViewMesoEdges::~GUIViewMesoEdges()
{
    delete[] _edges2Show;
    delete[] _junctions2Show;
    delete[] _additional2Show;
}


void
GUIViewMesoEdges::create()
{
    FXGLCanvas::create();
}


void
GUIViewMesoEdges::buildViewToolBars(GUIGlChildWindow &v)
{
    // build coloring tools
    {
        const std::vector<std::string> &names = gSchemeStorage.getNames();
        for (std::vector<std::string>::const_iterator i=names.begin(); i!=names.end(); ++i) {
            v.getColoringSchemesCombo().appendItem((*i).c_str());
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
    // for additional stuff
    new FXButton(v.getLocatorPopup(),
                 "\tLocate Additional\tLocate an additional structure within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEADD), &v, MID_LOCATEADD,
                 ICON_ABOVE_TEXT|FRAME_THICK|FRAME_RAISED);
}


void
GUIViewMesoEdges::setColorScheme(char* data)
{
    string name = (char*) data;
    if(myVisualizationChanger!=0) {
        if(myVisualizationChanger->getCurrentScheme()!=name) {
            myVisualizationChanger->setCurrentScheme(name);
        }
    }
    myVisualizationSettings = &gSchemeStorage.get(data);
    // lanes
    switch (myLaneColoringSchemes.getColorSetType(myVisualizationSettings->laneEdgeMode)) {
    case CST_SINGLE:
        myLaneColoringSchemes.getColorerInterface(myVisualizationSettings->laneEdgeMode)->resetColor(
            myVisualizationSettings->laneColorings[myVisualizationSettings->laneEdgeMode][0]);
        break;
    case CST_MINMAX:
        myLaneColoringSchemes.getColorerInterface(myVisualizationSettings->laneEdgeMode)->resetColor(
            myVisualizationSettings->laneColorings[myVisualizationSettings->laneEdgeMode][0],
            myVisualizationSettings->laneColorings[myVisualizationSettings->laneEdgeMode][1]);
        break;
    default:
        break;
    }
    // vehicles
    switch (myVehicleColoringSchemes.getColorSetType(myVisualizationSettings->vehicleMode)) {
    case CST_SINGLE:
        myVehicleColoringSchemes.getColorerInterface(myVisualizationSettings->vehicleMode)->resetColor(
            myVisualizationSettings->vehicleColorings[myVisualizationSettings->vehicleMode][0]);
        break;
    case CST_MINMAX:
        myVehicleColoringSchemes.getColorerInterface(myVisualizationSettings->vehicleMode)->resetColor(
            myVisualizationSettings->vehicleColorings[myVisualizationSettings->vehicleMode][0],
            myVisualizationSettings->vehicleColorings[myVisualizationSettings->vehicleMode][1]);
        break;
    case CST_MINMAX_OPT:
        myVehicleColoringSchemes.getColorerInterface(myVisualizationSettings->vehicleMode)->resetColor(
            myVisualizationSettings->vehicleColorings[myVisualizationSettings->vehicleMode][0],
            myVisualizationSettings->vehicleColorings[myVisualizationSettings->vehicleMode][1],
            myVisualizationSettings->vehicleColorings[myVisualizationSettings->vehicleMode][2]);
        break;
    default:
        break;
    }
    update();
}


void
GUIViewMesoEdges::doPaintGL(int mode, SUMOReal scale)
{
    if (myTrackedID>0) {
        GUIGlObject *o = gIDStorage.getObjectBlocking(myTrackedID);
        if (o!=0) {
            Boundary b;
            b.add(static_cast<GUIVehicle*>(o)->getPosition());
            b.grow(20);
            myChanger->centerTo(myGrid->getBoundary(), b, false);
        }
    }
    // init view settings
    glRenderMode(mode);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    // get the viewport settings
    const Boundary &nb = _net->getBoundary();
    {
        SUMOReal width = nb.getWidth();
        SUMOReal height = nb.getHeight();
        SUMOReal mzoom = myChanger->getZoom();
        SUMOReal cy = myChanger->getYPos();//cursorY;
        SUMOReal cx = myChanger->getXPos();//cursorY;

        // compute the visible area in horizontal direction
        SUMOReal mratioX = 1;
        SUMOReal mratioY = 1;
        SUMOReal xs = ((SUMOReal) myWidthInPixels / (SUMOReal) myApp->getMaxGLWidth())
                      / (myGrid->getBoundary().getWidth() / myNetScale) * myRatio;
        SUMOReal ys = ((SUMOReal) myHeightInPixels / (SUMOReal) myApp->getMaxGLHeight())
                      / (myGrid->getBoundary().getHeight() / myNetScale);
        if (xs<ys) {
            mratioX = 1;
            mratioY = ys/xs;
        } else {
            mratioY = 1;
            mratioX = xs/ys;
        }
        SUMOReal sxmin = nb.getCenter().x()
                         - mratioX * width * (SUMOReal) 100 / (mzoom) / (SUMOReal) 2. / (SUMOReal) .97;
        sxmin -= cx;
        SUMOReal sxmax = nb.getCenter().x()
                         + mratioX * width * (SUMOReal) 100 / (mzoom) / (SUMOReal) 2. / (SUMOReal) .97;
        sxmax -= cx;

        // compute the visible area in vertical direction
        SUMOReal symin = nb.getCenter().y()
                         - mratioY * height / mzoom * (SUMOReal) 100 / (SUMOReal) 2. / (SUMOReal) .97;
        symin -= cy;
        SUMOReal symax = nb.getCenter().y()
                         + mratioY * height / mzoom * (SUMOReal) 100 / (SUMOReal) 2. / (SUMOReal) .97;
        symax -= cy;

        // reset the tables of things to show if the viewport has changed
        if (myViewportSettings.differ(sxmin, symin, sxmax, symax)) {
            clearUsetable(_edges2Show, _edges2ShowSize);
            clearUsetable(_junctions2Show, _junctions2ShowSize);
            _net->myGrid.get(GLO_LANE|GLO_JUNCTION|GLO_DETECTOR,
                            sxmin, symin, sxmax, symax,
                            _edges2Show, _junctions2Show, _additional2Show);
            myViewportSettings.set(sxmin, symin, sxmax, symax);
        }
    }

    // compute lane width
    SUMOReal width = m2p(3.0) * scale;
    // compute which drawer shall be used
    if (myUseToolTips) {
//        myVehicleDrawer.setGLID(true);
        myLaneDrawer.setGLID(true);
        myDetectorDrawer.setGLID(true);
//        myROWDrawer.setGLID(true);
        myJunctionDrawer.setGLID(true);
    } else {
//        myVehicleDrawer.setGLID(false);
        myLaneDrawer.setGLID(false);
        myDetectorDrawer.setGLID(true);
//        myROWDrawer.setGLID(true);
        myJunctionDrawer.setGLID(true);
    }
    // draw
    {
        myDecalsLock.lock();
        for (std::vector<GUISUMOAbstractView::Decal>::iterator l=myDecals.begin(); l!=myDecals.end(); ++l) {
            GUISUMOAbstractView::Decal &d = *l;
            if (!d.initialised) {
                FXImage *i = MFXImageHelper::loadimage(getApp(), d.filename);
                if (i!=0) {
                    d.glID = GUITexturesHelper::add(i);
                    d.initialised = true;
                }
            }
            glPushMatrix();
            glTranslated(d.left, d.top, 0);
            glRotated(d.rot, 0, 0, 1);
            glColor3d(1,1,1);
            GUITexturesHelper::drawTexturedBox(d.glID, 0, 0, d.right-d.left, d.bottom-d.top);
            glPopMatrix();
        }
        myDecalsLock.unlock();
    }
    drawShapes(_net->getShapeContainer(), 0, width);

    myJunctionDrawer.drawGLJunctions(_junctions2Show, _junctions2ShowSize,
                                     _junctionColScheme, *myVisualizationSettings);
    myLaneDrawer.drawGLLanes(_edges2Show, _edges2ShowSize, width,
                             *myLaneColoringSchemes.getColorer(myVisualizationSettings->laneEdgeMode),
                             *myVisualizationSettings);
    if (myVisualizationSettings->drawJunctionName) {
        myJunctionDrawer.drawGLJunctionNames(_junctions2Show, _junctions2ShowSize,
                                             width, _junctionColScheme, *myVisualizationSettings);
    }
    myDetectorDrawer.drawGLDetectors(_additional2Show, _additional2ShowSize,
                                     width, *myVisualizationSettings);
/*    myROWDrawer.drawGLROWs(*_net, _edges2Show, _edges2ShowSize, width,
                           *myVisualizationSettings);*/
    if (myVisualizationSettings->drawEdgeName) {
        myLaneDrawer.drawGLLaneNames(_edges2Show, _edges2ShowSize, width,
                                     *myVisualizationSettings);
    }
    //
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
                        SUMOReal darken =
                            (SUMOReal) 0.4
                            / (SUMOReal) noReroutePlus1
                            * (SUMOReal) i;
                        drawRoute(vo, i, darken);
                    }
                } else {
                    drawRoute(vo, 0, 0.25);
                }
            }
        }
        break;
        default:
            break;
        }
    }
    // draw the Polygons
    drawShapes(_net->getShapeContainer(), 10, width);
    // draw vehicles only when they're visible
    /*
    if(scale*m2p(3)>1) {
        myVehicleDrawer[drawerToUse]->drawGLVehicles(_edges2Show,
            _edges2ShowSize, _vehicleColScheme);
    }
    */
    glPopMatrix();
}

RGBColor
GUIViewMesoEdges::getEdgeColor(GUIEdge *edge) const
{
    switch (edge->getPurpose()) {
    case GUIEdge::EDGEFUNCTION_NORMAL:
        return RGBColor((SUMOReal) 0, (SUMOReal) 0, (SUMOReal) 0);
    case GUIEdge::EDGEFUNCTION_SOURCE:
        return RGBColor((SUMOReal) 0, (SUMOReal) 0.2, (SUMOReal) 0);
    case GUIEdge::EDGEFUNCTION_SINK:
        return RGBColor((SUMOReal) 0.2, (SUMOReal) 0, (SUMOReal) 0);
    case GUIEdge::EDGEFUNCTION_INTERNAL:
        return RGBColor((SUMOReal) 0, (SUMOReal) 0, (SUMOReal) 0.2);
    default:
        break;
    }
    // hmmmm - should not happen
    return RGBColor((SUMOReal) 1, (SUMOReal) 1, (SUMOReal) 0);
}


void
GUIViewMesoEdges::doInit()
{}


void
GUIViewMesoEdges::drawRoute(const VehicleOps &vo, int routeNo, SUMOReal darken)
{
    if (myUseToolTips) {
        glPushName(vo.vehicle->getGlID());
    }
    myVehicleColoringSchemes.getColorer(myVisualizationSettings->vehicleMode)->setGlColor(*(vo.vehicle));
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
GUIViewMesoEdges::draw(const MSRoute &r)
{
    MSRouteIterator i = r.begin();
    for (; i!=r.end(); ++i) {
        const MSEdge *e = *i;
        const GUIEdge *ge = static_cast<const GUIEdge*>(e);
        const GUILaneWrapper &lane = ge->getLaneGeometry((size_t) 0);
        GLHelper::drawBoxLines(lane.getShape(), lane.getShapeRotations(), lane.getShapeLengths(), 1.0);
    }
}


void
GUIViewMesoEdges::showRoute(GUIVehicle *v, int index)
{
    VehicleOps vo;
    vo.vehicle = v;
    vo.type = VO_SHOW_ROUTE;
    vo.routeNo = index;
    myVehicleOps.push_back(vo);
    update();
}


void
GUIViewMesoEdges::hideRoute(GUIVehicle *v, int index)
{
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


bool
GUIViewMesoEdges::amShowingRouteFor(GUIVehicle *v, int index)
{
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


void
GUIViewMesoEdges::showViewschemeEditor()
{
    if (myVisualizationChanger==0) {
        myVisualizationChanger =
            new GUIDialog_ViewSettings(
                this, myVisualizationSettings,
                &myLaneColoringSchemes, 0,
                &myDecals, &myDecalsLock);
        myVisualizationChanger->create();
    }
    myVisualizationChanger->show();
}


long
GUIViewMesoEdges::onLeftBtnPress(FXObject *o,FXSelector sel,void *data)
{/*
    FXEvent *e = (FXEvent*) data;
    _leftButtonPressed=true;
    if (e->state&CAPSLOCKMASK) {
        if (makeCurrent()) {
            unsigned int id = getObjectUnderCursor();
            if (id!=0) {
                GUIGlObject *o = gIDStorage.getObjectBlocking(id);
                std::string n= o->getFullName();
                std::string name = n.substr(n.find(":")+1,n.length());
                GUIPointOfInterest *p= static_cast<GUIPointOfInterest *>(_net->getShapeContainer().getPOICont(1).get(name));
                setFirstPoint(p);
            } else {
                FXMessageBox::error(myApp,MBOX_OK,"Error Get Point","No Point at the Position","");
            }
            makeNonCurrent();
        }
    }
    if ((e->state&SHIFTMASK)!=0) {
        // try to get the object-id if so
        if (makeCurrent()) {
            unsigned int id = getObjectUnderCursor();
            setIdToMove(id);
            makeNonCurrent();
            if (id!=0) {
                GUIGlObject *o = gIDStorage.getObjectBlocking(id);
                std::string n= o->getFullName();
                std::string name = n.substr(n.find(":")+1,n.length());
                GUIPointOfInterest *p= static_cast<GUIPointOfInterest *>
                                       (_net->getShapeContainer().getPOICont(1).get(name));
                setPointToMove(p);
            } else {
                Position2D point = getPositionInformation();
                std::string Id= toString(point.x()) +  "," + toString(point.y());
                GUIPointOfInterest *p = new GUIPointOfInterest(gIDStorage, Id, "point",
                                        Position2D(point.x(), point.y()),RGBColor(0,0,0));
                _net->getShapeContainer().add(1, p);
                update();
            }
        }
        return 1;
    }
    */
    return GUISUMOAbstractView::onLeftBtnPress(o, sel, data);
}


void
GUIViewMesoEdges::setPointToMove(PointOfInterest *p)
{
    _pointToMove = p;
}


void
GUIViewMesoEdges::setIdToMove(unsigned int id)
{
    _IdToMove = id;
}



long
GUIViewMesoEdges::onLeftBtnRelease(FXObject *o,FXSelector sel,void *data)
{
    long ret = GUISUMOAbstractView::onLeftBtnRelease(o, sel, data);
    /*
    FXEvent *e = (FXEvent*) data;
    if (_leftButtonPressed && _firstPoint!=0 && e->state&CAPSLOCKMASK) {
        if (makeCurrent()) {
            unsigned int id = getObjectUnderCursor();
            if (id==-1) {
                GUIGlObject *o = gIDStorage.getObjectBlocking(id);
                std::string n= o->getFullName();
                std::string name = n.substr(n.find(":")+1,n.length());
                GUIPointOfInterest *p= static_cast<GUIPointOfInterest *>(_net->getShapeContainer().getPOICont(1).get(name));
                setSecondPoint(p);
//!!!				_net->getShapeContainer().addPair(_firstPoint,_secondPoint);
                update();
            } else {
                Position2D point = getPositionInformation();
                std::string Id= toString(point.x())+toString(point.y());
                GUIPointOfInterest *p =
                    new GUIPointOfInterest(gIDStorage, 1, Id, "point", point, RGBColor(0,0,0));
                _net->getShapeContainer().add(1, p);
                setSecondPoint(p);
//!!!				_net->getShapeContainer().addPair(_firstPoint,_secondPoint);
                SUMOReal dist = (SUMOReal)(sqrt(pow((_firstPoint->x() - _secondPoint->x()),2) + pow((_firstPoint->y() - _secondPoint->y()),2)));
                update();
            }
            _firstPoint=0;
            makeNonCurrent();
        }

    } else {
        delete _popup;
        _popup = 0;
        ungrab();
    }
    _leftButtonPressed=false;
    */
    return ret;
}


long
GUIViewMesoEdges::onMouseMove(FXObject *o,FXSelector sel,void *data)
{
    /*
    SUMOReal xpos = myChanger->getXPos();
    SUMOReal ypos = myChanger->getYPos();
    SUMOReal zoom = myChanger->getZoom();
    */
    /*
    FXEvent *e=(FXEvent*)data;

    if (_pointToMove!=0 && e->state&SHIFTMASK) {
        // Keep Color Informations
        Position2D point = getPositionInformation(e->win_x, e->win_y);
        _pointToMove->set(point.x(), point.y());
        if (!_leftButtonPressed) {
            _pointToMove=0;
        }
        updatePositionInformation();
        update();
    } else {
        if (_firstPoint!=0 && e->state&CAPSLOCKMASK && _leftButtonPressed) {
            //do Nothing
            return 1;
        }
    }
    */
    return GUISUMOAbstractView::onMouseMove(o, sel, data);
}



GUIColoringSchemesMap<GUIEdge> &
GUIViewMesoEdges::getLaneSchemesMap()
{
    return myLaneColoringSchemes;
}


/****************************************************************************/

