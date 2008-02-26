/****************************************************************************/
/// @file    TraCIServer.h
/// @author  Axel Wegener <wegener@itm.uni-luebeck.de>
/// @date    2007/10/24
/// @version $Id$
///
/// TraCI server used to control sumo by a remote TraCI client (e.g., ns2)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TRACISERVER_H
#define TRACISERVER_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef TRACI

#define BUILD_TCPIP
#include "foreign/tcpip/storage.h"
#include "utils/common/SUMOTime.h"

#include "utils/geom/Boundary.h"
#include "utils/geom/Position2D.h"
#include "utils/geom/GeomHelper.h"
#include "utils/options/OptionsCont.h"
#include "microsim/MSVehicle.h"

#include <map>
#include <string>

// ===========================================================================
// class declarations
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class TraCIServer
*/
namespace traci
{
class TraCIException
{
private:
    std::string what_;

public:
    TraCIException(std::string what)
            : what_(what) {}
    std::string what() const {
        return what_;
    }
};

// TraCIServer
// Allows communication of sumo with external program. The external
// program will control sumo.
class TraCIServer
{
public:

	struct RoadMapPos {
		std::string roadId;
		float pos;
		unsigned char laneId;
	};

    // Constructor
    // Reads the needed parameters out of static OptionsCont
    TraCIServer();

    // Destructor
    // final cleanup
    virtual ~TraCIServer(void);

    // start server
    void run();

private:

    bool dispatchCommand(tcpip::Storage& requestMsg, tcpip::Storage& respMsg);

    // process command setMaximumSpeed
    // This command causes the node given by nodeId to limit its speed to a maximum speed (float).
    // If maximum speed is set to a negative value, the individual speed limit for that node gets annihilated.
    // @param in contains unparsed parameters targetTime, ResultType
    // @param out contains node positions ready for output
    // @param length message length
    void commandSetMaximumSpeed(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    // process command simStep
    // This is the basic comman that encourage the mobility generator to simulate up to the given TargetTime.
    // Normaly, the network simulator sends this command every time unit to gain actual node positions.
    // Probably, the node positions can be desribed in a x- and y-position in a simualated 2D-world.
    // But node positions can be represented in many more ways, e.g. 2.5D, 3D or as points on a road network.
    // The desired representation of positions is given by the entry ResultType.
    // If more than one representation is needed, the command simulation step can be sent several times with the same Target Time and different ResultTypes.
    // @param in contains unparsed parameters targetTime, ResultType
    // @param out contains node positions ready for output
    // @param length message length
    void commandSimulationStep(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    void commandStopNode(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    void commandChangeLane(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    void commandChangeRoute(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    void commandChangeTarget(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    void commandCloseConnection(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    void commandSimulationParameter(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    // process command getTLStatus
    // The traffic light with the given id is asked for all state transitions, that will occur  within
    // a given time interval. Each status change is returned by a TLSwitch command.
    void commandGetTLStatus(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    // process command slowDown
    // Tell the node given by nodeID to slow down to the given speed (float) within the time intervall
    // given by duration. This simulates different methods of slowing down, like instant braking, coasting...
    // It's assumed that the speed reduction is linear
    // @param in contains nodeID(integer), speed (float), duration(double)
    void commandSlowDown(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    // command getAllTLIds
    // Returns a list of strings representing the ids of all traffic lights in the simulation
    void commandGetAllTLIds(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    void commandUpdateCalibrator(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

	void commandPositionConversion(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(TraCIException);

    void writeStatusCmd(tcpip::Storage& respMsg, int commandId, int status, std::string description);

	/**
	 * Converts a cartesian position to the closest road map position
	 * 
	 * @param pos	cartesian position that is to be converted
	 * @return the closest road map position to the cartesian position
	 */
	TraCIServer::RoadMapPos convertCartesianToRoadMap(Position2D pos);

    // port on which server is listening on
    int port_;

    // simulation begin and end time
    SUMOTime beginTime_;
    SUMOTime endTime_;

    // penetration rate, measurement of equipped vehicles in simulation
    float penetration_;

    // routeFile name of file which contains vehicle routes
    std::string routeFile_;

    // maps all internal vehicle ids to external id if equipped else to -1
    // Set isMapChanged_ to true, if altering this map
    std::map<std::string, int> equippedVehicles_;

    // maps all external vehicle ids to internal id
    // use method convertExt2IntId instead of accessing the map directly!!!
    std::map<int, std::string> ext2intId;
    bool isMapChanged_;
    void convertExt2IntId(int extId, std::string& intId);

    MSVehicle* getVehicleByExtId(int extId);

    // hold number of all equipped vehicles
    int numEquippedVehicles_;

    bool closeConnection_;

    Boundary* netBoundary_;
    const Boundary& getNetBoundary();


};
}


#endif

#endif
