//---------------------------------------------------------------------------//
//                        RORDLoader_Artemis.cpp -
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
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.2  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
// Revision 1.10  2003/10/31 08:00:31  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
// Revision 1.9  2003/10/15 11:55:11  dkrajzew
// false usage of rand() patched
//
// Revision 1.8  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.7  2003/07/18 12:35:06  dkrajzew
// removed some warnings
//
// Revision 1.6  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.5  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/05/20 09:48:34  dkrajzew
// debugging
//
// Revision 1.3  2003/03/20 16:39:15  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.2  2003/03/17 14:26:38  dkrajzew
// debugging
//
// Revision 1.1  2003/03/12 16:39:17  dkrajzew
// artemis route support added
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
#include <utils/importio/LineHandler.h>
#include <utils/gfx/RGBColor.h>
#include <utils/options/OptionsCont.h>
#include <utils/router/IDSupplier.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/convert/TplConvert.h>
#include <utils/common/UtilExceptions.h>
#include "RORoute.h"
#include "RORouteDef_OrigDest.h"
#include "ROEdgeVector.h"
#include "RONet.h"
#include "RORDLoader_Artemis.h"
#include "ROVehicleBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORDLoader_Artemis::RORDLoader_Artemis(ROVehicleBuilder &vb, RONet &net,
                                       unsigned int begin, unsigned int end,
                                       string file)
    : ROAbstractRouteDefLoader(vb, net, begin, end),
    myRouteIDSupplier(string("ARTEMIS_"), 0),
    myVehIDSupplier(string("ARTEMIS_"), 0),
    myPath(file), myCurrentTime(0)
{
}


RORDLoader_Artemis::~RORDLoader_Artemis()
{
}


void
RORDLoader_Artemis::closeReading()
{
}


std::string
RORDLoader_Artemis::getDataName() const
{
    return "artemis routes";
}


bool
RORDLoader_Artemis::myReadRoutesAtLeastUntil(unsigned int time)
{
    // go through the emitter nodes
    for(NodeFlows::iterator i=myNodeFlows.begin(); i!=myNodeFlows.end(); i++) {
        double flow = (*i).second; // (in veh/hour)
        // compute the time of the next vehicle emission
        long period = (long) (3600.0/flow);
        // get the name of the origin node
        string orig = (*i).first;
        // check whether a vehicle shall be emitted at this time
        if(time%period==0) {
            double prob = (double) rand() /
                ( static_cast<double>(RAND_MAX) + 1) * 100.0;
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
                        MsgHandler::getErrorInstance()->inform(
                            string("The origin edge '") + fromname + string("'is not known"));
                        return false;
                    }
                    if(to==0) {
                        MsgHandler::getErrorInstance()->inform(
                            string("The destination edge '") + toname + string("'is not known"));
                        return false;
                    }
                    if(time>=myBegin&&time<myEnd) {
                        // build the route
                        RORouteDef *route =
                            new RORouteDef_OrigDest(myRouteIDSupplier.getNext(),
                                RGBColor(-1, -1, -1), from, to);
                        _net.addRouteDef(route);
                        ROVehicleType *type = _net.getDefaultVehicleType();
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
RORDLoader_Artemis::init(OptionsCont &options)
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
RORDLoader_Artemis::ended() const
{
    return false;
}


unsigned int
RORDLoader_Artemis::getCurrentTimeStep() const
{
    return myCurrentTime;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


