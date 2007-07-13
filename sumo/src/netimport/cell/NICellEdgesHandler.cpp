/****************************************************************************/
/// @file    NICellEdgesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// A LineHandler-derivate to load edges form a cell-edges-file
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBCapacity2Lanes.h>
#include "NICellEdgesHandler.h"

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
NICellEdgesHandler::NICellEdgesHandler(NBNodeCont &nc,
                                       NBEdgeCont &ec,
                                       NBTypeCont &tc,
                                       const std::string &file,
                                       NBCapacity2Lanes capacity2Lanes)
        : FileErrorReporter("cell-edges", file),
        myNodeCont(nc), myEdgeCont(ec), myTypeCont(tc),
        myCapacity2Lanes(capacity2Lanes)
{}


NICellEdgesHandler::~NICellEdgesHandler()
{}


bool
NICellEdgesHandler::report(const std::string &result)
{
    // skip white lines (?)
    if (result.length()==0) {
        return true;
    }
    //
    StringTokenizer st(result);
    // skip white lines (?)
    if (st.size()==0) {
        return true;
    }
    // check mandatory parameter
    if (st.size()<4) {
        addError("The following cell-edges - entry does not match the format:\n" + result);
        throw ProcessError();
    }
    string id = NBHelpers::normalIDRepresentation(st.next());
    NBNode *from =
        myNodeCont.retrieve(NBHelpers::normalIDRepresentation(st.next()));
    NBNode *to =
        myNodeCont.retrieve(NBHelpers::normalIDRepresentation(st.next()));
    // check whether the nodes are known
    if (from==0||to==0) {
        if (from==0) {
            addError("The from-node is not known within the following entry:\n" + result);
        } else {
            addError("The to-node is not known within the following entry:\n" + result);
        }
        throw ProcessError();
    }
    // other values
    SUMOReal length = -1;
    SUMOReal speed = myTypeCont.getDefaultSpeed();
    int priority = myTypeCont.getDefaultPriority();
    size_t nolanes = myTypeCont.getDefaultNoLanes();
    // get the length
    try {
        length = TplConvert<char>::_2SUMOReal(st.next().c_str());
    } catch (NumberFormatException &) {
        addError("Non-numeric length entry in the following line:\n" + result);
    }
    // get the other parameter
    bool hadExpliciteLaneNo = false;
    while (st.hasNext()) {
        string name = st.next();
        if (!st.hasNext()) {
            addError("Something seems to be wrong with the following line:\n" + result);
        } else {
            string value = st.next();
            // do nothing on maximal vehicle number
            if (name=="-n"||name=="-s") {}
            // try to resolve capacity->nolanes
            if (name=="-c"&&!hadExpliciteLaneNo) {
                nolanes = myCapacity2Lanes.get(
                              TplConvert<char>::_2SUMOReal(value.c_str()));
            }
            // process maximal velocity
            if (name=="-v") {
                try {
                    speed = TplConvert<char>::_2SUMOReal(value.c_str());
                } catch (NumberFormatException &) {
                    addError("Non-numeric speed value:\n" + result);
                }
            }
            // process lane number
            if (name=="-l") {
                try {
                    nolanes = TplConvert<char>::_2int(value.c_str());
                    hadExpliciteLaneNo = true;
                } catch (NumberFormatException &) {
                    addError("Non-numeric lane number value:\n" + result);
                }
            }
        }
    }
    myEdgeCont.insert(new NBEdge(id, id, from, to, "DEFAULT",
                                 speed, nolanes, priority));
    return true;
}



/****************************************************************************/

