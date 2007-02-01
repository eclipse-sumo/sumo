/****************************************************************************/
/// @file    RemoteServer.h
/// @author  Axel Wegener
/// @date    20 May 2006
/// @version $Id: $
///
// Remote control sumo via XmlRpc messages
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
#ifndef RemoteServer_h
#define RemoteServer_h
// ===========================================================================
// included modules
// ===========================================================================
#include "XmlRpc.h"

#include <iostream>
#include <stdlib.h>
#include <list>
#include <deque>
#include <map>
#include <string>
#include "microsim/MSVehicle.h"
#include "microsim/MSEdge.h"
#include "microsim/MSLane.h"
#include "utils/geom/Position2DVector.h"
#include "utils/geom/Position2D.h"
#include "utils/geom/GeomHelper.h"
#include "utils/geom/Line2D.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace XmlRpc;
using namespace std;

XmlRpcServer s;

class RemoteSimuStep : public XmlRpcServerMethod
{
public:
    RemoteSimuStep(XmlRpcServer *s): XmlRpcServerMethod("RemoteSimuStep",s)
    {}

    void execute(XmlRpcValue & params, XmlRpcValue & result)
    {
        int targetTime = int(params[0]);
        //do SimulationStep with parameter target time
        SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
        SUMOTime targetTimeStep = (SUMOTime)targetTime;
        MSNet::getInstance()->simulate(currentTime,(SUMOTime)targetTime);
        /*for(SUMOTime step = currentTime+1 ;step<=targetTimeStep;step++)
        {
        	MSNet::getInstance()->simulationStep(0,step);
        }
           */
        result = "true";
    }
};


class CloseServer : public XmlRpcServerMethod
{
public:
    CloseServer(XmlRpcServer *s): XmlRpcServerMethod("CloseServer",s)
    {}

    void execute(XmlRpcValue & params, XmlRpcValue & result)
    {
        s.exit();
        cout<<"Server closed!"<<endl;
        result = "true";
    }
};

class GetRoadPos : public XmlRpcServerMethod
{
public:
    GetRoadPos(XmlRpcServer *s): XmlRpcServerMethod("GetRoadPos",s)
    {}

#define HALF_ROAD_WIDTH 1.5 //so the width of lane is 3 meters.


    void roadPos(Position2D * p, XmlRpcValue & result)
    {
        bool IsPosInRoad = false;
        vector<MSEdge*> edgeVector = MSEdge::getEdgeVector();

        for (vector<MSEdge*>::iterator i=edgeVector.begin(); i!=edgeVector.end();i++) {
            MSEdge * myEdge = *i;
            MSEdge::LaneCont * myLanes = myEdge->getLanes();
            for (MSEdge::LaneCont::iterator laneIndex=myLanes->begin(); laneIndex!=myLanes->end();laneIndex++) {
                MSLane * myLane = *laneIndex;
                const Position2DVector & myShp = myLane->getShape();

                const Position2DVector::ContType & points = myShp.getCont();

                if (myShp.distance((*p)) <= HALF_ROAD_WIDTH) {
                    result[0] = myShp.nearest_position_on_line_to_point(*p);
                    result[1] = myEdge->getID();
                    result[2] = myLane->getID();
                    IsPosInRoad = true;
                    break;
                }
            }
            if (IsPosInRoad == true)
                break;
        }

        if (IsPosInRoad == false) {
            result[0] = -1.0;   //double
            result[1] = "not in road";
            result[2] = "not in lane";
        }
    }

    void execute(XmlRpcValue & params, XmlRpcValue & result)
    {

        double x = double(params[0]);
        double y = double(params[1]);
        Position2D *p = new Position2D(x,y);

        roadPos(p,result);
    }
};



class RemoteGetAllCarPositions : public XmlRpcServerMethod
{
public:
    RemoteGetAllCarPositions(XmlRpcServer *ss): XmlRpcServerMethod("RemoteGetAllCarPositions",ss)
    {}

