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
// Revision 1.3  2006/01/16 13:21:28  dkrajzew
// computation of detector types validated for the 'messstrecke'-scenario
//
// Revision 1.2  2006/01/16 10:46:24  dkrajzew
// some initial work on  the dfrouter
//
// Revision 1.1  2006/01/13 16:40:56  ericnicolay
// base classes for the reading of the detectordefs
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
    myOptions(oc),  myContainer(con)
{
}


DFDetectorHandler::~DFDetectorHandler()
{
}


void
DFDetectorHandler::myStartElement(int element, const std::string&name,
                             const Attributes &attrs)
{
    if(name=="detector_definition") {
        string id;
        try {
            id = getString(attrs, SUMO_ATTR_ID);
        } catch (EmptyData&) {
            MsgHandler::getErrorInstance()->inform(
                "A detector without an id occured within '" + _file + ".");
            return;
        }
        string lane;
        try {
            lane = getString(attrs, SUMO_ATTR_LANE);
        } catch (EmptyData&) {
            MsgHandler::getErrorInstance()->inform(
                "A detector without a lane information occured within '" + _file + "' (detector id='" + id + ").");
            return;
        }
        SUMOReal pos;
        try {
            pos = getFloat(attrs, SUMO_ATTR_POS);
        } catch (EmptyData&) {
            MsgHandler::getErrorInstance()->inform(
                "A detector without a lane position occured within '" + _file + "' (detector id='" + id + ").");
            return;
        } catch (NumberFormatException&) {
            MsgHandler::getErrorInstance()->inform(
                "Not numeric lane position within '" + _file + "' (detector id='" + id + ").");
            return;
        }
        string mml_type = getStringSecure(attrs, SUMO_ATTR_TYPE, "");
        dfdetector_type type = TYPE_NOT_DEFINED;
        if(mml_type=="between") {
            type = BETWEEN_DETECTOR;
        } else if(mml_type=="source") {
            type = SOURCE_DETECTOR;
        } else if(mml_type=="highway_source") {
            type = HIGHWAY_SOURCE_DETECTOR;
        } else if(mml_type=="sink") {
            type = SINK_DETEKTOR;
        }
        DFDetector detector(id, lane, pos, type);
        if(!myContainer.addDetector(detector)) {
            MsgHandler::getErrorInstance()->inform(
                "Could not add detector '" + id + "' (probably the id is already used).");
        }
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


