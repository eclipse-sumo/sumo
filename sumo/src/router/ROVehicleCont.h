#ifndef ROVehicleCont_h
#define ROVehicleCont_h
//---------------------------------------------------------------------------//
//                        ROVehicleCont.h -
//  A container for vehicles
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
// Revision 1.4  2003/03/03 15:22:38  dkrajzew
// debugging
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
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
#include <map>
#include <queue>
#include <vector>
#include "ROVehicle.h"
#include <utils/common/NamedObjectCont.h>
#include "ROHelper.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROVehicleCont
 * A map of vehicle-ids to the vehicles themselves. Additionally supplies
 * a list of vehicles where these are sorted by their departure times.
 */
class ROVehicleCont : public NamedObjectCont<ROVehicle*> {
public:
    /// Constructor
    ROVehicleCont();

    /// Destructor
    ~ROVehicleCont();

    /// Returns the list of sorted vehicles
    std::priority_queue<ROVehicle*,
        std::vector<ROVehicle*>,
        ROHelper::VehicleByDepartureComperator> &sort();

    void eraseVehicle(ROVehicle *v);

private:
    /// The sorted vehicle list
    std::priority_queue<ROVehicle*,
        std::vector<ROVehicle*>,
        ROHelper::VehicleByDepartureComperator> _sorted;

private:
    /// we made the copy constructor invalid
    ROVehicleCont(const ROVehicleCont &src);

    /// we made the assignment operator invalid
    ROVehicleCont &operator=(const ROVehicleCont &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROVehicleCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

