#ifndef GUIAddWeightsStorage_h
#define GUIAddWeightsStorage_h
//---------------------------------------------------------------------------//
//                        GUIAddWeightsStorage.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 16 Jun 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.4  2005/05/04 07:45:15  dkrajzew
// level 3 warnings removed
//
// Revision 1.3  2005/02/17 10:33:29  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <map>
#include <vector>
#include <string>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * structure definitions
 * ======================================================================= */
struct GUIAddWeight {
    std::string edgeID;
    float abs;
    float add;
    float mult;
    SUMOTime timeBeg;
    SUMOTime timeEnd;
};

typedef std::vector<GUIAddWeight> GUIAddWeightsStorage;


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

