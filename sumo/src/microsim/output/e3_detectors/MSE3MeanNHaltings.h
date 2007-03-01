/****************************************************************************/
/// @file    MSE3MeanNHaltings.h
/// @author  Christian Roessel
/// @date    Fri Nov 28 2003 12:27 CET
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
#ifndef MSE3MeanNHaltings_h
#define MSE3MeanNHaltings_h
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

#include <microsim/output/MSDetectorHaltingContainerWrapper.h>
#include "MSE3Collector.h"


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
/*
class MSE3MeanNHaltings
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef MSE3Collector::HaltingMap Container;
    typedef std::map< MSVehicle*, MSE3Collector::HaltingMap::E3Halting> HaltingsMap;

    MSE3MeanNHaltings(const Container& container)
            : containerM(container)
    {}

    virtual ~MSE3MeanNHaltings(void)
    {}

    bool hasVehicle(MSVehicle& veh) const
    {
        return containerM.hasVehicle(&veh);
    }

    DetectorAggregate getValue(MSVehicle& veh)
    {
        HaltingsMap::const_iterator pair = containerM.containerM.find(&veh);
        assert(pair != containerM.containerM.end());
        return (SUMOReal) pair->second.nHalts;
    }

    static std::string getDetectorName(void)
    {
        return "meanNHaltsPerVehicle";
    }

private:
    const Container& containerM;

    MSE3MeanNHaltings();
    MSE3MeanNHaltings(const MSE3MeanNHaltings&);
    MSE3MeanNHaltings& operator=(const MSE3MeanNHaltings&);
};
*/

#endif

/****************************************************************************/

