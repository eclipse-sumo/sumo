/****************************************************************************/
/// @file    ROJTRHelpers.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id: $
///
// A set of helping functions
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
#include "ROJTRHelpers.h"
#include <router/RONet.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include "ROJTREdge.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
ROJTRHelpers::parseROJTREdges(RONet &net, std::set<ROJTREdge*> &into,
                                  const std::string &chars)
{
    StringTokenizer st(chars, ";");
    while (st.hasNext()) {
        string name = st.next();
        ROJTREdge *edge = static_cast<ROJTREdge*>(net.getEdge(name));
        if (edge==0) {
            MsgHandler::getErrorInstance()->inform("The edge '" + name + " declared as a sink was not found in the network.");
            throw ProcessError();
        }
        into.insert(edge);
    }
}



/****************************************************************************/

