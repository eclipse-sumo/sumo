//---------------------------------------------------------------------------//
//                        DFDetectorHandler.cpp -
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
// Revision 1.1  2006/01/13 16:40:56  ericnicolay
// base classes for the reading of the detectordefs
//
// Revision 1.13  2005/11/09 06:43:51  dkrajzew
// error handling improved
//
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
//#include "ROEdge.h"
//#include "ROLane.h"
//#include "RONode.h"
//#include "ROEdgeVector.h"
//#include <RONet.h>
#include "DFDetectorHandler.h"
//#include "ROAbstractEdgeBuilder.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

//#ifdef HALLIGALLI

/* =========================================================================
 * method definitions
 * ======================================================================= */
DFDetectorHandler::DFDetectorHandler(OptionsCont &oc, DFDetectorCon &con)
    : SUMOSAXHandler("Detector-Defintion"),
    _options(oc),  _con(con),_currentName()//,_currentDetector(0)
{
}


DFDetectorHandler::~DFDetectorHandler()
{
}


void
DFDetectorHandler::myStartElement(int element, const std::string&,
                             const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_EDGE:
        // in the first step, we do need the name to allocate the edge
        // in the second, we need it to know to which edge we have to add
        //  the following edges to
//        parseEdge(attrs);
        break;
    case SUMO_TAG_LANE:
        if(_process) {
  //          parseLane(attrs);
        }
        break;
    case SUMO_TAG_JUNCTION:
    //    parseJunction(attrs);
        break;
    case SUMO_TAG_CEDGE:
        if(_process) {
//            parseConnEdge(attrs);
        }
        break;
    default:
        break;
    }
}


void
DFDetectorHandler::parseDetector(const Attributes &attrs)
{
    try {

        _currentName = getString(attrs, SUMO_ATTR_ID);
//        _currentDetector = _net.getEdge(_currentName);
/*        if(_currentEdge==0) {
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
        }*/
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("An edge without an id occured within '")
            + _file + string("."));
        MsgHandler::getErrorInstance()->inform("Contact your net supplier!");
    }
}




void
DFDetectorHandler::myCharacters(int element, const std::string&,
                           const std::string &chars)
{
    if(element==SUMO_TAG_EDGES) {
//        preallocateEdges(chars);
    }
}


/*void
DFDetectorHandler::preallocateEdges(const std::string &chars)
{
    StringTokenizer st(chars);
    while(st.hasNext()) {
        string id = st.next();
        _net.addEdge(myEdgeBuilder.buildEdge(id)); // !!! where is the edge deleted when failing?
    }
}*/


void
DFDetectorHandler::myEndElement(int, const std::string&)
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


