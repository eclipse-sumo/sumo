//---------------------------------------------------------------------------//
//                        IDSupplier.cpp -
//  A class that generates enumerated and prefixed string-ids
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
// Revision 1.2  2003/02/07 10:52:57  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <string>
#include <sstream>
#include "IDSupplier.h"

using namespace std;

IDSupplier::IDSupplier(const std::string &prefix, long begin)
    : _current(begin), _prefix(prefix)
{
}

IDSupplier::~IDSupplier()
{
}

std::string
IDSupplier::getNext()
{
    ostringstream strm;
    strm << _prefix << _current++;
    return strm.str();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "IDSupplier.icc"
//#endif

// Local Variables:
// mode:C++
// End:


