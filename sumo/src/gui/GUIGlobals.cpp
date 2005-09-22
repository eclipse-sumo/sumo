//---------------------------------------------------------------------------//
//                        GUIGlobals.h -
//  Some global variables (yep)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.8  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 07:47:23  dkrajzew
// level 3 warnings removed
//
// Revision 1.5  2004/11/23 10:11:33  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.4  2004/08/02 11:49:11  dkrajzew
// gradients added; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "GUIGlobals.h"
#include <utils/gui/div/GUIGlobalSelection.h>

#include <algorithm>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * global variables definitions
 * ======================================================================= */
bool gQuitOnEnd;
bool gAllowAggregated;
bool gAllowAggregatedFloating;
bool gSuppressEndInfo;
SUMOReal gAggregationRememberingFactor;

GUISimInfo *gSimInfo = 0;


GUIAddWeightsStorage gAddWeightsStorage;

std::vector<int> gBreakpoints;



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

