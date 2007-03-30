/****************************************************************************/
/// @file    RORDLoader_Artemis.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 12 Mar 2003
/// @version $Id$
///
// A handler for Artemis-files
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
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream> // !!! debug only
#include <fstream>
#include <sstream>
#include <utils/importio/LineHandler.h>
#include <utils/gfx/RGBColor.h>
#include <utils/options/OptionsCont.h>
#include <utils/router/IDSupplier.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/TplConvert.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include "RORoute.h"
#include "RORouteDef_OrigDest.h"
#include "ROEdgeVector.h"
#include "RONet.h"
#include "RORDLoader_Artemis.h"
#include "ROVehicleBuilder.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
RORDLoader_Artemis::RORDLoader_Artemis(ROVehicleBuilder &vb, RONet &net,
                                       SUMOTime begin, SUMOTime end,
                                       string file)
        : ROAbstractRouteDefLoader(vb, net, begin, end),
        myRouteIDSupplier("ARTEMIS_", 0),
        myVehIDSupplier("ARTEMIS_", 0),
        myPath(file), myCurrentTime(0)
{}


RORDLoader_Artemis::~RORDLoader_Artemis()
{}


void
RORDLoader_Artemis::closeReading()
{}


std::string
RORDLoader_Artemis::getDataName() const
{
    return "artemis routes";
}


bool
RORDLoader_Artemis::myReadRoutesAtLeastUntil(SUMOTime time)
{
    // go through the emitter nodes
    for (NodeFlows::iterator i=myNodeFlows.begin(); i!=myNodeFlows.end(); i++) {
        SUMOReal flow = (*i).second; // (in veh/hour)
        // compute the time of the next vehicle emission
        SUMOTime period = (SUMOTime)(3600.0/flow);
        // get the name of the origin node
        string orig = (*i).first;
        // check whether a vehicle shall be emitted at this time
        if (time%period==0) {
            SUMOReal prob = randSUMO((SUMOReal) 100.);
            // check which destination to use
            DestProbVector poss = myNodeConnections[orig];
            for (DestProbVector::iterator j=poss.begin(); j!=poss.end(); j++) {
                prob -= (*j).second;
                // use the current node if the probability indicates it
                if (prob<0) {
                    // retrieve the edges
                    string fromname = orig + "SOURCE";
                    string toname = (*j).first + "SINK";
                    ROEdge *from = _net.getEdge(fromname);
                    ROEdge *to = _net.getEdge(toname);
                    if (from==0) {
                        MsgHandler::getErrorInstance()->inform("The origin edge '" + fromname + "'is not known");
                        return false;
                    }
                    if (to==0) {
                        MsgHandler::getErrorInstance()->inform("The destination edge '" + toname + "'is not known");
                        return false;
                    }
                    if (time>=myBegin&&time<myEnd) {
                        // build the route
                        RORouteDef *route =
                            new RORouteDef_OrigDest(myRouteIDSupplier.getNext(),
                                                    RGBColor(-1, -1, -1), from, to);
                        _net.addRouteDef(route);
                        ROVehicleType *type = 0;
                        string vehID = myVehIDSupplier.getNext();
                        _net.addVehicle(vehID,
                                        myVehicleBuilder.buildVehicle(vehID, route, time, type,
                                                                      RGBColor(),-1, 0));
                    }
                    j = poss.end()-1;
                }
            }
        }
    }
    myCurrentTime = time + 1;
    return true;
}


bool
RORDLoader_Artemis::init(OptionsCont &)
{
    // read the hv-destinations first
    myReadingHVDests = true;
    myFirstLine = true;
    myReader.setFileName(myPath + "/HVdests.txt");
    myReader.readAll(*this);
    // read the flows then
    myReadingHVDests = false;
    myFirstLine = true;
    myReader.setFileName(myPath + "/Flows.txt");
    myReader.readAll(*this);
    return true;
}


bool
RORDLoader_Artemis::report(const std::string &result)
{
    if (myReadingHVDests) {
        // parse hv-destinations
        if (myFirstLine) {
            myLineHandler.reinit(result, "\t", "\t", true);
            myFirstLine = false;
        } else {
            myLineHandler.parseLine(result);
            string nodeid = myLineHandler.get("NodeID");
            string destid = myLineHandler.get("DestID");
            SUMOReal perc =
                TplConvert<char>::_2SUMOReal(myLineHandler.get("Percent").c_str());
            myNodeConnections[nodeid].push_back(
                DestPercentage(destid, perc));
        }
    } else {
        // parse the flows
        if (myFirstLine) {
            myLineHandler.reinit(result, "\t", "\t", true);
            myFirstLine = false;
        } else {
            myLineHandler.parseLine(result);
            string nodeid = myLineHandler.get("NodeID");
            SUMOReal flow =
                TplConvert<char>::_2SUMOReal(myLineHandler.get("a0").c_str());
            myNodeFlows[nodeid] = flow;
        }
    }
    return true;
}


bool
RORDLoader_Artemis::ended() const
{
    return false;
}


SUMOTime
RORDLoader_Artemis::getCurrentTimeStep() const
{
    return myCurrentTime;
}



/****************************************************************************/

