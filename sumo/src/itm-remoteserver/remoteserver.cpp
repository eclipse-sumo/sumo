// RemoteServer.cpp
// @author: Thimor Bohn <bohn@itm.uni-luebeck.de

#include "itm-remoteserver/remoteconstants.h"
#include "itm-remoteserver/remoteserver.h"
#include "itm-remoteserver/socket.h"
#include "itm-remoteserver/storage.h"
#include "utils/common/SUMOTime.h"
#include "microsim/MSNet.h"
#include "microsim/MSVehicleControl.h"
#include "microsim/MSVehicle.h"
#include "utils/geom/Position2D.h"
#include "microsim/MSEdge.h"

#include <string>
#include <map>
#include <iostream>
#include <cstdlib>

using namespace std;

namespace itm
  {
  /*****************************************************************************/

  RemoteServer::RemoteServer(int port, SUMOTime endTime, float penetration)
      : port_(port), endTime_(endTime), penetration_(penetration), numEquippedVehicles_(0)
  {}

  /*****************************************************************************/

  RemoteServer::~RemoteServer()
  {}

  /*****************************************************************************/

  void
  RemoteServer::run()
  {
    try
      {
        Socket socket(port_);
        socket.accept();

        bool running = true;

        while (running)
          {
	    Storage in;
	    Storage out;
            socket.receiveExact(in);
            unsigned char cmd = in.readChar();
            switch (cmd)
              {
              case CMD_SIMSTEP:
                simStep(in, out);
                break;

              case CMD_CLOSE:
                running = false;
                out.writeChar(RTYPE_ERR);
                out.writeString("sumo shut down");
                break;

              default:
                running = false;
                out.writeChar(RTYPE_ERR);
                out.writeString("unknown command received. sumo shut down.");
                cerr << "unkown command received. sumo shut down" << endl;
                break;
              }

            socket.sendExact(out);
          }
      }
    catch (RemoteException e)
      {
        cerr << e.what() << endl;
      }
    catch (SocketException e)
      {
        cerr << e.what() << endl;
      }
  }

  /*****************************************************************************/

  void
  RemoteServer::simStep(Storage &in, Storage &out)
  throw (RemoteException)
  {
    // prepare out
    out.reset();
    // check parameters
    int targetTime = in.readInt();
    if (targetTime > endTime_)
      {
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
    try
      {
        // map containing all active equipped vehicles. maps external id to MSVehicle*
        map<int, const MSVehicle*> activeEquippedVehicles;
        // get access to all vehicles in simulation
        MSVehicleControl &vehControl = net->getVehicleControl();
        // iterate over all vehicles in simulation
        for (map<string, MSVehicle*>::const_iterator iter = vehControl.loadedVehBegin(); iter != vehControl.loadedVehEnd(); ++iter)
          {
            // selected vehicle
            const string vehicleId   = (*iter).first;
            const MSVehicle *vehicle = (*iter).second;
            // insert into equippedVehicleId if not contained
            if (equippedVehicles_.find(vehicleId) == equippedVehicles_.end())
              {
                // determine if vehicle is equipped
                double rnd = double(rand())/RAND_MAX;
                if (rnd <= penetration_)
                  {
                    // vehicle is equipped
                    equippedVehicles_[vehicleId] = numEquippedVehicles_++;
                  }
                else
                  {
                    // vehicle is not equipped
                    equippedVehicles_[vehicleId] = -1;
                  }
              }
            if (equippedVehicles_[vehicleId] >= 0)
              {
                int extId = equippedVehicles_[vehicleId];
                activeEquippedVehicles[extId] = vehicle;
                // vehicle is equipped
              }
          }

        out.writeChar( static_cast<unsigned char>( (rtype_abs?RTYPE_ABS:0) + (rtype_rel?RTYPE_REL:0) ) );
        out.writeInt(numEquippedVehicles_);
        // iterate over all active equipped vehicles
        for (map<int, const MSVehicle*>::iterator iter = activeEquippedVehicles.begin(); iter != activeEquippedVehicles.end(); ++iter)
          {
            int extId = (*iter).first;
            out.writeInt(extId);
            const MSVehicle* vehicle = (*iter).second;
            if (rtype_abs)
              {
                Position2D pos = vehicle->getPosition();
                out.writeFloat(pos.x());
                out.writeFloat(pos.y());
              }
            if (rtype_rel)
              {
                out.writeString(vehicle->getEdge()->getID());
                out.writeFloat(vehicle->getPositionOnLane());
              }
          }

      }
    catch (...)
      {
        out.writeChar(RTYPE_ERR);
        out.writeString("some error happen in command: simulation step. "
                        "sumo shut down");
        throw new RemoteException("some error happen in command: simulation step.");
        return;
      }

    return;
  }

  /*****************************************************************************/
}
