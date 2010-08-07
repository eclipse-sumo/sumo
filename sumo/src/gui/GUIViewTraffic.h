/****************************************************************************/
/// @file    GUIViewTraffic.h
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


    /** @brief Starts vehicle tracking
     * @param[in] id The glID of the vehicle to track
     */
    void startTrack(int id);


    /** @brief Stops vehicle tracking
     */
    void stopTrack();


    /** @brief Returns the id of the tracked vehicle (-1 if none)
     * @return The glID of the vehicle to track
     */
    int getTrackedID() const;

    void setColorScheme(const std::string &name);


    void showViewschemeEditor();



    void onGamingClick(Position2D pos);

protected:
    int doPaintGL(int mode, SUMOReal scale);


private:
    int myTrackedID;

protected:
    GUIViewTraffic() { }

};


#endif

/****************************************************************************/

