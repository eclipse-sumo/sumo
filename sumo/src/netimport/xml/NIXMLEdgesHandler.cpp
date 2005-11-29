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
// Revision 1.22  2005/11/29 13:31:16  dkrajzew
// debugging
//
// Revision 1.21  2005/11/15 10:15:49  dkrajzew
// debugging and beautifying for the next release
//
// Revision 1.20  2005/11/14 09:53:49  dkrajzew
// "speed-in-km" is now called "speed-in-kmh";
//  removed two files definition for arcview
//
// Revision 1.19  2005/10/17 09:18:44  dkrajzew
// got rid of the old MSVC memory leak checker
//
// Revision 1.18  2005/10/07 11:41:16  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.17  2005/09/23 06:04:00  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.16  2005/09/15 12:03:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.15  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.14  2004/11/23 10:23:51  dkrajzew
// debugging
//
// Revision 1.13  2004/08/02 12:44:28  dkrajzew
// using Position2D instead of two SUMOReals
//
// Revision 1.12  2004/01/12 15:36:36  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.11  2003/11/11 08:18:20  dkrajzew
// consequent geometry usage; made the code a little bit more pretty
//
// Revision 1.10  2003/07/16 15:34:07  dkrajzew
// conversion tools splitted and relocated to avoid mandatory inclusion
//  of unused files
//
// Revision 1.9  2003/07/07 08:32:19  dkrajzew
// adapted the importer to the new lane geometry description
//
// Revision 1.8  2003/06/19 10:59:34  dkrajzew
// error output patched
//
// Revision 1.7  2003/06/18 11:17:29  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.6  2003/05/20 09:44:35  dkrajzew
// some make-up done (splitting large methods)
//
// Revision 1.5  2003/04/01 15:26:15  dkrajzew
// insertion of nodes is now checked, but still unsafe;
//  districts are always weighted
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
// windows eol removed; template class definition inclusion depends now on
//  the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/10 06:56:14  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized;
//  options now recognize false numerical input
//
// Revision 1.3  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:12  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
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
// Numeric format exception is now being caught when the edge s length is
//  malicious
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
#include <iostream>
#include <map>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "NIXMLEdgesHandler.h"
#include <cmath>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/XMLBuildingExceptions.h>

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
NIXMLEdgesHandler::NIXMLEdgesHandler(NBNodeCont &nc,
									 NBEdgeCont &ec,
									 NBTypeCont &tc,
									 OptionsCont &options)
    : SUMOSAXHandler("xml-edges - file"),
    _options(options),
    myNodeCont(nc), myEdgeCont(ec), myTypeCont(tc)
{
}


NIXMLEdgesHandler::~NIXMLEdgesHandler()
{
}


void
NIXMLEdgesHandler::myStartElement(int element, const std::string &tag,
                                  const Attributes &attrs)
{
    if(tag=="edge") {
        // retrieve the id of the edge
        setID(attrs);
        // retrieve the name of the edge
        setName(attrs);
        // use default values, first
        myCurrentSpeed = myTypeCont.getDefaultSpeed();
        myCurrentPriority = myTypeCont.getDefaultPriority();
        myCurrentLaneNo = myTypeCont.getDefaultNoLanes();
        // check whether a type's values shall be used
        checkType(attrs);
        // speed, priority and the number of lanes have now default values;
        // try to read the real values from the file
        setGivenSpeed(attrs);
        setGivenLanes(attrs);
        setGivenPriority(attrs);
        // try to get the shape
        myShape = tryGetShape(attrs);
            // and how to spread the lanes
        myLanesSpread = getSpreadFunction(attrs);
        // try to set the nodes
        if(!setNodes(attrs)) {
            // return if this failed
            return;
        }
        // compute the edge's length
        setLength(attrs);
        /// insert the parsed edge into the edges map
        try {
            NBEdge *edge = 0;
            // the edge must be allocated in dependence to whether a shape
            //  is given
            if(myShape.size()==0) {
                edge = new NBEdge(
                    myCurrentID, myCurrentName,
                    myFromNode, myToNode,
                    myCurrentType, myCurrentSpeed,
                    myCurrentLaneNo, myLength, myCurrentPriority, myLanesSpread);
            } else {
                edge = new NBEdge(
                    myCurrentID, myCurrentName,
                    myFromNode, myToNode,
                    myCurrentType, myCurrentSpeed,
                    myCurrentLaneNo, myLength, myCurrentPriority,
                    myShape, myLanesSpread);
            }
            // insert the edge
            if(!myEdgeCont.insert(edge)) {
                addError(
                    string("Duplicate edge occured. ID='") + myCurrentID
                    + string("'"));
                delete edge;
            }
        } catch (...) {
            addError(
                string("Important information (propably the source or the destination node) missing in edge '")
                    + myCurrentID + string("'."));
        }
    }
}


