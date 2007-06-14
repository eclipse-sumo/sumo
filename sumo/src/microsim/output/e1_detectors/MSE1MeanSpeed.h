/****************************************************************************/
/// @file    MSE1MeanSpeed.h
/// @author  Christian Roessel
/// @date    Fri Jul 09 2004 21:31 CEST
/// @version $Id$
///
// »missingDescription«
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
#ifndef MSE1MeanSpeed_h
#define MSE1MeanSpeed_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSE1.h"
#include <string>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSE1MeanSpeed
 */
class MSE1MeanSpeed
{
public:

    static std::string getDetectorName(void)
    {
        return "E1MeanSpeed";
    }

protected:
    typedef SUMOReal DetectorAggregate;
    typedef E1::E1Container Container;

    MSE1MeanSpeed(const Container& container)
            :
            containerM(container)
    {}

    virtual ~MSE1MeanSpeed(void)
    {}

    bool hasVehicle(MSVehicle& veh) const
    {
        return containerM.hasVehicle(&veh);
    }

    DetectorAggregate getValue(MSVehicle& veh)
    {
        assert(&veh == containerM.vehOnDetectorM);
        // speed in [m/s]
        return containerM.vehLengthM /
               (containerM.leaveTimeM - containerM.entryTimeM);
    }

private:
    const Container& containerM;

    MSE1MeanSpeed();
    MSE1MeanSpeed(const MSE1MeanSpeed&);
    MSE1MeanSpeed& operator=(const MSE1MeanSpeed&);
};


#endif

/****************************************************************************/

