#ifndef GUIPerspectiveChanger_h
#define GUIPerspectiveChanger_h
//---------------------------------------------------------------------------//
//                        GUIPerspectiveChanger.h -
//  A class that allows to steer the visual output in dependence to user
//      interaction
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
// Revision 1.3  2003/03/12 16:55:18  dkrajzew
// centering of objects debugged
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

/* =========================================================================
 * class declarations
 * ======================================================================= */
class QMouseEvent;
class GUIViewTraffic;
class Position2D;
class Boundery;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUIPerspectiveChanger {
public:
    GUIPerspectiveChanger(GUIViewTraffic &callBack);
    virtual ~GUIPerspectiveChanger();
    virtual void mouseMoveEvent ( QMouseEvent * ) = 0;
    virtual void mousePressEvent ( QMouseEvent * ) = 0;
    virtual void mouseReleaseEvent ( QMouseEvent * ) = 0;
    virtual double getRotation() const = 0;
    virtual double getXPos() const = 0;
    virtual double getYPos() const = 0;
    virtual double getZoom() const = 0;
    bool changed() const;
    void otherChange();
    virtual void recenterView() = 0;
    void applied();
    virtual void centerTo(const Boundery &netBoundery,
        const Position2D &pos, double radius) = 0;
    virtual void centerTo(const Boundery &netBoundery,
        Boundery bound) = 0;
    virtual int getMouseXPosition() const = 0; // !!! should not be virtual
    virtual int getMouseYPosition() const = 0;// !!! should not be virtual
private:

protected:
    GUIViewTraffic &_callback;
    bool _changed;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIPerspectiveChanger.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

