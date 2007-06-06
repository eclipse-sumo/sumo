/****************************************************************************/
/// @file    MSNVehicles.h
/// @author  Christian Roessel
/// @date    Wed Oct 15 13:29:34 2003
/// @version $Id$
///
// * @author Christian Roessel
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
#ifndef MSNVehicles_h
#define MSNVehicles_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "MSDetectorCounterContainerWrapper.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSNVehicles
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::Count Container;

    MSNVehicles(const Container& counter);
    MSNVehicles(SUMOReal, const Container& counter);

    virtual ~MSNVehicles(void)
    {}

    DetectorAggregate getDetectorAggregate(void);

    static std::string getDetectorName(void)
    {
        return "nE2Vehicles";
    }

private:
    const Container& counterM;

    MSNVehicles();
    MSNVehicles(const MSNVehicles&);
    MSNVehicles& operator=(const MSNVehicles&);

};


#endif

/****************************************************************************/

