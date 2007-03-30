/****************************************************************************/
/// @file    ROVehicleType_Krauss.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Description of parameters of a krauss-modeled vehicle
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
#ifndef ROVehicleType_Krauss_h
#define ROVehicleType_Krauss_h
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
 * @class ROVehicleType_Krauss
 * The parameters of the Krauﬂ-model being a vehicle type.
 */
class ROVehicleType_Krauss : public ROVehicleType
{
public:
    /// Constructor - uses default values
    ROVehicleType_Krauss();

    /// Parametrised constructor
    ROVehicleType_Krauss(const std::string &id, const RGBColor &col,
                         SUMOReal length, SUMOVehicleClass vclass,
                         SUMOReal a, SUMOReal b, SUMOReal eps, SUMOReal maxSpeed);

    /// Destructor
    ~ROVehicleType_Krauss();

    /// Saves the vehicle type into the given stream using SUMO-XML
    std::ostream &xmlOut(std::ostream &os) const;

private:
    /// Krauﬂ-parameter
    SUMOReal myA, myB, myEps, myMaxSpeed;

public:
    /// default parameter
    static SUMOReal myDefault_A, myDefault_B, myDefault_EPS,
    myDefault_LENGTH, myDefault_MAXSPEED;

    static SUMOVehicleClass myDefault_CLASS;

};


#endif

/****************************************************************************/

