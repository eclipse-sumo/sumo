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
// Revision 1.4  2004/08/02 11:49:11  dkrajzew
// gradients added; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIGlobals.h"
#include <gui/GUIGlObjectStorage.h>
#include <gui/GUIGlobalSelection.h>

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
bool gAllowTextures;
bool gSuppressEndInfo;
float gAggregationRememberingFactor;
GUIGradientStorage *gGradients;

GUISimInfo *gSimInfo = 0;

FXApp *gFXApp = 0;

std::string gCurrentFolder;


GUISelectedStorage gSelected;

    /** @brief A container for numerical ids of objects
        in order to make them grippable by openGL */
GUIGlObjectStorage gIDStorage;

GUIAddWeightsStorage gAddWeightsStorage;

std::vector<int> gBreakpoints;



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

