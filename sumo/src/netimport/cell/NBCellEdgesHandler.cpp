/***************************************************************************
                          NBCellEdgesHandler.cpp
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
// Revision 1.1  2002/10/16 15:40:04  dkrajzew
// initial commit for cell importing classes
//
// Revision 1.1  2002/07/25 08:41:45  dkrajzew
// Visum7.5 and Cell import added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <utils/importio/LineHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/convert/TplConvert.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBCapacity2Lanes.h>
#include "NBCellEdgesHandler.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NBCellEdgesHandler::NBCellEdgesHandler(const std::string &file,
                                       bool warn, bool verbose,
                                       NBCapacity2Lanes capacity2Lanes)
    : _file(file), _capacity2Lanes(capacity2Lanes)
{
}

NBCellEdgesHandler::~NBCellEdgesHandler()
{
}

bool
NBCellEdgesHandler::report(const std::string &result)
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
        SErrorHandler::add(string("Within file '") + _file + string("':"), true);
        SErrorHandler::add(
            "The following cell-edges - entry does not match the format:",
            true);
        SErrorHandler::add(result.c_str(), true);
        throw ProcessError();
    }
    string id = NBHelpers::normalIDRepresentation(st.next());
    NBNode *from =
        NBNodeCont::retrieve(NBHelpers::normalIDRepresentation(st.next()));
    NBNode *to =
        NBNodeCont::retrieve(NBHelpers::normalIDRepresentation(st.next()));
    // check whether the nodes are known
    if(from==0||to==0) {
        SErrorHandler::add(string("Within file '") + _file + string("':"), true);
        if(from==0) {
            SErrorHandler::add("The from-node is not known within the following entry:", true);
        } else {
            SErrorHandler::add("The to-node is not known within the following entry:", true);
        }
        SErrorHandler::add(result.c_str(), true);
        throw ProcessError();
    }
    // other values
    float length;
    float speed = NBTypeCont::getDefaultSpeed();
    int priority = NBTypeCont::getDefaultPriority();
    size_t nolanes = NBTypeCont::getDefaultNoLanes();
    // get the length
    try {
        length = TplConvert<char>::_2float(st.next().c_str());
    } catch (NumberFormatException) {
        SErrorHandler::add(string("Within file '") + _file + string("':"), true);
        SErrorHandler::add("Non-numeric length entry in the following line:", true);
        SErrorHandler::add(result.c_str(), true);
    }
    // get the other parameter
    while(st.hasNext()) {
        string name = st.next();
        if(!st.hasNext()) {
            SErrorHandler::add(string("Within file '") + _file + string("':"), true);
            SErrorHandler::add("Something seems to be wrong with the following line:", true);
            SErrorHandler::add(result.c_str(), true);
        } else {
            string value = st.next();
            // do nothing on maximal vehicle number
            if(name=="-n"||name=="-s") {
            }
            // try to resolve capacity->nolanes
            if(name=="-c") {
                nolanes = _capacity2Lanes.get(
                    TplConvert<char>::_2float(value.c_str()));
            }
            // process maximal velocity
            if(name=="-v") {
                try {
                    speed = TplConvert<char>::_2float(value.c_str());
                } catch (NumberFormatException) {
                    SErrorHandler::add(string("Within file '") + _file + string("':"), true);
                    SErrorHandler::add("Non-numeric speed value:", true);
                    SErrorHandler::add(result.c_str(), true);
                }
            }
        }
    }
    NBEdgeCont::insert(new NBEdge(id, id, from, to, "DEFAULT",
        speed, nolanes, length, priority));
    return true;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBCellEdgesHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


