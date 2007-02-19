/****************************************************************************/
/// @file    MSXMLRawOut.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
//
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
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSXMLRawOut.h"

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
MSXMLRawOut::MSXMLRawOut()
{}


MSXMLRawOut::~MSXMLRawOut()
{}


void
MSXMLRawOut::write(OutputDevice* of, const MSEdgeControl &ec,
                   SUMOTime timestep,
                   unsigned int intend)
{
    of->getOStream() << "   <timestep time=\"" << timestep << "\">" << endl;
    const MSEdgeControl::EdgeCont &ec1 = ec.getSingleLaneEdges();
    for (MSEdgeControl::EdgeCont::const_iterator edg1=ec1.begin(); edg1!=ec1.end(); ++edg1) {
        writeEdge(of, **edg1, intend+3);
    }
    // multi lane edges
    const MSEdgeControl::EdgeCont &ec2 = ec.getMultiLaneEdges();
    for (MSEdgeControl::EdgeCont::const_iterator edg2=ec2.begin(); edg2!=ec2.end(); ++edg2) {
        writeEdge(of, **edg2, intend+3);
    }
    of->getOStream() << "   </timestep>" << endl;
}


void
MSXMLRawOut::writeEdge(OutputDevice* of, const MSEdge &edge,
                       unsigned int intend)
{
    //en
    bool dump = !MSGlobals::gOmitEmptyEdgesOnDump;
    if (!dump) {
        for (MSEdge::LaneCont::const_iterator lane=edge.getLanes()->begin(); lane!=edge.getLanes()->end(); ++lane) {
            if (((**lane).getVehicleNumber()!=0)) {
                dump = true;
                break;
            }
        }
    }
    //en
    if (dump) {
        string indent(intend, ' ');
        of->getOStream() << indent << "<edge id=\"" << edge.getID() << "\">" << endl;
        for (MSEdge::LaneCont::const_iterator lane=edge.getLanes()->begin(); lane!=edge.getLanes()->end(); ++lane) {
            writeLane(of, **lane, intend+3);
        }
        of->getOStream() << indent << "</edge>" << endl;
    }
}


void
MSXMLRawOut::writeLane(OutputDevice* of, const MSLane &lane,
                       unsigned int intend)
{
    string indent(intend , ' ');
    if (lane.myVehicles.empty() == true && lane.myVehBuffer == 0) {
        of->getOStream() << indent << "<lane id=\"" << lane.myID
        << "\"/>" << endl;
    } else { // not empty
        of->getOStream() << indent << "<lane id=\"" << lane.myID << "\">"
        << endl;
        if (lane.myVehBuffer != 0) {
            writeVehicle(of, *(lane.myVehBuffer), intend+3);
        }
        for (MSLane::VehCont::const_iterator veh = lane.myVehicles.begin();
                veh != lane.myVehicles.end(); ++veh) {

            writeVehicle(of, **veh, intend+3);
        }
        of->getOStream() << indent << "</lane>" << endl;
    }
}


void
MSXMLRawOut::writeVehicle(OutputDevice* of, const MSVehicle &veh,
                          unsigned int intend)
{
    string indent(intend , ' ');
    of->getOStream() << indent << "<vehicle id=\"" << veh.getID() << "\" pos=\""
    << veh.getPositionOnLane() << "\" speed=\"" << veh.getSpeed()
    << "\"/>" << endl;
}



/****************************************************************************/

