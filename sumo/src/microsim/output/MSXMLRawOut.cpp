//---------------------------------------------------------------------------//
//                        MSXMLRawOut.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 10.05.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.1  2004/08/02 12:05:54  dkrajzew
// raw-output added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cassert>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSXMLRawOut.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSXMLRawOut::MSXMLRawOut()
{
}


MSXMLRawOut::~MSXMLRawOut()
{
}


void
MSXMLRawOut::write(OutputDevice* of, const MSEdgeControl &ec,
                   unsigned int timestep,
                   unsigned int intend)
{
    of->getOStream() << "   <timestep id=\"" << timestep << "\">" << endl;
    const MSEdgeControl::EdgeCont &ec1 = ec.getSingleLaneEdges();
    for ( MSEdgeControl::EdgeCont::const_iterator edg1=ec1.begin(); edg1!=ec1.end(); ++edg1 ) {
        writeEdge(of, **edg1, intend+3);
    }
        // multi lane edges
    const MSEdgeControl::EdgeCont &ec2 = ec.getMultiLaneEdges();
    for ( MSEdgeControl::EdgeCont::const_iterator edg2=ec2.begin(); edg2!=ec2.end(); ++edg2 ) {
        writeEdge(of, **edg2, intend+3);
    }
    of->getOStream() << "   </timestep>" << endl;
}


void
MSXMLRawOut::writeEdge(OutputDevice* of, const MSEdge &edge,
                       unsigned int intend)
{
    //en
    bool dump = !MSGlobals::myOmitEmptyEdgesOnDump;
    if ( !dump ) {
        for ( MSEdge::LaneCont::const_iterator lane=edge.getLanes()->begin(); lane!=edge.getLanes()->end(); ++lane) {
            if( ((**lane).getVehicleNumber()!=0) ) {
                dump = true;
                break;
            }
        }
    }
    //en
    if ( dump ) {
        string indent( intend, ' ' );
        of->getOStream() << indent << "<edge id=\"" << edge.id() << "\">" << endl;
        for ( MSEdge::LaneCont::const_iterator lane=edge.getLanes()->begin(); lane!=edge.getLanes()->end(); ++lane) {
            writeLane(of, **lane, intend+3);
        }
        of->getOStream() << indent << "</edge>" << endl;
    }
}


void
MSXMLRawOut::writeLane(OutputDevice* of, const MSLane &lane,
                       unsigned int intend)
{
    string indent( intend , ' ' );
    if ( lane.myVehicles.empty() == true && lane.myVehBuffer == 0 ) {
        of->getOStream() << indent << "<lane id=\"" << lane.myID
            << "\"/>" << endl;
    } else { // not empty
        of->getOStream() << indent << "<lane id=\"" << lane.myID << "\">"
               << endl;
        if ( lane.myVehBuffer != 0 ) {
            writeVehicle(of, *(lane.myVehBuffer), intend+3);
        }
        for ( MSLane::VehCont::const_iterator veh = lane.myVehicles.begin();
            veh != lane.myVehicles.end(); ++veh ) {

            writeVehicle(of, **veh, intend+3);
        }
        of->getOStream() << indent << "</lane>" << endl;
    }
}


void
MSXMLRawOut::writeVehicle(OutputDevice* of, const MSVehicle &veh,
                          unsigned int intend)
{
    string indent( intend , ' ' );
    of->getOStream() << indent << "<vehicle id=\"" << veh.id() << "\" pos=\""
       << veh.pos() << "\" speed=\"" << veh.speed()
       << "\"/>" << endl;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
