/****************************************************************************/
/// @file    MSInternalLane.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2003
/// @version $Id$
///
// Representation of a lane over a junction
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
#ifndef MSInternalLane_h
#define MSInternalLane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLogicJunction.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSEdgeControl.h"
#include <bitset>
#include <deque>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <iostream>
#include "MSNet.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLink;
class MSMoveReminder;
class GUILaneWrapper;
class GUIGlObjectStorage;



// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSInternalLane
 * @brief Representation of a lane over a junction
 *
 * Junction-internal conflicts are managed by this class by setting the
 *  information about a vehicle being on an internal lane to the junction
 *  logic ("myFoesCont", the lane's index is stored in "myFoesIndex").
 * This logic than decides whether further vehicles may approach the
 *  junction.
 *
 * The simulation shall be able to use these lanes only when needed and work
 *  as previously thought without considering any junction-internal things.
 *
 * @todo Complete work on pass position
 * @todo Join with MSLane
 */
class MSInternalLane : public MSLane {
public:
    /** @brief Constructor
     *
     * @param[in] id The lane's id
     * @param[in] maxSpeed The speed allwoed on this lane
     * @param[in] length The lane's length
     * @param[in] edge The edge this lane belongs to
     * @param[in] numericalID The numerical id of the lane
     * @param[in] allowed Vehicle classes that explicitely may drive on this lane
     * @param[in] disallowed Vehicle classes that are explicitaly forbidden on this lane
     * @see SUMOVehicleClass
     * @see MSLane
     */
    MSInternalLane(const std::string &id, SUMOReal maxSpeed, SUMOReal length, MSEdge * const edge,
                   unsigned int numericalID, const Position2DVector &shape,
                   const SUMOVehicleClasses &allowed,
                   const SUMOVehicleClasses &disallowed) throw();


    /// @brief Destructor
    virtual ~MSInternalLane() throw();


    /// @name Additional initialisation
    /// @{

    /** @brief Sets the information where to report vehicles being on this lane to
     *
     * In addition to a lane's normal initialization (MSLane::initialize), an internal
     *  lane needs to know which junction has to be informed about a vehicle being
     *  on the internal lane. This information is set herein.
     *
     * @param[in] foescont The junction inner state to set information about vehicles into
     * @param[in] foesIdx This lane's index within this state
     */
    void setParentJunctionInformation(MSLogicJunction::InnerState * const foescont,
                                      unsigned int foesIdx) throw();


    /** @brief Sets the position that must be free so that vehicles can pass
     * @todo !!! incomplete usage; check
     */
    void setPassPosition(SUMOReal passPos) throw();
    /// @}



    /// @name Vehicle movement (longitudinal)
    /// @{

    virtual bool moveCritical(SUMOTime t);
    /// @}



private:
    /// @brief The container of junction-internal vehicle-occupied lanes
    MSLogicJunction::InnerState * myFoesCont;

    /// @brief The index on which to write into this container
    int myFoesIndex;

    /** @brief The position that must be free so that vehicles can pass
     * @todo !!! incomplete usage; check
     */
    SUMOReal myPassPosition;


private:
    /// @brief invalidated copy constructor
    MSInternalLane(const MSInternalLane &s);

    /// @brief invalidated assignment operator
    MSInternalLane &operator=(const MSInternalLane &s);


};


#endif

/****************************************************************************/

