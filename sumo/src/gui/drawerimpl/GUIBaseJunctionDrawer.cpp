//---------------------------------------------------------------------------//
//                        GUIBaseLaneDrawer.cpp -
//  Base class for lane drawing;
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 02.09.2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.3  2003/11/26 09:36:01  dkrajzew
// documentation added
//
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <guisim/GUIJunctionWrapper.h>
#include <utils/glutils/GLHelper.h>
#include "GUIBaseJunctionDrawer.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIBaseJunctionDrawer::GUIBaseJunctionDrawer(
        std::vector<GUIJunctionWrapper*> &junctions)
    : GUIJunctionDrawer(junctions)
{
}


GUIBaseJunctionDrawer::~GUIBaseJunctionDrawer()
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


