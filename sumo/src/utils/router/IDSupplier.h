#ifndef IDSupplier_h
#define IDSupplier_h
//---------------------------------------------------------------------------//
//                        IDSupplier.h -
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


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class IDSupplier
 * This class builds string ids by adding an increasing numerical value to a
 * previously given string
 */
class IDSupplier {
public:
    /// Constructor
    IDSupplier(const std::string &prefix="", long begin=0);

    /// Destructor
    ~IDSupplier();

    /// Returns the next id
    std::string getNext();

private:
    /// The current index
    long _current;

    /// The prefix to use
    std::string _prefix;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "IDSupplier.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

