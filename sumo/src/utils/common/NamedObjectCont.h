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
// Revision 1.5  2003/05/20 09:49:43  dkrajzew
// further work and debugging
//
// Revision 1.4  2003/03/18 13:16:57  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/02/10 17:42:36  roessel
// Added necessary keyword typename.
//
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
// updated
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
template<class T>
class NamedObjectCont {
public:
    /// Constructor
    NamedObjectCont();

    /// Destructor
    virtual ~NamedObjectCont();

    /** @brief Adds an item
        If another item with the same name is already known, false is reported
        and the item is not added. */
    virtual bool add(const std::string &id, T item);

    /** @brief Retrieves an item
        Returns 0 when no such item is stored within the container */
    T get(const std::string &id) const;

    /// Removes all items from the container (deletes them, too)
    void clear();

    /// Returns the number of items within the container
    size_t size() const;

    /// Removes the named item from the container
    void erase(const std::string &id);

protected:
    /// Definition of the container type
    typedef std::map< std::string, T > myCont;

    /// Definition of the container type iterator
    typedef typename myCont::iterator myContIt;

    /// The container
    myCont   _cont;

};

#ifndef MSVC
#ifndef EXTERNAL_TEMPLATE_DEFINITION
#include "NamedObjectCont.cpp"
#endif
#endif

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NamedObjectCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

