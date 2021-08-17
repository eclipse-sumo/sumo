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
/// @file    VariableSpeedSign.h
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
class MSLaneSpeedTrigger;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class VariableSpeedSign
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class VariableSpeedSign {
public:
    static std::vector<std::string> getLanes(const std::string& vssID);

    //static std::vector<double> getTimes(const std::string& vssID);
    //static std::vector<double> getSpeeds(const std::string& vssID);
    //static double getSpeed(const std::string& vssID);

    //static void setSpeed(const std::string& vssID, double speed, double begTime, double endTime);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

#ifndef LIBTRACI
#ifndef SWIG
    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static MSLaneSpeedTrigger* getVariableSpeedSign(const std::string& vssID);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
#endif
#endif

    /// @brief invalidated standard constructor
    VariableSpeedSign() = delete;
};


}
