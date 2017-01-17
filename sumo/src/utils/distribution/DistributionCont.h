/****************************************************************************/
/// @file    DistributionCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for distributions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef DistributionCont_h
#define DistributionCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <utils/distribution/Distribution.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DistributionCont
 * A container for distributions of different type.
 */
class DistributionCont {
public:
    /// Adds a distribution of the given type and name to the container
    static bool dictionary(const std::string& type, const std::string& id,
                           Distribution* d);

    /// retrieves the distribution described by a type and a name from the container
    static Distribution* dictionary(const std::string& type,
                                    const std::string& id);

private:
    /// Definition of a map from distribution ids to distributions
    typedef std::map<std::string, Distribution*> DistDict;

    /// Definition of a map from distribution types to distribution ids to distributions
    typedef std::map<std::string, DistDict> TypedDistDict;

    /// Map from distribution types to distribution ids to distributions
    static TypedDistDict myDict;

};


#endif

/****************************************************************************/