void
NIXMLEdgesHandler::setID(const Attributes &attrs)
{
    myCurrentID = "";
    try {
        myCurrentID = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        WRITE_WARNING("No id given... Skipping.");
    }
}


void
NIXMLEdgesHandler::setName(const Attributes &attrs)
{
    try {
        myCurrentName = getString(attrs, SUMO_ATTR_NAME);
    } catch (EmptyData) {
        myCurrentName = myCurrentID;
    }
}


void
NIXMLEdgesHandler::checkType(const Attributes &attrs)
{
    // try to get the type and maybe to overwrite default values for speed, priority and th enumber of lanes
    myCurrentType = "";
    try {
        myCurrentType = getString(attrs, SUMO_ATTR_TYPE);
        myCurrentSpeed = myTypeCont.getSpeed(myCurrentType);
        myCurrentPriority = myTypeCont.getPriority(myCurrentType);
        myCurrentLaneNo = myTypeCont.getNoLanes(myCurrentType);
    } catch (EmptyData) {
        myCurrentType = "";
    }
}


void
NIXMLEdgesHandler::setGivenSpeed(const Attributes &attrs)
{
    try {
        myCurrentSpeed =
            getFloatSecure(attrs, SUMO_ATTR_SPEED, (SUMOReal) myCurrentSpeed);
        if(_options.getBool("speed-in-kmh")) {
            myCurrentSpeed = myCurrentSpeed / (SUMOReal) 3.6;
        }
    } catch (NumberFormatException) {
        addError(
            string("Not numeric value for speed (at tag ID='")
            + myCurrentID + string("')."));
    }
}


void
NIXMLEdgesHandler::setGivenLanes(const Attributes &attrs)
{
    // try to get the number of lanes
    try {
        myCurrentLaneNo =
            getIntSecure(attrs, SUMO_ATTR_NOLANES, myCurrentLaneNo);
    } catch (NumberFormatException) {
        addError(
            string("Not numeric value for nolanes (at tag ID='")
            + myCurrentID + string("')."));
    }
}

void
NIXMLEdgesHandler::setGivenPriority(const Attributes &attrs)
{
    // try to get the priority
        // check whether the number of lanes shall be used
    if(_options.getBool("use-laneno-as-priority")) {
        myCurrentPriority = myCurrentLaneNo;
    }
    // try to retrieve given priority
    try {
        myCurrentPriority =
            getIntSecure(attrs, SUMO_ATTR_PRIORITY, myCurrentPriority);
    } catch (NumberFormatException) {
        addError(
            string("Not numeric value for priority (at tag ID='")
            + myCurrentID + string("')."));
    }
}


bool
NIXMLEdgesHandler::setNodes(const Attributes &attrs)
{
    // the names and the coordinates of the beginning and the end node
        // may be found, try
    myCurrentBegNodeID = getStringSecure(attrs, SUMO_ATTR_FROMNODE, "");
    myCurrentEndNodeID = getStringSecure(attrs, SUMO_ATTR_TONODE, "");
        // or their positions
    myBegNodeXPos = myBegNodeYPos = myEndNodeXPos = myEndNodeYPos = -1.0;
    myBegNodeXPos = tryGetPosition(attrs, SUMO_ATTR_XFROM, "XFrom");
    myBegNodeYPos = tryGetPosition(attrs, SUMO_ATTR_YFROM, "YFrom");
    myEndNodeXPos = tryGetPosition(attrs, SUMO_ATTR_XTO, "XTo");
    myEndNodeYPos = tryGetPosition(attrs, SUMO_ATTR_YTO, "YTo");
        // check with shape
    /*
    if(myShape.size()!=0) {
        myBegNodeXPos = myShape.getBegin().x();
        myBegNodeXPos = myShape.getBegin().y();
        myBegNodeXPos = myShape.getEnd().x();
        myBegNodeXPos = myShape.getEnd().y();
    }
    */
    // check the obtained values for nodes
    if(!insertNodesCheckingCoherence()) {
        if(!_options.getBool("omit-corrupt-edges")) {
			addError(
				string("On parsing edge '") + myCurrentID + string("':"));
            addError(
                string(" The data are not coherent or the nodes are not given..."));
        }
        return false;
    }
    return true;
}


SUMOReal
NIXMLEdgesHandler::tryGetPosition(const Attributes &attrs, int tag,
                                  const std::string &attrName)
{
    try {
        return getFloatSecure(attrs, tag, -1);
    } catch (NumberFormatException) {
        addError(
            string("Not numeric value for ") + attrName
                + (" (at tag ID='")
                + myCurrentID + string("')."));
        return -1.0;
    }
}


