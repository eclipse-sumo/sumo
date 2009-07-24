/****************************************************************************/
/// @file    NIXMLEdgesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Importer for network edges stored in XML
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <iostream>
#include <map>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include "NIXMLEdgesHandler.h"
#include <cmath>
#include <utils/xml/SUMOSAXHandler.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

const SUMOReal SUMOXML_INVALID_POSITION = -999999.;

// ===========================================================================
// method definitions
// ===========================================================================
NIXMLEdgesHandler::NIXMLEdgesHandler(NBNodeCont &nc,
                                     NBEdgeCont &ec,
                                     NBTypeCont &tc,
                                     NBDistrictCont &dc,
                                     OptionsCont &options) throw()
        : SUMOSAXHandler("xml-edges - file"),
        myOptions(options),
        myNodeCont(nc), myEdgeCont(ec), myTypeCont(tc), myDistrictCont(dc),
        myCurrentEdge(0), 
        myHaveReportedAboutExpansionCharactersDeprecation(false),
        myHaveReportedAboutFunctionDeprecation(false) {}


NIXMLEdgesHandler::~NIXMLEdgesHandler() throw() {}


void
NIXMLEdgesHandler::myStartElement(SumoXMLTag element,
                                  const SUMOSAXAttributes &attrs) throw(ProcessError) {
    if (element==SUMO_TAG_EDGE) {
        myIsUpdate = false;
        bool ok = true;
        // initialise the edge
        myCurrentEdge = 0;
        myExpansions.clear();
        // get the id, report an error if not given or empty...
        if (!attrs.setIDFromAttributes("edge", myCurrentID)) {
            return;
        }
        myCurrentEdge = myEdgeCont.retrieve(myCurrentID);
        // check deprecated (unused) attributes
        if (!myHaveReportedAboutFunctionDeprecation&&attrs.hasAttribute(SUMO_ATTR_FUNCTION)) {
            MsgHandler::getWarningInstance()->inform("While parsing edge '" + myCurrentID + "': 'function' is deprecated.\n All occurences are ignored.");
            myHaveReportedAboutFunctionDeprecation = true;
        }
        // use default values, first
        myCurrentSpeed = myTypeCont.getDefaultSpeed();
        myCurrentPriority = myTypeCont.getDefaultPriority();
        myCurrentLaneNo = myTypeCont.getDefaultNoLanes();
        // use values from the edge to overwrite if existing, then
        if(myCurrentEdge!=0) {
            myIsUpdate = true;
            if(!myHaveReportedAboutOverwriting) {
                MsgHandler::getMessageInstance()->inform("Duplicate edge id occured ('" + myCurrentID + "'); assuming overwriting is wished.");
                myHaveReportedAboutOverwriting = true;
            }
            myCurrentSpeed = myCurrentEdge->getSpeed();
            myCurrentPriority = myCurrentEdge->getPriority();
            myCurrentLaneNo = myCurrentEdge->getNoLanes();
            myCurrentType = myCurrentEdge->getTypeID();
        }
        // check whether a type's values shall be used
        myCurrentType = "";
        if (attrs.hasAttribute(SUMO_ATTR_TYPE)) {
            myCurrentType = attrs.getString(SUMO_ATTR_TYPE);
            if (myCurrentType=="") {
                MsgHandler::getErrorInstance()->inform("Edge '" + myCurrentID + "' has an empty type.");
                return;
            }
            if (!myTypeCont.knows(myCurrentType)) {
                MsgHandler::getErrorInstance()->inform("Type '" + myCurrentType + "' used by edge '" + myCurrentID + "' was not defined.");
                return;
            }
            myCurrentSpeed = myTypeCont.getSpeed(myCurrentType);
            myCurrentPriority = myTypeCont.getPriority(myCurrentType);
            myCurrentLaneNo = myTypeCont.getNoLanes(myCurrentType);
        }
        // speed, priority and the number of lanes have now default values;
        // try to read the real values from the file
        if (attrs.hasAttribute(SUMO_ATTR_SPEED)) {
            myCurrentSpeed = attrs.getSUMORealReporting(SUMO_ATTR_SPEED, "edge", myCurrentID.c_str(), ok);
        }
        if (myOptions.getBool("speed-in-kmh")) {
            myCurrentSpeed = myCurrentSpeed / (SUMOReal) 3.6;
        }
        // try to get the number of lanes
        if (attrs.hasAttribute(SUMO_ATTR_NOLANES)) {
            myCurrentLaneNo = attrs.getIntReporting(SUMO_ATTR_NOLANES, "edge", myCurrentID.c_str(), ok);
        }
        // try to get the priority
        if (attrs.hasAttribute(SUMO_ATTR_PRIORITY)) {
            myCurrentPriority = attrs.getIntReporting(SUMO_ATTR_PRIORITY, "edge", myCurrentID.c_str(), ok);
        }

        // try to get the shape
        myShape = tryGetShape(attrs);
        // and how to spread the lanes
        if (attrs.getStringSecure(SUMO_ATTR_SPREADFUNC, "")=="center") {
            myLanesSpread = NBEdge::LANESPREAD_CENTER;
        } else {
            myLanesSpread = NBEdge::LANESPREAD_RIGHT;
        }
        // try to set the nodes
        if (!setNodes(attrs)) {
            // return if this failed
            return;
        }
        // get the length or compute it
        if (attrs.hasAttribute(SUMO_ATTR_LENGTH)) {
            myLength = attrs.getSUMORealReporting(SUMO_ATTR_LENGTH, "edge", myCurrentID.c_str(), ok);
        } else {
            myLength = 0;
        }
        /// insert the parsed edge into the edges map
        if (!ok) {
            return;
        }
        // check whether a previously defined edge shall be overwritten
        if(myCurrentEdge!=0) {
            myCurrentEdge->reinit(myFromNode, myToNode, myCurrentType, myCurrentSpeed,
                    myCurrentLaneNo, myCurrentPriority, myShape, !myOptions.getBool("add-node-positions"),
                    myLanesSpread);
        } else {
            // the edge must be allocated in dependence to whether a shape is given
            if (myShape.size()==0) {
                myCurrentEdge = new NBEdge(myCurrentID, myFromNode, myToNode, myCurrentType, myCurrentSpeed,
                    myCurrentLaneNo, myCurrentPriority, myLanesSpread);
            } else {
                myCurrentEdge = new NBEdge(myCurrentID, myFromNode, myToNode, myCurrentType, myCurrentSpeed,
                    myCurrentLaneNo, myCurrentPriority, myShape, !myOptions.getBool("add-node-positions"),
                    myLanesSpread);
            }
            myCurrentEdge->setLoadedLength(myLength);
        }
    }
    if (element==SUMO_TAG_LANE) {
        if (myCurrentEdge==0) {
            if (!OptionsCont::getOptions().isInStringVector("remove-edges", myCurrentID)) {
                MsgHandler::getErrorInstance()->inform("Additional lane information could not been set - the edge with id '" + myCurrentID + "' is not known.");
            }
            return;
        }
        int lane = attrs.getIntSecure(SUMO_ATTR_ID, -1);
        if (lane<0) {
            MsgHandler::getErrorInstance()->inform("Missing lane-id in lane definition (edge '" + myCurrentID + "').");
            return;
        }
        // check whether this lane exists
        if (lane>=(int) myCurrentEdge->getNoLanes()) {
            MsgHandler::getErrorInstance()->inform("Lane-id is larger than number of lanes (edge '" + myCurrentID + "').");
            return;
        }
        // set information about allowed / disallowed vehicle classes
        string disallowed = attrs.getStringSecure(SUMO_ATTR_DISALLOW, "");
        string allowed = attrs.getStringSecure(SUMO_ATTR_ALLOW, "");
        string preferred = attrs.getStringSecure(SUMO_ATTR_PREFER, "");
        if (disallowed.find(";") != string::npos || allowed.find(";") != string::npos || preferred.find(";") != string::npos) {
            MsgHandler::getWarningInstance()->inform("Using ';' as a list separator is deprecated, use ' ' instead (edge '" + myCurrentID + "').");
        }
        if (disallowed!="") {
            StringTokenizer st(disallowed, "; ", true);
            while (st.hasNext()) {
                myCurrentEdge->disallowVehicleClass(lane, getVehicleClassID(st.next()));
            }
        }
        if (allowed!="") {
            StringTokenizer st(allowed, "; ", true);
            while (st.hasNext()) {
                myCurrentEdge->allowVehicleClass(lane, getVehicleClassID(st.next()));
            }
        }
        if (preferred!="") {
            StringTokenizer st(preferred, "; ", true);
            while (st.hasNext()) {
                myCurrentEdge->preferVehicleClass(lane, getVehicleClassID(st.next()));
            }
        }

        // set information about later beginning lanes
        if (attrs.hasAttribute(SUMO_ATTR_FORCE_LENGTH)) {
            bool ok = true;
            int forcedLength = attrs.getIntReporting(SUMO_ATTR_FORCE_LENGTH, "lane", myCurrentID.c_str(), ok); // !!! edge id
            if (ok) {
                int nameid = forcedLength;
                forcedLength = (int)(myCurrentEdge->getGeometry().length() - forcedLength);
                std::vector<Expansion>::iterator i;
                i = find_if(myExpansions.begin(), myExpansions.end(), expansion_by_pos_finder((SUMOReal) forcedLength));
                if (i==myExpansions.end()) {
                    Expansion e;
                    e.pos = (SUMOReal) forcedLength;
                    e.nameid = nameid;
                    for (unsigned int j=0; j<myCurrentEdge->getNoLanes(); j++) {
                        e.lanes.push_back(j);
                    }
                    myExpansions.push_back(e);
                }
                i = find_if(myExpansions.begin(), myExpansions.end(), expansion_by_pos_finder((SUMOReal) forcedLength));
                std::vector<int>::iterator k = find((*i).lanes.begin(), (*i).lanes.end(), lane);
                if (k!=(*i).lanes.end()) {
                    (*i).lanes.erase(k);
                }
            }
        }
    }
    if (element==SUMO_TAG_EXPANSION) {
        bool ok = true;
        Expansion e;
        e.pos = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, "expansion", 0, ok);
        if (ok) {
            if (myCurrentEdge==0) {
                if (!OptionsCont::getOptions().isInStringVector("remove-edges", myCurrentID)) {
                    MsgHandler::getErrorInstance()->inform("Additional lane information could not been set - the edge with id '" + myCurrentID + "' is not known.");
                }
                return;
            }
            if (e.pos<0) {
                e.pos = myCurrentEdge->getGeometry().length() + e.pos;
            }
            myExpansions.push_back(e);
            if(attrs.hasAttribute(SUMO_ATTR_LANES)) {
                parseExpansionLanes(attrs.getString(SUMO_ATTR_LANES));
            }
        }
    }
}


