/****************************************************************************/
/// @file    RORunningVehicle.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A router vehicle that has an initial speed (and possibly position)
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
#ifndef RORunningVehicle_h
#define RORunningVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include "ROVehicle.h"
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROVehicleBuilder;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORunningVehicle
 * @brief A router vehicle that has an initial speed (and possibly position)
 *
 * A vehicle which when being emitted, already has an initial speed and
 *  which also has a fixed position it shall be emitted at.
 *
 * @todo Ok, instances are added; still, - as no test existed - they are never used again
 * @todo Recheck when @extension: starting/ending state definition
 */
class RORunningVehicle : public ROVehicle
{
public:
    /** @brief Constructor
     *
     * @param[in] vb The vehicle builder
     * @param[in] id The id of the vehicle
     * @param[in] route The definition of the route the vehicle shall use
     * @param[in] depart The depart time of the vehicle
     * @param[in] type The type of the vehicle
     * @param[in] lane The lane the vehicle shall be emitted at
     * @param[in] pos The position on the lane the vehicle shall be emitted at
     * @param[in] speed The vehicle's initial speed
     * @param[in] col The color of the vehicle
     * @param[in] period The repetition period of the vehicle
     * @param[in] repNo The repetition number of the vehicle
     *
     * @todo Why is the vehicle builder given?
     * @extension: starting/ending state definition
     */
    RORunningVehicle(ROVehicleBuilder &vb,
                     const std::string &id, RORouteDef *route,
                     SUMOTime time, ROVehicleType *type, const std::string &lane,
                     SUMOReal pos, SUMOReal speed,
                     const std::string &col, int period, int repNo);


    /// @brief Destructor
    ~RORunningVehicle();


    /** @brief Writes the vehicle's definition to the given stream
     *
     * @todo unused!!!
     */
    void xmlOut(OutputDevice &dev) const;


    /** @brief Returns a copy of the vehicle using a new id, departure time and route
     *
     * @param[in] vb The vehicle builder to use
     * @param[in] id the new id to use
     * @param[in] depTime The new vehicle's departure time
     * @param[in] newRoute The new vehicle's route
     * @return The new vehicle
     *
     * @todo Is this used? What for if everything is replaced?
     */
    virtual ROVehicle *copy(ROVehicleBuilder &vb,
                            const std::string &id, unsigned int depTime,
                            RORouteDef *newRoute);

private:
    /// @brief The lane the vehicle shall depart from
    std::string myLane;

    /// @brief The position on the lane the vehicle shall depart from
    SUMOReal myPos;

    /// @brief The initial speed of the vehicle
    SUMOReal mySpeed;

};


#endif

/****************************************************************************/

