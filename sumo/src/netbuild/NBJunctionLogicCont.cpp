/****************************************************************************/
/// @file    NBJunctionLogicCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Class for the io-ing between junctions (nodes) and the computers
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

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include "NBLogicKeyBuilder.h"
#include "NBJunctionLogicCont.h"
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NBJunctionLogicCont::NBJunctionLogicCont()
{}


NBJunctionLogicCont::~NBJunctionLogicCont()
{
    clear();
}


bool
NBJunctionLogicCont::exists(const string &key)
{
    return _map.find(key)!=_map.end();
}

/*
int
NBJunctionLogicCont::try2convert(const string &key)
{
    int rot = 0;
    if(exists(string("b.") + key)) return rot;
    string run = key;
    run = NBLogicKeyBuilder::rotateKey(run);
    while(run!=key) {
        rot++;
        if(exists(string("b.") + run)) return rot;
        run = NBLogicKeyBuilder::rotateKey(run);
    }
    return -1;
}
*/

void
NBJunctionLogicCont::add(const std::string &key,
                             const std::string &xmlDescription)
{
    LogicMap::iterator i=_map.find(key);
    if (i!=_map.end()) {
        string tmp = (*i).second;
    }
    _map.insert(LogicMap::value_type(key, xmlDescription));
}


void
NBJunctionLogicCont::writeXML(ostream &into)
{
    for (LogicMap::iterator i=_map.begin(); i!=_map.end(); i++) {
        into << (*i).second << endl;
    }
    into << endl;
}


void
NBJunctionLogicCont::clear()
{
    _map.clear();
}



/****************************************************************************/

