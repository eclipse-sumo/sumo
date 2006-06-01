/***************************************************************************
                          RemoteServer.h
              Remote control sumo via XmlRpc messages
                             -------------------
    project              : SUMO
    subproject           : remote control
    begin                : 20 May 2006
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Axel Wegener
    email                : wegener@itm.uni-luebeck.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef RemoteServer_h
#define RemoteServer_h

//#pragma once

#include "/usr/local/include/xmlrpc/XmlRpc.h"

#include <iostream>
#include <stdlib.h>
#include <list>
#include <deque>
#include <map>
#include <string>
#include <microsim/MSVehicle.h>

using namespace XmlRpc;
using namespace std;

typedef std::map< std::string, MSVehicle* > DictType;
//class RemoteServer;
/*
class forrest : public XmlRpcServerMethod
{
public:
forrest(XmlRpcServer *ss): XmlRpcServerMethod("forrest",ss){}

void execute(XmlRpcValue & params, XmlRpcValue & result)
{
string s1 = (string)params[0];
result = s1+(string)params[1];

}
}forrest(&s);
*/

XmlRpcServer s;
//int nodeNumber;

/*
class SetCarsNumber : public XmlRpcServerMethod
{
public:
    SetCarsNumber(XmlRpcServer *s): XmlRpcServerMethod("SetCarsNumber",s){}

    void execute(XmlRpcValue & params, XmlRpcValue & result)
    {
    	 nodeNumber = int(params[0]);
    }
};
*/

class RemoteSimuStep : public XmlRpcServerMethod
{
public:
	RemoteSimuStep(XmlRpcServer *s): XmlRpcServerMethod("RemoteSimuStep",s){}

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
	CloseServer(XmlRpcServer *s): XmlRpcServerMethod("CloseServer",s){}

	void execute(XmlRpcValue & params, XmlRpcValue & result)
	{
		s.exit();

		//delete MSNet::getInstance();
		//delete SharedOutputDevices::getInstance();

		result = "true";

	}
};


class RemoteGetPositions : public XmlRpcServerMethod
{
public:
	RemoteGetPositions(XmlRpcServer *ss): XmlRpcServerMethod("RemoteGetPositions",ss){}

	void execute(XmlRpcValue & params, XmlRpcValue & result)
	{
		try
		{	
			int nodeId = int(params[0]);	
			if(nodeId == -1)
			{//return the positions for all cars
				//RemoteServer::carsNumber
				//cout<< nodeNumber <<"nnnnnnnnnnnnn"<<endl;
				/*
				for(int i = 0; i < nodeNumber; i++)
					{
						result[i]["carCondition"] = 0;
						result[i]["carID"]    = 0;
						result[i]["carPos"]   = 0;
						result[i]["carPos_X"] = 0;
						result[i]["carPos_Y"] = 0;
						result[i]["carSpeed"] = 0;
					}
					*/
				if(MSVehicle::myDict.size() == 0)
				{
					// There is no car in the sumo
					//result["resultCode"] = 0;
					return;
				}
				int index = 0;
				for(DictType::iterator i=MSVehicle::myDict.begin(); i!=MSVehicle::myDict.end(); i++) 
				{
					MSVehicle *veh = (*i).second;
					//int index = atoi(veh->getID().c_str());
					if(veh->getState().pos()==0 && veh->getState().speed() == 0)
					{
						//carCondition 0:no such car
						//             1:car's pos and speed are both 0
						//             2:normal car
						/*
						result[index]["carCondition"] = 1;
						result[index]["carID"]   = veh->getID();
						result[index]["carPos"]   = 0;
						result[index]["carPos_X"] = 0;
						result[index]["carPos_Y"] = 0;
						result[index]["carSpeed"] = 0;
						*/
					}
					else
					{
						Position2D p2d = veh->position();
						result[index]["carCondition"] = 2;
						result[index]["carID"]   =  veh->getID();
						result[index]["carPos"]   = veh->pos();
						result[index]["carPos_X"] = p2d.x();
						result[index]["carPos_Y"] = p2d.y();
						result[index]["carSpeed"] = veh->speed();
						index++;
					}		
					
					//cout<<endl<<index<<"     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<endl;
				}
				/*if (i == MSVehicle::myDict.end())
				{
				result[index] = "no car";
				return;
				}		*/
			}
			else
			{  // return the car's positions whose ID is the "nodeId" 
				//MSNet::getInstance()->myVehicleControl-			
				char buf[20];
				//itoa(nodeId, buf, 10); because linux has no itoa
        sprintf(buf, "%d\n", nodeId);
				//std::string s = new string(buf);
				string s(buf);
				DictType::iterator it = MSVehicle::myDict.find(s);
				if (it == MSVehicle::myDict.end()) 
				{	// id not in myDict(car not find or no such car)
					return;
				}else
				{
					MSVehicle *veh = (*it).second;

					if(veh->getState().pos()==0 && veh->getState().speed() == 0)
					{
						result[0]["carCondition"] = 1;  //the car is found,but the pos and speed are both zero.
						result[0]["carID"]   = veh->getID();
						result[0]["carPos"]   = 0;
						result[0]["carPos_X"] = 0;
						result[0]["carPos_Y"] = 0;
						result[0]["carSpeed"] = 0;
						return;
					}
					/*if(veh == NULL)
					{
					result = "no find";
					return;
					}*/
					Position2D p2d = veh->position();
					result[0]["carCondition"] = 2;
					result[0]["carID"]   = veh->getID();
					result[0]["carPos"]   = veh->pos();
					result[0]["carPos_X"] = p2d.x();
					result[0]["carPos_Y"] = p2d.y();
					result[0]["carSpeed"] = veh->speed();
				}

			}//end if
		}//end try
		catch (...)
		{     // -1 means some error happen
			//result["resultCode"] = -1;
			return;
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
		RemoteGetPositions *rgp = new RemoteGetPositions(&s);
		//SetCarsNumber *sCarNum = new SetCarsNumber(&s);

		//MSNet::getInstance()->initialiseSimulation();
		//MSNet::getInstance()->simulationStep(0,0);

		XmlRpc::setVerbosity(5);
		// Create the server socket on the specified port
		s.bindAndListen(port);
		// Enable introspection
		s.enableIntrospection(true);
		// Wait for requests indefinitely
		s.work(-1.0);

		this->net = MSNet::getInstance();
		//when SumoTime is 0; the cars' number is biggest
		//RemoteServer::TotalCarsNumber = MSVehicle::myDict.size();
		//nodeNumber = MSVehicle::myDict.size();
		//nodeNumber = 800;
	}
	~RemoteServer(void) {};
	
	
public:	
	//static int TotalCarsNumber;

private:
	MSNet *net;
	
};

#endif
