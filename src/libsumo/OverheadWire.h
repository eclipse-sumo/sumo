/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2021 German Aerospace Center (DLR) and others.
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
/// @file    OverheadWire.h
/// @author  Jakob Erdmann
/// @date    16.03.2020
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <vector>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
class MSOverheadWire;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OverheadWire
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class OverheadWire {
public:
    static std::string getLaneID(const std::string& stopID);
    static double getStartPos(const std::string& stopID);
    static double getEndPos(const std::string& stopID);
    static std::string getName(const std::string& stopID);
    static int getVehicleCount(const std::string& stopID);
    static std::vector<std::string> getVehicleIDs(const std::string& stopID);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

#ifndef LIBTRACI
#ifndef SWIG
    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static MSStoppingPlace* getOverheadWire(const std::string& id);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
#endif
#endif

    /// @brief invalidated standard constructor
    OverheadWire() = delete;
};


}
