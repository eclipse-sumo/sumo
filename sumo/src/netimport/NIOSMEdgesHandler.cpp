/****************************************************************************/
/// @file    NIOSMEdgesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id:NIOSMEdgesHandler.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include "NIOSMEdgesHandler.h"
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
NIOSMEdgesHandler::NIOSMEdgesHandler(const std::map<int, NIOSMNode*> &osmNodes,
                                     NBNodeCont &nc,
                                     NBEdgeCont &ec,
                                     NBTypeCont &tc,
                                     NBDistrictCont &dc,
                                     OptionsCont &options)
        : SUMOSAXHandler("osm-edges - file"),
        myOptions(options), myOSMNodes(osmNodes),
        myNodeCont(nc), myEdgeCont(ec), myTypeCont(tc), myDistrictCont(dc)
{
}


NIOSMEdgesHandler::~NIOSMEdgesHandler() throw()
{
    std::map<int, int> nodeUsage;
    // mark which nodes are used
    for (std::map<std::string, Edge*>::iterator i=myEdgeMap.begin(); i!=myEdgeMap.end(); ++i) {
        Edge *e = (*i).second;
        if (!e->myCurrentIsRoad) {
            continue;
        }
        for (std::vector<int>::iterator j=e->myCurrentNodes.begin(); j!=e->myCurrentNodes.end(); ++j) {
            if (nodeUsage.find(*j)==nodeUsage.end()) {
                nodeUsage[*j] = 0;
            }
            nodeUsage[*j] = nodeUsage[*j] + 1;
        }
    }
    // instatiate edges
    for (std::map<std::string, Edge*>::iterator i=myEdgeMap.begin(); i!=myEdgeMap.end(); ++i) {
        Edge *e = (*i).second;
        if (!e->myCurrentIsRoad) {
            continue;
        }
        // build nodes;
        //  the from- and to-nodes must be built in any case
        //  the geometry nodes are only built if more than one edge references them
        NBNode *currentFrom = insertNodeChecking(*e->myCurrentNodes.begin());
        NBNode *last = insertNodeChecking(*(e->myCurrentNodes.end()-1));
        int running = 0;
        std::vector<int> passed;
        for (std::vector<int>::iterator j=e->myCurrentNodes.begin(); j!=e->myCurrentNodes.end(); ++j) {
            passed.push_back(*j);
            if (nodeUsage[*j]>1&&j!=e->myCurrentNodes.end()-1&&j!=e->myCurrentNodes.begin()) {
                NBNode *currentTo = insertNodeChecking(*j);
                insertEdge(e, running, currentFrom, currentTo, passed);
                currentFrom = currentTo;
                running++;
                passed.clear();
            }
        }
        if (running==0) {
            running = -1;
        }
        insertEdge(e, running, currentFrom, last, passed);
    }
    for (std::map<std::string, Edge*>::iterator i=myEdgeMap.begin(); i!=myEdgeMap.end(); ++i) {
        delete(*i).second;
    }
}


NBNode *
NIOSMEdgesHandler::insertNodeChecking(int id)
{
    NBNode *from = myNodeCont.retrieve(toString(id));
    if (from==0) {
        NIOSMNode *n = myOSMNodes.find(id)->second;
        Position2D pos(n->lon, n->lat);
        GeoConvHelper::x2cartesian(pos);
        from = new NBNode(toString(id), pos);
        if (!myNodeCont.insert(from)) {
            MsgHandler::getErrorInstance()->inform("Could not insert node '" + toString(id) + "').");
            delete from;
            return 0;
        }
    }
    return from;
}


