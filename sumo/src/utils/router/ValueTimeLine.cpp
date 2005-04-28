//---------------------------------------------------------------------------//
//                        ValueTimeLine.cpp -
//  A list of time ranges with float values
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
// Revision 1.5  2005/04/28 09:02:49  dkrajzew
// level3 warnings removed
//
// Revision 1.4  2004/02/06 08:47:11  dkrajzew
// debugged things that came up during the compilation under linux
//
// Revision 1.3  2004/01/26 07:17:49  dkrajzew
// "specialisation" of the time line to handle interpolation added
//
// Revision 1.2  2003/02/07 10:52:57  dkrajzew
// updated
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

#include <utility>
#include <vector>
#include <algorithm>
#include <cassert>
#include "ValueTimeLine.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
//using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


