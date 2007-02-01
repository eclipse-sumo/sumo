/****************************************************************************/
/// @file    NICellNodesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id: $
///
// A LineHandler-derivate to load nodes form a cell-nodes-file
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
#include <utils/importio/LineHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NICellNodesHandler.h"

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
NICellNodesHandler::NICellNodesHandler(NBNodeCont &nc,
                                       const std::string &file)
        : FileErrorReporter("cell-nodes", file), myNodeCont(nc)
{}

NICellNodesHandler::~NICellNodesHandler()
{}

bool
NICellNodesHandler::report(const std::string &result)
{
    if (result.length()>0) {
        StringTokenizer st(result);
        if (st.size()!=3) {
            addError("The following cell-nodes - entry contains an error:\n" + result);
        } else {
            string id = NBHelpers::normalIDRepresentation(st.next());
            SUMOReal x, y;
            try {
                x = TplConvert<char>::_2SUMOReal(st.next().c_str());
                y = TplConvert<char>::_2SUMOReal(st.next().c_str());
                if (!myNodeCont.insert(id, Position2D(x, y))) {
                    MsgHandler::getErrorInstance()->inform("Could not build node '" + id + "'.");
                }
            } catch (NumberFormatException) {
                addError("The following cell-nodes - entry contains a non-digit position information:\n" + result);
                throw ProcessError();
            }
        }
    }
    return true;
}



/****************************************************************************/

