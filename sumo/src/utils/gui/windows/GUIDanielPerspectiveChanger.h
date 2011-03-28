/****************************************************************************/
/// @file    GUIDanielPerspectiveChanger.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class that allows to steer the visual output in dependence to
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIDanielPerspectiveChanger_h
#define GUIDanielPerspectiveChanger_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/geom/Position2D.h>
#include "GUIPerspectiveChanger.h"


// ===========================================================================
// class declarations
// ===========================================================================
class Boundary;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDanielPerspectiveChanger
 * This changer has the following behaviour:
 * - zooming by pressing the right mouse button and moving the
 *  mouse vertically
 * - rotation by pressing the right mouse button and moving the
 *  mouse horizontally
 * - network movement by pressing the left mouse button and
 *  moving the mouse
 */
class GUIDanielPerspectiveChanger :
            public GUIPerspectiveChanger {
public:
    /// Constructor
    GUIDanielPerspectiveChanger(GUISUMOAbstractView &callBack);

    /// Destructor
    ~GUIDanielPerspectiveChanger();

    void onLeftBtnPress(void *data);
    bool onLeftBtnRelease(void *data);
    void onRightBtnPress(void *data);
    bool onRightBtnRelease(void *data);
    void onMouseWheel(void *data);
    void onMouseMove(void *data);

    /// Returns the rotation of the canvas stored in this changer
    virtual SUMOReal getRotation() const;

    /// Returns the x-offset of the field to show stored in this changer
    virtual SUMOReal getXPos() const;

    /// Returns the y-offset of the field to show stored in this changer
    virtual SUMOReal getYPos() const;

    /// Returns the zoom factor computed stored in this changer
    virtual SUMOReal getZoom() const;

    /// recenters the view to display the whole network
    void recenterView();

    /// Centers the view to the given position, setting it to a size that covers the radius
    void centerTo(const Boundary &netBoundary,
                  const Position2D &pos, SUMOReal radius, bool applyZoom=true);

    /// Centers the view to show the given boundary
    void centerTo(const Boundary &netBoundary,
                  Boundary bound, bool applyZoom=true);

    /** @brief Sets the viewport */
    void setViewport(SUMOReal zoom, SUMOReal xPos, SUMOReal yPos);


    /* @brief Adapts the viewport so that a change in canvass size keeps most of the
     * view intact (by showing more / less instead of zooming)
     * The canvass is clipped/enlarged on the left side of the screen
     *
     * @param[in] width The original width of the canvas in pixels
     * @param[in] height The original height of the canvas in pixels
     * @param[in] change The horizontal change in canvas size in pixels
     */
    void changeCanvassLeft(int width, int height, int change);

private:
    /* Performs the view movement
     * @param[in] xdiff the change to myViewCenter in pixel
     * @param[in] ydiff the change to myViewCenter in pixel
     */
    void move(int xdiff, int ydiff);

    /// Performs the zooming of the view
    void zoom(int diff);

    /// Performs the rotation of the view
    void rotate(int diff);

private:
    /// @brief The inverted offset to the center of the network in meter
    Position2D myViewCenter;

    /// the scale of the net (the maximum size, either width or height)
    SUMOReal myNetScale;

    /// the current rotation
    SUMOReal myRotation;

    /// the current zoom factor
    SUMOReal myZoom;

    /// the current mouse state
    int myMouseButtonState;

    /// Information whether the user has moved the cursor while pressing a mouse button
    bool myMoveOnClick;

};


#endif

/****************************************************************************/

