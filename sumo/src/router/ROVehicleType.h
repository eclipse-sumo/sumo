/****************************************************************************/
/// @file    ROVehicleType.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Base class for vehicle types
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
#include <utils/gfx/RGBColor.h>
#include "ReferencedItem.h"
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehicleType
 * @brief The base class for vehicle types.
 */
class ROVehicleType : public ReferencedItem
{
public:
    /// Constructor
    ROVehicleType(const std::string &id, const RGBColor &col,
                  SUMOReal length, SUMOVehicleClass vclass);

    /// Destructor
    virtual ~ROVehicleType();

    /// Saves the type using SUMO-XML
    virtual std::ostream &xmlOut(std::ostream &os) const = 0;

    /// Returns the name of the type
    std::string getID() const;

    // Returns the length the vehicles of this type have
    SUMOReal getLength() const;

    /// Returns the class of the vehicle
    SUMOVehicleClass getClass() const;


protected:
    /// The name of the type
    std::string myID;

    /// The color of the type
    RGBColor myColor;

    /// The length of the vehicle
    SUMOReal myLength;

    /// The class of the vehicle
    SUMOVehicleClass myClass;

};


#endif

/****************************************************************************/

