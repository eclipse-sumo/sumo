#ifndef NIVissimVehicleType_h
#define NIVissimVehicleType_h
//---------------------------------------------------------------------------//
//                        NIVissimVehicleType.h -  ccc
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


#include <utils/gfx/RGBColor.h>
#include <string>
#include <map>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIVissimVehicleType {
public:
    NIVissimVehicleType(int id, const std::string &name,
        const std::string &category, double length, const RGBColor &color,
        double amax, double dmax);
    ~NIVissimVehicleType();
    static bool dictionary(int id, const std::string &name,
        const std::string &category, double length, const RGBColor &color,
        double amax, double dmax);
    static bool dictionary(int id, NIVissimVehicleType *o);
    static NIVissimVehicleType *dictionary(int id);
    static void clearDict();

private:
    int myID;
    std::string myName;
    std::string myCategory;
    double myLength;
    RGBColor myColor;
    double myAMax, myDMax;
private:
    typedef std::map<int, NIVissimVehicleType *> DictType;
    static DictType myDict;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimVehicleType.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

