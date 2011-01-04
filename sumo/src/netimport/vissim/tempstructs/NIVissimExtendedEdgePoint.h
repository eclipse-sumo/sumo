/****************************************************************************/
/// @file    NIVissimExtendedEdgePoint.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
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
#ifndef NIVissimExtendedEdgePoint_h
#define NIVissimExtendedEdgePoint_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/VectorHelper.h>
#include <utils/geom/Position2D.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimExtendedEdgePoint {
public:
    /** @brief Constructor
     * @param[in] edgeid The id of the Vissim-edge
     * @param[in] lanes Lanes on which this point lies
     * @param[in] position The position of this point at the edge
     * @param[in] assignedVehicles Vehicle (type) indices which should be regarded by this point
     */
    NIVissimExtendedEdgePoint(int edgeid, const IntVector &lanes,
                              SUMOReal position, const IntVector &assignedVehicles) throw();
    ~NIVissimExtendedEdgePoint();
    int getEdgeID() const;
    SUMOReal getPosition() const;
    Position2D getGeomPosition() const;
    const IntVector &getLanes() const;


    /** @brief Resets lane numbers if all lanes shall be used
     *
     * If myLanes contains a -1, the content of myLanes is replaced
     *  by indices of all lanes of the given edge.
     *
     * @param[in] The built edge
     */
    void recheckLanes(const NBEdge * const edge) throw();

private:
    int myEdgeID;
    IntVector myLanes;
    SUMOReal myPosition;
    IntVector myAssignedVehicles;
};


#endif

/****************************************************************************/

