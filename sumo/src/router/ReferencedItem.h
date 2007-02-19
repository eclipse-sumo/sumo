/****************************************************************************/
/// @file    ReferencedItem.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Something that holds the number of references to it
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ReferencedItem_h
#define ReferencedItem_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ReferencedItem
 * Basic class for structures which are referenced by other classes. The wish
 * is to have an information whether an instance of a derived class was already
 * saved (together with a referencing class) or shall be saved this time.
 */
class ReferencedItem
{
public:
    /// Constructor
    ReferencedItem() : _wasSaved(false)
    { }

    /// Destructor
    virtual ~ReferencedItem()
    { }

    /// Returns the information whether this item was already saved
    bool isSaved() const
    {
        return _wasSaved;
    }

    /// Marks the item as saved
    void markSaved()
    {
        _wasSaved = true;
    }

private:
    /// Information whether the item was already saved
    bool _wasSaved;

};


#endif

/****************************************************************************/

