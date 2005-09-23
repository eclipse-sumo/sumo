/***************************************************************************
                          NICellEdgesHandler.cpp
             A LineHandler-derivate to load edges form a cell-edges-file
                             -------------------
    project              : SUMO
    begin                : Fri, 19 Jul 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.9  2005/09/23 06:02:04  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.6  2004/11/23 10:23:52  dkrajzew
// debugging
//
// Revision 1.5  2004/07/02 09:35:23  dkrajzew
// adaptation of current FastLane import
//
// Revision 1.4  2004/01/12 15:31:18  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.3  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/06/18 11:14:48  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.1  2003/02/07 11:10:56  dkrajzew
// names changed
//
// Revision 1.1  2002/10/16 15:40:04  dkrajzew
// initial commit for cell importing classes
//
// Revision 1.1  2002/07/25 08:41:45  dkrajzew
// Visum7.5 and Cell import added
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
NICellEdgesHandler::NICellEdgesHandler(NBNodeCont &nc,
                                       NBEdgeCont &ec,
                                       NBTypeCont &tc,
                                       const std::string &file,
                                       NBCapacity2Lanes capacity2Lanes)
    : FileErrorReporter("cell-edges", file), _capacity2Lanes(capacity2Lanes),
    myNodeCont(nc), myEdgeCont(ec), myTypeCont(tc)
{
}


NICellEdgesHandler::~NICellEdgesHandler()
{
}


bool
NICellEdgesHandler::report(const std::string &result)
{
    // skip white lines (?)
    if(result.length()==0) {
        return true;
    }
    //
    StringTokenizer st(result);
    // skip white lines (?)
    if(st.size()==0) {
        return true;
    }
    // check mandatory parameter
    if(st.size()<4) {
        addError(
            string("The following cell-edges - entry does not match the format:")
            + '\n' + result.c_str());
        throw ProcessError();
    }
    string id = NBHelpers::normalIDRepresentation(st.next());
    NBNode *from =
        myNodeCont.retrieve(NBHelpers::normalIDRepresentation(st.next()));
    NBNode *to =
        myNodeCont.retrieve(NBHelpers::normalIDRepresentation(st.next()));
    // check whether the nodes are known
    if(from==0||to==0) {
        if(from==0) {
            addError(
                string("The from-node is not known within the following entry:")
                + '\n' + result.c_str());
        } else {
            addError(
                string("The to-node is not known within the following entry:")
                + '\n' + result.c_str());
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
    } catch (NumberFormatException) {
        addError(
            string("Non-numeric length entry in the following line:")
            + '\n' + result.c_str());
    }
    // get the other parameter
    while(st.hasNext()) {
        string name = st.next();
        if(!st.hasNext()) {
            addError(
                string("Something seems to be wrong with the following line:")
                + '\n' + result.c_str());
        } else {
            string value = st.next();
            // do nothing on maximal vehicle number
            if(name=="-n"||name=="-s") {
            }
            // try to resolve capacity->nolanes
            if(name=="-c") {
                nolanes = _capacity2Lanes.get(
                    TplConvert<char>::_2SUMOReal(value.c_str()));
            }
            // process maximal velocity
            if(name=="-v") {
                try {
                    speed = TplConvert<char>::_2SUMOReal(value.c_str());
                } catch (NumberFormatException) {
                    addError(
                        string("Non-numeric speed value:")
                        + '\n' + result.c_str());
                }
            }
            // process lane number
            if(name=="-l") {
                try {
                    nolanes = TplConvert<char>::_2int(value.c_str());
                } catch (NumberFormatException) {
                    addError(
                        string("Non-numeric lane number value:")
                        + '\n' + result.c_str());
                }
            }
        }
    }
    myEdgeCont.insert(new NBEdge(id, id, from, to, "DEFAULT",
        speed, nolanes, length, priority));
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


