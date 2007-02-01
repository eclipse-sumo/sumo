/****************************************************************************/
/// @file    MSE3Traveltime.h
/// @author  Christian Roessel
/// @date    Thu Nov 27 2003 15:25 CET
/// @version $Id: $
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
#ifndef MSE3Traveltime_h
#define MSE3Traveltime_h
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
#include <microsim/MSUnit.h>
#include <microsim/MSNet.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSE3Traveltime
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::TimeMap Container;
    typedef Container::InnerContainer TravelTimeMap;

    MSE3Traveltime(const Container& container)
            :
            containerM(container)
    {}

    virtual ~MSE3Traveltime(void)
    {}

    bool hasVehicle(MSVehicle& veh) const
    {
        return containerM.hasVehicle(&veh);
    }

    DetectorAggregate getValue(MSVehicle& veh)   // [s]
    {
        TravelTimeMap::const_iterator pair =
            containerM.containerM.find(&veh);
        assert(pair != containerM.containerM.end());
        MSUnit::Seconds startTime = pair->second;
        MSUnit::Seconds time = (MSUnit::Seconds) MSNet::getInstance()->simSeconds();
        assert(time > startTime);
        return  time - startTime;
    }

    static std::string getDetectorName(void)
    {
        return "meanTraveltime";
    }

private:
    const Container& containerM;

    MSE3Traveltime();
    MSE3Traveltime(const MSE3Traveltime&);
    MSE3Traveltime& operator=(const MSE3Traveltime&);

};


#endif

/****************************************************************************/

