#ifndef NIVissimTrafficDescription_h
#define NIVissimTrafficDescription_h
//---------------------------------------------------------------------------//
//                        NIVissimTrafficDescription.h -  ccc
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
// Revision 1.4  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
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
#include "NIVissimVehicleClassVector.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIVissimTrafficDescription {
public:
    NIVissimTrafficDescription(const std::string &id, const std::string &name,
        const NIVissimVehicleClassVector &vehicleTypes);
    ~NIVissimTrafficDescription();
    static bool dictionary(const std::string &id, const std::string &name,
        const NIVissimVehicleClassVector &vehicleTypes);
    static bool dictionary(const std::string &id, NIVissimTrafficDescription *o);
    static NIVissimTrafficDescription *dictionary(const std::string &id);
    static void clearDict();
private:
    std::string myID;
    std::string myName;
    NIVissimVehicleClassVector myVehicleTypes;
private:
    typedef std::map<std::string, NIVissimTrafficDescription*> DictType;
    static DictType myDict;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimTrafficDescription.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

