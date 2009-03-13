/****************************************************************************/
/// @file    ROVehicleType.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Base class for routers' representation of vehicle types
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
#ifndef ROVehicleType_h
#define ROVehicleType_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include "ReferencedItem.h"
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehicleType
 * @brief Base class for routers' representation of vehicle types
 *
 * The idea is to let a router use arbitary vehicle types. Due to this, we have
 *  a base class which holds only the values a router needs (name, length, class,
 *  and a color). from this class, other vehicle types may be derived which hold
 *  the real (simulation) model's values.
 *
 * Vehicle types are derived from "ReferencedItem" in order to know whether they
 *  were saved.
 *
 * Derivations start with "ROVehicleType_".
 *
 * @see ReferencedItem
 * @todo Check whether the color really has to be stored explicitely
 */
class ROVehicleType : public ReferencedItem {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the vehicle type
     * @param[in] col A string storing the vehicle type's color
     * @param[in] length The length of vehicles of this type
     * @param[in] vclass The vehicle class of vehicles of this type
     */
    ROVehicleType(const std::string &id, const std::string &col,
                  SUMOReal length, SUMOVehicleClass vclass) throw();


    /// @brief Destructor
    virtual ~ROVehicleType() throw();


    /** @brief Saves the type using SUMO-XML
     *
     * Writes the vehicle type definition into the given stream.
     * To be implemented by real vehicle classes
     *
     * @param[in] dev The device to write the definition into
     * @return The same device for further usage
     */
    virtual OutputDevice &writeXMLDefinition(OutputDevice &dev) const = 0;


    /** @brief Returns the name of the type
     *
     * @return The id of the vehicle type
     */
    const std::string &getID() const throw() {
        return myID;
    }


    /** @brief Returns the length the vehicles of this type have
     *
     * @return the length of vehicles of this type
     */
    SUMOReal getLength() const throw() {
        return myLength;
    }


    /** @brief Returns the class of the vehicle
     *
     * @return The class of vehicles of this type
     */
    SUMOVehicleClass getClass() const throw() {
        return myClass;
    }


protected:
    /// @brief The id of the type
    std::string myID;

    /// @brief The color of the type
    std::string myColor;

    /// @brief The length of vehicles of this type
    SUMOReal myLength;

    /// @brief The class of vehicles of this type
    SUMOVehicleClass myClass;


private:
    /// @brief Invalidated copy constructor
    ROVehicleType(const ROVehicleType &src);

    /// @brief Invalidated assignment operator
    ROVehicleType &operator=(const ROVehicleType &src);

};


#endif

/****************************************************************************/

