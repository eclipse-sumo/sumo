//---------------------------------------------------------------------------//
//                        RONetHandler.cpp -
//  The handler for SUMO-Networks
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
// Revision 1.12  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.11  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.10  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2004/04/14 13:53:50  roessel
// Changes and additions in order to implement supplementary-weights.
//
// Revision 1.8  2004/02/10 07:16:05  dkrajzew
// removed some debug-variables
//
// Revision 1.7  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// Revision 1.6  2003/09/05 15:22:44  dkrajzew
// handling of internal lanes added
//
// Revision 1.5  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
// this handler puts it further to output instances.
// changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
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

#include <string>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "ROEdge.h"
#include "ROLane.h"
#include "RONode.h"
#include "ROEdgeVector.h"
#include "RONet.h"
#include "RONetHandler.h"
#include "ROAbstractEdgeBuilder.h"

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
RONetHandler::RONetHandler(OptionsCont &oc, RONet &net,
                           ROAbstractEdgeBuilder &eb)
    : SUMOSAXHandler("sumo-network"),
    _options(oc), _net(net), _currentName(),
    _currentEdge(0), myEdgeBuilder(eb)
{
}


RONetHandler::~RONetHandler()
{
}


void
RONetHandler::myStartElement(int element, const std::string&,
                             const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_EDGE:
        // in the first step, we do need the name to allocate the edge
        // in the second, we need it to know to which edge we have to add
        //  the following edges to
        parseEdge(attrs);
        break;
    case SUMO_TAG_LANE:
        if(_process) {
            parseLane(attrs);
        }
        break;
    case SUMO_TAG_JUNCTION:
        parseJunction(attrs);
        break;
    case SUMO_TAG_CEDGE:
        if(_process) {
            parseConnEdge(attrs);
        }
        break;
    default:
        break;
    }
}


void
RONetHandler::parseEdge(const Attributes &attrs)
{
    try {
        _currentName = getString(attrs, SUMO_ATTR_ID);
        _currentEdge = _net.getEdge(_currentName);
        if(_currentEdge==0) {
            MsgHandler::getErrorInstance()->inform(
                string("An unknown edge occured within '")
                + _file + string("."));
            MsgHandler::getErrorInstance()->inform("Contact your net supplier!");
        }
        string type = getString(attrs, SUMO_ATTR_FUNC);
        _process = true;
        if(type=="normal") {
            _currentEdge->setType(ROEdge::ET_NORMAL);
        } else if(type=="source") {
            _currentEdge->setType(ROEdge::ET_SOURCE);
        } else if(type=="sink") {
            _currentEdge->setType(ROEdge::ET_SINK);
        } else if(type=="internal") {
            _process = false;
        } else {
            throw 1; // !!!
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("An edge without an id occured within '")
            + _file + string("."));
        MsgHandler::getErrorInstance()->inform("Contact your net supplier!");
    }
}


void
RONetHandler::parseLane(const Attributes &attrs)
{
    SUMOReal maxSpeed = -1;
    SUMOReal length = -1;
    // get the speed
    try {
        maxSpeed = getFloat(attrs, SUMO_ATTR_MAXSPEED);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("A lane without a maxspeed definition occured within '")
            + _file + string("'."));
        return;
    } // !!! NumberFormatException
    // get the length
    try {
        length = getFloat(attrs, SUMO_ATTR_LENGTH);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("A lane without a length definition occured within '")
            + _file + string("'."));
        return;
    } // !!! NumberFormatException
    string id = getStringSecure(attrs, SUMO_ATTR_ID, "");
    if(id.length()==0) {
        MsgHandler::getErrorInstance()->inform("Could not retrieve the id of a lane.");
        return;
    }
    // add when both values are valid
    if(maxSpeed>0&&length>0&&id.length()>0) {
        _currentEdge->addLane(new ROLane(id, length, maxSpeed));
    }
}


void
RONetHandler::parseJunction(const Attributes &attrs)
{
    try {
        _currentName = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("A junction without an id occured within '")
            + _file + string("'."));
        MsgHandler::getErrorInstance()->inform("Contact your net supplier!");
    }
}


void
RONetHandler::parseConnEdge(const Attributes &attrs)
{
    // no error by now
    bool error = false;
    // try to get the edge to connect the current edge to
    try {
        // get the edge to connect
        string succID = getString(attrs, SUMO_ATTR_ID);
        ROEdge *succ = _net.getEdge(succID);
        if(succ!=0&&_currentEdge!=0) {
            // connect edge
            _currentEdge->addFollower(succ);
        } else {
            MsgHandler::getErrorInstance()->inform(
                string("The succeding edge '") + succID
                + string("' does not exist."));
            error = true;
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("A succeding edge has no id.");
        error = true;
    }
    // check whether everything was ok
    if(error) {
        if(_currentName.length()!=0) {
            MsgHandler::getErrorInstance()->inform(
                string(" At edge '") + _currentName + string("'."));
        }
        MsgHandler::getErrorInstance()->inform(" Contact your net supplier.");
    }
}


void
RONetHandler::myCharacters(int element, const std::string&,
                           const std::string &chars)
{
    if(element==SUMO_TAG_EDGES) {
        preallocateEdges(chars);
    }
}


void
RONetHandler::preallocateEdges(const std::string &chars)
{
    StringTokenizer st(chars);
    while(st.hasNext()) {
        string id = st.next();
        _net.addEdge(myEdgeBuilder.buildEdge(id)); // !!! where is the edge deleted when failing?
    }
}


void
RONetHandler::myEndElement(int, const std::string&)
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


