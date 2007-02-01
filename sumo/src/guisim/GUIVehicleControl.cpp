/****************************************************************************/
/// @file    GUIVehicleControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id: $
///
// The class responsible for building and deletion of vehicles within the gui
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

#include <microsim/MSCORN.h>
#include "GUIVehicleControl.h"
#include "GUIVehicle.h"
#include "GUINet.h"
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// member method definitions
// ===========================================================================
GUIVehicleControl::GUIVehicleControl()
        : MSVehicleControl()
{}


GUIVehicleControl::~GUIVehicleControl()
{}


MSVehicle *
GUIVehicleControl::buildVehicle(std::string id, MSRoute* route,
                                SUMOTime departTime,
                                const MSVehicleType* type,
                                int repNo, int repOffset)
{
    myLoadedVehNo++;
    return new GUIVehicle(
               gIDStorage, id, route, departTime, type, repNo, repOffset);
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
    for (VehicleDictType::iterator i=myVehicleDict.begin(); i!=myVehicleDict.end(); i++) {
        MSVehicle *veh = (*i).second;
        if (veh->running()) {
            ret.push_back((*i).first);
        }
    }
    return ret;
}


std::vector<size_t>
GUIVehicleControl::getVehicleIDs()
{
    std::vector<size_t> ret;
    ret.reserve(myVehicleDict.size());
    for (VehicleDictType::iterator i=myVehicleDict.begin(); i!=myVehicleDict.end(); i++) {
        MSVehicle *veh = (*i).second;
        if (veh->running()) {
            ret.push_back(static_cast<GUIVehicle*>((*i).second)->getGlID());
        }
    }
    return ret;
}



/****************************************************************************/

