/****************************************************************************/
/// @file    MSEDDetectorInterface.h
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 20:04 CET
/// @version $Id$
///
//	»missingDescription«
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
#ifndef MSEDDetectorInterface_h
#define MSEDDetectorInterface_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDetectorInterfaceCommon.h"
#include <string>

/// @namespace ED The namespace ED contains the specifics of the ED
/// (event data) detectors. These detector update their value if a
/// specific event happens (e.g. a vehicle halts).
/// @see namespace LD, namespace TD

namespace ED
{
/// This abstract class declares the particular methods for the ED
/// (event data) detectors. Here, there are no particular methods
/// like in TD::MSDetectorInterface and
/// LD::MSDetectorInterface. The particular method update() of a
/// concrete ED::MSDetector is inherited there.
class MSDetectorInterface
            :
            public MSDetectorInterfaceCommon
{
public:
    /// Dtor.
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

} // end namespace ED


#endif

/****************************************************************************/

