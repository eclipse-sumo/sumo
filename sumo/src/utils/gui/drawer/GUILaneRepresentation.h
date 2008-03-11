/****************************************************************************/
/// @file    GUILaneRepresentation.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef GUILaneRepresentation_h
#define GUILaneRepresentation_h



// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/globjects/GUIGlObject.h>

class GUILaneRepresentation : public GUIGlObject
{
public:
    /// Constructor
    GUILaneRepresentation(GUIGlObjectStorage &idStorage, std::string fullName) throw()
            : GUIGlObject(idStorage, fullName) {}

    /// Constructor for objects joining gl-objects
    GUILaneRepresentation(GUIGlObjectStorage &idStorage,
                          std::string fullName, size_t glID) throw()
            : GUIGlObject(idStorage, fullName, glID) {}

    virtual ~GUILaneRepresentation() throw() { }

    /** returns the "visualisation length"; this length may differ to the
        real length */
    virtual SUMOReal visLength() const = 0;

    virtual const Position2DVector &getShape() const = 0;
    virtual const DoubleVector &getShapeRotations() const = 0;
    virtual const DoubleVector &getShapeLengths() const = 0;

    virtual void selectSucessors() { }

};


#endif

/****************************************************************************/

