/****************************************************************************/
/// @file    MSTDDetectorInterface.h
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 20:03 CET
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
#ifndef MSTDDetectorInterface_h
#define MSTDDetectorInterface_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDetectorInterfaceCommon.h"
#include <microsim/MSUpdateEachTimestep.h>

/// @namespace TD The namespace TD contains the specifics of the TD
/// (timestep data) detectors. These detectors update their value
/// every timestep and provide access to this timestep data.
/// @see namespace ED, namespace LD

namespace TD
{
/// This abstract class declares the particular methods for the TD
/// (timestep data) detectors.
class MSDetectorInterface
            :
            public MSDetectorInterfaceCommon
            , public MSUpdateEachTimestep< MSDetectorInterface >
{
public:

    /// Get the detector's value compiled in the last/current
    /// (depending on the time of the query) timestep.
    ///
    /// @return The detector's value compiled in one timestep.
    ///
    virtual SUMOReal getCurrent(void) const = 0;

    /// Dtor.
    virtual ~MSDetectorInterface(void)
    {}

protected:

    /// Ctor. Passed the id to base class.
    ///
    /// @param id The detector's id.
    MSDetectorInterface(std::string id)
            :
            MSDetectorInterfaceCommon(id)
    {}

private:

};

} // end namespace TD

namespace Detector
{
/// Alias for easier access to the MSUpdateEachTimestep interface
/// for TD::MSDetectorInterface detectors.
typedef MSUpdateEachTimestep< TD::MSDetectorInterface > UpdateE2Detectors;
}


#endif

/****************************************************************************/

