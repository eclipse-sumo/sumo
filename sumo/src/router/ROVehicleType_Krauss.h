/****************************************************************************/
/// @file    ROVehicleType_Krauss.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A Krauss vehicle type
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROVehicleType_Krauss_h
#define ROVehicleType_Krauss_h


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
#include "ROVehicleType.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehicleType_Krauss
 * @brief A Krauss vehicle type
 *
 * @todo What about set/unset type parameter?
 */
class ROVehicleType_Krauss : public ROVehicleType {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the vehicle type
     * @param[in] col A string storing the vehicle type's color
     * @param[in] length The length of vehicles of this type
     * @param[in] vclass The vehicle class of vehicles of this type
     * @param[in] a The type's maximum acceleration
     * @param[in] b The type's maximum deceleration
     * @param[in] eps The type's driver imperfection
     * @param[in] maxSpeed The type's maximum velocity
     * @param[in] tau The type's driver reaction time
     */
    ROVehicleType_Krauss(const std::string &id, const std::string &col,
                         SUMOReal length, SUMOVehicleClass vclass,
                         SUMOReal a, SUMOReal b, SUMOReal eps, SUMOReal maxSpeed,
                         SUMOReal tau) throw();


    /// @brief Destructor
    ~ROVehicleType_Krauss() throw();


    /// @name inherited from ROVehicleType
    //@{

    /** @brief Saves the type using SUMO-XML
     *
     * Writes the definition of the vehicle type.
     *  The vehicle class is only saved if it is not SVC_UNKNOWN.
     *
     * @param[in] dev The device to write the definition into
     * @return The same device for further usage
     */
    OutputDevice &writeXMLDefinition(OutputDevice &dev) const;
    //@}

private:
    /// @brief Krauss-parameter
    SUMOReal myA, myB, myEps, myMaxSpeed, myTau;


private:
    /// @brief Invalidated copy constructor
    ROVehicleType_Krauss(const ROVehicleType_Krauss &src);

    /// @brief Invalidated assignment operator
    ROVehicleType_Krauss &operator=(const ROVehicleType_Krauss &src);

};


#endif

/****************************************************************************/

