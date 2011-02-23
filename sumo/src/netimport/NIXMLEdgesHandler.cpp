/****************************************************************************/
/// @file    NIXMLEdgesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Importer for network edges stored in XML
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
        myHaveReportedAboutFunctionDeprecation(false),
        myHaveWarnedAboutDeprecatedVClass(false) {}


NIXMLEdgesHandler::~NIXMLEdgesHandler() throw() {}


void
NIXMLEdgesHandler::myStartElement(SumoXMLTag element,
                                  const SUMOSAXAttributes &attrs) throw(ProcessError) {
    if (element==SUMO_TAG_EDGE) {
        myIsUpdate = false;
        bool ok = true;
        // initialise the edge
        myCurrentEdge = 0;
        mySplits.clear();
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
        myCurrentSpeed = myTypeCont.getSpeed("");
        myCurrentPriority = myTypeCont.getPriority("");
        myCurrentLaneNo = myTypeCont.getNoLanes("");
        myAllowed = myTypeCont.getAllowedClasses("");
        myNotAllowed = myTypeCont.getDisallowedClasses("");
        // check whether a type's values shall be used
        myCurrentType = "";
        if (attrs.hasAttribute(SUMO_ATTR_TYPE)) {
            myCurrentType = attrs.getStringReporting(SUMO_ATTR_TYPE, "edge", myCurrentID.c_str(), ok);
            if (!ok) {
                return;
            }
            if (!myTypeCont.knows(myCurrentType)) {
                MsgHandler::getErrorInstance()->inform("Type '" + myCurrentType + "' used by edge '" + myCurrentID + "' was not defined.");
                return;
            }
            myCurrentSpeed = myTypeCont.getSpeed(myCurrentType);
            myCurrentPriority = myTypeCont.getPriority(myCurrentType);
            myCurrentLaneNo = myTypeCont.getNoLanes(myCurrentType);
            myAllowed = myTypeCont.getAllowedClasses(myCurrentType);
            myNotAllowed = myTypeCont.getDisallowedClasses(myCurrentType);
        }
        // use values from the edge to overwrite if existing, then
        if (myCurrentEdge!=0) {
            myIsUpdate = true;
            if (!myHaveReportedAboutOverwriting) {
                MsgHandler::getMessageInstance()->inform("Duplicate edge id occured ('" + myCurrentID + "'); assuming overwriting is wished.");
                myHaveReportedAboutOverwriting = true;
            }
            myCurrentSpeed = myCurrentEdge->getSpeed();
            myCurrentPriority = myCurrentEdge->getPriority();
            myCurrentLaneNo = myCurrentEdge->getNoLanes();
            myCurrentType = myCurrentEdge->getTypeID();
            myAllowed = myCurrentEdge->getAllowedVehicleClasses();
            myNotAllowed = myCurrentEdge->getDisallowedVehicleClasses();
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
        // try to get the allowed/disallowed classes
        if (attrs.hasAttribute(SUMO_ATTR_ALLOW) || attrs.hasAttribute(SUMO_ATTR_DISALLOW)) {
            std::string allowS = attrs.hasAttribute(SUMO_ATTR_ALLOW) ? attrs.getStringSecure(SUMO_ATTR_ALLOW, "") : getVehicleClassNames(myAllowed);
            std::string disallowS = attrs.hasAttribute(SUMO_ATTR_DISALLOW) ? attrs.getStringSecure(SUMO_ATTR_DISALLOW, "") : getVehicleClassNames(myNotAllowed);
            myAllowed.clear();
            myNotAllowed.clear();
            parseVehicleClasses("", allowS, disallowS, myAllowed, myNotAllowed, myHaveWarnedAboutDeprecatedVClass);
        }

        // try to get the shape
        myShape = tryGetShape(attrs);
        // and how to spread the lanes
        if (attrs.getOptStringReporting(SUMO_ATTR_SPREADFUNC, "edge", myCurrentID.c_str(), ok, "")=="center") {
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
        if (myCurrentEdge!=0) {
            myCurrentEdge->reinit(myFromNode, myToNode, myCurrentType, myCurrentSpeed,
                                  myCurrentLaneNo, myCurrentPriority, myShape,
                                  myLanesSpread);
        } else {
            // the edge must be allocated in dependence to whether a shape is given
            if (myShape.size()==0) {
                myCurrentEdge = new NBEdge(myCurrentID, myFromNode, myToNode, myCurrentType, myCurrentSpeed,
                                           myCurrentLaneNo, myCurrentPriority, myLanesSpread);
            } else {
                myCurrentEdge = new NBEdge(myCurrentID, myFromNode, myToNode, myCurrentType, myCurrentSpeed,
                                           myCurrentLaneNo, myCurrentPriority, myShape,
                                           myLanesSpread, OptionsCont::getOptions().getBool("xml.keep-shape"));
            }
            myCurrentEdge->setLoadedLength(myLength);
        }
        myCurrentEdge->setVehicleClasses(myAllowed, myNotAllowed);
    }
    if (element==SUMO_TAG_LANE) {
        if (myCurrentEdge==0) {
            if (!OptionsCont::getOptions().isInStringVector("remove-edges", myCurrentID)) {
                MsgHandler::getErrorInstance()->inform("Additional lane information could not been set - the edge with id '" + myCurrentID + "' is not known.");
            }
            return;
        }
        bool ok = true;
        int lane = attrs.getIntReporting(SUMO_ATTR_ID, "lane", 0, ok);
        std::vector<std::string> disallowed, allowed, preferred;
        SUMOSAXAttributes::parseStringVector(attrs.getOptStringReporting(SUMO_ATTR_DISALLOW, "lane", 0, ok, ""), disallowed);
        SUMOSAXAttributes::parseStringVector(attrs.getOptStringReporting(SUMO_ATTR_ALLOW, "lane", 0, ok, ""), allowed);
        SUMOSAXAttributes::parseStringVector(attrs.getOptStringReporting(SUMO_ATTR_PREFER, "lane", 0, ok, ""), preferred);
        if (!ok) {
            return;
        }
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
        for (std::vector<std::string>::iterator i=disallowed.begin(); i!=disallowed.end(); ++i) {
            myCurrentEdge->disallowVehicleClass(lane, getVehicleClassID(*i));
        }
        for (std::vector<std::string>::iterator i=allowed.begin(); i!=allowed.end(); ++i) {
            myCurrentEdge->allowVehicleClass(lane, getVehicleClassID(*i));
        }
        for (std::vector<std::string>::iterator i=preferred.begin(); i!=preferred.end(); ++i) {
            myCurrentEdge->preferVehicleClass(lane, getVehicleClassID(*i));
        }

        // set information about later beginning lanes
        if (attrs.hasAttribute(SUMO_ATTR_FORCE_LENGTH)) {
            bool ok = true;
            int forcedLength = attrs.getIntReporting(SUMO_ATTR_FORCE_LENGTH, "lane", myCurrentID.c_str(), ok); // !!! edge id
            if (ok) {
                int nameid = forcedLength;
                forcedLength = (int)(myCurrentEdge->getGeometry().length() - forcedLength);
                std::vector<Split>::iterator i;
                i = find_if(mySplits.begin(), mySplits.end(), split_by_pos_finder((SUMOReal) forcedLength));
                if (i==mySplits.end()) {
                    Split e;
                    e.pos = (SUMOReal) forcedLength;
                    e.nameid = nameid;
                    for (unsigned int j=0; j<myCurrentEdge->getNoLanes(); j++) {
                        e.lanes.push_back(j);
                    }
                    mySplits.push_back(e);
                }
                i = find_if(mySplits.begin(), mySplits.end(), split_by_pos_finder((SUMOReal) forcedLength));
                std::vector<int>::iterator k = find((*i).lanes.begin(), (*i).lanes.end(), lane);
                if (k!=(*i).lanes.end()) {
                    (*i).lanes.erase(k);
                }
            }
        }
    }
    if (element==SUMO_TAG_SPLIT) {
        bool ok = true;
        Split e;
        e.pos = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, "split", 0, ok);
        if (ok) {
            if (e.pos<0) {
                e.pos += myCurrentEdge->getGeometry().length();
            }
            std::vector<Split>::iterator i = find_if(mySplits.begin(), mySplits.end(), split_by_pos_finder(e.pos));
            if (i!=mySplits.end()) {
                MsgHandler::getErrorInstance()->inform("Edge '" + myCurrentID + "' has already a split at position " + toString(e.pos) + ".");
                return;
            }
            e.nameid = (int)e.pos;
            if (myCurrentEdge==0) {
                if (!OptionsCont::getOptions().isInStringVector("remove-edges", myCurrentID)) {
                    MsgHandler::getErrorInstance()->inform("Additional lane information could not been set - the edge with id '" + myCurrentID + "' is not known.");
                }
                return;
            }
            std::vector<std::string> lanes;
            SUMOSAXAttributes::parseStringVector(attrs.getOptStringReporting(SUMO_ATTR_LANES, "split", 0, ok, ""), lanes);
            for (std::vector<std::string>::iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                try {
                    int lane = TplConvert<char>::_2int((*i).c_str());
                    e.lanes.push_back(lane);
                } catch (NumberFormatException &) {
                    MsgHandler::getErrorInstance()->inform("Error on parsing a split (edge '" + myCurrentID + "').");
                } catch (EmptyData &) {
                    MsgHandler::getErrorInstance()->inform("Error on parsing a split (edge '" + myCurrentID + "').");
                }
            }
            if (e.lanes.size()==0) {
                MsgHandler::getErrorInstance()->inform("Missing lane information in split of edge '" + myCurrentID + "'.");
            } else {
                mySplits.push_back(e);
            }
        }
    }
}


bool
NIXMLEdgesHandler::setNodes(const SUMOSAXAttributes &attrs) throw() {
    // the names and the coordinates of the beginning and the end node
    // may be found, try
    bool ok = true;
    std::string begNodeID = myIsUpdate ? myCurrentEdge->getFromNode()->getID() : "";
    std::string endNodeID = myIsUpdate ? myCurrentEdge->getToNode()->getID() : "";
    begNodeID = attrs.hasAttribute(SUMO_ATTR_FROMNODE) ? attrs.getStringReporting(SUMO_ATTR_FROMNODE, "edge", 0, ok) : begNodeID;
    endNodeID = attrs.hasAttribute(SUMO_ATTR_TONODE) ? attrs.getStringReporting(SUMO_ATTR_TONODE, "edge", 0, ok) : endNodeID;
    if (!ok) {
        return false;
    }
    // or their positions !!! deprecated
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
    UNUSED_PARAMETER(attrName);
    bool ok = true;
    return attrs.getOptSUMORealReporting(attrID, "edge", myCurrentID.c_str(), ok, SUMOXML_INVALID_POSITION);
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
    bool ok = true;
    std::string shpdef = attrs.getOptStringReporting(SUMO_ATTR_SHAPE, "edge", 0, ok, "");
    if (shpdef=="") {
        return Position2DVector();
    }
    Position2DVector shape1 = GeomConvHelper::parseShapeReporting(shpdef, "edge", 0, ok, true);
    Position2DVector shape;
    for (int i=0; i<(int) shape1.size(); ++i) {
        Position2D pos(shape1[i]);
        if (!GeoConvHelper::x2cartesian(pos)) {
            MsgHandler::getErrorInstance()->inform("Unable to project coordinates for edge '" + myCurrentID + "'.");
        }
        shape.push_back(pos);
    }
    return shape;
}


void
NIXMLEdgesHandler::myEndElement(SumoXMLTag element) throw(ProcessError) {
    if (element==SUMO_TAG_EDGE && myCurrentEdge!=0) {
        if (!myIsUpdate) {
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
        if (mySplits.size()!=0) {
            std::vector<Split>::iterator i, i2;
            sort(mySplits.begin(), mySplits.end(), split_sorter());
            NBEdge *e = myCurrentEdge;
            unsigned int noLanesMax = e->getNoLanes();
            // compute the node positions and sort the lanes
            for (i=mySplits.begin(); i!=mySplits.end(); ++i) {
                (*i).gpos = e->getGeometry().positionAtLengthPosition((*i).pos);
                sort((*i).lanes.begin(), (*i).lanes.end());
                noLanesMax = MAX2(noLanesMax, (unsigned int)(*i).lanes.size());
            }
            // split the edge
            std::vector<int> currLanes;
            for (unsigned int l=0; l<e->getNoLanes(); ++l) {
                currLanes.push_back(l);
            }
            std::string edgeid = e->getID();
            SUMOReal seen = 0;
            for (i=mySplits.begin(); i!=mySplits.end(); ++i) {
                const Split &exp = *i;
                assert(exp.lanes.size()!=0);
                if (exp.pos>0 && e->getGeometry().length()+seen>exp.pos) {
                    std::string nid = edgeid + "." +  toString(exp.nameid);
                    NBNode *rn = new NBNode(nid, exp.gpos);
                    if (myNodeCont.insert(rn)) {
                        //  split the edge
                        std::string nid = myCurrentID + "." +  toString(exp.nameid);
                        std::string pid = e->getID();
                        myEdgeCont.splitAt(myDistrictCont, e, exp.pos-seen, rn,
                                           pid, nid, e->getNoLanes(), (unsigned int) exp.lanes.size());
                        seen = exp.pos;
                        std::vector<int> newLanes = exp.lanes;
                        NBEdge *pe = myEdgeCont.retrieve(pid);
                        NBEdge *ne = myEdgeCont.retrieve(nid);
                        // reconnect lanes
                        pe->invalidateConnections(true);
                        //  new on right
                        unsigned int rightMostP = currLanes[0];
                        unsigned int rightMostN = newLanes[0];
                        for (int l=0; l<(int) rightMostP-(int) rightMostN; ++l) {
                            pe->addLane2LaneConnection(0, ne, l, NBEdge::L2L_VALIDATED, true);
                        }
                        //  new on left
                        unsigned int leftMostP = currLanes.back();
                        unsigned int leftMostN = newLanes.back();
                        for (int l=0; l<(int) leftMostN-(int) leftMostP; ++l) {
                            pe->addLane2LaneConnection(pe->getNoLanes()-1, ne, leftMostN-l, NBEdge::L2L_VALIDATED, true);
                        }
                        //  all other connected
                        for (unsigned int l=0; l<noLanesMax; ++l) {
                            if (find(currLanes.begin(), currLanes.end(), l)==currLanes.end()) {
                                continue;
                            }
                            if (find(newLanes.begin(), newLanes.end(), l)==newLanes.end()) {
                                continue;
                            }
                            pe->addLane2LaneConnection(l-rightMostP, ne, l-rightMostN, NBEdge::L2L_VALIDATED, true);
                        }
                        // move to next
                        e = ne;
                        currLanes = newLanes;
                    } else {
                        MsgHandler::getWarningInstance()->inform("Error on parsing a split (edge '" + myCurrentID + "').");
                    }
                }  else if (exp.pos==0) {
                    e->decLaneNo(e->getNoLanes()-(int)exp.lanes.size());
                    currLanes = exp.lanes;
                } else {
                    MsgHandler::getWarningInstance()->inform("Split at '" + toString(exp.pos) + "' lies beyond the edge's length (edge '" + myCurrentID + "').");
                }
            }
            // patch lane offsets
            e = myEdgeCont.retrieve(edgeid);
            i = mySplits.begin();
            if ((*i).pos!=0) {
                e = e->getToNode()->getOutgoingEdges()[0];
            }
            for (; i!=mySplits.end(); ++i) {
                unsigned int maxLeft = (*i).lanes.back();
                if (maxLeft<noLanesMax) {
                    Position2DVector g = e->getGeometry();
                    g.move2side(SUMO_const_laneWidthAndOffset*(noLanesMax-1-maxLeft));
                    e->setGeometry(g);
                }
                if (e->getToNode()->getOutgoingEdges().size()!=0) {
                    e = e->getToNode()->getOutgoingEdges()[0];
                }
            }
        }
    }
}



/****************************************************************************/