bool
NIXMLEdgesHandler::setNodes(const SUMOSAXAttributes &attrs) {
    // the names and the coordinates of the beginning and the end node
    // may be found, try
    string begNodeID = myIsUpdate ? myCurrentEdge->getFromNode()->getID() : "";
    string endNodeID = myIsUpdate ? myCurrentEdge->getToNode()->getID() : "";
    begNodeID = attrs.hasAttribute(SUMO_ATTR_FROMNODE) ? attrs.getStringSecure(SUMO_ATTR_FROMNODE, "") : begNodeID;
    endNodeID = attrs.hasAttribute(SUMO_ATTR_TONODE) ? attrs.getStringSecure(SUMO_ATTR_TONODE, "") : endNodeID;
    // or their positions
    SUMOReal begNodeXPos = tryGetPosition(attrs, SUMO_ATTR_XFROM, "XFrom");
    SUMOReal begNodeYPos = tryGetPosition(attrs, SUMO_ATTR_YFROM, "YFrom");
    SUMOReal endNodeXPos = tryGetPosition(attrs, SUMO_ATTR_XTO, "XTo");
    SUMOReal endNodeYPos = tryGetPosition(attrs, SUMO_ATTR_YTO, "YTo");
    if (begNodeXPos!=SUMOXML_INVALID_POSITION&&begNodeYPos!=SUMOXML_INVALID_POSITION) {
        Position2D pos(begNodeXPos, begNodeYPos);
        GeoConvHelper::x2cartesian(pos);
        begNodeXPos = pos.x();
        begNodeYPos = pos.y();
    }
    if (endNodeXPos!=SUMOXML_INVALID_POSITION&&endNodeYPos!=SUMOXML_INVALID_POSITION) {
        Position2D pos(endNodeXPos, endNodeYPos);
        GeoConvHelper::x2cartesian(pos);
        endNodeXPos = pos.x();
        endNodeYPos = pos.y();
    }
    // check the obtained values for nodes
    myFromNode = insertNodeChecking(Position2D(begNodeXPos, begNodeYPos), begNodeID, "from");
    myToNode = insertNodeChecking(Position2D(endNodeXPos, endNodeYPos), endNodeID, "to");
    return myFromNode!=0&&myToNode!=0;
}


