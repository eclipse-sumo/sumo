//---------------------------------------------------------------------------//
//                        ROArtemisRouteDefHandler.cpp -
//  A handler for Artemis-files
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 12 Mar 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2003/03/12 16:39:17  dkrajzew
// artemis route support added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <string>
#include <iostream> // !!! debug only
#include <fstream>
#include <sstream>
//#include <ios_base>
#include <utils/importio/LineHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/router/IDSupplier.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/convert/TplConvert.h>
#include <utils/common/UtilExceptions.h>
//#include "RORouteAlternative.h"
#include "RORoute.h"
#include "ROOrigDestRouteDef.h"
#include "ROEdgeVector.h"
#include "RONet.h"
#include "ROArtemisRouteDefHandler.h"

using namespace std;

ROArtemisRouteDefHandler::ROArtemisRouteDefHandler(RONet &net, string file)
    : ROTypedRoutesLoader(net),
    myRouteIDSupplier(string("ARTEMIS_"), 0),
    myVehIDSupplier(string("ARTEMIS_"), 0),
    myPath(file)
{
}

ROArtemisRouteDefHandler::~ROArtemisRouteDefHandler()
{
}


ROTypedRoutesLoader *
ROArtemisRouteDefHandler::getAssignedDuplicate(const std::string &file) const
{
    return new ROArtemisRouteDefHandler(_net, file);
}


void
ROArtemisRouteDefHandler::closeReading()
{
}


std::string
ROArtemisRouteDefHandler::getDataName() const
{
    return "artemis routes";
}


bool
ROArtemisRouteDefHandler::readNextRoute(long start)
{
    // go through the emitter nodes
    for(NodeFlows::iterator i=myNodeFlows.begin(); i!=myNodeFlows.end(); i++) {
        double flow = (*i).second; // (in veh/hour)
        // compute the time of the next vehicle emission
        long period = (long) (3600.0/flow);
        // get the name of the origin node
        string orig = (*i).first;
        // check whether a vehicle shall be emitted at this time
        if(start%period==0) {
            double prob = (double)rand() / (double)(RAND_MAX+1) * 100.0;
            // check which destination to use
            DestProbVector poss = myNodeConnections[orig];
            for(DestProbVector::iterator j=poss.begin(); j!=poss.end(); j++) {
                prob -= (*j).second;
                // use the current node if the propability indicates it
                if(prob<0) {
                    // retrieve the edges
                    string fromname = orig + string("SOURCE");
                    string toname = (*j).first + string("SINK");
                    ROEdge *from = _net.getEdge(fromname);
                    ROEdge *to = _net.getEdge(toname);
                    if(from==0) {
                        SErrorHandler::add(
                            string("The origin edge '") + fromname + string("'is not known"));
                        return false;
                    }
                    if(to==0) {
                        SErrorHandler::add(
                            string("The destination edge '") + toname + string("'is not known"));
                        return false;
                    }
                    // build the route
                    RORouteDef *route = 
                        new ROOrigDestRouteDef(myRouteIDSupplier.getNext(), 
                        from, to);
                    _net.addRouteDef(route);
                    ROVehicleType *type = _net.getDefaultVehicleType();
                    string vehID = myVehIDSupplier.getNext();
                    _net.addVehicle(vehID, 
                        new ROVehicle(vehID, route, 
                            start, type, -1));
                }
            }
        }
    }
    _nextRouteRead = true;
    _currentTimeStep = start + 1;
}



bool
ROArtemisRouteDefHandler::startReadingSteps()
{
    return true;
}


bool
ROArtemisRouteDefHandler::myInit(OptionsCont &options)
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
ROArtemisRouteDefHandler::report(const std::string &result)
{
    if(myReadingHVDests) {
        // parse hv-destinations 
        if(myFirstLine) {
            myLineHandler.reinit(result, "\t", "\t", true);
            myFirstLine = false;
        } else {
            myLineHandler.parseLine(result);
            string nodeid = myLineHandler.get("NodeID");
            string destid = myLineHandler.get("DestID");
            double perc = 
                TplConvert<char>::_2float(myLineHandler.get("Percent").c_str());
            myNodeConnections[nodeid].push_back( 
                DestPercentage(destid, perc));
        }
    } else {
        // parse the flows
        if(myFirstLine) {
            myLineHandler.reinit(result, "\t", "\t", true);
            myFirstLine = false;
        } else {
            myLineHandler.parseLine(result);
            string nodeid = myLineHandler.get("NodeID");
            double flow = 
                TplConvert<char>::_2float(myLineHandler.get("a0").c_str());
            myNodeFlows[nodeid] = flow;
        }
    }
    return true;
}



bool 
ROArtemisRouteDefHandler::checkFile(const std::string &file) const
{
    return FileHelpers::exists(file + "/HVdests.txt") 
        &&
        FileHelpers::exists(file + "/Flows.txt");
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROArtemisRouteDefHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


