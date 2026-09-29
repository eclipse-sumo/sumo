/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2017-2026 German Aerospace Center (DLR) and others.
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
/// @file    MeanData.cpp
/// @author  Angelo Banse
/// @date    11.11.2020
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include <libsumo/TraCIConstants.h>
#include <libsumo/MeanData.h>
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_MEANDATA_VARIABLE, libsumo::CMD_SET_MEANDATA_VARIABLE> Dom;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
MeanData::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}

int
MeanData::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(MeanData, MEANDATA)
LIBTRACI_PARAMETER_IMPLEMENTATION(MeanData, MEANDATA)


double
MeanData::getAttributeValue(const std::string& meanDataID, const std::string& laneID, const std::string& attr) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 2);
    StoHelp::writeTypedString(content, laneID);
    StoHelp::writeTypedString(content, attr);
    std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
    return Dom::getDouble(libsumo::VAR_MEANDATA_LANE, meanDataID, &content);
}


std::vector<std::string>
MeanData::getIDs(const std::string&meanDataID) {
    return Dom::getStringVector(libsumo::VAR_MEANDATA_IDS, meanDataID);
}


std::vector<double>
MeanData::getAttributeValues(const std::string& meanDataID, const std::string& attr) {
    tcpip::Storage content;
    StoHelp::writeTypedString(content, attr);
    std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
    return Dom::getDoubleVector(libsumo::VAR_MEANDATA_IDS, meanDataID, &content);
}

}


/****************************************************************************/
