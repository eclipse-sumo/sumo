#ifndef NIVissimSource_h
#define NIVissimSource_h
//---------------------------------------------------------------------------//
//                        NIVissimSource.h -  ccc
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

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIVissimSource {
public:
    NIVissimSource(const std::string &id, const std::string &name,
        const std::string &edgeid, double q, bool exact,
        int vehicle_combination, double beg, double end);
    ~NIVissimSource();
    static bool dictionary(const std::string &id, const std::string &name,
        const std::string &edgeid, double q, bool exact,
        int vehicle_combination, double beg, double end);
    static bool dictionary(const std::string &id, NIVissimSource *o);
    static NIVissimSource *dictionary(const std::string &id);
    static void clearDict();
private:
    std::string myID;
    std::string myName;
    std::string myEdgeID;
    double myQ;
    bool myExact;
    int myVehicleCombination;
    double myTimeBeg;
    double myTimeEnd;

private:
    typedef std::map<std::string, NIVissimSource*> DictType;
    static DictType myDict;
};




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimSource.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

