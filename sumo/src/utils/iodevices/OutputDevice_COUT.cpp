//---------------------------------------------------------------------------//
//                        OutputDevice_COUT.cpp -
//  An output device that encapsulates cout
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
// $Log$
// Revision 1.2  2004/08/02 13:01:16  dkrajzew
// documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include "OutputDevice_COUT.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
OutputDevice_COUT::OutputDevice_COUT()
{
}


OutputDevice_COUT::~OutputDevice_COUT()
{
}


bool
OutputDevice_COUT::ok()
{
    return true;
}


void
OutputDevice_COUT::close()
{
    throw 1;
}

std::ostream &
OutputDevice_COUT::getOStream()
{
    return cout;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

