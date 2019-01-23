/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBParking.h
/// @author  Jakob Erdmann
/// @date    Tue, 14 Nov 2017
/// @version $Id$
///
// The representation of an imported parking area
/****************************************************************************/
#ifndef SUMO_NBParking_H
#define SUMO_NBParking_H

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include "utils/common/Named.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class NBEdgeCont;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class NBParking
* @brief The representation of a single pt stop
*/
class NBParking : public Named {

public:
    /**@brief Constructor
    * @param[in] id The id of the pt stop
    * @param[in] position The position of the pt stop
    * @param[in] edgeId The edge id of the pt stop
    * @param[in] length The length of the pt stop
    */
    NBParking(const std::string& id, const std::string& edgeID, const std::string& name = "");

    void write(OutputDevice& device, NBEdgeCont& ec) const;

private:
    std::string myEdgeID;
    std::string myName;

};

class NBParkingCont : public std::vector<NBParking> {

public:
    /// @brief add edges that must be kept
    void addEdges2Keep(const OptionsCont& oc, std::set<std::string>& into);
};

#endif //SUMO_NBParking_H
