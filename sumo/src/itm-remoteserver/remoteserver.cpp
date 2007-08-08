/****************************************************************************/
/// @file    remoteserver.cpp
/// @author  Thimor Bohn <bohn@itm.uni-luebeck.de>
/// @date    2007/03/13
/// @version $Id$
///
/// socket server user to control sumo by remote client
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
// RemoteServer.cpp
// @author: Thimor Bohn <bohn@itm.uni-luebeck.de
// ===========================================================================
// compiler pragmas
// ===========================================================================


// ===========================================================================
// included modules
// ===========================================================================
#include "remoteconstants.h"
#include "remoteserver.h"
#include "foreign/tcpip/socket.h"
#include "foreign/tcpip/storage.h"
#include "utils/common/SUMOTime.h"
#include "microsim/MSNet.h"
#include "microsim/MSVehicleControl.h"
#include "microsim/MSVehicle.h"
#include "utils/geom/Position2D.h"
#include "microsim/MSEdge.h"
#include "microsim/MSRouteHandler.h"
#include "microsim/MSRouteLoaderControl.h"
#include "microsim/MSRouteLoader.h"

#include <string>
#include <map>
#include <iostream>
#include <cstdlib>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace tcpip;


// ===========================================================================
// method definitions
// ===========================================================================
namespace itm {
/*****************************************************************************/

RemoteServer::RemoteServer(int port, SUMOTime endTime, float penetration, string routeFile)
        : port_(port), endTime_(endTime), penetration_(penetration), 
	  routeFile_(routeFile), numEquippedVehicles_(0) {}

/*****************************************************************************/

RemoteServer::~RemoteServer() {}

/*****************************************************************************/

void
RemoteServer::run() {
    try {
        Socket socket(port_);
        socket.accept();

        bool running = true;

        while (running) {
            Storage in;
            Storage out;
            socket.receiveExact(in);
            unsigned char cmd = in.readChar();
            switch (cmd) {
	    
            case CMD_SIMINFO:
	        cout << "case CMD_SIMINFO:" << endl;
                simInfo(out);
                break;
            
            case CMD_SIMSTEP:
	        cout << "case CMD_SIMSTEP:" << endl;
                simStep(in, out);
                break;

	    case CMD_CLOSE:
                cout << "case CMD_CLOSE:" << endl;
                running = false;
                out.writeChar(RTYPE_ERR);
                out.writeString("sumo shut down");
                break;

	    default:
                cout << "default:" << endl;
                running = false;
                out.writeChar(RTYPE_ERR);
                out.writeString("unknown command received. sumo shut down.");
                cerr << "unkown command received. sumo shut down" << endl;
                break;
            }
	    
	    cout << "socket.sendExact(out);" << endl;
            socket.sendExact(out);
        }
    } catch (RemoteException e) {
        cerr << e.what() << endl;
    } catch (SocketException e) {
        cerr << e.what() << endl;
    }
}

/*****************************************************************************/

void
RemoteServer::simStep(tcpip::Storage &in, tcpip::Storage &out)
throw (RemoteException) {
    // prepare out
    out.reset();
    // check parameters
    int targetTime = in.readInt();
    if (targetTime > endTime_) {
        out.writeChar(RTYPE_ERR);
        out.writeString("targetTime > endTime. sumo shut down");
        cerr << "targetTime > endTime. sumo shut down" << endl;
        throw new RemoteException("targerTime > endTime");
        return;
    }
    unsigned char rtype = in.readChar();
    bool rtype_abs      = ((rtype & RTYPE_ABS) == RTYPE_ABS);
    bool rtype_rel      = ((rtype & RTYPE_REL) == RTYPE_REL);

    // do simulation step
    MSNet *net = MSNet::getInstance();
    SUMOTime currentTime    = net->getCurrentTimeStep();
    SUMOTime targetTimeStep = static_cast<SUMOTime>(targetTime);
    net->simulate(currentTime, targetTimeStep);

    // prepare output
    try {
        // map containing all active equipped vehicles. maps external id to MSVehicle*
        map<int, const MSVehicle*> activeEquippedVehicles;
        // get access to all vehicles in simulation
        MSVehicleControl &vehControl = net->getVehicleControl();
        // iterate over all vehicles in simulation
        for (map<string, MSVehicle*>::const_iterator iter = vehControl.loadedVehBegin(); iter != vehControl.loadedVehEnd(); ++iter) {
            // selected vehicle
            const string vehicleId   = (*iter).first;
            const MSVehicle *vehicle = (*iter).second;
            // insert into equippedVehicleId if not contained
            if (equippedVehicles_.find(vehicleId) == equippedVehicles_.end()) {
                // determine if vehicle is equipped
                double rnd = double(rand())/RAND_MAX;
                if (rnd <= penetration_) {
                    // vehicle is equipped
                    equippedVehicles_[vehicleId] = numEquippedVehicles_++;
                } else {
                    // vehicle is not equipped
                    equippedVehicles_[vehicleId] = -1;
                }
            }
            if (equippedVehicles_[vehicleId] >= 0) {
                int extId = equippedVehicles_[vehicleId];
                activeEquippedVehicles[extId] = vehicle;
                // vehicle is equipped
            }
        }

        out.writeChar( static_cast<unsigned char>( (rtype_abs?RTYPE_ABS:0) + (rtype_rel?RTYPE_REL:0) ) );
        out.writeInt(numEquippedVehicles_);
        // iterate over all active equipped vehicles
        for (map<int, const MSVehicle*>::iterator iter = activeEquippedVehicles.begin(); iter != activeEquippedVehicles.end(); ++iter) {
            int extId = (*iter).first;
            out.writeInt(extId);
            const MSVehicle* vehicle = (*iter).second;
            if (rtype_abs) {
                Position2D pos = vehicle->getPosition();
                out.writeFloat(pos.x());
                out.writeFloat(pos.y());
		out.writeFloat(vehicle->getSpeed());
		cout << extId << ", " << pos.x() << ", " << pos.y() << ", " << vehicle->getSpeed() << endl;
            }
            if (rtype_rel) {
                out.writeString(vehicle->getEdge()->getID());
                out.writeFloat(vehicle->getPositionOnLane());
            }
        }

    } catch (...) {
        out.writeChar(RTYPE_ERR);
        out.writeString("some error happen in command: simulation step. "
                        "sumo shut down");
        throw new RemoteException("some error happen in command: simulation step.");
        return;
    }

    return;
}

/*****************************************************************************/

void
RemoteServer::simInfo(tcpip::Storage &out)
throw (RemoteException) {
    // prepare out
    cout << "out.reset();" << endl;
    out.reset();
    
    /*
    cout << "MSVehicleControl vc;" << endl;
    MSVehicleControl vc;
    
    cout << "MSRouteHandler rh(routeFile_, vc, true, false, 0, 0);" << endl;
    MSRouteHandler rh(routeFile_, vc, true, false, 0, 0);
    
    cout << "MSNet *net = MSNet::getInstance();" << endl;
    MSNet *net = MSNet::getInstance();
    
    cout << "MSRouteLoader rl(*net, &rh);" << endl;
    MSRouteLoader rl(*net, &rh);
    
    cout << "vector<MSRouteLoader*> lv;" << endl;
    vector<MSRouteLoader*> lv;
    
    cout << "lv.push_back(&rl);" << endl;
    lv.push_back(&rl);
    
    cout << "MSRouteLoaderControl rlc(*net, 0, lv);" << endl;
    MSRouteLoaderControl rlc(*net, 0, lv);
    
    cout << "rlc.loadNext(endTime_);" << endl;
    rlc.loadNext(endTime_);
    
    cout << "size_t numVehicles = rlc.myVehCont.size();" << endl;
    size_t numVehicles = rlc.myVehCont.size();
    cout << " *** numVehicles = " << numVehicles << endl;
    */
    
    size_t numVehicles = 21;
    cout << "out.writeChar(RTYPE_NONE);" << endl;
    out.writeChar(RTYPE_NONE);
    
    cout << "out.writeInt(numVehicles);" << endl;
    out.writeInt(static_cast<int>(numVehicles));
    
    /*
    cout << "delete net;" << endl;
    delete net;
    */
}

/*****************************************************************************/
}