void
NIOSMEdgesHandler::insertEdge(NIOSMEdgesHandler::Edge *e, int index, NBNode *from, NBNode *to,
                              const std::vector<int> &passed)
{
    // patch the id
    string id = e->id;
    if (index>=0) {
        id = id + toString(index);
    }
    // convert the shape
    Position2DVector shape;
    for (std::vector<int>::const_iterator i=passed.begin(); i!=passed.end(); ++i) {
        NIOSMNode *n = myOSMNodes.find(*i)->second;
        Position2D pos(n->lon, n->lat);
        GeoConvHelper::x2cartesian(pos);
        shape.push_back_noDoublePos(pos);
    }
    int noLanes = 0;
    SUMOReal speed = 0;
    vector<SUMOVehicleClass> allowedClasses;
    vector<SUMOVehicleClass> disallowedClasses;
    bool defaultsToOneWay = false;
    string myHighWayType = e->myHighWayType;
    int myNoLanes = e->myNoLanes;
    double myMaxSpeed = e->myMaxSpeed;
    if (e->myCurrentIsRoad) {

        // set sensible defaults depending on highway type 
        if (myHighWayType=="motorway") { // de:Autobahn (A00)
            noLanes = 2;
            speed = (SUMOReal)(160./3.6);
            defaultsToOneWay = true;
        } else if (myHighWayType=="motorway_link") { // de:Autobahn-Zubringer, -Anschlussstelle
            noLanes = 1;
            speed = (SUMOReal)(80./3.6);
            defaultsToOneWay = true;
        } else if (myHighWayType=="trunk") { // de:Schnellstrasse (B00)
            noLanes = 2;
            speed = (SUMOReal)(130./3.6);
        } else if (myHighWayType=="trunk_link") { // de:Schnellstrassen-Anschlussstelle
            noLanes = 1;
            speed = (SUMOReal)(80./3.6);
        } else if (myHighWayType=="primary") { // de:Bundesstrasse (B00)
            noLanes = 1;
            speed = (SUMOReal)(100./3.6);
        } else if (myHighWayType=="primary_link") { // de:Bundesstrassen-Anschlussstelle
            noLanes = 1;
            speed = (SUMOReal)(80./3.6);
        } else if (myHighWayType=="secondary"||myHighWayType=="secondary_link") { // de:Landstrasse (Fahrbahnmarkierungen)
            noLanes = 1;
            speed = (SUMOReal)(100./3.6);
        } else if (myHighWayType=="tertiary") { // de:Kreisstrasse (keine Fahrbahnmarkierungen)
            noLanes = 1;
            speed = (SUMOReal)(80./3.6);
        } else if (myHighWayType=="unclassified") { //de:Nebenstrasse
            noLanes = 1;
            speed = (SUMOReal)(80./3.6);
        } else if (myHighWayType=="unsurfaced") { // de:Unbefestigte Strasse
            noLanes = 1;
            speed = (SUMOReal)(30./3.6);
        } else if (myHighWayType=="track") { // de:Feldweg
            noLanes = 1;
            speed = (SUMOReal)(20./3.6);
        } else if (myHighWayType=="residential") { //de:Wohngebiet
            noLanes = 1;
            speed = (SUMOReal)(50./3.6);
        } else if (myHighWayType=="service") { // de:Erschliessungsweg (Parkplatz etc.)
            noLanes = 1;
            speed = (SUMOReal)(20./3.6);
        } else if (myHighWayType=="bicycle") {
            noLanes = 1;
            speed = (SUMOReal)(25./3.6);
            allowedClasses.push_back(SVC_BICYCLE);
        } else if (myHighWayType=="footway"||myHighWayType=="pedestrian"||myHighWayType=="steps"||myHighWayType=="stairs") {
            noLanes = 1;
            speed = (SUMOReal)(5./3.6);
            allowedClasses.push_back(SVC_PEDESTRIAN);
        } else {
            WRITE_WARNING("New value for 'motorway' found: " + myHighWayType);
            noLanes = 1;
            speed = (SUMOReal)(50./3.6);
        }

        // if we had been able to extract the number of lanes, override the highway type default
        if (myNoLanes >= 0) noLanes = myNoLanes;
        // if we had been able to extract the maximum speed, override the highway type default
        if (myMaxSpeed >= 0) speed = myMaxSpeed / 3.6;

    }

    if (from->getPosition().almostSame(to->getPosition())) {
        cout << e->id << " " << shape.size() << " ";
        cout << from->getPosition() << " " << to->getPosition() << endl;
        WRITE_WARNING("Same position");
        return;
    }
    if (noLanes!=0&&speed!=0) {
        bool addSecond = true;
        // TODO: e->myIsOneWay=="-1" for oneway streets in opposite direction?
        if (e->myIsOneWay=="true"||e->myIsOneWay=="yes"||e->myIsOneWay=="1"||(defaultsToOneWay && e->myIsOneWay!="no" && e->myIsOneWay!="false" && e->myIsOneWay!="0")) {
            addSecond = false;
        }
        if (e->myIsOneWay!=""&&e->myIsOneWay!="false"&&e->myIsOneWay!="no"&&e->myIsOneWay!="true"&&e->myIsOneWay!="yes") {
            WRITE_WARNING("New value for oneway found: " + e->myIsOneWay);
        }
        NBEdge::LaneSpreadFunction lsf = addSecond
                                         ? NBEdge::LANESPREAD_RIGHT
                                         : NBEdge::LANESPREAD_CENTER;
        NBEdge *nbe = new NBEdge(id, e->id, from, to, myHighWayType,
                                 speed, noLanes, -1, shape, lsf);
        if (!myEdgeCont.insert(nbe)) {
            delete nbe;
            throw ProcessError("Could not add edge '" + id + "'.");
        }
        if (addSecond) {
            nbe = new NBEdge("-" + id, e->id, to, from, myHighWayType,
                             speed, noLanes, -1, shape.reverse(), lsf);
            if (!myEdgeCont.insert(nbe)) {
                delete nbe;
                throw ProcessError("Could not add edge '-" + id + "'.");
            }
        }
    }
}


