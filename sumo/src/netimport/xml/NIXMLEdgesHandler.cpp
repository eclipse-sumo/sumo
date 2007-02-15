/****************************************************************************/
/// @file    NIXMLEdgesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Realises the loading of the edges given in a
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
#include <iostream>
#include <map>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include "NIXMLEdgesHandler.h"
#include <cmath>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/geoconv/GeoConvHelper.h>

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
NIXMLEdgesHandler::NIXMLEdgesHandler(NBNodeCont &nc,
                                     NBEdgeCont &ec,
                                     NBTypeCont &tc,
                                     NBDistrictCont &dc,
                                     OptionsCont &options)
        : SUMOSAXHandler("xml-edges - file"),
        _options(options),
        myNodeCont(nc), myEdgeCont(ec), myTypeCont(tc), myDistrictCont(dc)
{}


NIXMLEdgesHandler::~NIXMLEdgesHandler()
{}


void
NIXMLEdgesHandler::myStartElement(int element, const std::string &/*name*/,
                                  const Attributes &attrs)
{
    if (element==SUMO_TAG_EDGE) {
        // retrieve the id of the edge
        setID(attrs);
        // retrieve the name of the edge
        setName(attrs);
        // use default values, first
        myCurrentSpeed = myTypeCont.getDefaultSpeed();
        myCurrentPriority = myTypeCont.getDefaultPriority();
        myCurrentLaneNo = myTypeCont.getDefaultNoLanes();
        myCurrentEdgeFunction = NBEdge::EDGEFUNCTION_NORMAL;
        // check whether a type's values shall be used
        checkType(attrs);
        // speed, priority and the number of lanes have now default values;
        // try to read the real values from the file
        setGivenSpeed(attrs);
        setGivenLanes(attrs);
        setGivenPriority(attrs);
        setGivenType(attrs);
        // try to get the shape
        myShape = tryGetShape(attrs);
        // and how to spread the lanes
        myLanesSpread = getSpreadFunction(attrs);
        // try to set the nodes
        if (!setNodes(attrs)) {
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
            if (myShape.size()==0) {
                edge = new NBEdge(
                           myCurrentID, myCurrentName,
                           myFromNode, myToNode,
                           myCurrentType, myCurrentSpeed,
                           myCurrentLaneNo, myLength, myCurrentPriority, myLanesSpread,
                           myCurrentEdgeFunction);
            } else {
                edge = new NBEdge(
                           myCurrentID, myCurrentName,
                           myFromNode, myToNode,
                           myCurrentType, myCurrentSpeed,
                           myCurrentLaneNo, myLength, myCurrentPriority,
                           myShape, myLanesSpread,
                           myCurrentEdgeFunction);
            }
            // insert the edge
            if (!myEdgeCont.insert(edge)) {
                addError("Duplicate edge occured. ID='" + myCurrentID + "'");
                delete edge;
            }
        } catch (...) {
            addError(
                "Important information (probably the source or the destination node) missing in edge '"
                + myCurrentID + "'.");
        }
    }
    if (element==SUMO_TAG_LANE) {
        NBEdge *edge = myEdgeCont.retrieve(myCurrentID);
        if (edge==0) {
            if (!OptionsSubSys::helper_CSVOptionMatches("remove-edges", myCurrentID)) {
                addError("Additional lane information could not been set - the edge with id '" + myCurrentID + "' is not known.");
            }
            return;
        }
        int lane = getIntSecure(attrs, "id", -1);
        if (lane<0) {
            addError("Missing lane-id in lane definition (edge '" + myCurrentID + "').");
            return;
        }
        // check whether this lane exists
        /*
        if(edge->getNoLanes()<lane) {
            edge->incLaneNo(lane - edge->getNoLanes());
        }
        */
        // set information about allwed / disallowed vehicle classes
        string disallowed = getStringSecure(attrs, "disallow", "");
        string allowed = getStringSecure(attrs, "allow", "");
        if (disallowed!="") {
            StringTokenizer st(disallowed, ";");
            while (st.hasNext()) {
                edge->disallowVehicleClass(lane, getVehicleClassID(st.next()));
            }
        }
        if (allowed!="") {
            StringTokenizer st(allowed, ";");
            while (st.hasNext()) {
                string next = st.next();
                if (next[0]=='-') {
                    edge->disallowVehicleClass(lane, getVehicleClassID(next.substr(1)));
                } else {
                    edge->allowVehicleClass(lane, getVehicleClassID(next));
                }
            }
        }
        // set information about later beginning lanes

        SUMOReal forcedLength = getFloatSecure(attrs, "forceLength", -1);
        // to split?
        if (forcedLength>0) {
            // maybe the edge has already been split at this position
            string nid = myCurrentID + "/" +  toString(forcedLength);
            if (myNodeCont.retrieve(nid)==0) {
                SUMOReal splitLength = forcedLength;
                // split only if not
                //  get the proper edge - it may be a previously splitted edge
                bool toNext = true;
                do {
                    toNext = true;
                    if (edge->getToNode()->getOutgoingEdges().size()!=1) {
                        toNext = false;
                        break;
                    }
                    string nextID = edge->getToNode()->getOutgoingEdges()[0]->getID();
                    if (nextID.substr(0, myCurrentID.length()+1)!=myCurrentID+"/") {
                        toNext = false;
                        break;
                    }
                    SUMOReal dist = TplConvert<char>::_2SUMOReal(nextID.substr(myCurrentID.length()+1).c_str());
                    if (forcedLength>dist) {
                        toNext = false;
                        splitLength -= dist;
                        break;
                    } else {}
                    if (toNext) {
                        edge = edge->getToNode()->getOutgoingEdges()[0];
                    }
                } while (toNext);

                //  build the node
                if (edge->getGeometry().length()-splitLength>0) {
                    Position2D p = edge->getGeometry().positionAtLengthPosition(edge->getGeometry().length() - splitLength);
                    NBNode *rn = new NBNode(nid, p);
                    if (myNodeCont.insert(rn)) {
                        //  split the edge
                        myEdgeCont.splitAt(myDistrictCont, edge, splitLength, rn,
                                           edge->getID(), nid, edge->getNoLanes(), edge->getNoLanes());
                    } else {
                        // hmm, the node could not be build!?
                        delete rn;
                        addError("Could not insert node '" + nid + "' for edge splitting.");
                        return;
                    }
                    NBEdge *e = myEdgeCont.retrieve(nid);
                    bool cont = true;
                    do {
                        cont = false;
                        const EdgeVector &ev = e->getFromNode()->getIncomingEdges();
                        if (ev.size()==1) {
                            NBEdge *prev = ev[0];
                            string idp = prev->getID();
                            string idp2 = idp.substr(0, idp.find('/'));
                            string idc = e->getID();
                            string idc2 = idc.substr(0, idc.find('/'));
                            if (idp2==idc2) {
                                e = prev;
                                if (prev->getNoLanes()>1) {
                                    assert(prev->getNoLanes()>1);
                                    prev->decLaneNo(1);
                                } else {
                                    MsgHandler::getWarningInstance()->inform("Could not split edge '" + prev->getID() + "'.");
                                }
                                cont = true;
                            }
                        }
                    } while (cont);
                } else {
                    MsgHandler::getWarningInstance()->inform("Could not split edge '" + edge->getID() + "'.");
                }
            }
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
    if(hasAttribute(attrs, SUMO_ATTR_NAME)) {
        myCurrentName = getString(attrs, SUMO_ATTR_NAME);
    } else {
        myCurrentName = myCurrentID;
    }
}


void
NIXMLEdgesHandler::checkType(const Attributes &attrs)
{
    // try to get the type and maybe to overwrite default values for speed, priority and th enumber of lanes
    myCurrentEdgeFunction = NBEdge::EDGEFUNCTION_NORMAL;
    myCurrentType = "";
    if(hasAttribute(attrs, SUMO_ATTR_TYPE)) {
        myCurrentType = getString(attrs, SUMO_ATTR_TYPE);
        myCurrentSpeed = myTypeCont.getSpeed(myCurrentType);
        myCurrentPriority = myTypeCont.getPriority(myCurrentType);
        myCurrentLaneNo = myTypeCont.getNoLanes(myCurrentType);
        myCurrentEdgeFunction = myTypeCont.getFunction(myCurrentType);
    } else {
        myCurrentType = "";
    }
}


void
NIXMLEdgesHandler::setGivenSpeed(const Attributes &attrs)
{
    try {
        myCurrentSpeed =
            getFloatSecure(attrs, SUMO_ATTR_SPEED, (SUMOReal) myCurrentSpeed);
        if (_options.getBool("speed-in-kmh")) {
            myCurrentSpeed = myCurrentSpeed / (SUMOReal) 3.6;
        }
    } catch (NumberFormatException) {
        addError("Not numeric value for speed (at tag ID='" + myCurrentID + "').");
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
        addError("Not numeric value for nolanes (at tag ID='" + myCurrentID + "').");
    }
}


void
NIXMLEdgesHandler::setGivenPriority(const Attributes &attrs)
{
    // try to retrieve given priority
    try {
        myCurrentPriority =
            getIntSecure(attrs, SUMO_ATTR_PRIORITY, myCurrentPriority);
    } catch (NumberFormatException) {
        addError("Not numeric value for priority (at tag ID='" + myCurrentID + "').");
    }
}


void
NIXMLEdgesHandler::setGivenType(const Attributes &attrs)
{
    // try to get the tpe
    string func = getStringSecure(attrs, SUMO_ATTR_FUNC, "");
    if (func=="") {
        return;
    }
    if (func=="source") {
        myCurrentEdgeFunction = NBEdge::EDGEFUNCTION_SOURCE;
    } else if (func=="sink") {
        myCurrentEdgeFunction = NBEdge::EDGEFUNCTION_SINK;
    } else if (func!="normal") {
        addError("Unknown edge function '" + func + "' in edge '" + myCurrentID + "'.");
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
//    myNodeCont.addGeoreference(Position2D((SUMOReal) (x / 100000.0), (SUMOReal) (y / 100000.0)));
    if (myBegNodeXPos!=-1&&myBegNodeYPos!=-1) {
        Position2D pos(myBegNodeXPos, myBegNodeYPos);
        GeoConvHelper::remap(pos);
        myBegNodeXPos = pos.x();
        myBegNodeYPos = pos.y();
    }
    if (myEndNodeXPos!=-1&&myEndNodeYPos!=-1) {
        Position2D pos(myEndNodeXPos, myEndNodeYPos);
        GeoConvHelper::remap(pos);
        myEndNodeXPos = pos.x();
        myEndNodeYPos = pos.y();
    }
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
    if (!insertNodesCheckingCoherence()) {
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
        addError("Not numeric value for " + attrName + " (at tag ID='" + myCurrentID + "').");
        return -1.0;
    }
}


bool
NIXMLEdgesHandler::insertNodesCheckingCoherence()
{
    // check if both coordinates and names are given.
    // if so, store them in the nodes-map
    MsgHandler *msgh = _options.getBool("omit-corrupt-edges")
                       ? MsgHandler::getWarningInstance()
                       : MsgHandler::getErrorInstance();
    bool coherent = false;
    if (myBegNodeXPos!=-1.0 &&
            myBegNodeYPos!=-1.0 &&
            myEndNodeXPos!=-1.0 &&
            myEndNodeYPos!=-1.0 &&
            myCurrentBegNodeID!="" &&
            myCurrentEndNodeID!="") {

        Position2D begPos(myBegNodeXPos, myBegNodeYPos);
        Position2D endPos(myEndNodeXPos, myEndNodeYPos);
        coherent = true;
        if (!myNodeCont.insert(myCurrentBegNodeID, begPos)) {
            msgh->inform("On parsing edge '" + myCurrentID + "':\n Position of node '" + myCurrentBegNodeID + "' mismatches previous positions");
            coherent = false;
        }
        if (!myNodeCont.insert(myCurrentEndNodeID, endPos)) {
            msgh->inform("On parsing edge '" + myCurrentID + "':\n Position of node '" + myCurrentEndNodeID + "' mismatches previous positions");
            coherent = false;
        }
    }


    myFromNode = myToNode = 0;
    // if the node coordinates are given, but no names for them, insert the nodes only
    if (myBegNodeXPos!=-1.0 &&
            myBegNodeYPos!=-1.0 &&
            myEndNodeXPos!=-1.0 &&
            myEndNodeYPos!=-1.0 &&
            myCurrentBegNodeID=="" &&
            myCurrentEndNodeID=="") {

        Position2D begPos(myBegNodeXPos, myBegNodeYPos);
        Position2D endPos(myEndNodeXPos, myEndNodeYPos);
        myFromNode = myNodeCont.retrieve(begPos);
        myToNode = myNodeCont.retrieve(endPos);
        if (myFromNode!=0 && myToNode!=0) {
            coherent = true;
        } else {
            if (myFromNode==0) {
                myFromNode = new NBNode(myNodeCont.getFreeID(), begPos);
                if (!myNodeCont.insert(myFromNode)) {
                    msgh->inform("On parsing edge '" + myCurrentID + "':\n Could not insert from-node '" + myFromNode->getID() + "'");
                    return false;
                }
            }
            if (myToNode==0) {
                myToNode = new NBNode(myNodeCont.getFreeID(), endPos);
                if (!myNodeCont.insert(myToNode)) {
                    msgh->inform("On parsing edge '" + myCurrentID + "':\n Could not insert to-node '" + myToNode->getID() + "'");
                    return false;
                }
            }
            coherent = true;
        }
    } else {
        myFromNode = myNodeCont.retrieve(myCurrentBegNodeID);
        myToNode = myNodeCont.retrieve(myCurrentEndNodeID);
    }
    // if only the names of the nodes are known, get the coordinates
    if (myFromNode!=0 && myToNode!=0 &&
            myBegNodeXPos==-1.0 &&
            myBegNodeYPos==-1.0 &&
            myEndNodeXPos==-1.0 &&
            myEndNodeYPos==-1.0) {

        myBegNodeXPos = myFromNode->getPosition().x();
        myBegNodeYPos = myFromNode->getPosition().y();
        myEndNodeXPos = myToNode->getPosition().x();
        myEndNodeYPos = myToNode->getPosition().y();
        coherent = true;
    }
    if (!coherent) {
        msgh->inform("On parsing edge '" + myCurrentID + "':\n Either the name or the position of a node is not given.");
    }
    return coherent;
}


void
NIXMLEdgesHandler::setLength(const Attributes &attrs)
{
    // get the length or compute it
    if(hasAttribute(attrs, SUMO_ATTR_LENGTH)) {
        try {
            myLength = getFloat(attrs, SUMO_ATTR_LENGTH);
        } catch (NumberFormatException) {
            addError("Not numeric value for length (at tag ID='" + myCurrentID + "').");
        }
    } else {
        if (myBegNodeXPos!=-1.0 &&
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
    }
}



Position2DVector
NIXMLEdgesHandler::tryGetShape(const Attributes &attrs)
{
    string shpdef;
    shpdef = getStringSecure(attrs, SUMO_ATTR_SHAPE, "");
    // return if no shape was given
    if (shpdef=="") {
        return Position2DVector();
    }
    // try to build shape
    try {
        Position2DVector shape1 = GeomConvHelper::parseShape(shpdef);
        Position2DVector shape;
        for (size_t i=0; i<shape1.size(); ++i) {
            Position2D pos(shape1[i]);
            GeoConvHelper::remap(pos);
            shape.push_back(pos);
        }

        if (shape.size()==1) {
            addError("The shape of edge '" + myCurrentID + "' has only one entry.");
        }
        return shape;
    } catch (EmptyData) {
        addError("At least one number is missing in shape definition for edge '" + myCurrentID + "'.");
    } catch (NumberFormatException) {
        addError("A non-numeric value occured in shape definition for edge '" + myCurrentID + "'.");
    }
    return Position2DVector();
}


NBEdge::LaneSpreadFunction
NIXMLEdgesHandler::getSpreadFunction(const Attributes &attrs)
{
    if (getStringSecure(attrs, SUMO_ATTR_SPREADFUNC, "")=="center") {
        return NBEdge::LANESPREAD_CENTER;
    }
    return NBEdge::LANESPREAD_RIGHT;
}


void
NIXMLEdgesHandler::myCharacters(int /*element*/, const std::string &/*name*/,
                                const std::string &/*chars*/)
{}


void
NIXMLEdgesHandler::myEndElement(int /*element*/, const std::string &/*name*/)
{}



/****************************************************************************/

