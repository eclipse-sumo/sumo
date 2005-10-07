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
// Revision 1.7  2005/10/07 11:40:10  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.1  2004/10/22 12:50:03  dksumo
// initial checkin into an internal, standalone SUMO CVS
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
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
        const std::string &category, SUMOReal length, const RGBColor &color,
        SUMOReal amax, SUMOReal dmax);
    ~NIVissimVehicleType();
    static bool dictionary(int id, const std::string &name,
        const std::string &category, SUMOReal length, const RGBColor &color,
        SUMOReal amax, SUMOReal dmax);
    static bool dictionary(int id, NIVissimVehicleType *o);
    static NIVissimVehicleType *dictionary(int id);
    static void clearDict();

private:
    int myID;
    std::string myName;
    std::string myCategory;
    SUMOReal myLength;
    RGBColor myColor;
    SUMOReal myAMax, myDMax;
private:
    typedef std::map<int, NIVissimVehicleType *> DictType;
    static DictType myDict;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

