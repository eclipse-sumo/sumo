/****************************************************************************/
/// @file    IDSupplier.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class that generates enumerated and prefixed string-ids
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef IDSupplier_h
#define IDSupplier_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class IDSupplier
 * This class builds string ids by adding an increasing numerical value to a
 * previously given string
 */
class IDSupplier {
public:
    /// Constructor
    IDSupplier(const std::string &prefix="", long begin=0);

    /** @brief Constructor 
     * @param[in] prefix The string to use as ID prefix
     * @param[in] knownIDs List of IDs that should never be returned by this 
     * IDSupplier
     **/
    IDSupplier(const std::string &prefix, const std::vector<std::string> &knownIDs);

    /// Destructor
    ~IDSupplier();

    /// Returns the next id
    std::string getNext();

private:
    /// The current index
    long myCurrent;

    /// The prefix to use
    std::string myPrefix;

};


#endif

/****************************************************************************/

