/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2021 German Aerospace Center (DLR) and others.
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
/// @file    Simulation.cpp
/// @author  Laura Bieker-Walz
/// @author  Robert Hilbrich
/// @date    15.09.2017
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>
#include <cstdlib>

#include <foreign/tcpip/socket.h>
#define LIBTRACI 1
#include "Connection.h"
#include "Domain.h"
#include <libsumo/StorageHelper.h>
#include <libsumo/GUI.h>
#include <libsumo/Simulation.h>


namespace libtraci {

typedef Domain<libsumo::CMD_GET_SIM_VARIABLE, libsumo::CMD_SET_SIM_VARIABLE> Dom;


// ===========================================================================
// static member definitions
// ===========================================================================
std::pair<int, std::string>
Simulation::init(int port, int numRetries, const std::string& host, const std::string& label, FILE* const pipe) {
    Connection::connect(host, port, numRetries, label, pipe);
    switchConnection(label);
    return getVersion();
}


std::pair<int, std::string>
Simulation::start(const std::vector<std::string>& cmd, int port, int numRetries, const std::string& label, const bool verbose,
                  const std::string& /* traceFile */, bool /* traceGetters */, void* /* _stdout */) {
    if (port == -1) {
        port = tcpip::Socket::getFreeSocketPort();
    }
    std::ostringstream oss;
    for (const std::string& s : cmd) {
        oss << s << " ";
    }
    oss << "--remote-port " << port << " 2>&1";
#ifndef WIN32
    oss << " &";
#endif
    if (verbose) {
        std::cout << "Calling " << oss.str() << std::endl;
    }
#ifdef WIN32
    FILE* pipe = _popen(oss.str().c_str(), "r");
#else
    FILE* pipe = popen(oss.str().c_str(), "r");
#endif
    return init(port, numRetries, "localhost", label, pipe);
}


bool
Simulation::isLibsumo() {
    return false;
}


bool
Simulation::hasGUI() {
    try {
        GUI::getIDList();
        return true;
    } catch (libsumo::TraCIException&) {
        return false;
    }
}


void
Simulation::switchConnection(const std::string& label) {
    Connection::switchCon(label);
}


const std::string&
Simulation::getLabel() {
    return Connection::getActive().getLabel();
}


void
Simulation::setOrder(int order) {
    Connection::getActive().setOrder(order);
}


void
Simulation::load(const std::vector<std::string>& args) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_STRINGLIST);
    content.writeStringList(args);
    Connection::getActive().doCommand(libsumo::CMD_LOAD, -1, "", &content);
}


bool
Simulation::isLoaded() {
    return Connection::isActive();
}


void
Simulation::step(const double time) {
    Connection::getActive().simulationStep(time);
}


void
Simulation::close(const std::string& /* reason */) {
    Connection::getActive().close();
}


std::pair<int, std::string>
Simulation::getVersion() {
    tcpip::Storage& inMsg = Connection::getActive().doCommand(libsumo::CMD_GETVERSION, -1, "");
    inMsg.readUnsignedByte(); // msg length
    inMsg.readUnsignedByte(); // libsumo::CMD_GETVERSION again, see #7284
    const int traciVersion = inMsg.readInt(); // to fix evaluation order
    return std::make_pair(traciVersion, inMsg.readString());
}


int
Simulation::getCurrentTime() {
    return Dom::getInt(libsumo::VAR_TIME_STEP, "");
}


double
Simulation::getTime() {
    return Dom::getDouble(libsumo::VAR_TIME, "");
}


double
Simulation::getEndTime() {
    return Dom::getDouble(libsumo::VAR_END, "");
}


