#ifndef GUIDanielPerspectiveChanger_h
#define GUIDanielPerspectiveChanger_h
//---------------------------------------------------------------------------//
//                        GUIDanielPerspectiveChanger.h -
//  A class that allows to steer the visual output in dependence to
//      user interaction
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
// $Log$
// Revision 1.8  2004/03/19 12:54:07  dkrajzew
// porting to FOX
//
// Revision 1.7  2003/11/11 08:40:03  dkrajzew
// consequent position2D instead of two doubles implemented
//
// Revision 1.6  2003/07/18 12:29:28  dkrajzew
// removed some warnings
//
// Revision 1.5  2003/06/05 11:37:30  dkrajzew
// class templates applied
//
// Revision 1.4  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.3  2003/04/04 08:37:50  dkrajzew
// view centering now applies net size; closing problems debugged; comments added; tootip button added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utils/geom/Position2D.h>
#include "GUIPerspectiveChanger.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class Boundery;


/* =========================================================================
 * class definitions
 * ======================================================================= */
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

    long onLeftBtnPress(FXObject *o,FXSelector sel,void *data);
    long onLeftBtnRelease(FXObject *o,FXSelector sel,void *data);
    long onRightBtnPress(FXObject *o,FXSelector sel,void *data);
    long onRightBtnRelease(FXObject *o,FXSelector sel,void *data);
    long onMouseMove(FXObject *o,FXSelector sel,void *data);

    /// Returns the rotation of the canvas stored in this changer
    virtual double getRotation() const;

    /// Returns the x-offset of the field to show stored in this changer
    virtual double getXPos() const;

    /// Returns the y-offset of the field to show stored in this changer
    virtual double getYPos() const;

    /// Returns the zoom factor computed stored in this changer
    virtual double getZoom() const;

    /// recenters the view to display the whole network
    void recenterView();

    /// Centers the view to the given position, setting it to a size that covers the radius
    void centerTo(const Boundery &netBoundery,
        const Position2D &pos, double radius);

    /// Centers the view to show the given boundery
    void centerTo(const Boundery &netBoundery,
        Boundery bound);

    /// Returns the last mouse x-position an event occured at
    int getMouseXPosition() const;

    /// Returns the last mouse y-position an event occured at
    int getMouseYPosition() const;

private:
    /// Performs the view movement
    void move(int xdiff, int ydiff);

    /// Performs the zooming of the view
    void zoom(int diff);

    /// Performs the rotation of the view
    void rotate(int diff);

private:
    /// the current center of the view
    Position2D myViewCenter;

    /// the sizes of the window
    int _widthInPixels, _heightInPixels;

    /// the current mouse position
    int _mouseXPosition, _mouseYPosition;

    /// the scale of the net (the maximum size, either width or height)
    double _netScale;

    /// the current rotation
    double _rotation;

    /// the current zoom factor
    double _zoom;

    /// the current mouse state
    MouseState _mouseButtonState;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

