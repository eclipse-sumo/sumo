#ifndef GUILaneRepresentation_h
#define GUILaneRepresentation_h
//---------------------------------------------------------------------------//
//                        GUILaneRepresentation.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.5  2005/09/23 06:07:54  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:19:10  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:49:08  dkrajzew
// code style adapted
//
// Revision 1.3  2005/06/14 11:29:50  dksumo
// documentation added
//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/gui/globjects/GUIGlObject.h>

class GUILaneRepresentation : public GUIGlObject {
public:
    /// Constructor
    GUILaneRepresentation(GUIGlObjectStorage &idStorage, std::string fullName)
		: GUIGlObject(idStorage, fullName) {}

    /// Constructor for objects joining gl-objects
    GUILaneRepresentation(GUIGlObjectStorage &idStorage,
			std::string fullName, size_t glID)
		: GUIGlObject(idStorage, fullName, glID) {}

	virtual ~GUILaneRepresentation() { }

    /** returns the begin position of the lane */
    virtual const Position2D &getBegin() const = 0;

    /** returns the end position of the lane */
    virtual const Position2D &getEnd() const = 0;

    /** returns the direction of the lane (!!! describe better) */
    virtual const Position2D &getDirection() const = 0;

    /** returns the rotation of the lane (!!! describe better) */
    virtual SUMOReal getRotation() const = 0;

    /** returns the "visualisation length"; this length may differ to the
        real length */
    virtual SUMOReal visLength() const = 0;

    virtual const Position2DVector &getShape() const = 0;
    virtual const DoubleVector &getShapeRotations() const = 0;
    virtual const DoubleVector &getShapeLengths() const = 0;

    virtual void selectSucessors() { }

};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
