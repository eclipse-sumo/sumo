#ifndef NamedObjectCont_h
#define NamedObjectCont_h
//---------------------------------------------------------------------------//
//                        NamedObjectCont.h -
//  A map over named objects
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

#include <map>
#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NamedObjectCont
 * An associative storage (map) for objects (pointers to them to be exact),
 * which do have a name.
 */
template<class _I>
class NamedObjectCont {
public:
    /// Constructor
    NamedObjectCont();

    /// Destructor
    virtual ~NamedObjectCont();

    /** @brief Adds an item
        If another item with the same name is already known, false is reported
        and the item is not added. */
    virtual bool add(const std::string &id, _I item);

    /** @brief Retrieves an item
        Returns 0 when no such item is stored within the container */
    _I get(const std::string &id) const;

    /// Removes all items from the container (deletes them, too)
    void clear();

    /// Returns the number of items within the container
    size_t size() const;

    /// Removes the named item from the container
    void erase(const std::string &id);

protected:
    /// Definition of the container type
    typedef std::map<std::string, _I> myCont;

    /// The container
    myCont  _cont;

};


#ifndef EXTERNAL_TEMPLATE_DEFINITION
#include "NamedObjectCont.cpp"
#endif

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NamedObjectCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

