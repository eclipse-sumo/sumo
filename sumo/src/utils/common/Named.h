#ifndef Named_h
#define Named_h
//---------------------------------------------------------------------------//
//                        Named.h -
//  An object that has a name
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
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
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
 * The base class for objects which do have a name (an id).
 */
class Named {
public:
    /// Constructor
    Named(const std::string &id) : _id(id) { }

    /// Destructor
    virtual ~Named() { }

    /// Returns the name
    std::string getID() const { return _id; }

protected:
    /// The name of the object
    std::string _id;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "Named.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

