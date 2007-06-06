/****************************************************************************/
/// @file    MSDensity.h
/// @author  Christian Roessel
/// @date    Tue Sep  9 22:27:23 2003
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
#ifndef MSDensity_h
#define MSDensity_h

//


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
// class declarations
// ===========================================================================
class MSVehicle;
class MSLane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSDensity
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::Count Container;

    MSDensity(SUMOReal lengthInMeters,
              const Container& counter);

    virtual ~MSDensity(void)
    {}

    DetectorAggregate getDetectorAggregate(void);

    static std::string getDetectorName(void)
    {
        return "density";
    }

private:
    SUMOReal detectorLengthM; //  [km]
    const Container& counterM;

    MSDensity();
    MSDensity(const MSDensity&);
    MSDensity& operator=(const MSDensity&);
};


#endif

/****************************************************************************/

