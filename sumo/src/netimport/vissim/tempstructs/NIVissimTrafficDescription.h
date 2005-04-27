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
// Revision 1.6  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.5  2003/10/27 10:51:55  dkrajzew
// edges speed setting implemented (only on an edges begin)
//
// Revision 1.4  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
    NIVissimTrafficDescription(int id, const std::string &name,
        const NIVissimVehicleClassVector &vehicleTypes);
    ~NIVissimTrafficDescription();
    static bool dictionary(int id, const std::string &name,
        const NIVissimVehicleClassVector &vehicleTypes);
    static bool dictionary(int id, NIVissimTrafficDescription *o);
    static NIVissimTrafficDescription *dictionary(int id);
    static void clearDict();
    static double meanSpeed(int id);
    double meanSpeed() const;
private:
    int myID;
    std::string myName;
    NIVissimVehicleClassVector myVehicleTypes;
private:
    typedef std::map<int, NIVissimTrafficDescription*> DictType;
    static DictType myDict;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