    void execute(XmlRpcValue & params, XmlRpcValue & result)
    {//return the positions for all cars
        try {
            if (MSVehicle::myDict.size() == 0) {
                cout<<"There is no car in the sumo"<<endl;
                return;
            }
            int index = 0;
            for (MSVehicle::DictType::iterator i=MSVehicle::myDict.begin(); i!=MSVehicle::myDict.end(); i++) {
                MSVehicle *veh = (*i).second;
                if (veh->getState().pos()==0 && veh->getState().speed() == 0) {
                    //carCondition 0:no such car
                    //             1:car's pos and speed are both 0
                    //             2:normal car
                } else {
                    Position2D p2d = veh->getPosition();
                    result[index]["carCondition"] = 2;
                    result[index]["carID"]    = veh->getID();
                    result[index]["carPos"]   = veh->getPositionOnLane();
                    result[index]["carPos_X"] = p2d.x();
                    result[index]["carPos_Y"] = p2d.y();
                    result[index]["carSpeed"] = veh->getSpeed();
                    index++;
                }
            }//end for
        }//end try
        catch (...) {
            cout<<" some error happen in RemoteGetAllCarPositions.execute()"<<endl;
            //result["resultCode"] = -1;
            return;
        }
    }//end execute
};

class RemoteGetOneCarPos : public XmlRpcServerMethod
{
public:
    RemoteGetOneCarPos(XmlRpcServer *ss): XmlRpcServerMethod("RemoteGetOneCarPos",ss)
    {}

    void execute(XmlRpcValue & params, XmlRpcValue & result)
    {// return the car's positions whose ID is the "nodeId"
        string s = string(params);
        MSVehicle::DictType::iterator it = MSVehicle::myDict.find(s);
        if (it == MSVehicle::myDict.end()) {	// id not in myDict(car not find or no such car)
            result[0]["carCondition"] = 0;
            result[0]["carID"]    = 0;
            result[0]["carPos"]   = 0;
            result[0]["carPos_X"] = 0;
            result[0]["carPos_Y"] = 0;
            result[0]["carSpeed"] = 0;
            return;
        } else {
            MSVehicle *veh = (*it).second;
            if (veh->getState().pos()==0 && veh->getState().speed() == 0) {
                result[0]["carCondition"] = 1;  //the car is found,but the pos and speed are both zero.
                result[0]["carID"]   = veh->getID();
                result[0]["carPos"]   = 0;
                result[0]["carPos_X"] = 0;
                result[0]["carPos_Y"] = 0;
                result[0]["carSpeed"] = 0;
                return;
            }
            Position2D p2d = veh->position();
            result[0]["carCondition"] = 2;
            result[0]["carID"]    = veh->getID();
            result[0]["carPos"]   = veh->getPositionOnLane();
            result[0]["carPos_X"] = p2d.x();
            result[0]["carPos_Y"] = p2d.y();
            result[0]["carSpeed"] = veh->getSpeed();
        }
    }//end execute
};


class RemoteServer
{
public:

    RemoteServer(int port, SUMOTime endTime)
    {
        RemoteSimuStep *rss = new RemoteSimuStep(&s);
        CloseServer *cs = new CloseServer(&s);
        RemoteGetAllCarPositions *getAllPos = new RemoteGetAllCarPositions(&s);
        GetRoadPos *getRpos = new GetRoadPos(&s);
        RemoteGetOneCarPos *getOnePos = new RemoteGetOneCarPos(&s);

        XmlRpc::setVerbosity(1);
        s.bindAndListen(port);  // Create the server socket on the specified port
        s.enableIntrospection(true);  // Enable introspection
        s.work(-1.0);   // Wait for requests indefinitely

        this->net = MSNet::getInstance();

    }
    ~RemoteServer(void)
    {};

public:

private:
    MSNet *net;

};


#endif

/****************************************************************************/

