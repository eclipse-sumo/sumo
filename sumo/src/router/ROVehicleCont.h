/****************************************************************************/
/// @file    ROVehicleCont.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A container for vehicles sorted by their departure time
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROVehicleCont_h
#define ROVehicleCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <queue>
#include <vector>
#include "ROVehicle.h"
#include <utils/common/NamedObjectCont.h>
#include "ROHelper.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehicleCont
 * @brief A container for vehicles sorted by their departure time
 *
 * A map of vehicle-ids to the vehicles themselves. Besides the functionality
 *  implemented in NamedObjectCont, this class stores vehicles sorted by their
 *  departure time.
 */
class ROVehicleCont : public NamedObjectCont<ROVehicle*> {
public:
    /// @brief Constructor
    ROVehicleCont();


    /// @brief Destructor
    ~ROVehicleCont();


    /** @brief Returns the vehicle that departs most early
     *
     * Returns the first vehicle from the internal list of sorted vehicles
     *  or 0 if this list is empty.
     *
     * @return The vehicle that departs most early
     */
    const ROVehicle* getTopVehicle() const;


    /** @brief Adds a vehicle to the container
     *
     * Tries to add the vehicle to the container using NamedObjectCont::add.
     *  If this succeeds, the vehicle is also added to the internal sorted
     *  list of vehicles.
     *
     * Returns the value from NamedObjectCont::add.
     *
     * @param[in] id The id of the vehicle to add
     * @param[in] item The vehicle to add
     * @return Whether the vehicle could be added
     * @see NamedObjectCont::add
     */
    virtual bool add(const std::string& id, ROVehicle* item);


    /** @brief Deletes all vehicles stored; clears the lists
     *
     * Calls NamedObjectCont::clear and replaces the internal,
     *  sorted list by an empty one.
     *
     * @see NamedObjectCont::clear
     */
    void clear();


    /** @brief Tries to remove (and delete) the named vehicle
     *
     * Calls NamedObjectCont::erase. If this succeeds, the vehicle is removed
     *  from the internal sorted list. This method takes care whether the
     *  vehicle to remove is the top-most one (the one with the earliest
     *  depart time). If not, the internal list is rebuild.
     *
     * @param[in] id The id of the vehicle to remove
     * @return Whether the vehicle could be removed
     * @see NamedObjectCont::erase
     */
    bool erase(const std::string& id);


private:
    /** @brief Rebuild the internal, sorted list
     *
     * Rebuilds the internal, sorted list by clearing it, first, and then
     *  adding all vehicles stored.
     *
     * @see NamedObjectCont::clear
     */
    void rebuildSorted();


private:
    /// @brief The sorted vehicle list
    mutable std::priority_queue<ROVehicle*, std::vector<ROVehicle*>, ROVehicleByDepartureComperator> mySorted;


private:
    /// @brief Invalidated copy constructor
    ROVehicleCont(const ROVehicleCont& src);

    /// @brief Invalidated assignment operator
    ROVehicleCont& operator=(const ROVehicleCont& src);

};


#endif

/****************************************************************************/

