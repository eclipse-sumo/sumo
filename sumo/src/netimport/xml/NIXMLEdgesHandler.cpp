/***************************************************************************
                          NIXMLEdgesHandler.cpp
			  Realises the loading of the edges given in a
			  XML-format
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.5  2003/04/01 15:26:15  dkrajzew
// insertion of nodes is now checked, but still unsafe; districts are always weighted
//
// Revision 1.4  2003/03/20 16:34:54  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/18 13:08:30  dkrajzew
// usage of node position within xml-edge descriptions allowed
//
// Revision 1.2  2003/02/13 15:55:15  dkrajzew
// xml-loaders now use new options
//
// Revision 1.1  2003/02/07 11:16:30  dkrajzew
// names changed
//
// Revision 1.1  2002/10/16 15:45:36  dkrajzew
// initial commit for xml-importing classes
//
// Revision 1.7  2002/06/21 10:13:27  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.6  2002/06/17 15:19:30  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.5  2002/06/11 16:00:42  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/10 06:56:14  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized; options now recognize false numerical input
//
// Revision 1.3  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:12  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.5  2002/04/09 12:21:25  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.4  2002/03/22 10:50:04  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.3  2002/03/20 08:32:41  dkrajzew
// Numeric format exception is now being caught when the edge s length is malicious
//
// Revision 1.2  2002/03/15 09:20:51  traffic
// Warnings (unused variables) patched
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include <map>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "NIXMLEdgesHandler.h"
#include <cmath>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/convert/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/XMLBuildingExceptions.h>

/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NIXMLEdgesHandler::NIXMLEdgesHandler(OptionsCont &options,
                                     bool warn, bool verbose)
    : SUMOSAXHandler("xml-edges - file", warn, verbose),
    _options(options)
{
}


NIXMLEdgesHandler::~NIXMLEdgesHandler()
{
}


void
NIXMLEdgesHandler::myStartElement(int element, const std::string &tag,
                                  const Attributes &attrs)
{
    string id;
    if(tag=="edge") {
        try {
            id = getString(attrs, SUMO_ATTR_ID);
            // retrieve the name of the edge
            string name;
            try {
                name = getString(attrs, SUMO_ATTR_NAME);
            } catch (EmptyData) {
                name = id;
            }
            // try to get the type and maybe to overwrite default values for speed, priority and th enumber of lanes
            string type;
            double speed = NBTypeCont::getDefaultSpeed();
            int priority = NBTypeCont::getDefaultPriority();
            int noLanes = NBTypeCont::getDefaultNoLanes();
            try {
                type = getString(attrs, SUMO_ATTR_TYPE);
                speed = NBTypeCont::getSpeed(type);
                priority = NBTypeCont::getPriority(type);
                noLanes = NBTypeCont::getNoLanes(type);
            } catch (EmptyData) {
                if(_warn) {
                    cout << "No type given... Using default." << endl;
                }
            }
            /// speed, priority and the number of lanes have now default values;
            /// try to read the real values from the file
            try {
                speed = getFloatSecure(attrs, SUMO_ATTR_SPEED, speed);
                if(_options.getBool("speed-in-km")) {
                    speed = speed / 3.6;
                }
            } catch (NumberFormatException) {
                addError(
                    string("Not numeric value for Speed (at tag ID='")
                    + id + string("')."));
            }
            // try to get the number of lanes
            try {
                noLanes = getIntSecure(attrs, SUMO_ATTR_NOLANES, noLanes);
            } catch (NumberFormatException) {
                addError(
                    string("Not numeric value for NoLanes (at tag ID='")
                    + id + string("')."));
            }
            // try to get the priority
                // check whether the number of lanes shall be used
            if(_options.getBool("use-laneno-as-priority")) {
                priority = noLanes;
            }
                // try to retrieve given priority
            try {
                priority = getIntSecure(attrs, SUMO_ATTR_PRIORITY, priority);
            } catch (NumberFormatException) {
                addError(
                    string("Not numeric value for Priority (at tag ID='")
                    + id + string("')."));
            }
            // the names and the coordinates of
            // the beginning and the end nodes may be found, try
            string from = getStringSecure(attrs, SUMO_ATTR_FROMNODE, "");;
            string to = getStringSecure(attrs, SUMO_ATTR_TONODE, "");;
            double xb, xe, yb, ye;
            xb = xe = yb = ye = -1.0;
            try {
                xb = getFloatSecure(attrs, SUMO_ATTR_XFROM, -1);
            } catch (NumberFormatException) {
                addError(
                    string("Not numeric value for XFrom (at tag ID='")
                    + id + string("')."));
            }
            try {
                yb = getFloatSecure(attrs, SUMO_ATTR_YFROM, -1);
            } catch (NumberFormatException) {
                addError(
                    string("Not numeric value for YFrom (at tag ID='")
                    + id + string("')."));
            }
            try {
                xe = getFloatSecure(attrs, SUMO_ATTR_XTO, -1);
            } catch (NumberFormatException) {
                addError(
                    string("Not numeric value for XTo (at tag ID='")
                    + id + string("')."));
            }
            try {
                ye = getFloatSecure(attrs, SUMO_ATTR_YTO, -1);
            } catch (NumberFormatException) {
                addError(
                    string("Not numeric value for YTo (at tag ID='")
                    + id + string("')."));
            }
            // check if both coordinates and names are given.
            // if so, store them in the nodes-map
            bool coherent = false;
            if(xb!=-1.0 && xe!=-1.0 && ye!=-1.0 && yb!=-1.0 && from!="" && to!="") {
                if(NBNodeCont::insert(from, xb, yb))
                    if(NBNodeCont::insert(to, xe, ye))
                        coherent = true;
            }
            NBNode *fromNode;
            NBNode *toNode;
            // if the node coordinates are given, but no names for them, insert the nodes only
            if(xb!=-1.0 && xe!=-1.0 && ye!=-1.0 && yb!=-1.0 && from=="" && to=="") {
                fromNode = NBNodeCont::retrieve(xb, yb);
                toNode = NBNodeCont::retrieve(xe, ye);
                if(fromNode!=0 && toNode!=0) {
                    coherent = true;
                } else {
                    if(fromNode==0) {
                        fromNode = new NBNode(NBNodeCont::getFreeID(), xb, yb);
                        if(!NBNodeCont::insert(fromNode)) {
                            cout << "nope, NIVissimDisturbance" << endl;
                            throw 1;
                        }
                    }
                    if(toNode==0) {
                        toNode = new NBNode(NBNodeCont::getFreeID(), xe, ye);
                        if(!NBNodeCont::insert(toNode)) {
                            cout << "nope, NIVissimDisturbance" << endl;
                            throw 1;
                        }
                    }
                    coherent = true;
                }
            } else {
                fromNode = NBNodeCont::retrieve(from);
                toNode = NBNodeCont::retrieve(to);
            }
            // if only the names of the nodes are known, insert
            if(fromNode!=0 && toNode!=0 &&
	            xb==-1.0 && xe==-1.0 && ye==-1.0 && yb==-1.0) {
                xb = fromNode->getXCoordinate();
                yb = fromNode->getYCoordinate();
                xe = toNode->getXCoordinate();
                ye = toNode->getYCoordinate();
                coherent = true;
            }
            // check if the data were coherent
            if(!coherent&&!_options.getBool("omit-corrupt-edges")) {
                addError(
                    string("The data are not coherent or the nodes are not given..."));
            }
            // get the length or compute it
            double length;
            try {
                length = getFloat(attrs, SUMO_ATTR_LENGTH);
            } catch (EmptyData) {
                if(xb!=-1.0 && xe!=-1.0 && ye!=-1.0 && yb!=-1.0) {
                    length = sqrt((xb-xe)*(xb-xe) + (yb-ye)*(yb-ye));
                    if(_warn) {
                        cout << "Computed length = " << length << endl;
                    }
                } else {
                    length = 0;
                    if(_warn) {
                        cout << "Continuing with length=0" << endl;
                    }
                }
            } catch (NumberFormatException) {
                addError(
                    string("Not numeric value for length (at tag ID='")
                    + id + string("')."));
            }
            /// insert the parsed edge into the edges map
            if(coherent) {
                try {
                    NBEdge *edge = new NBEdge(id, name, fromNode, toNode, type, speed, noLanes, length, priority);
                    if(!NBEdgeCont::insert(edge)) {
                        addError(
                            string("Duplicate edge occured. ID='") + id
                            + string("'"));
                        delete edge;
                    }
                } catch (...) {
                    addError(
                        string("Error: Important information (propably the source or the destination node) missing in edge '")
                        + id + string("'."));
                }
            }
        } catch (EmptyData) {
            cout << "No id given... Skipping." << endl;
        }
    }
}

void
NIXMLEdgesHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
}

void
NIXMLEdgesHandler::myEndElement(int element, const std::string &name)
{
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIXMLEdgesHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:
