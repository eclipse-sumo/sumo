/****************************************************************************/
/// @file    MSJunctionLogic.cpp
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id: $
///
// kinds of logic-implementations.
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

#include "MSJunctionLogic.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
MSJunctionLogic::DictType MSJunctionLogic::myDict;


// ===========================================================================
// member method definitions
// ===========================================================================
unsigned int
MSJunctionLogic::nLinks()
{
    return myNLinks;
}

//-------------------------------------------------------------------------//

unsigned int
MSJunctionLogic::nInLanes()
{
    return myNInLanes;
}

//-------------------------------------------------------------------------//

MSJunctionLogic::MSJunctionLogic(unsigned int nLinks,
                                 unsigned int nInLanes) :
        myNLinks(nLinks),
        myNInLanes(nInLanes)
{}

//-------------------------------------------------------------------------//

MSJunctionLogic::~MSJunctionLogic()
{}

//-------------------------------------------------------------------------//

bool
MSJunctionLogic::dictionary(string id, MSJunctionLogic* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------//

MSJunctionLogic*
MSJunctionLogic::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}

//-------------------------------------------------------------------------//

void
MSJunctionLogic::clear()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}


void
MSJunctionLogic::replace(std::string id, MSJunctionLogic* junction)
{
    myDict[id] = junction;
}



/****************************************************************************/

