/****************************************************************************/
/// @file    ROVehicleType_ID.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 07.12.2005
/// @version $Id$
///
// A type which is only described by its id (other values are invalid)
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
#ifndef ROVehicleType_ID_h
#define ROVehicleType_ID_h


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
 * @class ROVehicleType_ID
 * @brief A type which is only described by its id (other values are invalid)
 *
 * This is a router representation of types which were not defined. The type
 *  is not saved, but still can be referenced.
 *
 * @todo Check whether this rather should be realised by a boolean marker in ROVehicleType
 */
class ROVehicleType_ID : public ROVehicleType {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the vehicle type
     */
    ROVehicleType_ID(const std::string &id) throw();


    /// @brief Destructor
    ~ROVehicleType_ID() throw();


    /// @name inherited from ROVehicleType
    //@{

    /** @brief Saves the type using SUMO-XML
     *
     * Does nothing.
     *
     * @param[in] dev The device to write the definition into
     * @return The same device for further usage
     */
    OutputDevice &writeXMLDefinition(OutputDevice &dev) const;
    //@}


private:
    /// @brief Invalidated copy constructor
    ROVehicleType_ID(const ROVehicleType_ID &src);

    /// @brief Invalidated assignment operator
    ROVehicleType_ID &operator=(const ROVehicleType_ID &src);

};


#endif

/****************************************************************************/

