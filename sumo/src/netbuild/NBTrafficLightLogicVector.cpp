//---------------------------------------------------------------------------//
//                        NBTrafficLightLogicVector.cpp -
//  A vector of traffic lights logics
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
// Revision 1.12  2005/09/23 06:01:06  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.11  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.10  2005/04/27 11:48:26  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.9  2003/12/04 13:03:58  dkrajzew
// possibility to pass the tl-type from the netgenerator added
//
// Revision 1.8  2003/11/17 07:26:02  dkrajzew
// computations needed for collecting e2-values over multiple lanes added
//
// Revision 1.7  2003/09/25 09:02:51  dkrajzew
// multiple lane in tl-logic - bug patched
//
// Revision 1.6  2003/06/16 08:02:45  dkrajzew
// further work on Vissim-import
//
// Revision 1.5  2003/06/05 11:43:36  dkrajzew
// class templates applied; documentation added
//
// Revision 1.4  2003/03/20 16:23:10  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.3  2003/03/03 14:59:22  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <iostream>
#include <set>
#include <cassert>
#include <utils/common/ToString.h>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicVector.h"

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
NBTrafficLightLogicVector::NBTrafficLightLogicVector(
        const NBConnectionVector &inLanes, std::string type)
    : myInLinks(inLanes), myType(type)
{
}

NBTrafficLightLogicVector::~NBTrafficLightLogicVector()
{
    for(LogicVector::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        delete (*i);
    }
}


void
NBTrafficLightLogicVector::add(NBTrafficLightLogic *logic)
{
    if(logic==0) {
        return;
    }
    if(!contains(logic)) {
        _cont.push_back(logic);
    } else {
        delete logic;
    }
}


void
NBTrafficLightLogicVector::add(const NBTrafficLightLogicVector &cont)
{
    for(LogicVector::const_iterator i=cont._cont.begin(); i!=cont._cont.end(); i++) {
        if(!contains(*i)) {
            NBTrafficLightLogic *logic = *i;
            add(new NBTrafficLightLogic(*logic));
        }
    }
}


void
NBTrafficLightLogicVector::writeXML(std::ostream &os) const
{
    SUMOReal distance = 250;
    set<string> inLanes;
    for(NBConnectionVector::const_iterator j=myInLinks.begin(); j!=myInLinks.end(); j++) {
        assert((*j).getFromLane()>=0&&(*j).getFrom()!=0);
        string id = (*j).getFrom()->getID() + string("_") + toString<int>((*j).getFromLane());
        inLanes.insert(id);
    }
    size_t pos = 0;
    (*(_cont.begin()))->writeXML(os, pos++, distance, myType, inLanes);
//    for(LogicVector::const_iterator i=_cont.begin(); i!=_cont.end(); i++) {
//        (*i)->writeXML(os, pos++, distance, myType, inLanes);
//    }
}

bool
NBTrafficLightLogicVector::contains(NBTrafficLightLogic *logic) const
{
    for(LogicVector::const_iterator i=_cont.begin(); i!=_cont.end(); i++) {
        if((*i)->equals(*logic)) {
            return true;
        }
    }
    return false;
}


int
NBTrafficLightLogicVector::size() const
{
    return _cont.size();
}


const std::string &
NBTrafficLightLogicVector::getType() const
{
    return myType;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