void
NIOSMEdgesHandler::myStartElement(SumoXMLTag element,
                                  const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    myParentElements.push_back(element);
    if (element==SUMO_TAG_WAY) {
        myCurrentEdge = new Edge();
        myCurrentEdge->myNoLanes = -1;
        myCurrentEdge->myMaxSpeed = -1;
        try {
            // retrieve the id of the edge
            myCurrentEdge->id = attrs.getString(SUMO_ATTR_ID);
            myCurrentEdge->myCurrentIsRoad = false;
        } catch (EmptyData &) {
            WRITE_WARNING("No edge id given... Skipping.");
            return;
        }
    }
    if (element==SUMO_TAG_ND) {
        try {
            // retrieve the id of the (geometry) node
            int ref = attrs.getInt(SUMO_ATTR_REF);
            if (myOSMNodes.find(ref)==myOSMNodes.end()) {
                MsgHandler::getErrorInstance()->inform("The referenced geometry information (ref='" + toString(ref) + "') is not known");
                return;
            }
            myCurrentEdge->myCurrentNodes.push_back(ref);
        } catch (EmptyData &) {
            WRITE_WARNING("No node id given... Skipping.");
            return;
        }
    }
    if (element==SUMO_TAG_TAG&&myParentElements.size()>2&&myParentElements[myParentElements.size()-2]==SUMO_TAG_WAY) {
        string key, value;
        try {
            // retrieve the id of the (geometry) node
            key = attrs.getString(SUMO_ATTR_K);
        } catch (EmptyData &) {
            WRITE_WARNING("'tag' in edge '" + myCurrentEdge->id + "' misses a value.");
            return;
        }
        try {
            // retrieve the id of the (geometry) node
            value = attrs.getString(SUMO_ATTR_V);
        } catch (EmptyData &) {
            WRITE_WARNING("'value' in edge '" + myCurrentEdge->id + "' misses a value.");
            return;
        }
        if (key=="highway") {
            myCurrentEdge->myHighWayType = value;
            myCurrentEdge->myCurrentIsRoad = true;
        } else if (key=="lanes") {
            stringstream ss(value);
            ss >> myCurrentEdge->myNoLanes;
        } else if (key=="maxspeed") {
            stringstream ss(value);
            ss >> myCurrentEdge->myMaxSpeed;
        } else if (key=="junction") {
            if ((value == "roundabout") && (myCurrentEdge->myIsOneWay == "")) myCurrentEdge->myIsOneWay = "yes";
        } else if (key=="oneway") {
            myCurrentEdge->myIsOneWay = value;
        } else if (key=="name") {
//            myCurrentEdge->myCurrentName = value;
            /*
            } else if(key=="ele") {
            // !!! elevation; later use?
            } else if(key=="barrier") {
            // !!! barrier information; later use?
            } else if(key=="amenity") {
            // !!! some kind of a poi; later use?
            } else if(key=="tourism") {
            // !!! some kind of a poi; later use?
            } else if(key=="armitary") {
            // !!! some kind of a poi; later use?
            } else if(key=="leisure") {
            // !!! some kind of a poi; later use?
            } else if(key=="man_made") {
            // !!! some kind of a poi; later use?
            } else if(key=="historic") {
            // !!! some kind of a poi; later use?
            } else if(key=="shop") {
            // !!! some kind of a poi; later use?
            } else if(key=="sport") {
            // !!! information about halts/stations; later use?
            } else if(key!="notes"&&key!="uic_ref"&&key!="glass_bottles"&&key!="source"&&key!="denomination"&&key!="note"&&key!="created_by"&&key!="ref"&&key!="converted_by"&&key!="time"&&key!="place"&&key!="is_in"&&key!="layer"&&key!="religion") {
            WRITE_WARNING("New key found: " + key);
            if(key=="junction") {
                int bla = 0;
            }
            */
        }
    }
}


void
NIOSMEdgesHandler::myCharacters(SumoXMLTag element,
                                const std::string &chars) throw(ProcessError)
{
}


