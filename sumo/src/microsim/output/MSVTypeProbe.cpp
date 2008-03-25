/****************************************************************************/
/// @file    MSVTypeProbe.cpp
/// @author  Tino Morenz
/// @date    Wed, 24.10.2007
/// @version $Id: $
///
// Writes positions of vehicles that have a certain (named) type
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/WrappingCommand.h>
#include <microsim/MSLane.h>
#include <utils/iodevices/OutputDevice.h>

#include "MSVTypeProbe.h"

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
MSVTypeProbe::MSVTypeProbe(const string &id, 
                           const string &vType) throw()
        : Named(id), myVType(vType)
{
}


MSVTypeProbe::~MSVTypeProbe() throw()
{
}


void
MSVTypeProbe::writeXMLOutput(OutputDevice &dev,
                                SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    const std::string indent("    ");
    dev << indent << "<timestep time=\"" << startTime << "\" id=\"" << getID() << "\" vtype=\"" << myVType << "\">" << "\n";
    MSVehicleControl &vc = MSNet::getInstance()->getVehicleControl();
    std::map<std::string, MSVehicle*>::const_iterator it = vc.loadedVehBegin();
    std::map<std::string, MSVehicle*>::const_iterator end = vc.loadedVehEnd();
    for (;it != end; ++it) {
        const MSVehicle *veh=(*it).second;
        if (myVType=="" || myVType==veh->getVehicleType().getID()) {
            if (!veh->running()) {
                continue;
            }
            dev << indent << indent
            << "<vehicle id=\"" << veh->getID()
            << "\" edge=\"" << veh->getEdge()->getID()
            << "\" lane=\"" << veh->getLane().getID()
            << "\" pos_on_lane=\"" << veh->getPositionOnLane()
            << "\" x=\"" << veh->getPosition().x()
            << "\" y=\"" << veh->getPosition().y()
            << "\" speed=\"" << veh->getSpeed()
            << "\"/>" << "\n";
        }

    }
    dev << indent << "</timestep>" << "\n";
}


void
MSVTypeProbe::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError)
{
    dev.writeXMLHeader("vehicle-type-probes");
}


