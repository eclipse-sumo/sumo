/****************************************************************************/
/// @file    remoteserver.h
/// @author  Thimor Bohn <bohn@itm.uni-luebeck.de>
/// @date    2007/03/13
/// @version $Id$
///
/// socket server used to control sumo by remote client
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
#ifndef REMOTESERVER_H
#define REMOTESERVER_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#define BUILD_TCPIP
#include "foreign/tcpip/storage.h"
#include "utils/common/SUMOTime.h"

#include "utils/geom/Boundary.h"
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
* @class remoteserver
*/
namespace itm
{
	class RemoteException
	{
	private:
		std::string what_;

	public:
		RemoteException( std::string what )
			: what_(what)
		{}
		std::string what() const
		{
			return what_;
		}
	};

	// RemoteServer
	// Allows communication of sumo with external program. The external
	// program will control sumo.
	// @author: Thimor Bohn <bohn@itm.uni-luebeck.de>
	class RemoteServer
	{
	public:
		// Constructor
		// Reads the needed parameters out of static OptionsCont
		RemoteServer();

		// Destructor
		// final cleanup
		virtual ~RemoteServer(void);

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
		void commandSetMaximumSpeed(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(RemoteException);

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
		void commandSimulationStep(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(RemoteException);

		void commandCloseConnection(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(RemoteException);

		void commandSimulationParameter(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) throw(RemoteException);

		void writeStatusCmd(tcpip::Storage& respMsg, int commandId, int status, std::string description);

		// port on which server is listening on
		int port_;

		// simulation end time
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