SUMOReal
NIXMLEdgesHandler::tryGetPosition(const SUMOSAXAttributes &attrs, SumoXMLAttr attrID,
                                  const std::string &attrName) {
    try {
        return attrs.getFloatSecure(attrID, SUMOXML_INVALID_POSITION);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Not numeric value for " + attrName + " (at tag ID='" + myCurrentID + "').");
        return SUMOXML_INVALID_POSITION;
    }
}


NBNode *
NIXMLEdgesHandler::insertNodeChecking(const Position2D &pos,
                                      const std::string &name, const std::string &dir) {
    NBNode *ret = 0;
    if (name=="" && (pos.x()==SUMOXML_INVALID_POSITION || pos.y()==SUMOXML_INVALID_POSITION)) {
        MsgHandler::getErrorInstance()->inform("Neither the name nor the position of the " + dir + "-node is given for edge '" + myCurrentID + "'.");
        return ret;
    }
    if (name!="") {
        if (pos.x()!=SUMOXML_INVALID_POSITION && pos.y()!=SUMOXML_INVALID_POSITION) {
            // the node is named and it has a position given
            if (!myNodeCont.insert(name, pos)) {
                MsgHandler::getErrorInstance()->inform("Position of " + dir + "-node '" + name + "' mismatches previous positions.");
                return 0;
            }
        }
        // the node is given by its name
        ret = myNodeCont.retrieve(name);
        if (ret==0) {
            MsgHandler::getErrorInstance()->inform("Edge's '" + myCurrentID + "' " + dir + "-node '" + name + "' is not known.");
        }
    } else {
        ret = myNodeCont.retrieve(pos);
        if (ret==0) {
            ret = new NBNode(myNodeCont.getFreeID(), pos);
            if (!myNodeCont.insert(ret)) {
                MsgHandler::getErrorInstance()->inform("Could not insert " + dir + "-node at position " + toString(pos) + ".");
                delete ret;
                return 0;
            }
        }
    }
    return ret;
}


