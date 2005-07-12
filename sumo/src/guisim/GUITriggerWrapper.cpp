//---------------------------------------------------------------------------//
//                        GUITriggerWrapper.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.3  2005/07/12 12:19:31  dkrajzew
// code style adapted
//
// Revision 1.3  2005/06/14 11:16:23  dksumo
// documentation added
//


/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUITriggerWrapper.h"

/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUITriggerWrapper::GUITriggerWrapper()
{
}


GUITriggerWrapper::~GUITriggerWrapper()
{
}


Boundary
GUITriggerWrapper::getBoundary() const
{
    return Boundary();
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
