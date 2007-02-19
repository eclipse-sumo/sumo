/****************************************************************************/
/// @file    MSLDDetectorInterface.h
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 20:04 CET
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
#ifndef MSLDDetectorInterface_h
#define MSLDDetectorInterface_h
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

#include "MSDetectorInterfaceCommon.h"
#include <string>
// ===========================================================================
// class declarations
// ===========================================================================

class MSVehicle;

/// @namespace LD The namespace LD contains the specifics of the LD
/// (leave data) detectors. These detector update their value if a
/// vehicles leaves the detector.
/// @see namespace ED, namespace TD

namespace LD
{
/// This abstract class declares the particular methods for the LD
/// (leave data) detectors.
class MSDetectorInterface
            :
            public MSDetectorInterfaceCommon
{
public:

    /// Perform detector update if vehicle leaves the detector.
    ///
    /// @param veh The leaving vehicle.
    ///
    virtual void leave(MSVehicle& veh) = 0;

    /// Dtor
    virtual ~MSDetectorInterface(void)
    {}

protected:

    /// Ctor. Passed the id to base class.
    ///
    /// @param id The detector's id.
    MSDetectorInterface(std::string id)
            : MSDetectorInterfaceCommon(id)
    {}

private:

};

} // end namespace LD


#endif

/****************************************************************************/

