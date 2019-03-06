/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    IDSupplier.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// A class that generates enumerated and prefixed string-ids
/****************************************************************************/
#ifndef IDSupplier_h
#define IDSupplier_h


// ===========================================================================
// included modules
// ===========================================================================

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
    IDSupplier(const std::string& prefix = "", long long int begin = 0);

    /** @brief Constructor
     * @param[in] prefix The string to use as ID prefix
     * @param[in] knownIDs List of IDs that should never be returned by this
     * IDSupplier
     **/
    IDSupplier(const std::string& prefix, const std::vector<std::string>& knownIDs);

    /// Destructor
    ~IDSupplier();

    /// Returns the next id
    std::string getNext();

    /// make sure that the given id is never supplied
    void avoid(const std::string& id);

private:
    /// The current index
    long long int myCurrent;

    /// The prefix to use
    std::string myPrefix;

};


#endif

/****************************************************************************/

