#ifndef ROVehicleType_ID_h
#define ROVehicleType_ID_h
//---------------------------------------------------------------------------//
//                        ROVehicleType_ID.h -
//  A type which is only described by its id
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 07.12.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.1  2006/01/09 13:29:32  dkrajzew
// debugging vehicle color usage
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include "ROVehicleType.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROVehicleType_ID
 * The parameters of the Krauﬂ-model being a vehicle type.
 */
class ROVehicleType_ID : public ROVehicleType {
public:
    /// Parametrised constructor
	ROVehicleType_ID(const std::string &id);

    /// Destructor
	~ROVehicleType_ID();

    /// Saves the vehicle type into the given stream using SUMO-XML
	std::ostream &xmlOut(std::ostream &os) const;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

