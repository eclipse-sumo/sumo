#ifndef GUIJunctionWrapper_h
#define GUIJunctionWrapper_h
//---------------------------------------------------------------------------//
//                        GUIJunctionWrapper.h -
//  Holds geometrical values for a junction
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 1 Jul 2003
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
// Revision 1.10  2005/05/04 07:59:59  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.9  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.8  2004/04/02 11:18:37  dkrajzew
// recenter view - icon added to the popup menu
//
// Revision 1.7  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.6  2003/12/09 11:27:50  dkrajzew
// removed some dead code
//
// Revision 1.5  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.4  2003/07/22 14:59:27  dkrajzew
// changes due to new detector handling
//
// Revision 1.3  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/07/16 15:24:55  dkrajzew
// GUIGrid now handles the set of things to draw in another manner than
//  GUIEdgeGrid did; Further things to draw implemented
//
// Revision 1.1  2003/07/07 08:14:48  dkrajzew
// first steps towards the usage of a real lane and junction geometry
//  implemented
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utility>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/HaveBoundary.h>
#include <utils/foxtools/FXMutex.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIGlObject.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class MSJunction;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIJunctionWrapper
 * As MSJunctions do not have a graphical representation but a complex
 *  inheritance tree, this class is used to encapsulate the geometry of an
 *  abstract junction and to be used as a gl-object.
 */
class GUIJunctionWrapper :
            public GUIGlObject,
            public HaveBoundary {
public:
    /// constructor
    GUIJunctionWrapper( GUIGlObjectStorage &idStorage,
        MSJunction &junction,
        const Position2DVector &shape);

    /// destructor
    virtual ~GUIJunctionWrapper();

    /// Returns a popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    /** @brief Build this gl-object's parameter window
        Throws an exception in fact as junctions do not have any parameter */
    GUIParameterTableWindow *getParameterWindow(
        GUIMainWindow &app, GUISUMOAbstractView &parent);

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the shape of the junction
    const Position2DVector &getShape() const;

    /// Returns the boundary of the junction
    Boundary getBoundary() const;

	//{
	Boundary getCenteringBoundary() const;
	//}

protected:

    /// returns the information whether this item is still an active part of the sim
	bool active() const { return true; }

protected:

    /// A reference to the real junction
    MSJunction &myJunction;

    /// The junction's shape
    Position2DVector myShape;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

