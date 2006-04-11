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
// Revision 1.12  2006/04/11 10:59:07  dkrajzew
// all structures now return their id via getID()
//
// Revision 1.11  2006/03/17 08:57:51  dkrajzew
// changed the Event-interface (execute now gets the current simulation time, event handlers are non-static)
//
// Revision 1.10  2006/03/08 13:13:20  dkrajzew
// debugging
//
// Revision 1.9  2006/02/27 12:05:32  dkrajzew
// WAUTs and WAUT-API added
//
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
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>

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
/* -------------------------------------------------------------------------
 * method definitions for the Switching Procedures
 * ----------------------------------------------------------------------- */
MSTLLogicControl::WAUTSwitchProcedure_JustSwitch::WAUTSwitchProcedure_JustSwitch(
        MSTrafficLightLogic *from, MSTrafficLightLogic *to, bool synchron)
    : MSTLLogicControl::WAUTSwitchProcedure(from, to, synchron)
{
}


MSTLLogicControl::WAUTSwitchProcedure_JustSwitch::~WAUTSwitchProcedure_JustSwitch()
{
}


bool
MSTLLogicControl::WAUTSwitchProcedure_JustSwitch::trySwitch()
{
    return true;
}






MSTLLogicControl::WAUTSwitchProcedure_GSP::WAUTSwitchProcedure_GSP(
        MSTrafficLightLogic *from, MSTrafficLightLogic *to, bool synchron)
    : MSTLLogicControl::WAUTSwitchProcedure(from, to, synchron)
{
}


MSTLLogicControl::WAUTSwitchProcedure_GSP::~WAUTSwitchProcedure_GSP()
{
}


bool
MSTLLogicControl::WAUTSwitchProcedure_GSP::trySwitch()
{
    SUMOReal gsp = getGSPValue(myFrom);
    int bla = 0;
    return true;
}


SUMOReal
MSTLLogicControl::WAUTSwitchProcedure_GSP::getGSPValue(MSTrafficLightLogic *from) const
{
    string val = from->getParameterValue("GSP");
    if(val.length()==0) {
        return -1;
    }
    return TplConvert<char>::_2SUMOReal(val.c_str());
}






MSTLLogicControl::WAUTSwitchProcedure_Stretch::WAUTSwitchProcedure_Stretch(
        MSTrafficLightLogic *from, MSTrafficLightLogic *to, bool synchron)
    : MSTLLogicControl::WAUTSwitchProcedure(from, to, synchron)
{
}


MSTLLogicControl::WAUTSwitchProcedure_Stretch::~WAUTSwitchProcedure_Stretch()
{
}


bool
MSTLLogicControl::WAUTSwitchProcedure_Stretch::trySwitch()
{
    int noBereiche = getStretchBereicheNo(myFrom);
    for(int i=0; i<noBereiche; i++) {
        StretchBereichDef def = getStretchBereichDef(myFrom, i+1);
        int bla = 0;
    }
    return true;
}


int
MSTLLogicControl::WAUTSwitchProcedure_Stretch::getStretchBereicheNo(MSTrafficLightLogic *from) const
{
    int no = 0;
    while(from->getParameterValue("B" + toString(no+1) + ".begin")!="") {
        no++;
    }
    return no;
}



MSTLLogicControl::WAUTSwitchProcedure_Stretch::StretchBereichDef
MSTLLogicControl::WAUTSwitchProcedure_Stretch::getStretchBereichDef(MSTrafficLightLogic *from, int index) const
{
    StretchBereichDef def;
    def.begin = TplConvert<char>::_2SUMOReal(from->getParameterValue("B" + toString(index) + ".begin").c_str());
    def.end = TplConvert<char>::_2SUMOReal(from->getParameterValue("B" + toString(index) + ".end").c_str());
    def.fac = TplConvert<char>::_2SUMOReal(from->getParameterValue("B" + toString(index) + ".factor").c_str());
    return def;
}


/* -------------------------------------------------------------------------
 * method definitions for MSTLLogicControl
 * ----------------------------------------------------------------------- */
MSTLLogicControl::MSTLLogicControl()
    : myNetWasLoaded(false)
{
}


MSTLLogicControl::~MSTLLogicControl()
{
    std::map<std::string, TLSLogicVariants>::const_iterator i;
    for(i=myLogics.begin(); i!=myLogics.end(); ++i) {
        const TLSLogicVariants &vars = (*i).second;
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
    std::map<std::string, TLSLogicVariants>::const_iterator i;
    for(i=myLogics.begin(); i!=myLogics.end(); ++i) {
        const TLSLogicVariants &vars = (*i).second;
        std::map<std::string, MSTrafficLightLogic *>::const_iterator j;
        for(j=vars.ltVariants.begin(); j!=vars.ltVariants.end(); ++j) {
            ret.push_back((*j).second);
        }
    }
    return ret;
}

const MSTLLogicControl::TLSLogicVariants &
MSTLLogicControl::get(const std::string &id) const
{
    std::map<std::string, TLSLogicVariants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        throw 1;
    }
    return (*i).second;
}


MSTrafficLightLogic *
MSTLLogicControl::get(const std::string &id, const std::string &subid) const
{
    std::map<std::string, TLSLogicVariants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        return 0;
    }
    const std::map<std::string, MSTrafficLightLogic *> &vars = (*i).second.ltVariants;
    std::map<std::string, MSTrafficLightLogic *>::const_iterator j = vars.find(subid);
    if(j==vars.end()) {
        return 0;
    }
    return (*j).second;
}