int
Simulation::getLoadedNumber() {
    return Dom::getInt(libsumo::VAR_LOADED_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getLoadedIDList() {
    return Dom::getStringVector(libsumo::VAR_LOADED_VEHICLES_IDS, "");
}


int
Simulation::getDepartedNumber() {
    return Dom::getInt(libsumo::VAR_DEPARTED_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getDepartedIDList() {
    return Dom::getStringVector(libsumo::VAR_DEPARTED_VEHICLES_IDS, "");
}


int
Simulation::getArrivedNumber() {
    return Dom::getInt(libsumo::VAR_ARRIVED_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getArrivedIDList() {
    return Dom::getStringVector(libsumo::VAR_ARRIVED_VEHICLES_IDS, "");
}


int
Simulation::getParkingStartingVehiclesNumber() {
    return Dom::getInt(libsumo::VAR_PARKING_STARTING_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getParkingStartingVehiclesIDList() {
    return Dom::getStringVector(libsumo::VAR_PARKING_STARTING_VEHICLES_IDS, "");
}


int
Simulation::getParkingEndingVehiclesNumber() {
    return Dom::getInt(libsumo::VAR_PARKING_ENDING_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getParkingEndingVehiclesIDList() {
    return Dom::getStringVector(libsumo::VAR_PARKING_ENDING_VEHICLES_IDS, "");
}


int
Simulation::getStopStartingVehiclesNumber() {
    return Dom::getInt(libsumo::VAR_STOP_STARTING_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getStopStartingVehiclesIDList() {
    return Dom::getStringVector(libsumo::VAR_STOP_STARTING_VEHICLES_IDS, "");
}


int
Simulation::getStopEndingVehiclesNumber() {
    return Dom::getInt(libsumo::VAR_STOP_ENDING_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getStopEndingVehiclesIDList() {
    return Dom::getStringVector(libsumo::VAR_STOP_ENDING_VEHICLES_IDS, "");
}


int
Simulation::getCollidingVehiclesNumber() {
    return Dom::getInt(libsumo::VAR_COLLIDING_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getCollidingVehiclesIDList() {
    return Dom::getStringVector(libsumo::VAR_COLLIDING_VEHICLES_IDS, "");
}


int
Simulation::getEmergencyStoppingVehiclesNumber() {
    return Dom::getInt(libsumo::VAR_EMERGENCYSTOPPING_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getEmergencyStoppingVehiclesIDList() {
    return Dom::getStringVector(libsumo::VAR_EMERGENCYSTOPPING_VEHICLES_IDS, "");
}


int
Simulation::getStartingTeleportNumber() {
    return Dom::getInt(libsumo::VAR_TELEPORT_STARTING_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getStartingTeleportIDList() {
    return Dom::getStringVector(libsumo::VAR_TELEPORT_STARTING_VEHICLES_IDS, "");
}


int
Simulation::getEndingTeleportNumber() {
    return Dom::getInt(libsumo::VAR_TELEPORT_ENDING_VEHICLES_NUMBER, "");
}


std::vector<std::string>
Simulation::getEndingTeleportIDList() {
    return Dom::getStringVector(libsumo::VAR_TELEPORT_ENDING_VEHICLES_IDS, "");
}


int
Simulation::getDepartedPersonNumber() {
    return Dom::getInt(libsumo::VAR_DEPARTED_PERSONS_NUMBER, "");
}


std::vector<std::string>
Simulation::getDepartedPersonIDList() {
    return Dom::getStringVector(libsumo::VAR_DEPARTED_PERSONS_IDS, "");
}


int
Simulation::getArrivedPersonNumber() {
    return Dom::getInt(libsumo::VAR_ARRIVED_PERSONS_NUMBER, "");
}


std::vector<std::string>
Simulation::getArrivedPersonIDList() {
    return Dom::getStringVector(libsumo::VAR_ARRIVED_PERSONS_IDS, "");
}


std::vector<std::string>
Simulation::getBusStopIDList() {
    return Dom::getStringVector(libsumo::VAR_BUS_STOP_ID_LIST, "");
}

int
Simulation::getBusStopWaiting(const std::string& stopID) {
    return Dom::getInt(libsumo::VAR_BUS_STOP_WAITING, stopID);
}

std::vector<std::string>
Simulation::getBusStopWaitingIDList(const std::string& stopID) {
    return Dom::getStringVector(libsumo::VAR_BUS_STOP_WAITING_IDS, stopID);
}


std::vector<std::string>
Simulation::getPendingVehicles() {
    return Dom::getStringVector(libsumo::VAR_PENDING_VEHICLES, "");
}

std::vector<libsumo::TraCICollision>
Simulation::getCollisions() {
    std::vector<libsumo::TraCICollision> result;
    return result;
}



double
Simulation::getDeltaT() {
    return Dom::getDouble(libsumo::VAR_DELTA_T, "");
}


libsumo::TraCIPositionVector
Simulation::getNetBoundary() {
    return Dom::getPolygon(libsumo::VAR_NET_BOUNDING_BOX, "");
}


int
Simulation::getMinExpectedNumber() {
    return Dom::getInt(libsumo::VAR_MIN_EXPECTED_VEHICLES, "");
}


libsumo::TraCIPosition
Simulation::convert2D(const std::string& edgeID, double pos, int laneIndex, bool toGeo) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 2);
    content.writeUnsignedByte(libsumo::POSITION_ROADMAP);
    content.writeString(edgeID);
    content.writeDouble(pos);
    content.writeUnsignedByte(laneIndex);
    content.writeUnsignedByte(libsumo::TYPE_UBYTE);
    content.writeUnsignedByte(toGeo ? libsumo::POSITION_LON_LAT : libsumo::POSITION_2D);
    return Dom::getPos(libsumo::POSITION_CONVERSION, "", &content, toGeo);
}


libsumo::TraCIPosition
Simulation::convert3D(const std::string& edgeID, double pos, int laneIndex, bool toGeo) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 2);
    content.writeUnsignedByte(libsumo::POSITION_ROADMAP);
    content.writeString(edgeID);
    content.writeDouble(pos);
    content.writeUnsignedByte(laneIndex);
    content.writeUnsignedByte(libsumo::TYPE_UBYTE);
    content.writeUnsignedByte(toGeo ? libsumo::POSITION_LON_LAT_ALT : libsumo::POSITION_3D);
    return Dom::getPos3D(libsumo::POSITION_CONVERSION, "", &content, toGeo);
}


libsumo::TraCIRoadPosition
Simulation::convertRoad(double x, double y, bool isGeo, const std::string& vClass) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 3);
    content.writeUnsignedByte(isGeo ? libsumo::POSITION_LON_LAT : libsumo::POSITION_2D);
    content.writeDouble(x);
    content.writeDouble(y);
    content.writeUnsignedByte(libsumo::TYPE_UBYTE);
    content.writeUnsignedByte(libsumo::POSITION_ROADMAP);
    StoHelp::writeTypedString(content, vClass);
    tcpip::Storage& ret = Dom::get(libsumo::POSITION_CONVERSION, "", &content, libsumo::POSITION_ROADMAP);
    libsumo::TraCIRoadPosition result;
    result.edgeID = ret.readString();
    result.pos = ret.readDouble();
    result.laneIndex = ret.readByte();
    return result;
}


libsumo::TraCIPosition
Simulation::convertGeo(double x, double y, bool fromGeo) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 2);
    content.writeUnsignedByte(fromGeo ? libsumo::POSITION_LON_LAT : libsumo::POSITION_2D);
    content.writeDouble(x);
    content.writeDouble(y);
    content.writeUnsignedByte(libsumo::TYPE_UBYTE);
    content.writeUnsignedByte(fromGeo ? libsumo::POSITION_2D : libsumo::POSITION_LON_LAT);
    return Dom::getPos(libsumo::POSITION_CONVERSION, "", &content, !fromGeo);
}


double
Simulation::getDistance2D(double x1, double y1, double x2, double y2, bool isGeo, bool isDriving) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 3);
    content.writeUnsignedByte(isGeo ? libsumo::POSITION_LON_LAT : libsumo::POSITION_2D);
    content.writeDouble(x1);
    content.writeDouble(y1);
    content.writeUnsignedByte(isGeo ? libsumo::POSITION_LON_LAT : libsumo::POSITION_2D);
    content.writeDouble(x2);
    content.writeDouble(y2);
    content.writeUnsignedByte(isDriving ? libsumo::REQUEST_DRIVINGDIST : libsumo::REQUEST_AIRDIST);
    return Dom::getDouble(libsumo::DISTANCE_REQUEST, "", &content);
}


double
Simulation::getDistanceRoad(const std::string& edgeID1, double pos1, const std::string& edgeID2, double pos2, bool isDriving) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 3);
    content.writeUnsignedByte(libsumo::POSITION_ROADMAP);
    content.writeString(edgeID1);
    content.writeDouble(pos1);
    content.writeUnsignedByte(0);
    content.writeUnsignedByte(libsumo::POSITION_ROADMAP);
    content.writeString(edgeID2);
    content.writeDouble(pos2);
    content.writeUnsignedByte(0);
    content.writeUnsignedByte(isDriving ? libsumo::REQUEST_DRIVINGDIST : libsumo::REQUEST_AIRDIST);
    return Dom::getDouble(libsumo::DISTANCE_REQUEST, "", &content);
}


libsumo::TraCIStage
Simulation::findRoute(const std::string& fromEdge, const std::string& toEdge, const std::string& vType, const double depart, const int routingMode) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 5);
    StoHelp::writeTypedString(content, fromEdge);
    StoHelp::writeTypedString(content, toEdge);
    StoHelp::writeTypedString(content, vType);
    StoHelp::writeTypedDouble(content, depart);
    StoHelp::writeTypedInt(content, routingMode);
    return Dom::getTraCIStage(libsumo::FIND_ROUTE, "", &content);
}


std::vector<libsumo::TraCIStage>
Simulation::findIntermodalRoute(const std::string& fromEdge, const std::string& toEdge,
                                const std::string& modes, double depart, const int routingMode, double speed, double walkFactor,
                                double departPos, double arrivalPos, const double departPosLat,
                                const std::string& pType, const std::string& vType, const std::string& destStop) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 13);
    StoHelp::writeTypedString(content, fromEdge);
    StoHelp::writeTypedString(content, toEdge);
    StoHelp::writeTypedString(content, modes);
    StoHelp::writeTypedDouble(content, depart);
    StoHelp::writeTypedInt(content, routingMode);
    StoHelp::writeTypedDouble(content, speed);
    StoHelp::writeTypedDouble(content, walkFactor);
    StoHelp::writeTypedDouble(content, departPos);
    StoHelp::writeTypedDouble(content, arrivalPos);
    StoHelp::writeTypedDouble(content, departPosLat);
    StoHelp::writeTypedString(content, pType);
    StoHelp::writeTypedString(content, vType);
    StoHelp::writeTypedString(content, destStop);
    tcpip::Storage& result = Dom::get(libsumo::FIND_INTERMODAL_ROUTE, "", &content);
    int numStages = result.readInt();
    std::vector<libsumo::TraCIStage> ret;
    while (numStages-- > 0) {
        libsumo::TraCIStage s;
        StoHelp::readCompound(result, 13);
        s.type = StoHelp::readTypedInt(result);
        s.vType = StoHelp::readTypedString(result);
        s.line = StoHelp::readTypedString(result);
        s.destStop = StoHelp::readTypedString(result);
        s.edges = StoHelp::readTypedStringList(result);
        s.travelTime = StoHelp::readTypedDouble(result);
        s.cost = StoHelp::readTypedDouble(result);
        s.length = StoHelp::readTypedDouble(result);
        s.intended = StoHelp::readTypedString(result);
        s.depart = StoHelp::readTypedDouble(result);
        s.departPos = StoHelp::readTypedDouble(result);
        s.arrivalPos = StoHelp::readTypedDouble(result);
        s.description = StoHelp::readTypedString(result);
        ret.emplace_back(s);
    }
    return ret;
}

LIBTRACI_PARAMETER_IMPLEMENTATION(Simulation, SIM)

void
Simulation::clearPending(const std::string& routeID) {
    Dom::setString(libsumo::CMD_CLEAR_PENDING_VEHICLES, "", routeID);
}


void
Simulation::saveState(const std::string& fileName) {
    Dom::setString(libsumo::CMD_SAVE_SIMSTATE, "", fileName);
}

double
Simulation::loadState(const std::string& fileName) {
    Dom::setString(libsumo::CMD_LOAD_SIMSTATE, "", fileName);
    return 0.;
}

void
Simulation::writeMessage(const std::string& msg) {
    Dom::setString(libsumo::CMD_MESSAGE, "", msg);
}


void
Simulation::subscribe(const std::vector<int>& varIDs, double begin, double end, const libsumo::TraCIResults& params) {
    libtraci::Connection::getActive().subscribe(libsumo::CMD_SUBSCRIBE_SIM_VARIABLE, "", begin, end, -1, -1, varIDs, params);
}


const libsumo::TraCIResults
Simulation::getSubscriptionResults() {
    return libtraci::Connection::getActive().getAllSubscriptionResults(libsumo::RESPONSE_SUBSCRIBE_SIM_VARIABLE)[""];
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(Simulation, SIM)

}


/****************************************************************************/
