//---------------------------------------------------------------------------//
//                        NIVissimClosedLaneDef.cpp -  ccc
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2003/06/05 11:46:55  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <utils/common/IntVector.h>
#include "NIVissimClosedLaneDef.h"


NIVissimClosedLaneDef::NIVissimClosedLaneDef(int lane,
                                             const IntVector &assignedVehicles)
    : myLaneNo(lane), myAssignedVehicles(assignedVehicles)
{
}


NIVissimClosedLaneDef::~NIVissimClosedLaneDef()
{
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimClosedLaneDef.icc"
//#endif

// Local Variables:
// mode:C++
// End:


