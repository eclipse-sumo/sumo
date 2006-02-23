#ifndef MSTLLogicControl_h
#define MSTLLogicControl_h
//---------------------------------------------------------------------------//
//                        MSTLLogicControl.h -
//  A class that holds all traffic light logics used
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2006/02/23 11:27:57  dkrajzew
// tls may have now several programs
//
// Revision 1.6  2005/10/10 11:56:09  dkrajzew
// reworking the tls-API: made tls-control non-static; made net an element of traffic lights
//
// Revision 1.5  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 11:09:53  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/05/04 08:22:19  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2005/01/27 14:22:45  dkrajzew
// ability to open the complete phase definition added; code style adapted
//
// Revision 1.1  2004/11/23 10:18:42  dkrajzew
// all traffic lights moved to microsim/traffic_lights
//
// Revision 1.2  2003/07/30 09:16:10  dkrajzew
// a better (correct?) processing of yellow lights added; debugging
//
// Revision 1.1  2003/06/05 16:08:36  dkrajzew
// traffic lights are no longer junction-bound; a separate control is necessary
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <map>
#include "MSTrafficLightLogic.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSTLLogicControl {
public:
    struct Variants {
        MSTrafficLightLogic *defaultTL;
        std::map<std::string, MSTrafficLightLogic*> ltVariants;
    };

    /// Constructor
    MSTLLogicControl();

    /// Destructor
    ~MSTLLogicControl();

    /// For all traffic lights, the requests are masked away if they have red light (not yellow)
    void maskRedLinks();

    /// For all traffic lights, the requests are masked away if they have yellow light
    void maskYellowLinks();

    std::vector<MSTrafficLightLogic*> getAllLogics() const;

    const Variants &get(const std::string &id) const; // !!! reference?
    MSTrafficLightLogic *get(const std::string &id, const std::string &subid) const; // !!! reference?
    MSTrafficLightLogic *getActive(const std::string &id) const; // !!! reference?

    bool add(const std::string &id, const std::string &subID,
        MSTrafficLightLogic *logic, bool newDefault=true);

    bool knows(const std::string &id) const;

    void markNetLoadingClosed();

    bool isActive(const MSTrafficLightLogic *tl) const;

    void switchTo(const std::string &id, const std::string &subid);

protected:

    std::map<std::string, Variants> myLogics;
    std::vector<MSTrafficLightLogic*> myActiveLogics;

    bool myNetWasLoaded;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

