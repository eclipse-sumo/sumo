/****************************************************************************/
/// @file    ROVehicleType_ID.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 07.12.2005
/// @version $Id: $
///
// A type which is only described by its id (other values are invalid)
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
#ifndef ROVehicleType_ID_h
#define ROVehicleType_ID_h
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

#include <string>
#include <iostream>
#include "ROVehicleType.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehicleType_ID
 * The parameters of the Krauﬂ-model being a vehicle type.
 */
class ROVehicleType_ID : public ROVehicleType
{
public:
    /// Parametrised constructor
    ROVehicleType_ID(const std::string &id);

    /// Destructor
    ~ROVehicleType_ID();

    /// Saves the vehicle type into the given stream using SUMO-XML
    std::ostream &xmlOut(std::ostream &os) const;

};


#endif

/****************************************************************************/

