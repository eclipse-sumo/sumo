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
/// @file    TraCIServerAPI_Calibrator.cpp
/// @author  Jakob Erdmann
/// @date    16.03.2020
///
// APIs for getting/setting Calibrator values via TraCI
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/trigger/MSCalibrator.h>
#include <libsumo/Calibrator.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/StorageHelper.h>
#include "TraCIServerAPI_Calibrator.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Calibrator::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                      tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(libsumo::RESPONSE_GET_CALIBRATOR_VARIABLE, variable, id);
    try {
        if (!libsumo::Calibrator::handleVariable(id, variable, &server, &inputStorage)) {
            return server.writeErrorStatusCmd(libsumo::CMD_GET_CALIBRATOR_VARIABLE, "Get Calibrator Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_CALIBRATOR_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_GET_CALIBRATOR_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_Calibrator::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                      tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::CMD_SET_FLOW && variable != libsumo::VAR_PARAMETER) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_CALIBRATOR_VARIABLE, "Change Calibrator State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();

    try {
        // process
        switch (variable) {
            case libsumo::CMD_SET_FLOW: {
                StoHelp::readCompound(inputStorage, 8, "A compound object of size 8 is needed for setting calibrator flow.");
                const double begin = StoHelp::readTypedDouble(inputStorage, "Setting flow requires the begin time as the first (double) value.");
                const double end = StoHelp::readTypedDouble(inputStorage, "Setting flow requires the end time as the second (double) value.");
                const double vehsPerHour = StoHelp::readTypedDouble(inputStorage, "Setting flow requires the number of vehicles per hour as the third (double) value.");
                const double speed = StoHelp::readTypedDouble(inputStorage, "Setting flow requires the speed as the fourth (double) value.");
                const std::string typeID = StoHelp::readTypedString(inputStorage, "Setting flow requires the type id as the fifth (string) value.");
                const std::string routeID = StoHelp::readTypedString(inputStorage, "Setting flow requires the route id as the sixth (string) value.");
                const std::string departLane = StoHelp::readTypedString(inputStorage, "Setting flow requires the departLane as the seventh (string) value.");
                const std::string departSpeed = StoHelp::readTypedString(inputStorage, "Setting flow requires the departSpeed as the eigth (string) value.");
                libsumo::Calibrator::setFlow(id, begin, end, vehsPerHour, speed, typeID, routeID, departLane, departSpeed);
                break;
            }
            case libsumo::VAR_PARAMETER: {
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting a parameter.");
                const std::string name = StoHelp::readTypedString(inputStorage, "The name of the parameter must be given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The value of the parameter must be given as a string.");
                libsumo::Calibrator::setParameter(id, name, value);
                break;
            }
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_CALIBRATOR_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_CALIBRATOR_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
