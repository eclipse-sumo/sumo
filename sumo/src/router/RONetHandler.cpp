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
// Revision 1.4  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <string>
#include <utils/options/OptionsCont.h>
#include <utils/common/SErrorHandler.h>
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

using namespace std;


RONetHandler::RONetHandler(OptionsCont &oc, RONet &net)
    : SUMOSAXHandler("sumo-network", true, true),
    _options(oc), _net(net), _currentName(),
    _currentEdge(0)
{
}


RONetHandler::~RONetHandler()
{
}


void
RONetHandler::myStartElement(int element, const std::string &name,
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
        parseLane(attrs);
        break;
    case SUMO_TAG_JUNCTION:
        parseJunction(attrs);
        break;
    case SUMO_TAG_CEDGE:
        parseConnEdge(attrs);
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
            SErrorHandler::add(
                string("An unknown edge occured within '")
                + _file + string("."));
            SErrorHandler::add("Contact your net supplier!");
        }
        string type = getString(attrs, SUMO_ATTR_FUNC);
        if(type=="normal") {
            _currentEdge->setType(ROEdge::ET_NORMAL);
        } else if(type=="source") {
            _currentEdge->setType(ROEdge::ET_SOURCE);
        } else if(type=="sink") {
            _currentEdge->setType(ROEdge::ET_SINK);
        } else {
            throw 1; // !!!
        }
    } catch (EmptyData) {
        SErrorHandler::add(
            string("An edge without an id occured within '")
            + _file + string("."));
        SErrorHandler::add("Contact your net supplier!");
    }
}


void
RONetHandler::parseLane(const Attributes &attrs)
{
    double maxSpeed = -1;
    double length = -1;
    // get the speed
    try {
        maxSpeed = getFloat(attrs, SUMO_ATTR_MAXSPEED);
    } catch (EmptyData) {
        SErrorHandler::add(
            string("A lane without a maxspeed definition occured within '")
            + _file + string("'."));
        return;
    } // !!! NumberFormatException
    // get the length
    try {
        length = getFloat(attrs, SUMO_ATTR_LENGTH);
    } catch (EmptyData) {
        SErrorHandler::add(
            string("A lane without a length definition occured within '")
            + _file + string("'."));
        return;
    } // !!! NumberFormatException
    string id = getStringSecure(attrs, SUMO_ATTR_ID, "");
    if(id.length()==0) {
        SErrorHandler::add("Could not retrieve the id of a lane.");
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
        SErrorHandler::add(
            string("A junction without an id occured within '")
            + _file + string("'."));
        SErrorHandler::add("Contact your net supplier!");
    }
}


void
RONetHandler::parseConnEdge(const Attributes &attrs) {
    // no error by now
    bool error = false;
    // try to get the edge to connect the current edge to
    try {
        // get the edge to connect
        string succID = getString(attrs, SUMO_ATTR_ID);
        ROEdge *succ = _net.getEdge(succID);
        if(succ!=0&&_currentEdge!=0) {
            // connect edge
            _currentEdge->addSucceeder(succ);
        } else {
            SErrorHandler::add(
                string("The succeding edge '") + succID
                + string("' does not exist."));
            error = true;
        }
    } catch (EmptyData) {
        SErrorHandler::add("A succeding edge has no id.");
        error = true;
    }
    // check whether everything was ok
    if(error) {
        if(_currentName.length()!=0) {
            SErrorHandler::add(
                string(" At edge '") + _currentName + string("'."));
        }
        SErrorHandler::add(" Contact your net supplier.");
    }
}




void
RONetHandler::myCharacters(int element, const std::string &name,
                           const std::string &chars)
{
    if(element==SUMO_TAG_EDGES) {
        preallocateEdges(chars);
    }
/*    if(_step==1&&element==SUMO_TAG_outedges) {
        parseoutedges(chars);
    }*/
}

void
RONetHandler::preallocateEdges(const std::string &chars) {
    StringTokenizer st(chars);
    while(st.hasNext()) {
        string id = st.next();
        _net.addEdge(id, new ROEdge(id));
    }
}


void
RONetHandler::myEndElement(int element, const std::string &name)
{
 /*   switch(element) {
    case SUMO_TAG_EDGE:
        if(_currentName!="") {
            _net.addEdge(_currentName, _currentEdge);
        }
        _currentName = "";
        break;
    default:
        break;
    }*/
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "RONetHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


