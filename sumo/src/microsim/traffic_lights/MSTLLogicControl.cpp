//---------------------------------------------------------------------------//
//                        MSTLLogicControl.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.8  2006/02/23 11:27:57  dkrajzew
// tls may have now several programs
//
// Revision 1.7  2005/11/09 06:36:48  dkrajzew
// changing the LSA-API: MSEdgeContinuation added; changed the calling API
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
// Revision 1.3  2003/08/04 11:42:35  dkrajzew
// missing deletion of traffic light logics on closing a network added
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
#pragma warning(disable: 4503)


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
#include <algorithm>
#include <cassert>
#include "MSTrafficLightLogic.h"
#include "MSTLLogicControl.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSTLLogicControl::MSTLLogicControl()
    : myNetWasLoaded(false)
{
}


MSTLLogicControl::~MSTLLogicControl()
{
    std::map<std::string, Variants>::const_iterator i;
    for(i=myLogics.begin(); i!=myLogics.end(); ++i) {
        const Variants &vars = (*i).second;
        std::map<std::string, MSTrafficLightLogic *>::const_iterator j;
        for(j=vars.ltVariants.begin(); j!=vars.ltVariants.end(); ++j) {
            delete (*j).second;
        }
    }
}


void
MSTLLogicControl::maskRedLinks()
{
    for_each(myActiveLogics.begin(), myActiveLogics.end(), mem_fun(&MSTrafficLightLogic::maskRedLinks));
}


void
MSTLLogicControl::maskYellowLinks()
{
    for_each(myActiveLogics.begin(), myActiveLogics.end(), mem_fun(&MSTrafficLightLogic::maskYellowLinks));
}


std::vector<MSTrafficLightLogic*>
MSTLLogicControl::getAllLogics() const
{
    std::vector<MSTrafficLightLogic*> ret;
    std::map<std::string, Variants>::const_iterator i;
    for(i=myLogics.begin(); i!=myLogics.end(); ++i) {
        const Variants &vars = (*i).second;
        std::map<std::string, MSTrafficLightLogic *>::const_iterator j;
        for(j=vars.ltVariants.begin(); j!=vars.ltVariants.end(); ++j) {
            ret.push_back((*j).second);
        }
    }
    return ret;
}

const MSTLLogicControl::Variants &
MSTLLogicControl::get(const std::string &id) const
{
    std::map<std::string, Variants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        throw 1;
    }
    return (*i).second;
}


MSTrafficLightLogic *
MSTLLogicControl::get(const std::string &id, const std::string &subid) const
{
    std::map<std::string, Variants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        throw 1;
    }
    const std::map<std::string, MSTrafficLightLogic *> &vars = (*i).second.ltVariants;
    std::map<std::string, MSTrafficLightLogic *>::const_iterator j = vars.find(subid);
    if(j==vars.end()) {
        throw 1;
    }
    return (*j).second;
}


bool
MSTLLogicControl::add(const std::string &id, const std::string &subID,
                      MSTrafficLightLogic *logic, bool newDefault)
{
    if(myLogics.find(id)==myLogics.end()) {
        Variants var;
        var.defaultTL = 0;
        myLogics[id] = var;
    }
    std::map<std::string, Variants>::iterator i = myLogics.find(id);
    Variants &tlmap = (*i).second;
    if(tlmap.ltVariants.find(subID)!=tlmap.ltVariants.end()) {
        return false;
    }
    // assert the liks are set
    if(myNetWasLoaded) {
        // this one has not yet its links set
        assert(tlmap.defaultTL!=0);
        logic->adaptLinkInformationFrom(*(tlmap.defaultTL));
    }
    // add to the list of active
    if(tlmap.ltVariants.size()==0) {
        tlmap.defaultTL = logic;
        myActiveLogics.push_back(logic);
    } else if(newDefault) {
        std::vector<MSTrafficLightLogic*>::iterator i =
            find(myActiveLogics.begin(), myActiveLogics.end(), tlmap.defaultTL);
        assert(i!=myActiveLogics.end());
        *i = logic;
        tlmap.defaultTL = logic;
    }
    // add to the list of logic
    tlmap.ltVariants[subID] = logic;
    return true;
}


bool
MSTLLogicControl::knows(const std::string &id) const
{
    std::map<std::string, Variants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        return false;
    }
    return true;
}


void
MSTLLogicControl::markNetLoadingClosed()
{
    myNetWasLoaded = true;
}


bool
MSTLLogicControl::isActive(const MSTrafficLightLogic *tl) const
{
    std::map<std::string, Variants>::const_iterator i = myLogics.find(tl->id());
    if(i==myLogics.end()) {
        throw 1;
    }
    return (*i).second.defaultTL == tl;
}


MSTrafficLightLogic *
MSTLLogicControl::getActive(const std::string &id) const
{
    std::map<std::string, Variants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        throw 1;
    }
    return (*i).second.defaultTL;
}


void
MSTLLogicControl::switchTo(const std::string &id, const std::string &subid)
{
    std::map<std::string, Variants>::iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        throw 1;
    }
    MSTrafficLightLogic *touse = (*i).second.ltVariants[subid];
    std::vector<MSTrafficLightLogic*>::iterator j =
        find(myActiveLogics.begin(), myActiveLogics.end(), (*i).second.defaultTL);
    assert(j!=myActiveLogics.end());
    *j = touse;
    (*i).second.defaultTL = touse;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


