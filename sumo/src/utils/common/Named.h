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
// Revision 1.7  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/04/28 09:02:46  dkrajzew
// level3 warnings removed
//
// Revision 1.5  2004/12/16 12:18:02  dkrajzew
// debugging
//
// Revision 1.4  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.3  2003/05/20 09:49:43  dkrajzew
// further work and debugging
//
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class Named
 * The base class for objects which do have a name (an id).
 */
class Named {
public:
    /// Constructor
    Named(const std::string &id) : _id(id) { }

    /// Destructor
    virtual ~Named() { }

    /// Returns the name
    const std::string &getID() const { return _id; }

protected:
    /// The name of the object
    std::string _id;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

