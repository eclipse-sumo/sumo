#ifndef ROVehicleType_h
#define ROVehicleType_h
//---------------------------------------------------------------------------//
//                        ROVehicleType.h -
//  Basic class for vehicle types
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

#include <iostream>
#include <string>
#include "ReferencedItem.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROVehicleType
 * The base class for vehicle types.
 */
class ROVehicleType : public ReferencedItem {
public:
    /// Constructor
    ROVehicleType(const std::string &id);

    /// Destructor
	virtual ~ROVehicleType();

    /// Saves the type using SUMO-XML
	virtual std::ostream &xmlOut(std::ostream &os);

    /// Returns the name of the type
    std::string getID() const;

protected:
    /// The name of the type
    std::string _id;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROVehicleType.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

