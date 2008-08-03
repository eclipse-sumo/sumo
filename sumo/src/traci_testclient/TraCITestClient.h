/****************************************************************************/
/// @file    TraCITestClient.h
/// @author  Friedemann Wesner <wesner@itm.uni-luebeck.de>
/// @date    2008/04/07
///
/// A dummy client to simulate communication to a TraCI server
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

namespace testclient
{
// ===========================================================================
// Definitions
// ===========================================================================
struct Position2D {
	float x;
	float y;
};

struct Position3D {
	float x;
	float y;
	float z;
};

struct PositionRoadMap {
	std::string roadId;
	float pos;
	int laneId;
};

struct BoundingBox {
	Position2D lowerLeft;
	Position2D upperRight;
};

typedef std::vector<Position2D> Polygon;

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
	void commandSimulationStep(double time, int posFormat);

	void commandSetMaximumSpeed(int nodeId, float speed);

	void commandStopNode(int nodeId, testclient::Position2D pos, float radius, double waitTime);
	void commandStopNode(int nodeId, testclient::Position3D pos, float radius, double waitTime);
	void commandStopNode(int nodeId, testclient::PositionRoadMap pos, float radius, double waitTime);

	void commandChangeLane(int nodeId, int laneId, float fixTime);

	void commandSlowDown(int nodeId, float minSpeed, double timeInterval);

	void commandChangeRoute(int nodeId, std::string roadId, double travelTime);

	void commandChangeTarget(int nodeId, std::string roadId);

	void commandPositionConversion(testclient::Position2D pos, int posId);
	void commandPositionConversion(testclient::Position3D pos, int posId);
	void commandPositionConversion(testclient::PositionRoadMap pos, int posId);

	void commandDistanceRequest(testclient::Position2D pos1, testclient::Position2D pos2, int flag);
	void commandDistanceRequest(testclient::Position3D pos1, testclient::Position3D pos2, int flag);
	void commandDistanceRequest(testclient::Position2D pos1, testclient::Position3D pos2, int flag);
	void commandDistanceRequest(testclient::Position3D pos1, testclient::Position2D pos2, int flag);
	void commandDistanceRequest(testclient::PositionRoadMap pos1, testclient::PositionRoadMap pos2, int flag);
	void commandDistanceRequest(testclient::PositionRoadMap pos1, testclient::Position2D pos2, int flag);
	void commandDistanceRequest(testclient::PositionRoadMap pos1, testclient::Position3D pos2, int flag);
	void commandDistanceRequest(testclient::Position2D pos1, testclient::PositionRoadMap pos2, int flag);
	void commandDistanceRequest(testclient::Position3D pos1, testclient::PositionRoadMap pos2, int flag);

	void commandScenario(int flag, int domain, int domainId, int variable, int valueDataType);
	void commandScenario(int flag, int domain, int domainId, int variable, std::string stringVal);
	void commandScenario(int flag, int domain, int domainId, int variable, testclient::Position3D pos3dVal);
	void commandScenario(int flag, int domain, int domainId, int variable, testclient::PositionRoadMap roadPosVal);

	void commandGetTLStatus(int tlId, double intervalStart, double intervalEnd);

	void commandClose();

private:
	void writeResult();

	void errorMsg(std::stringstream& msg);

	void commandStopNode(int nodeId, testclient::Position2D* pos2D,
									testclient::Position3D* pos3D,
									testclient::PositionRoadMap* posRoad, 
									float radius, double waitTime);

	void commandPositionConversion(testclient::Position2D* pos2D,
									testclient::Position3D* pos3D,
									testclient::PositionRoadMap* posRoad,
									int posId);

	void commandDistanceRequest(testclient::Position2D* pos1_2D, 
								testclient::Position3D* pos1_3D,
								testclient::PositionRoadMap* pos1_Road,
								testclient::Position2D* pos2_2D,
								testclient::Position3D* pos2_3D,
								testclient::PositionRoadMap* pos2_Road,
								int flag);

	void commandScenario(int flag, int domain, int domainId, int variable, int valueDataType,
						 int* intVal, int* byteVal, int* ubyteVal, 
						 float* floatVal, double* doubleVal, std::string* stringVal,
						 testclient::Position3D* pos3dVal, testclient::PositionRoadMap* roadPosVal,
						 testclient::BoundingBox* boxVal, testclient::Polygon* polyVal,
						 testclient::TLPhaseList* tlphaseVal);

	// validation of received command responses
	bool reportResultState(tcpip::Storage& inMsg, int command);

	bool validateSimulationStep(tcpip::Storage& inMsg);

	bool validateStopNode(tcpip::Storage& inMsg);

	bool validatePositionConversion(tcpip::Storage& inMsg);

	bool validateDistanceRequest(tcpip::Storage& inMsg);

	bool validateScenario(tcpip::Storage& inMsg);

	bool validateGetTLStatus(tcpip::Storage& inMsg);

private:
	tcpip::Socket* socket;

	std::string outputFileName;

	std::stringstream answerLog;
};

}



#endif