Position2DVector
NIXMLEdgesHandler::tryGetShape(const SUMOSAXAttributes &attrs) throw() {
    if (!attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        return Position2DVector();
    }
    // try to build shape
    try {
        string shpdef = attrs.getStringSecure(SUMO_ATTR_SHAPE, "");
        if(shpdef=="") {
            return Position2DVector();
        }
        Position2DVector shape1 = GeomConvHelper::parseShape(shpdef);
        Position2DVector shape;
        for (int i=0; i<(int) shape1.size(); ++i) {
            Position2D pos(shape1[i]);
            GeoConvHelper::x2cartesian(pos);
            shape.push_back(pos);
        }
        return shape;
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("At least one number is missing in shape definition for edge '" + myCurrentID + "'.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("A non-numeric value occured in shape definition for edge '" + myCurrentID + "'.");
    }
    return Position2DVector();
}


void
NIXMLEdgesHandler::myCharacters(SumoXMLTag element,
                                const std::string &chars) throw(ProcessError) {
    if (element==SUMO_TAG_EXPANSION&&chars.length()!=0) {
        if(!myHaveReportedAboutExpansionCharactersDeprecation) {
            myHaveReportedAboutExpansionCharactersDeprecation = true;
            MsgHandler::getWarningInstance()->inform("Defining edge expansion lanes in characters is deprecated; use attribute 'lanes' instead.");
        }
        parseExpansionLanes(chars);
    }
}


void
NIXMLEdgesHandler::parseExpansionLanes(const std::string &val) throw(ProcessError) {
    if (myExpansions.size()!=0) {
        Expansion &e = myExpansions.back();
        StringTokenizer st(val, ";");
        while (st.hasNext()) {
            try {
                int lane = TplConvert<char>::_2int(st.next().c_str());
                e.lanes.push_back(lane);
            } catch (NumberFormatException &) {
                MsgHandler::getErrorInstance()->inform("Error on parsing an expansion (edge '" + myCurrentID + "').");
            } catch (EmptyData &) {
                MsgHandler::getErrorInstance()->inform("Error on parsing an expansion (edge '" + myCurrentID + "').");
            }
        }
    } else {
        MsgHandler::getErrorInstance()->inform("Error on parsing an expansion (edge '" + myCurrentID + "').");
    }
}


void
NIXMLEdgesHandler::myEndElement(SumoXMLTag element) throw(ProcessError) {
    if (element==SUMO_TAG_EDGE && myCurrentEdge!=0) {
        if(!myIsUpdate) {
            try {
                if (!myEdgeCont.insert(myCurrentEdge)) {
                    MsgHandler::getErrorInstance()->inform("Duplicate edge occured. ID='" + myCurrentID + "'");
                    delete myCurrentEdge;
                }
            } catch (InvalidArgument &e) {
                MsgHandler::getErrorInstance()->inform(e.what());
                throw;
            } catch (...) {
                MsgHandler::getErrorInstance()->inform("An important information is missing in edge '" + myCurrentID + "'.");
            }
        }
        if (myExpansions.size()!=0) {
            std::vector<Expansion>::iterator i, i2;
            sort(myExpansions.begin(), myExpansions.end(), expansions_sorter());
            NBEdge *e = myCurrentEdge;
            // compute the node positions and sort the lanes
            for (i=myExpansions.begin(); i!=myExpansions.end(); ++i) {
                (*i).gpos = e->getGeometry().positionAtLengthPosition((*i).pos);
                sort((*i).lanes.begin(), (*i).lanes.end());
            }
            // patch lane information
            //  !!! hack: normally, all lanes to remove should always be supplied
            for (i=myExpansions.begin(); i!=myExpansions.end(); ++i) {
                vector<int>::iterator k;
                vector<int> lanes;
                for (unsigned int l=0; l<e->getNoLanes(); ++l) {
                    if (find((*i).lanes.begin(), (*i).lanes.end(), l)==(*i).lanes.end()) {
                        lanes.push_back((int) l);
                    }
                }
                for (i2=i+1; i2!=myExpansions.end(); ++i2) {
                    for (vector<int>::iterator k=lanes.begin(); k!=lanes.end(); ++k) {
                        if (find((*i2).lanes.begin(), (*i2).lanes.end(), *k)!=(*i2).lanes.end()) {
                            (*i2).lanes.erase(find((*i2).lanes.begin(), (*i2).lanes.end(), *k));
                        }
                    }
                }
            }
            // split the edge
            int lastRightLane = 0;
            for (i=myExpansions.begin(); i!=myExpansions.end(); ++i) {
                const Expansion &exp = *i;
                assert(exp.lanes.size()!=0);
                if (exp.pos>0 && e->getGeometry().length()-exp.pos>0) {
                    string nid = myCurrentID + "/" +  toString(exp.nameid);
                    NBNode *rn = new NBNode(nid, exp.gpos);
                    if (myNodeCont.insert(rn)) {
                        //  split the edge
                        string nid = myCurrentID + "/" +  toString(exp.nameid);
                        string pid = myCurrentID;
                        myEdgeCont.splitAt(myDistrictCont, e, exp.pos, rn,
                                           pid, nid, e->getNoLanes(), e->getNoLanes());
                        NBEdge *pe = myEdgeCont.retrieve(pid);
                        NBEdge *ne = myEdgeCont.retrieve(nid);
                        pe->decLaneNo(ne->getNoLanes()-exp.lanes.size());
                        // reconnect lanes
                        // how many lanes on the right are missing?
                        int off = exp.lanes[0] - lastRightLane;
                        lastRightLane = exp.lanes[0];
                        pe->addLane2LaneConnections(0, ne, off, pe->getNoLanes(), NBEdge::L2L_VALIDATED, true);
                        // add to the right?
                        if (off>0) {
                            pe->addLane2LaneConnection(0, ne, off-1, NBEdge::L2L_VALIDATED, false);
                        }
                        // add to the left?
                        if (off+exp.lanes.size()<ne->getNoLanes()) {
                            pe->addLane2LaneConnection(pe->getNoLanes()-1, ne, (unsigned int) (off+exp.lanes.size()), NBEdge::L2L_VALIDATED, false);
                        }
                        // move to next
                        e = pe;
                    } else {
                        MsgHandler::getWarningInstance()->inform("Error on parsing an expansion (edge '" + myCurrentID + "').");
                    }
                } else {
                    MsgHandler::getWarningInstance()->inform("Expansion at '" + toString(exp.pos) + "' lies beyond the edge's length (edge '" + myCurrentID + "').");
                }
            }
        }
    }
}



/****************************************************************************/

