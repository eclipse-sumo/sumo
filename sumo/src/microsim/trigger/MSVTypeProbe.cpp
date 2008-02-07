/****************************************************************************/
/// @file    MSVTypeProbe.cpp
/// @author  Tino Morenz
/// @date    Wed, 24.10.2007
/// @version $Id: $
///
// A probe for a specific vehicle type
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

MSVTypeProbe::MSVTypeProbe(const string &id, MSNet &net,
                           const string &fileName, const string &vType,
                           SUMOTime probeFreq) throw()
        : MSTrigger(id), myNet(net), myFileName(fileName), myVType(vType),
        myProbeFreq(probeFreq), myOutDev(OutputDevice::getDevice(myFileName))
{

    writeXMLProlog();

    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        new WrappingCommand<MSVTypeProbe>(this, &MSVTypeProbe::execute),
        myProbeFreq,
        MSEventControl::ADAPT_AFTER_EXECUTION);
}

MSVTypeProbe::~MSVTypeProbe() throw()
{
    writeXMLEpilog();
}


SUMOTime
MSVTypeProbe::execute(SUMOTime timestep) throw(ProcessError)
{
    const std::string indent("   ");

    myOutDev << indent << "<timestep time=\"" << timestep << "\">" << "\n";

    std::map<std::string, MSVehicle*>::const_iterator it = MSNet::getInstance()->getVehicleControl().loadedVehBegin();

    for (;it != MSNet::getInstance()->getVehicleControl().loadedVehEnd(); it++) {

        const MSVehicle *veh=(*it).second;
        const std::string type = veh->getVehicleType().getID();
        if (type == myVType) {
            myVehicles.push_back(veh);

            if (!veh->running()) {
                continue;
            }

            myOutDev << indent << indent
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
    myOutDev << indent << "</timestep>" << "\n";

    return myProbeFreq;
}

void
MSVTypeProbe::writeXMLProlog()
{
    myOutDev << "<?xml version=\"1.0\" standalone=\"yes\"?>" << "\n";
    myOutDev << "<vehicle-type-probes type=\"" << myVType << "\">" << "\n";
}

void
MSVTypeProbe::writeXMLEpilog()
{
    myOutDev << "</vehicle-type-probes>" << "\n";
}

