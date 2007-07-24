/****************************************************************************/
/// @file    GUIViewMesoEdges.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: GUIViewMesoEdges.h 96 2007-06-06 07:40:46Z behr_mi $
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
#ifndef GUIViewMesoEdges_h
#define GUIViewMesoEdges_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_MESOSIM

#include <string>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include <gui/GUISUMOViewParent.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
//#include "drawerimpl/GUIVehicleDrawer.h"
#include <gui/drawerimpl/GUIDetectorDrawer.h>
#include "GUIBaseEdgeDrawer.h"
#include <gui/drawerimpl/GUIJunctionDrawer.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class GUINet;
class GUISUMOViewParent;
class GUIVehicle;
class GUILaneWrapper;
class GUIEdge;
class GUIPerspectiveChanger;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIViewMesoEdges
 * Microsocopic view at the simulation
 */
class GUIViewMesoEdges : public GUISUMOAbstractView
{
public:
    /// constructor
    GUIViewMesoEdges(FXComposite *p, GUIMainWindow &app,
                     GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis);

    /// constructor
    GUIViewMesoEdges(FXComposite *p, GUIMainWindow &app,
                     GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis,
                     FXGLCanvas *share);
    void init(GUINet &net) ;

    void create();

    /// destructor
    virtual ~GUIViewMesoEdges();

    /// builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow &);

    long onLeftBtnPress(FXObject *o,FXSelector sel,void *data);
    long onLeftBtnRelease(FXObject*,FXSelector,void*);
    long onMouseMove(FXObject *o,FXSelector sel,void *data);
    void setColorScheme(char* data);


    /// shows a vehicle route
    void showRoute(GUIVehicle *v, int index=-1);

    /// hides a vehicle route
    void hideRoute(GUIVehicle *v, int index=-1);
    void showViewschemeEditor();

    /// Returns the information whether the route of the given vehicle is shown
    bool amShowingRouteFor(GUIVehicle *v, int index=-1);

    /// Returns the list of available vehicle coloring schemes
    static GUIColoringSchemesMap<GUIVehicle> &getVehiclesSchemesMap();

    /// Returns the list of available lane coloring schemes
    static GUIColoringSchemesMap<GUIEdge> &getLaneSchemesMap();


protected:
    void doPaintGL(int mode, SUMOReal scale);

    void doInit();

    /// returns the color of the edge
    RGBColor getEdgeColor(GUIEdge *edge) const;

    void drawRoute(const VehicleOps &vo, int routeNo, SUMOReal darken);

    void setPointToMove(PointOfInterest *p);
    void setIdToMove(unsigned int id);
    void draw(const MSRoute &r);

protected:
    /** @brief Instances of the vehicle drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not */
    //GUIVehicleDrawer myVehicleDrawer;

    /** @brief Instances of the lane drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not */
    GUIBaseEdgeDrawer<GUIEdge, GUIEdge, GUILaneWrapper> myLaneDrawer;

    /** @brief Instances of the junction drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not */
    GUIJunctionDrawer myJunctionDrawer;

    /** @brief Instances of the detectors drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not */
    GUIDetectorDrawer myDetectorDrawer;

    /** @brief Instances of the right of way drawers
        A drawer is chosen in dependence to whether the full or the simple
        geometry shall be used and whether to show tooltips or not /
    GUIROWDrawer myROWDrawer;
    */

    /// The coloring scheme of junctions to use
    JunctionColoringScheme _junctionColScheme;

    int myTrackedID;

    /** @brief Pointers to tables holding the information which of the items are visible
        All vehicles on visible edges will be drawn */
    size_t *_edges2Show, *_junctions2Show, *_additional2Show;

    /// The absolut numbers of the array sizes
    size_t _edges2ShowSize, _junctions2ShowSize, _additional2ShowSize;



    PointOfInterest *_pointToMove;
    unsigned int _IdToMove;  // for deleting the _pointToMove also into gIDStorage
    bool _leftButtonPressed; // set to true if the left Button is pressed and keep pressed

    GUINet *_net;

    /** @brief The list of vehicle coloring schemes that may be used */
    static GUIColoringSchemesMap<GUIVehicle> myVehicleColoringSchemes;

    /** @brief The list of coloring schemes that may be used */
    static GUIColoringSchemesMap<GUIEdge> myLaneColoringSchemes;


protected:
    std::vector<GUIEdge*> myEmptyEdges;
    std::vector<GUIJunctionWrapper*> myEmptyJunctions;

    GUIViewMesoEdges()
            : myLaneDrawer(myEmptyEdges),
            myJunctionDrawer(myEmptyJunctions),
            myDetectorDrawer(GUIGlObject_AbstractAdd::getObjectList())/*,
            myROWDrawer(myEmptyEdges)*/
    { }

};


#endif
#endif

/****************************************************************************/

