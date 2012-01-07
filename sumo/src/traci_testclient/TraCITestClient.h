/****************************************************************************/
/// @file    TraCITestClient.h
/// @author  Friedemann Wesner
/// @author  Daniel Krajzewicz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @date    2008/04/07
/// @version $Id$
///
/// A dummy client to simulate communication to a TraCI server
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TRACITESTCLIENT_H
#define TRACITESTCLIENT_H

// ===========================================================================
// included modules
// ===========================================================================
#include <string>
#include <sstream>
#include <vector>

#include <foreign/tcpip/socket.h>
#include <utils/common/SUMOTime.h>

namespace testclient {
// ===========================================================================
// Definitions
// ===========================================================================
struct Position {
    SUMOReal x;
    SUMOReal y;
};

struct Position3D {
    SUMOReal x;
    SUMOReal y;
    SUMOReal z;
};

struct PositionRoadMap {
    std::string roadId;
    SUMOReal pos;
    int laneId;
};

struct BoundingBox {
    Position lowerLeft;
    Position upperRight;
};

typedef std::vector<Position> Polygon;

struct TLPhase {
    std::string precRoadId;
    std::string succRoadId;
    int phase;
};

typedef std::vector<TLPhase> TLPhaseList;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCITestClient
 */
class TraCITestClient {

public:
    TraCITestClient(std::string outputFileName = "testclient_result.out");

    ~TraCITestClient();

    bool run(std::string fileName, int port, std::string host = "localhost");

    bool connect(int port, std::string host = "localhost");

    bool close();

    // simulation commands
    void commandSimulationStep2(SUMOTime time);

    void commandPositionConversion(testclient::Position pos, int posId);
    void commandPositionConversion(testclient::Position3D pos, int posId);
    void commandPositionConversion(testclient::PositionRoadMap pos, int posId);

    void commandDistanceRequest(testclient::Position pos1, testclient::Position pos2, int flag);
    void commandDistanceRequest(testclient::Position3D pos1, testclient::Position3D pos2, int flag);
    void commandDistanceRequest(testclient::Position pos1, testclient::Position3D pos2, int flag);
    void commandDistanceRequest(testclient::Position3D pos1, testclient::Position pos2, int flag);
    void commandDistanceRequest(testclient::PositionRoadMap pos1, testclient::PositionRoadMap pos2, int flag);
    void commandDistanceRequest(testclient::PositionRoadMap pos1, testclient::Position pos2, int flag);
    void commandDistanceRequest(testclient::PositionRoadMap pos1, testclient::Position3D pos2, int flag);
    void commandDistanceRequest(testclient::Position pos1, testclient::PositionRoadMap pos2, int flag);
    void commandDistanceRequest(testclient::Position3D pos1, testclient::PositionRoadMap pos2, int flag);

    void commandGetVariable(int domID, int varID, const std::string& objID);
    void commandGetVariablePlus(int domID, int varID, const std::string& objID, std::ifstream& defFile);
    void commandSubscribeVariable(int domID, const std::string& objID, int beginTime, int endTime, int varNo, std::ifstream& defFile);
    void commandSetValue(int domID, int varID, const std::string& objID, std::ifstream& defFile);

    void commandClose();

private:
    void writeResult();

    void errorMsg(std::stringstream& msg);

    void commandPositionConversion(testclient::Position* pos2D,
                                   testclient::Position3D* pos3D,
                                   testclient::PositionRoadMap* posRoad,
                                   int posId);

    void commandDistanceRequest(testclient::Position* pos1_2D,
                                testclient::Position3D* pos1_3D,
                                testclient::PositionRoadMap* pos1_Road,
                                testclient::Position* pos2_2D,
                                testclient::Position3D* pos2_3D,
                                testclient::PositionRoadMap* pos2_Road,
                                int flag);

    // validation of received command responses
    bool reportResultState(tcpip::Storage& inMsg, int command, bool ignoreCommandId = false);

    bool validateSimulationStep2(tcpip::Storage& inMsg);
    bool validateSubscription(tcpip::Storage& inMsg);

    bool validatePositionConversion(tcpip::Storage& inMsg);

    bool validateDistanceRequest(tcpip::Storage& inMsg);

    bool readAndReportTypeDependent(tcpip::Storage& inMsg, int valueDataType);


    /** @brief Parses the next value type / value pair from the stream and inserts it into the storage
     *
     * @param[out] into The storage to add the value type and the value into
     * @param[in] defFile The file to read the values from
     * @param[out] msg If any error occurs, this should be filled
     * @return The number of written bytes
     */
    int setValueTypeDependant(tcpip::Storage& into, std::ifstream& defFile, std::stringstream& msg);



private:
    tcpip::Socket* socket;

    std::string outputFileName;

    std::stringstream answerLog;
};

}

#endif