bool
MSTLLogicControl::add(const std::string &id, const std::string &subID,
                      MSTrafficLightLogic *logic, bool newDefault)
{
    if(myLogics.find(id)==myLogics.end()) {
        TLSLogicVariants var;
        var.defaultTL = 0;
        myLogics[id] = var;
    }
    std::map<std::string, TLSLogicVariants>::iterator i = myLogics.find(id);
    TLSLogicVariants &tlmap = (*i).second;
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
    std::map<std::string, TLSLogicVariants>::const_iterator i = myLogics.find(id);
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
    std::map<std::string, TLSLogicVariants>::const_iterator i = myLogics.find(tl->getID());
    if(i==myLogics.end()) {
        return false;
    }
    return (*i).second.defaultTL == tl;
}


MSTrafficLightLogic *
MSTLLogicControl::getActive(const std::string &id) const
{
    std::map<std::string, TLSLogicVariants>::const_iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        return 0;
    }
    return (*i).second.defaultTL;
}


bool
MSTLLogicControl::switchTo(const std::string &id, const std::string &subid)
{
    std::map<std::string, TLSLogicVariants>::iterator i = myLogics.find(id);
    if(i==myLogics.end()) {
        return false;
    }
    MSTrafficLightLogic *touse = (*i).second.ltVariants[subid];
    std::vector<MSTrafficLightLogic*>::iterator j =
        find(myActiveLogics.begin(), myActiveLogics.end(), (*i).second.defaultTL);
    if(j==myActiveLogics.end()) {
        return false;
    }
    *j = touse;
    (*i).second.defaultTL = touse;
    return true;
}


bool
MSTLLogicControl::addWAUT(SUMOTime refTime, const std::string &id,
                          const std::string &startProg)
{
    if(myWAUTs.find(id)!=myWAUTs.end()) {
        return false;
    }
    WAUT w;
    w.id = id;
    w.refTime = refTime;
    w.startProg = startProg;
    myWAUTs[id] = w;
    return true;
}


bool
MSTLLogicControl::addWAUTSwitch(const std::string &wautid,
                                SUMOTime when, const std::string &to)
{
    if(myWAUTs.find(wautid)==myWAUTs.end()) {
        return false;
    }
    WAUTSwitch s;
    s.to = to;
    s.when = when;
    myWAUTs[wautid].switches.push_back(s);
    if(myWAUTs[wautid].switches.size()==1) {
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            new SwitchInitCommand(*this, wautid),
            when-myWAUTs[wautid].refTime, MSEventControl::NO_CHANGE);
    }
    return true;
}


bool
MSTLLogicControl::addWAUTJunction(const std::string &wautid,
                                  const std::string &junc,
                                  const std::string &proc,
                                  bool synchron)
{
    if(myWAUTs.find(wautid)==myWAUTs.end()) {
        return false;
    }
    if(myLogics.find(junc)==myLogics.end()) {
        return false;
    }
    WAUTJunction j;
    j.junction = junc;
    j.procedure = proc;
    j.synchron = synchron;
    myWAUTs[wautid].junctions.push_back(j);
    // set the current program
    TLSLogicVariants &vars = myLogics.find(junc)->second;
    switchTo(vars.defaultTL->getID(), myWAUTs[wautid].startProg);
    return true;
}


SUMOTime
MSTLLogicControl::initWautSwitch(MSTLLogicControl::SwitchInitCommand &cmd)
{
    const std::string &wautid = cmd.getWAUTID();
    int index = cmd.getIndex();
    WAUTSwitch s = myWAUTs[wautid].switches[index];
    for(std::vector<WAUTJunction>::iterator i=myWAUTs[wautid].junctions.begin(); i!=myWAUTs[wautid].junctions.end(); ++i) {

        TLSLogicVariants &vars = myLogics.find((*i).junction)->second;
        MSTrafficLightLogic *from = vars.defaultTL;
        MSTrafficLightLogic *to = vars.ltVariants.find(s.to)->second;

        WAUTSwitchProcedure *proc = 0;
        if((*i).procedure=="GSP") {
            proc = new WAUTSwitchProcedure_GSP(from, to, (*i).synchron);
        } else if((*i).procedure=="Stretch") {
            proc = new WAUTSwitchProcedure_Stretch(from, to, (*i).synchron);
        } else {
            proc = new WAUTSwitchProcedure_JustSwitch(from, to, (*i).synchron);
        }

        WAUTSwitchProcess p;
        p.junction = (*i).junction;
        p.proc = proc;
        p.from = from;
        p.to = to;

        myCurrentlySwitched.push_back(p);
    }
    index++;
    if(index==myWAUTs[wautid].switches.size()) {
        return 0;
    }
    return myWAUTs[wautid].switches[index].when - myWAUTs[wautid].switches[index-1].when;
}


void
MSTLLogicControl::check2Switch()
{
    for(std::vector<WAUTSwitchProcess>::iterator i=myCurrentlySwitched.begin(); i!=myCurrentlySwitched.end(); ) {
        const WAUTSwitchProcess &proc = *i;
        if(proc.proc->trySwitch()) {
            delete proc.proc;
            switchTo((*i).to->getID(), (*i).to->getSubID());
            i = myCurrentlySwitched.erase(i);
        } else {
            ++i;
        }
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


