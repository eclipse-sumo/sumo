//---------------------------------------------------------------------------//
//                        GUIGlObject_AbstractAdd.cpp -
//  Base class for additional objects (emitter, detectors etc.)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.3  2005/10/07 11:45:32  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.2  2005/09/15 12:19:44  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:01:55  dksumo
// renamed boundery to boundary
//
// Revision 1.1  2004/10/22 12:50:50  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
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

#include "GUIGlObject_AbstractAdd.h"
#include <cassert>
#include <iostream>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
std::map<std::string, GUIGlObject_AbstractAdd*> GUIGlObject_AbstractAdd::myObjects;
std::vector<GUIGlObject_AbstractAdd*> GUIGlObject_AbstractAdd::myObjectList;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIGlObject_AbstractAdd::GUIGlObject_AbstractAdd(GUIGlObjectStorage &idStorage,
                                               std::string fullName,
                                               GUIGlObjectType type)
    : GUIGlObject(idStorage, fullName), myGlType(type)
{
//!!!    assert(myObjects.find(fullName)==myObjects.end());
    myObjects[fullName] = this;
    myObjectList.push_back(this);
}


GUIGlObject_AbstractAdd::GUIGlObject_AbstractAdd(GUIGlObjectStorage &idStorage,
                                               std::string fullName,
                                               size_t glID,
                                               GUIGlObjectType type)
    : GUIGlObject(idStorage, fullName, glID), myGlType(type)
{
//!!!    assert(myObjects.find(fullName)==myObjects.end());
    myObjects[fullName] = this;
    myObjectList.push_back(this);
}


GUIGlObject_AbstractAdd::~GUIGlObject_AbstractAdd()
{
}



GUIGlObjectType
GUIGlObject_AbstractAdd::getType() const
{
    return myGlType;
}


void
GUIGlObject_AbstractAdd::clearDictionary()
{
    std::map<std::string, GUIGlObject_AbstractAdd*>::iterator i;
    for(i=myObjects.begin(); i!=myObjects.end(); i++) {
//!!!        delete (*i).second;
    }
    myObjects.clear();
    myObjectList.clear();
}


GUIGlObject_AbstractAdd *
GUIGlObject_AbstractAdd::get(const std::string &name)
{
    std::map<std::string, GUIGlObject_AbstractAdd*>::iterator i =
        myObjects.find(name);
    if(i==myObjects.end()) {
        return 0;
    }
    return (*i).second;
}


const std::vector<GUIGlObject_AbstractAdd*> &
GUIGlObject_AbstractAdd::getObjectList()
{
    return myObjectList;
}


std::vector<size_t>
GUIGlObject_AbstractAdd::getIDList()
{
    std::vector<size_t> ret;
    for(std::vector<GUIGlObject_AbstractAdd*>::iterator i=myObjectList.begin(); i!=myObjectList.end(); ++i) {
        ret.push_back((*i)->getGlID());
    }
    return ret;
}


Boundary
GUIGlObject_AbstractAdd::getCenteringBoundary() const
{
    Boundary b;
    b.add(getPosition());
    b.grow(10);
    return b;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


