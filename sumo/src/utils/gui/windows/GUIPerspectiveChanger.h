/****************************************************************************/
/// @file    GUIPerspectiveChanger.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A virtual class that allows to steer the visual output in dependence to
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
#ifndef GUIPerspectiveChanger_h
#define GUIPerspectiveChanger_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include "GUISUMOAbstractView.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;
class Position2D;
class Boundary;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIPerspectiveChanger
 * This is the interface for implementation of own classes that handle the
 * interaction between the user and a display field.
 * While most of our (IVF) interfaces allow zooming by choosing the rectangle
 * to show, other types of interaction are possible and have been implemented.
 * To differ between the behaviours, all types of interaction between the
 * user and the canvas are send to this class: mouse moving, mouse button
 * pressing and releasing.
 */
class GUIPerspectiveChanger {
public:
    enum MouseState {
        MOUSEBTN_NONE = 0,
        MOUSEBTN_LEFT = 1,
        MOUSEBTN_RIGHT = 2,
        MOUSEBTN_MIDDLE = 4
    };

    /// Constructor
    GUIPerspectiveChanger(GUISUMOAbstractView &callBack);

    /// Destructor
    virtual ~GUIPerspectiveChanger();

    virtual void onLeftBtnPress(void *data);
    virtual bool onLeftBtnRelease(void *data);
    virtual void onRightBtnPress(void *data);
    virtual bool onRightBtnRelease(void *data);
    virtual void onMouseWheel(void *data);
    virtual void onMouseMove(void *data);

    /// Returns the rotation of the canvas stored in this changer
    virtual SUMOReal getRotation() const = 0;

    /// Returns the x-offset of the field to show stored in this changer
    virtual SUMOReal getXPos() const = 0;

    /// Returns the y-offset of the field to show stored in this changer
    virtual SUMOReal getYPos() const = 0;

    /// Returns the zoom factor computed stored in this changer
    virtual SUMOReal getZoom() const = 0;

    /// recenters the view to display the whole network
    virtual void recenterView() = 0;

    /** @brief Centers the view to the given position,
        setting it to a size that covers the radius.
        Used for: Centering of vehicles and junctions */
    virtual void centerTo(const Boundary &netBoundary,
                          const Position2D &pos, SUMOReal radius, bool applyZoom=true) = 0;

    /** @brief Centers the view to show the given boundary
        Used for: Centering of lanes */
    virtual void centerTo(const Boundary &netBoundary,
                          Boundary bound, bool applyZoom=true) = 0;

    /** @brief Sets the viewport
        Used for: Adapting a new viewport */
    virtual void setViewport(SUMOReal zoom, SUMOReal xPos, SUMOReal yPos) = 0;

    /// Returns the last mouse x-position an event occured at
    FXint getMouseXPosition() const;

    /// Returns the last mouse y-position an event occured at
    FXint getMouseYPosition() const;

protected:
    /// The parent window (canvas to scale)
    GUISUMOAbstractView &myCallback;

    /// the current mouse position
    FXint myMouseXPosition, myMouseYPosition;

};


#endif

/****************************************************************************/

