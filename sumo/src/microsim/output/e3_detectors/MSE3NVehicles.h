/****************************************************************************/
/// @file    MSE3NVehicles.h
/// @author  Christian Roessel
/// @date    Thu Nov 27 2003 20:24 CET
/// @version $Id$
///
// / @author  Christian Roessel <christian.roessel@dlr.de>
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
#ifndef MSE3NVehicles_h
#define MSE3NVehicles_h
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

#include <microsim/output/MSDetectorContainerWrapper.h>
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSE3NVehicles
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::VehicleMap Container;
    typedef Container::InnerContainer VehiclesMap;

    MSE3NVehicles(const Container& container)
            : containerM(container)
    {}

    virtual ~MSE3NVehicles(void)
    {}

    bool hasVehicle(MSVehicle& veh) const
    {
        return containerM.hasVehicle(&veh);
    }

    DetectorAggregate getValue(MSVehicle&)
    {
        return 1;
    }

    static std::string getDetectorName(void)
    {
        return "nE3Vehicles";
    }

private:
    const Container& containerM;

    MSE3NVehicles();
    MSE3NVehicles(const MSE3NVehicles&);
    MSE3NVehicles& operator=(const MSE3NVehicles&);

};


#endif

/****************************************************************************/

