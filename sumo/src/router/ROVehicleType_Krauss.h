#ifndef ROVehicleType_Krauss_h
#define ROVehicleType_Krauss_h
//---------------------------------------------------------------------------//
//                        ROVehicleType_Krauss.h -
//  Description of parameters of a krauss-modeled vehicle
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.4  2003/07/22 15:14:13  dkrajzew
// debugging (false vehicle length)
//
// Revision 1.3  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include "ROVehicleType.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROVehicleType_Krauss
 * The parameters of the Krauﬂ-model being a vehicle type.
 */
class ROVehicleType_Krauss : public ROVehicleType {
public:
    /// Constructor - uses default values
    ROVehicleType_Krauss();

    /// Parametrised constructor
	ROVehicleType_Krauss(const std::string &id, const RGBColor &col,
        double length,
        double a, double b, double eps, double maxSpeed);

    /// Destructor
	~ROVehicleType_Krauss();

    /// Saves the vehicle type into the given stream using SUMO-XML
	std::ostream &xmlOut(std::ostream &os);

private:
    /// Krauﬂ-parameter
	double myA, myB, myEps, myMaxSpeed;

private:
    /// default parameter
    static double myDefault_A, myDefault_B, myDefault_EPS,
        myDefault_LENGTH, myDefault_MAXSPEED;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROVehicleType_Krauss.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

