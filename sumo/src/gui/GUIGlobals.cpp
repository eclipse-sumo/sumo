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
// Revision 1.5  2004/11/23 10:11:33  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.4  2004/08/02 11:49:11  dkrajzew
// gradients added; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIGlobals.h"
#include <utils/gui/div/GUIGlobalSelection.h>

#include <algorithm>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * global variables definitions
 * ======================================================================= */
bool gQuitOnEnd;
bool gStartAtBegin;
bool gAllowAggregated;
bool gAllowAggregatedFloating;
bool gSuppressEndInfo;
float gAggregationRememberingFactor;

GUISimInfo *gSimInfo = 0;


GUIAddWeightsStorage gAddWeightsStorage;

std::vector<int> gBreakpoints;



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

