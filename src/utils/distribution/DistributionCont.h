/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    DistributionCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for distributions
/****************************************************************************/
#ifndef DistributionCont_h
#define DistributionCont_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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

    /// delete all stored distributions
    static void clear();

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

