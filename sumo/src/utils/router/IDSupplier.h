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
// Revision 1.6  2005/10/07 11:47:09  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 12:21:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/28 09:02:49  dkrajzew
// level3 warnings removed
//
// Revision 1.3  2004/07/02 09:48:08  dkrajzew
// some style changes
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
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

#endif

// Local Variables:
// mode:C++
// End:

