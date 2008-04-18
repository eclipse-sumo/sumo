/****************************************************************************/
/// @file    ROLane.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A single lane the router may use
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
#ifndef ROLane_h
#define ROLane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/Named.h>
#include <utils/common/SUMOVehicleClass.h>

class ROEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROLane
 * @brief A single lane the router may use
 */
class ROLane : public Named
{
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the lane
     * @param[in] length The length of the lane
     * @param[in] maxSpeed The maximum speed allowed on the lane
     * @param[in] allowed Vehicle classes that may pass this lane
     * @param[in] disallowed Vehicle classes that may not pass this lane
     */
    ROLane(const std::string &id, SUMOReal length, SUMOReal maxSpeed,
           const std::vector<SUMOVehicleClass> &allowed,
           const std::vector<SUMOVehicleClass> &disallowed) throw()
           : Named(id), myLength(length), myMaxSpeed(maxSpeed),
           myAllowedClasses(allowed), myNotAllowedClasses(disallowed)
    {
    }


    /// @brief Destructor
    ~ROLane() throw() { }


    /** @brief Returns the length of the lane
     * @return The length of this lane
     */
    SUMOReal getLength() const throw() {
        return myLength;
    }


    /** @brief Returns the maximum speed allowed on this lane
     * @return The maximum speed allowed on this lane
     */
    SUMOReal getSpeed() const throw() {
        return myMaxSpeed;
    }


    /** @brief Returns the list of allowed vehicle classes
     * @return The list of vehicle classes allowed on this lane
     */
    const std::vector<SUMOVehicleClass> &getAllowedClasses() const throw() {
        return myAllowedClasses;
    }


    /** @brief Returns the list of not allowed vehicle classes
     * @return The list of vehicle classes not allowed on this lane
     */
    const std::vector<SUMOVehicleClass> &getNotAllowedClasses() const throw() {
        return myNotAllowedClasses;
    }


private:
    /// @brief The length of the lane
    SUMOReal myLength;

    /// @brief The maximum speed allowed on the lane
    SUMOReal myMaxSpeed;

    /// @brief The list of allowed vehicle classes
    std::vector<SUMOVehicleClass> myAllowedClasses;

    /// @brief The list of disallowed vehicle classes
    std::vector<SUMOVehicleClass> myNotAllowedClasses;


private:
    /// @brief Invalidated copy constructor
    ROLane(const ROLane &src);

    /// @brief Invalidated assignment operator
    ROLane &operator=(const ROLane &src);

};


#endif

/****************************************************************************/

