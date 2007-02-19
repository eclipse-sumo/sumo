/****************************************************************************/
/// @file    NIVissimExtendedEdgePoint.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
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
#ifndef NIVissimExtendedEdgePoint_h
#define NIVissimExtendedEdgePoint_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <utils/common/IntVector.h>
#include <utils/geom/Position2D.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimExtendedEdgePoint
{
public:
    NIVissimExtendedEdgePoint(int edgeid, IntVector &lanes,
                              SUMOReal position, IntVector &assignedVehicles);
    ~NIVissimExtendedEdgePoint();
    int getEdgeID() const;
    SUMOReal getPosition() const;
    Position2D getGeomPosition() const;
    const IntVector &getLanes() const;
private:
    int myEdgeID;
    IntVector myLanes;
    SUMOReal myPosition;
    IntVector myAssignedVehicles;
};


#endif

/****************************************************************************/

