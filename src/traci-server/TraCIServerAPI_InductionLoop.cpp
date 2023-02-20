/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
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
/// @file    TraCIServerAPI_InductionLoop.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    07.05.2009
///
// APIs for getting/setting induction loop values via TraCI
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/output/MSDetectorControl.h>
#include <libsumo/InductionLoop.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/StorageHelper.h>
#include "TraCIServerAPI_InductionLoop.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_InductionLoop::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
        tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(libsumo::RESPONSE_GET_INDUCTIONLOOP_VARIABLE, variable, id);
    try {
        if (!libsumo::InductionLoop::handleVariable(id, variable, &server, &inputStorage)) {
            switch (variable) {
                case libsumo::LAST_STEP_VEHICLE_DATA: {
                    std::vector<libsumo::TraCIVehicleData> vd = libsumo::InductionLoop::getVehicleData(id);
                    tcpip::Storage tempContent;
                    int cnt = 0;
                    StoHelp::writeTypedInt(tempContent, (int)vd.size());
                    ++cnt;
                    for (const libsumo::TraCIVehicleData& svd : vd) {
                        StoHelp::writeTypedString(tempContent, svd.id);
                        ++cnt;
                        StoHelp::writeTypedDouble(tempContent, svd.length);
                        ++cnt;
                        StoHelp::writeTypedDouble(tempContent, svd.entryTime);
                        ++cnt;
                        StoHelp::writeTypedDouble(tempContent, svd.leaveTime);
                        ++cnt;
                        StoHelp::writeTypedString(tempContent, svd.typeID);
                        ++cnt;
                    }
                    StoHelp::writeCompound(server.getWrapperStorage(), cnt);
                    server.getWrapperStorage().writeStorage(tempContent);
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE,
                                                      "Get Induction Loop Variable: unsupported variable " + toHex(variable, 2)
                                                      + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_InductionLoop::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
        tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::VAR_PARAMETER
            && variable != libsumo::VAR_VIRTUAL_DETECTION
       ) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_INDUCTIONLOOP_VARIABLE, "Set Induction Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    // process
    try {
        switch (variable) {
            case libsumo::VAR_VIRTUAL_DETECTION: {
                double time = -1;
                if (!server.readTypeCheckingDouble(inputStorage, time)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_INDUCTIONLOOP_VARIABLE, "Setting time since last detection requires a double.", outputStorage);
                }
                libsumo::InductionLoop::overrideTimeSinceDetection(id, time);
                break;
            }
            case libsumo::VAR_PARAMETER: {
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting a parameter.");
                const std::string name = StoHelp::readTypedString(inputStorage, "The name of the parameter must be given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The value of the parameter must be given as a string.");
                libsumo::InductionLoop::setParameter(id, name, value);
                break;
            }
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_INDUCTIONLOOP_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_INDUCTIONLOOP_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
