/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NBParking.h
/// @author  Jakob Erdmann
/// @date    Tue, 14 Nov 2017
///
// The representation of an imported parking area
/****************************************************************************/
#pragma once
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
* @brief The representation of an imported parking area
*/
class NBParking : public Named {

public:
    /**@brief Constructor
    * @param[in] id The id of the parking area
    * @param[in] edgeId The edge id of the parking area
    * @param[in] name The optional verbose description of the parking area
    */
    NBParking(const std::string& id, const std::string& edgeID, const std::string& name = "");

    void write(OutputDevice& device, NBEdgeCont& ec) const;

    const std::string getEdgeID() const {
        return myEdgeID;
    }

private:
    std::string myEdgeID;
    std::string myName;

};

class NBParkingCont : public std::vector<NBParking> {

public:
    /// @brief add edges that must be kept
    void addEdges2Keep(const OptionsCont& oc, std::set<std::string>& into);
};

