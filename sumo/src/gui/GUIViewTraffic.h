/****************************************************************************/
/// @file    GUIViewTraffic.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A view on the simulation; this view is a microscopic one
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIViewTraffic_h
#define GUIViewTraffic_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include "GUISUMOViewParent.h"
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class GUISUMOViewParent;
class GUIVehicle;
class GUILaneWrapper;
class MSRoute;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIViewTraffic
 * Microsocopic view at the simulation
 */
class GUIViewTraffic : public GUISUMOAbstractView {
public:
    /// constructor
    GUIViewTraffic(FXComposite *p, GUIMainWindow &app,
                   GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis,
                   FXGLCanvas *share);
    /// destructor
    virtual ~GUIViewTraffic();

    /// builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow &);

    void startTrack(int id);
    void stopTrack();
    int getTrackedID() const;

    void setColorScheme(const std::string &name);


    /** @brief Shows a vehicle's route(s)
     * @param[in] v The vehicle to show routes for
     * @param[in] index The index of the route to show (-1: "all routes")
     * @see GUISUMOAbstractView::showRoute
     */
    void showRoute(GUIVehicle * v, int index=-1) throw();

    ///
    void showBestLanes(GUIVehicle *v);

    /** @brief Stops showing a vehicle's routes
     * @param[in] v The vehicle to stop showing routes for
     * @param[in] index The index of the route to hide (-1: "all routes")
     * @see GUISUMOAbstractView::hideRoute
     */
    void hideRoute(GUIVehicle * v, int index=-1) throw();

    ///
    void hideBestLanes(GUIVehicle *v);

    void showViewschemeEditor();



    /** @brief Returns the information whether the given route of the given vehicle is shown
     * @param[in] v The vehicle which route may be shown
     * @param[in] index The index of the route (-1: "all routes")
     * @return Whether the route with the given index is shown
     * @see GUISUMOAbstractView::amShowingRouteFor
     */
    bool amShowingRouteFor(GUIVehicle * v, int index=-1) throw();

    /// Returns the information whether the route of the given vehicle is shown
    bool amShowingBestLanesFor(GUIVehicle *v);


protected:
    int doPaintGL(int mode, SUMOReal scale);

    void drawRoute(const VehicleOps &vo, int routeNo, SUMOReal darken);
    void drawBestLanes(const VehicleOps &vo);

    void draw(const MSRoute &r);

protected:
    int myTrackedID;

    GUINet *myNet;

protected:
    GUIViewTraffic() { }

};


#endif

/****************************************************************************/