bool
NIXMLEdgesHandler::insertNodesCheckingCoherence()
{
    // check if both coordinates and names are given.
    // if so, store them in the nodes-map
    bool coherent = false;
    if( myBegNodeXPos!=-1.0 &&
        myBegNodeYPos!=-1.0 &&
        myEndNodeXPos!=-1.0 &&
        myEndNodeYPos!=-1.0 &&
        myCurrentBegNodeID!="" &&
        myCurrentEndNodeID!="") {

        Position2D begPos(myBegNodeXPos, myBegNodeYPos);
        Position2D endPos(myEndNodeXPos, myEndNodeYPos);
        if(myNodeCont.insert(myCurrentBegNodeID, begPos)) {
            if(myNodeCont.insert(myCurrentEndNodeID, endPos)) {
                coherent = true;
            }
        }
    }


    myFromNode = myToNode = 0;
        // if the node coordinates are given, but no names for them, insert the nodes only
    if( myBegNodeXPos!=-1.0 &&
        myBegNodeYPos!=-1.0 &&
        myEndNodeXPos!=-1.0 &&
        myEndNodeYPos!=-1.0 &&
        myCurrentBegNodeID=="" &&
        myCurrentEndNodeID=="") {

        Position2D begPos(myBegNodeXPos, myBegNodeYPos);
        Position2D endPos(myEndNodeXPos, myEndNodeYPos);
        myFromNode = myNodeCont.retrieve(begPos);
        myToNode = myNodeCont.retrieve(endPos);
        if(myFromNode!=0 && myToNode!=0) {
            coherent = true;
        } else {
            if(myFromNode==0) {
                myFromNode =
                    new NBNode(myNodeCont.getFreeID(), begPos);
                if(!myNodeCont.insert(myFromNode)) {
                    throw 1;
                }
            }
            if(myToNode==0) {
                myToNode =
                    new NBNode(myNodeCont.getFreeID(), endPos);
                if(!myNodeCont.insert(myToNode)) {
                    throw 1;
                }
            }
            coherent = true;
        }
    } else {
        myFromNode = myNodeCont.retrieve(myCurrentBegNodeID);
        myToNode = myNodeCont.retrieve(myCurrentEndNodeID);
    }
    // if only the names of the nodes are known, get the coordinates
    if(myFromNode!=0 && myToNode!=0 &&
        myBegNodeXPos==-1.0 &&
        myBegNodeYPos==-1.0 &&
        myEndNodeXPos==-1.0 &&
        myEndNodeYPos==-1.0 ) {

        myBegNodeXPos = myFromNode->getPosition().x();
        myBegNodeYPos = myFromNode->getPosition().y();
        myEndNodeXPos = myToNode->getPosition().x();
        myEndNodeYPos = myToNode->getPosition().y();
        coherent = true;
    }
    return coherent;
}


void
NIXMLEdgesHandler::setLength(const Attributes &attrs)
{
    // get the length or compute it
    try {
        myLength = getFloat(attrs, SUMO_ATTR_LENGTH);
    } catch (EmptyData) {
        if( myBegNodeXPos!=-1.0 &&
            myBegNodeYPos!=-1.0 &&
            myEndNodeXPos!=-1.0 &&
            myEndNodeYPos!=-1.0) {

            myLength = sqrt(
                (myBegNodeXPos-myEndNodeXPos)*(myBegNodeXPos-myEndNodeXPos)
                +
                (myBegNodeYPos-myEndNodeYPos)*(myBegNodeYPos-myEndNodeYPos));
        } else {
            myLength = 0;
        }
    } catch (NumberFormatException) {
        addError(
            string("Not numeric value for length (at tag ID='")
                + myCurrentID + string("')."));
    }
}



Position2DVector
NIXMLEdgesHandler::tryGetShape(const Attributes &attrs)
{
    string shpdef;
    try {
        shpdef = getString(attrs, SUMO_ATTR_SHAPE);
    } catch (EmptyData) {
    }
    // return if no shape was given
    if(shpdef=="") {
        return Position2DVector();
    }
    // try to build shape
    try {
        Position2DVector shape = GeomConvHelper::parseShape(shpdef);
        if(shape.size()==1) {
            addError(string("The shape of edge '") + myCurrentID
                + string("' has only one entry."));
        }
        return shape;
    } catch (EmptyData) {
        addError(
            string("At least one number is missing in shape definition for edge '")
            + myCurrentID + string("'."));
    } catch (NumberFormatException) {
        addError(
            string("A non-numeric value occured in shape definition for edge '")
            + myCurrentID + string("'."));
    }
    return Position2DVector();
}


NBEdge::LaneSpreadFunction
NIXMLEdgesHandler::getSpreadFunction(const Attributes &attrs)
{
    if(getStringSecure(attrs, SUMO_ATTR_SPREADFUNC, "")=="center") {
        return NBEdge::LANESPREAD_CENTER;
    }
    return NBEdge::LANESPREAD_RIGHT;
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

// Local Variables:
// mode:C++
// End:
