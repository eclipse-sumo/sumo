/****************************************************************************/
/// @file    GUIVehicleControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles (gui-version)
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

#include <microsim/MSCORN.h>
#include "GUIVehicleControl.h"
#include "GUIVehicle.h"
#include "GUINet.h"
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUIVehicleControl::GUIVehicleControl()
        : MSVehicleControl()
{}


GUIVehicleControl::~GUIVehicleControl()
{}


MSVehicle *
GUIVehicleControl::buildVehicle(const std::string &id, MSRoute* route,
                                SUMOTime departTime,
                                const MSVehicleType* type,
                                int repNo, int repOffset)
{
    myLoadedVehNo++;
    return new GUIVehicle(
               gIDStorage, id, route, departTime, type, repNo, repOffset, myLoadedVehNo-1);
}


void
GUIVehicleControl::deleteVehicle(MSVehicle *veh)
{
    static_cast<GUIVehicle*>(veh)->setRemoved();
    if (gIDStorage.remove(static_cast<GUIVehicle*>(veh)->getGlID())) {
        MSVehicleControl::deleteVehicle(veh);
    }
}


std::vector<std::string>
GUIVehicleControl::getVehicleNames()
{
    std::vector<std::string> ret;
    ret.reserve(myVehicleDict.size());
    for (VehicleDictType::iterator i=myVehicleDict.begin(); i!=myVehicleDict.end(); ++i) {
        MSVehicle *veh = (*i).second;
        if (veh->isOnRoad()) {
            ret.push_back((*i).first);
        }
    }
    return ret;
}


std::vector<GLuint>
GUIVehicleControl::getVehicleIDs()
{
    std::vector<GLuint> ret;
    ret.reserve(myVehicleDict.size());
    for (VehicleDictType::iterator i=myVehicleDict.begin(); i!=myVehicleDict.end(); ++i) {
        MSVehicle *veh = (*i).second;
        if (veh->isOnRoad()) {
            ret.push_back(static_cast<GUIVehicle*>((*i).second)->getGlID());
        }
    }
    return ret;
}



/****************************************************************************/