void
NIOSMEdgesHandler::myEndElement(SumoXMLTag element) throw(ProcessError)
{
    myParentElements.pop_back();
    if (element==SUMO_TAG_WAY) {
        if (myCurrentEdge->myCurrentIsRoad) {
            myEdgeMap[myCurrentEdge->id] = myCurrentEdge;
        } else {
            delete myCurrentEdge;
        }
        myCurrentEdge = 0;
    }
}
/*
        // convert the shape, first
        Position2DVector shape;
        for(std::vector<NIOSMNode*>::iterator i=myCurrentNodes.begin(); i!=myCurrentNodes.end(); ++i) {
            Position2D p((*i)->lon*100000.0, (*i)->lat*100000.0);
            GeoConvHelper::x2cartesian(p);
            shape.push_back_noDoublePos(p);
        }
        //
        int noLanes = 0;
        SUMOReal speed = 0;
        vector<SUMOVehicleClass> allowedClasses;
        vector<SUMOVehicleClass> disallowedClasses;
        bool allowBothDirs = false;
        if(myCurrentIsRoad) {
            if(myHighWayType=="motorway"||myHighWayType=="motorway_link") {
                noLanes = 2;
                speed = (SUMOReal) (80/3.6);
            } else if(myHighWayType=="residential") {
                noLanes = 1;
                speed = (SUMOReal) (30./3.6);
                allowBothDirs = true;
            } else if(myHighWayType=="primary"||myHighWayType=="primary_link"||myHighWayType=="cycleway") {
                // !!! not sure about "cycleway"!
                noLanes = 2;
                speed = (SUMOReal) (50./3.6);
            } else if(myHighWayType=="secondary"||myHighWayType=="secondary_link"||myHighWayType=="unclassified") {
                noLanes = 1;
                speed = (SUMOReal) (50./3.6);
            } else if(myHighWayType=="tertiary"||myHighWayType=="trunk"||myHighWayType=="trunk_link") {
                noLanes = 1;
                speed = (SUMOReal) (30./3.6);
                allowBothDirs = true;
            } else if(myHighWayType=="service"||myHighWayType=="servcie"||myHighWayType=="services") { // !!!
                noLanes = 1;
                speed = (SUMOReal) (30./3.6);
                allowedClasses.push_back(SVC_PEDESTRIAN);
                allowedClasses.push_back(SVC_BICYCLE);
                allowedClasses.push_back(SVC_DELIVERY);
                allowBothDirs = true;
            } else if(myHighWayType=="bicycle") {
                noLanes = 1;
                speed = (SUMOReal) (25./3.6);
                allowedClasses.push_back(SVC_BICYCLE);
                allowBothDirs = true;
            } else if(myHighWayType=="footway"||myHighWayType=="pedestrian"||myHighWayType=="steps"||myHighWayType=="stairs") {
                noLanes = 1;
                speed = (SUMOReal) (5./3.6);
                allowedClasses.push_back(SVC_PEDESTRIAN);
                allowBothDirs = true;
            } else {
                WRITE_WARNING("New value for 'motorway' found: " + myHighWayType);
                noLanes = 1;
                speed = (SUMOReal) (50./3.6);
            }
        }
        NBNode *from, *to;
        NBEdge *e = 0;
        {
        }
        {
            int endID = (*(myCurrentNodes.end()-1))->id;
            to = myNodeCont.retrieve(toString(endID));
            if(to==0) {
                to = new NBNode(toString(endID), shape[-1]);
                if(!myNodeCont.insert(to)) {
                    MsgHandler::getErrorInstance()->inform("Could not insert node '" + toString(endID) + "').");
                    delete from;
                    return;
                }
            }
        }
        if(from->getPosition().almostSame(to->getPosition())) {
            WRITE_WARNING("Same position");
            return;
        }
        if(noLanes!=0&&speed!=0) {
            bool addSecond = true;
            if(myIsOneWay=="true"||myIsOneWay=="yes"||!allowBothDirs) {
                addSecond = false;
            }
            if(myIsOneWay!=""&&myIsOneWay!="false"&&myIsOneWay!="no"&&myIsOneWay!="true"&&myIsOneWay!="yes") {
                WRITE_WARNING("New value for oneway found: " + myIsOneWay);
            }
            NBEdge::LaneSpreadFunction lsf = addSecond
                ? NBEdge::LANESPREAD_RIGHT
                : NBEdge::LANESPREAD_CENTER;
            e = new NBEdge(myCurrentID, myCurrentID, from, to, myHighWayType,
                speed, noLanes, -1, shape, lsf);
            if (!myEdgeCont.insert(e)) {
                delete e;
                throw ProcessError("Could not add edge '" + myCurrentID + "'.");
            }
            if(addSecond) {
                e = new NBEdge("-" + myCurrentID, "-" + myCurrentID, to, from, myHighWayType,
                    speed, noLanes, -1, shape.reverse(), lsf);
                if (!myEdgeCont.insert(e)) {
                    delete e;
                    throw ProcessError("Could not add edge '-" + myCurrentID + "'.");
                }
            }
        }
    }
}
*/


/****************************************************************************/

