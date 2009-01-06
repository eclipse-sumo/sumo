/****************************************************************************/
/// @file    MSXMLRawOut.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Realises dumping the complete network state
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
void
MSXMLRawOut::write(OutputDevice &of, const MSEdgeControl &ec,
                   SUMOTime timestep,
                   unsigned int intend) throw(IOError)
{
    of << "   <timestep time=\"" << timestep << "\">" << "\n";
    const MSEdgeControl::EdgeCont &ec1 = ec.getSingleLaneEdges();
    for (MSEdgeControl::EdgeCont::const_iterator edg1=ec1.begin(); edg1!=ec1.end(); ++edg1) {
        writeEdge(of, **edg1, intend+3);
    }
    // multi lane edges
    const MSEdgeControl::EdgeCont &ec2 = ec.getMultiLaneEdges();
    for (MSEdgeControl::EdgeCont::const_iterator edg2=ec2.begin(); edg2!=ec2.end(); ++edg2) {
        writeEdge(of, **edg2, intend+3);
    }
    of << "   </timestep>" << "\n";
}


void
MSXMLRawOut::writeEdge(OutputDevice &of, const MSEdge &edge,
                       unsigned int intend) throw(IOError)
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
        of << indent << "<edge id=\"" << edge.getID() << "\">" << "\n";
        for (MSEdge::LaneCont::const_iterator lane=edge.getLanes()->begin(); lane!=edge.getLanes()->end(); ++lane) {
            writeLane(of, **lane, intend+3);
        }
        of << indent << "</edge>" << "\n";
    }
}


void
MSXMLRawOut::writeLane(OutputDevice &of, const MSLane &lane,
                       unsigned int intend) throw(IOError)
{
    string indent(intend , ' ');
    if (lane.myVehicles.empty() == true && lane.myVehBuffer.size() == 0) {
        of << indent << "<lane id=\"" << lane.myID
        << "\"/>" << "\n";
    } else { // not empty
        of << indent << "<lane id=\"" << lane.myID << "\">"
        << "\n";
        for (vector<MSVehicle*>::const_iterator veh = lane.myVehBuffer.begin();
                veh != lane.myVehBuffer.end(); ++veh) {
            writeVehicle(of, **veh, intend+3);
        }
        for (MSLane::VehCont::const_iterator veh = lane.myVehicles.begin();
                veh != lane.myVehicles.end(); ++veh) {

            writeVehicle(of, **veh, intend+3);
        }
        of << indent << "</lane>" << "\n";
    }
}


void
MSXMLRawOut::writeVehicle(OutputDevice &of, const MSVehicle &veh,
                          unsigned int intend) throw(IOError)
{
    string indent(intend , ' ');
    of << indent << "<vehicle id=\"" << veh.getID() << "\" pos=\""
    << veh.getPositionOnLane() << "\" speed=\"" << veh.getSpeed()
    << "\"/>" << "\n";
}



/****************************************************************************/

