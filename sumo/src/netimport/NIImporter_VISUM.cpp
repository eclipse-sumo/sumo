/****************************************************************************/
/// @file    NIImporter_VISUM.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// A VISUM network importer
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include <netbuild/NBDistrict.h>
#include <utils/common/TplConvertSec.h>

#include <netbuild/NBNetBuilder.h>
#include "NIImporter_VISUM.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_VISUM::loadNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether the option is set (properly)
    if (!oc.isSet("visum")) {
        return;
    }
    // build the handler
    NIImporter_VISUM loader(nb, oc.getString("visum"),
                            NBCapacity2Lanes(oc.getFloat("capacity-norm")),
                            oc.getBool("visum.use-type-priority"));
    loader.load();
}



// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_VISUM::NIImporter_VISUM(NBNetBuilder &nb,
                                   const std::string &file,
                                   NBCapacity2Lanes capacity2Lanes,
                                   bool useVisumPrio) throw()
        : myNetBuilder(nb), myFileName(file),
        myCapacity2Lanes(capacity2Lanes), myUseVisumPrio(useVisumPrio) {
    // the order of process is important!
    // set1
    addParser("VSYS", &NIImporter_VISUM::parse_VSysTypes);
    addParser("STRECKENTYP", &NIImporter_VISUM::parse_Types);
    addParser("KNOTEN", &NIImporter_VISUM::parse_Nodes);
    addParser("BEZIRK", &NIImporter_VISUM::parse_Districts);
    addParser("PUNKT", &NIImporter_VISUM::parse_Point);


    // set2
    // two types of "strecke"
    addParser("STRECKE", &NIImporter_VISUM::parse_Edges);
    addParser("STRECKEN", &NIImporter_VISUM::parse_Edges);
    addParser("KANTE", &NIImporter_VISUM::parse_Kante);


    // set3
    addParser("ANBINDUNG", &NIImporter_VISUM::parse_Connectors);
    // two types of "abbieger"
    addParser("ABBIEGEBEZIEHUNG", &NIImporter_VISUM::parse_Turns);
    addParser("ABBIEGER", &NIImporter_VISUM::parse_Turns);

    addParser("STRECKENPOLY", &NIImporter_VISUM::parse_EdgePolys);
    addParser("FAHRSTREIFEN", &NIImporter_VISUM::parse_Lanes);
    addParser("FLAECHENELEMENT", &NIImporter_VISUM::parse_PartOfArea);


    // set4
    // two types of lsa
    addParser("LSA", &NIImporter_VISUM::parse_TrafficLights);
    addParser("SIGNALANLAGE", &NIImporter_VISUM::parse_TrafficLights);
    // two types of knotenzulsa
    addParser("KNOTENZULSA", &NIImporter_VISUM::parse_NodesToTrafficLights);
    addParser("LSAZUKNOTEN", &NIImporter_VISUM::parse_NodesToTrafficLights);
    addParser("SIGNALANLAGEZUKNOTEN", &NIImporter_VISUM::parse_NodesToTrafficLights);
    // two types of signalgruppe
    addParser("LSASIGNALGRUPPE", &NIImporter_VISUM::parse_SignalGroups);
    addParser("SIGNALGRUPPE", &NIImporter_VISUM::parse_SignalGroups);
    // two types of ABBZULSASIGNALGRUPPE
    addParser("ABBZULSASIGNALGRUPPE", &NIImporter_VISUM::parse_TurnsToSignalGroups);
    addParser("SIGNALGRUPPEZUABBIEGER", &NIImporter_VISUM::parse_TurnsToSignalGroups);
    addParser("SIGNALGRUPPEZUFSABBIEGER", &NIImporter_VISUM::parse_TurnsToSignalGroups);

    addParser("TEILFLAECHENELEMENT", &NIImporter_VISUM::parse_AreaSubPartElement);

    // two types of LSAPHASE
    addParser("LSAPHASE", &NIImporter_VISUM::parse_Phases);
    addParser("PHASE", &NIImporter_VISUM::parse_Phases);

    addParser("LSASIGNALGRUPPEZULSAPHASE", &NIImporter_VISUM::parse_SignalGroupsToPhases);
    addParser("FAHRSTREIFENABBIEGER", &NIImporter_VISUM::parse_LanesConnections);
}


NIImporter_VISUM::~NIImporter_VISUM() throw() {
    for (NIVisumTL_Map::iterator j=myNIVisumTLs.begin(); j!=myNIVisumTLs.end(); j++) {
        delete j->second;
    }
}


void
NIImporter_VISUM::addParser(const std::string &name, ParsingFunction function) throw() {
    TypeParser p;
    p.name = name;
    p.function = function;
    p.position = -1;
    mySingleDataParsers.push_back(p);
}


void
NIImporter_VISUM::load() throw(ProcessError) {
    // open the file
    if (!myLineReader.setFile(myFileName)) {
        throw ProcessError("Can not open visum-file '" + myFileName + "'.");
    }
    // scan the file for data positions
    while (myLineReader.hasMore()) {
        std::string line = myLineReader.readLine();
        if (line.length()>0 && line[0]=='$') {
            ParserVector::iterator i;
            for (i=mySingleDataParsers.begin(); i!=mySingleDataParsers.end(); i++) {
                std::string dataName = "$" + (*i).name + ":";
                if (line.substr(0, dataName.length())==dataName) {
                    (*i).position = myLineReader.getPosition();
                    (*i).pattern = line.substr(dataName.length());
                    WRITE_MESSAGE("Found: " + dataName + " at " + toString<int>(myLineReader.getPosition()));
                }
            }
        }
    }
    // go through the parsers and process all entries
    for (ParserVector::iterator i=mySingleDataParsers.begin(); i!=mySingleDataParsers.end(); i++) {
        if ((*i).position<0) {
            // do not process using parsers for which no information was found
            continue;
        }
        // ok, the according information is stored in the file
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing " + (*i).name + "...");
        // reset the line reader and let it point to the begin of the according data field
        myLineReader.reinit();
        myLineReader.setPos((*i).position);
        // prepare the line parser
        myLineParser.reinit((*i).pattern);
        // read
        bool singleDataEndFound = false;
        while (myLineReader.hasMore() && !singleDataEndFound) {
            std::string line = myLineReader.readLine();
            if (line.length()==0||line[0]=='*'||line[0]=='$') {
                singleDataEndFound = true;
            } else {
                myLineParser.parseLine(line);
                try {
                    myCurrentID = "<unknown>";
                    (this->*(*i).function)();
                } catch (OutOfBoundsException &) {
                    MsgHandler::getErrorInstance()->inform("Too short value line in " + (*i).name + " occured.");
                } catch (NumberFormatException &) {
                    MsgHandler::getErrorInstance()->inform("A value in " + (*i).name + " should be numeric but is not (id='" + myCurrentID + "').");
                } catch (UnknownElement &e) {
                    MsgHandler::getErrorInstance()->inform("One of the needed values ('" + std::string(e.what()) + "') is missing in " + (*i).name + ".");
                }
            }
        }
        // close single reader processing
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // build traffic lights
    for (NIVisumTL_Map::iterator j=myNIVisumTLs.begin(); j!=myNIVisumTLs.end(); j++) {
        j->second->build(myNetBuilder.getTLLogicCont());
    }
    // build district shapes
    for (std::map<NBDistrict*, Position2DVector>::const_iterator k=myDistrictShapes.begin(); k!=myDistrictShapes.end(); ++k) {
        (*k).first->addShape((*k).second);
    }
}





void
NIImporter_VISUM::parse_VSysTypes() {
    std::string name = myLineParser.know("VSysCode") ? myLineParser.get("VSysCode").c_str() : myLineParser.get("CODE").c_str();
    std::string type = myLineParser.know("VSysMode") ? myLineParser.get("VSysMode").c_str() : myLineParser.get("Typ").c_str();
    myVSysTypes[name] = type;
}


void
NIImporter_VISUM::parse_Types() {
    // get the id
    myCurrentID = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
    // get the maximum speed
    SUMOReal speed = getNamedFloat("v0-IV", "V0IV");
    // get the priority
    int priority = TplConvert<char>::_2int(myLineParser.get("Rang").c_str());
    // try to retrieve the number of lanes
    SUMOReal cap = getNamedFloat("Kap-IV", "KAPIV");
    int nolanes = myCapacity2Lanes.get(cap);
    // insert the type
    myNetBuilder.getTypeCont().insert(myCurrentID, nolanes, speed/(SUMOReal) 3.6, priority);
}


void
NIImporter_VISUM::parse_Nodes() {
    // get the id
    myCurrentID = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
    // get the position
    SUMOReal x = getNamedFloat("XKoord");
    SUMOReal y = getNamedFloat("YKoord");
    Position2D pos(x, y);
    if (!GeoConvHelper::x2cartesian(pos)) {
        MsgHandler::getErrorInstance()->inform("Unable to project coordinates for node " + myCurrentID + ".");
        return;
    }
    // add to the list
    if (!myNetBuilder.getNodeCont().insert(myCurrentID, pos)) {
        MsgHandler::getErrorInstance()->inform("Duplicate node occured ('" + myCurrentID + "').");
    }
}


void
NIImporter_VISUM::parse_Districts() {
    // get the id
    myCurrentID = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
    // get the information whether the source and the destination
    //  connections are weighted
    //bool sourcesWeighted = getWeightedBool("Proz_Q");
    //bool destWeighted = getWeightedBool("Proz_Z");
    // get the node information
    SUMOReal x = getNamedFloat("XKoord");
    SUMOReal y = getNamedFloat("YKoord");
    Position2D pos(x, y);
    if (!GeoConvHelper::x2cartesian(pos, false)) {
        MsgHandler::getErrorInstance()->inform("Unable to project coordinates for district " + myCurrentID + ".");
        return;
    }
    // build the district
    NBDistrict *district = new NBDistrict(myCurrentID, pos);
    if (!myNetBuilder.getDistrictCont().insert(district)) {
        MsgHandler::getErrorInstance()->inform("Duplicate district occured ('" + myCurrentID + "').");
        delete district;
        return;
    }
    if (myLineParser.know("FLAECHEID")) {
        long flaecheID = TplConvert<char>::_2long(myLineParser.get("FLAECHEID").c_str());
        myShapeDistrictMap[flaecheID] = district;
    }
}


void
NIImporter_VISUM::parse_Point() {
    long id = TplConvert<char>::_2long(myLineParser.get("ID").c_str());
    SUMOReal x = TplConvert<char>::_2SUMOReal(myLineParser.get("XKOORD").c_str());
    SUMOReal y = TplConvert<char>::_2SUMOReal(myLineParser.get("YKOORD").c_str());
    Position2D pos(x, y);
    if (!GeoConvHelper::x2cartesian(pos, false)) {
        MsgHandler::getErrorInstance()->inform("Unable to project coordinates for point " + toString(id) + ".");
        return;
    }
    myPoints[id] = pos;
}


void
NIImporter_VISUM::parse_Edges() {
    if (myLineParser.know("VSYSSET") && myLineParser.get("VSYSSET")=="") {
        // no vehicle allowed; don't add
        return;
    }
    // get the id
    myCurrentID = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
    // get the from- & to-node and validate them
    NBNode *from = getNamedNode("VonKnot", "VonKnotNr");
    NBNode *to = getNamedNode("NachKnot", "NachKnotNr");
    if (!checkNodes(from, to)) {
        return;
    }
    // get the type
    std::string type = myLineParser.know("Typ") ? myLineParser.get("Typ") : myLineParser.get("TypNr");
    // get the speed
    SUMOReal speed = myNetBuilder.getTypeCont().getSpeed(type);;
    if (!OptionsCont::getOptions().getBool("visum.use-type-speed")) {
        try {
            speed = myLineParser.know("v0-IV")
                    ? TplConvertSec<char>::_2SUMORealSec(myLineParser.get("v0-IV").c_str(), -1)
                    : TplConvertSec<char>::_2SUMORealSec(myLineParser.get("V0IV").c_str(), -1);
            speed = speed / (SUMOReal) 3.6;
        } catch (OutOfBoundsException) {}
    }
    if (speed<=0) {
        speed = myNetBuilder.getTypeCont().getSpeed(type);
    }

    // get the information whether the edge is a one-way
    bool oneway = myLineParser.know("Einbahn")
                  ? TplConvert<char>::_2bool(myLineParser.get("Einbahn").c_str())
                  : true;
    // get the number of lanes
    int nolanes = myNetBuilder.getTypeCont().getNoLanes(type);
    if (!OptionsCont::getOptions().getBool("visum.recompute-laneno")) {
        try {
            if (!OptionsCont::getOptions().getBool("visum.use-type-laneno")) {
                nolanes = myLineParser.know("Fahrstreifen")
                          ? TplConvertSec<char>::_2intSec(myLineParser.get("Fahrstreifen").c_str(), 0)
                          : TplConvertSec<char>::_2intSec(myLineParser.get("ANZFAHRSTREIFEN").c_str(), 0);
            }
        } catch (UnknownElement) {
        }
    } else {
        SUMOReal cap = myLineParser.know("KAPIV")
                       ? TplConvertSec<char>::_2SUMORealSec(myLineParser.get("KAPIV").c_str(), -1)
                       : TplConvertSec<char>::_2SUMORealSec(myLineParser.get("KAP-IV").c_str(), -1);
        nolanes = myCapacity2Lanes.get(cap);
    }
    // check whether the id is already used
    //  (should be the opposite direction)
    bool oneway_checked = oneway;
    NBEdge *previous = myNetBuilder.getEdgeCont().retrieve(myCurrentID);
    if (previous!=0) {
        myCurrentID = '-' + myCurrentID;
        previous->setLaneSpreadFunction(NBEdge::LANESPREAD_RIGHT);
        oneway_checked = false;
    }
    if (find(myTouchedEdges.begin(), myTouchedEdges.end(), myCurrentID)!=myTouchedEdges.end()) {
        oneway_checked = false;
    }
    std::string tmpid = '-' + myCurrentID;
    if (find(myTouchedEdges.begin(), myTouchedEdges.end(), tmpid)!=myTouchedEdges.end()) {
        previous = myNetBuilder.getEdgeCont().retrieve(tmpid);
        if (previous!=0) {
            previous->setLaneSpreadFunction(NBEdge::LANESPREAD_RIGHT);
        }
        oneway_checked = false;
    }
    // add the edge
    int prio = myUseVisumPrio ? myNetBuilder.getTypeCont().getPriority(type) : -1;
    if (nolanes!=0&&speed!=0) {
        NBEdge::LaneSpreadFunction lsf = oneway_checked
                                         ? NBEdge::LANESPREAD_CENTER
                                         : NBEdge::LANESPREAD_RIGHT;
        NBEdge *e = new NBEdge(myCurrentID, from, to, type, speed, nolanes, prio, lsf);
        if (!myNetBuilder.getEdgeCont().insert(e)) {
            delete e;
            MsgHandler::getErrorInstance()->inform("Duplicate edge occured ('" + myCurrentID + "').");
        }
    }
    myTouchedEdges.push_back(myCurrentID);
    // nothing more to do, when the edge is a one-way street
    if (oneway) {
        return;
    }
    // add the opposite edge
    myCurrentID = '-' + myCurrentID;
    if (nolanes!=0&&speed!=0) {
        NBEdge::LaneSpreadFunction lsf = oneway_checked
                                         ? NBEdge::LANESPREAD_CENTER
                                         : NBEdge::LANESPREAD_RIGHT;
        NBEdge *e = new NBEdge(myCurrentID, from, to, type, speed, nolanes, prio, lsf);
        if (!myNetBuilder.getEdgeCont().insert(e)) {
            delete e;
            MsgHandler::getErrorInstance()->inform("Duplicate edge occured ('" + myCurrentID + "').");
        }
    }
    myTouchedEdges.push_back(myCurrentID);
}


void
NIImporter_VISUM::parse_Kante() {
    long id = TplConvert<char>::_2long(myLineParser.get("ID").c_str());
    long from = TplConvert<char>::_2long(myLineParser.get("VONPUNKTID").c_str());
    long to = TplConvert<char>::_2long(myLineParser.get("NACHPUNKTID").c_str());
    myEdges[id] = std::make_pair(from, to);
}


void
NIImporter_VISUM::parse_PartOfArea() {
    long flaecheID = TplConvert<char>::_2long(myLineParser.get("FLAECHEID").c_str());
    long flaechePartID = TplConvert<char>::_2long(myLineParser.get("TFLAECHEID").c_str());
    if (mySubPartsAreas.find(flaechePartID)==mySubPartsAreas.end()) {
        mySubPartsAreas[flaechePartID] = std::vector<long>();
    }
    mySubPartsAreas[flaechePartID].push_back(flaecheID);
}


void
NIImporter_VISUM::parse_Connectors() {
    if (OptionsCont::getOptions().getBool("visum.no-connectors")) {
        // do nothing, if connectors shall not be imported
        return;
    }
    // get the source district
    std::string bez = NBHelpers::normalIDRepresentation(myLineParser.get("BezNr"));
    // get the destination node
    NBNode *dest = getNamedNode("KnotNr");
    if (dest==0) {
        return;
    }
    // get the weight of the connection
    SUMOReal proz = getWeightedFloat("Proz");
    if (proz>0) {
        proz /= 100.;
    } else {
        proz = 1;
    }
    // get the duration to wait
    SUMOReal retard = -1;
    if (myLineParser.know("t0-IV")) {
        retard = getNamedFloat("t0-IV", -1);
    }
    // get the type;
    //  use a standard type with a large speed when a type is not given
    std::string type = myLineParser.know("Typ")
                       ? NBHelpers::normalIDRepresentation(myLineParser.get("Typ"))
                       : "";
    // add the connectors as an edge
    std::string id = bez + "-" + dest->getID();
    // get the information whether this is a sink or a source
    std::string dir = myLineParser.get("Richtung");
    if (dir.length()==0) {
        dir = "QZ";
    }
    // build the source when needed
    if (dir.find('Q')!=std::string::npos) {
        const std::vector<NBEdge*> &edges = dest->getOutgoingEdges();
        bool hasContinuation = false;
        for (std::vector<NBEdge*>::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
            if (!(*i)->isMacroscopicConnector()) {
                hasContinuation = true;
            }
        }
        if (!hasContinuation) {
            // obviously, there is no continuation on the net
            MsgHandler::getWarningInstance()->inform("Incoming connector '" + id + "' will not be build - would be not connected to network.");
        } else {
            NBNode *src = buildDistrictNode(bez, dest, true);
            if (src==0) {
                MsgHandler::getErrorInstance()->inform("The district '" + bez + "' could not be built.");
                return;
            }
            NBEdge *edge = new NBEdge(id, src, dest, "VisumConnector",
                                      OptionsCont::getOptions().getFloat("visum.connector-speeds"),
                                      OptionsCont::getOptions().getInt("visum.connector-laneno"),
                                      -1, NBEdge::LANESPREAD_RIGHT);
            edge->setAsMacroscopicConnector();
            if (!myNetBuilder.getEdgeCont().insert(edge)) {
                MsgHandler::getErrorInstance()->inform("A duplicate edge id occured (ID='" + id + "').");
                return;
            }
            edge = myNetBuilder.getEdgeCont().retrieve(id);
            if (edge!=0) {
                myNetBuilder.getDistrictCont().addSource(bez, edge, proz);
            }
        }
    }
    // build the sink when needed
    if (dir.find('Z')!=std::string::npos) {
        const std::vector<NBEdge*> &edges = dest->getIncomingEdges();
        bool hasPredeccessor = false;
        for (std::vector<NBEdge*>::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
            if (!(*i)->isMacroscopicConnector()) {
                hasPredeccessor = true;
            }
        }
        if (!hasPredeccessor) {
            // obviously, the network is not connected to this node
            MsgHandler::getWarningInstance()->inform("Outgoing connector '" + id + "' will not be build - would be not connected to network.");
        } else {
            NBNode *src = buildDistrictNode(bez, dest, false);
            if (src==0) {
                MsgHandler::getErrorInstance()->inform("The district '" + bez + "' could not be built.");
                return;
            }
            id = "-" + id;
            NBEdge *edge = new NBEdge(id, dest, src, "VisumConnector",
                                      OptionsCont::getOptions().getFloat("visum.connector-speeds"),
                                      OptionsCont::getOptions().getInt("visum.connector-laneno"),
                                      -1, NBEdge::LANESPREAD_RIGHT);
            edge->setAsMacroscopicConnector();
            if (!myNetBuilder.getEdgeCont().insert(edge)) {
                MsgHandler::getErrorInstance()->inform("A duplicate edge id occured (ID='" + id + "').");
                return;
            }
            edge = myNetBuilder.getEdgeCont().retrieve(id);
            if (edge!=0) {
                myNetBuilder.getDistrictCont().addSink(bez, edge, proz);
            }
        }
    }
}


void
NIImporter_VISUM::parse_Turns() {
    if (myLineParser.know("VSYSSET") && myLineParser.get("VSYSSET")=="") {
        // no vehicle allowed; don't add
        return;
    }
    // retrieve the nodes
    NBNode *from = getNamedNode("VonKnot", "VonKnotNr");
    NBNode *via = getNamedNode("UeberKnot", "UeberKnotNr");
    NBNode *to = getNamedNode("NachKnot", "NachKnotNr");
    if (from==0||via==0||to==0) {
        return;
    }
    // all nodes are known
    std::string type = myLineParser.know("VSysCode")
                       ? myLineParser.get("VSysCode")
                       : myLineParser.get("VSYSSET");
    if (myVSysTypes.find(type)!=myVSysTypes.end() && myVSysTypes.find(type)->second=="IV") {
        // try to set the turning definition
        NBEdge *src = from->getConnectionTo(via);
        NBEdge *dest = via->getConnectionTo(to);
        // check both
        if (src==0) {
            // maybe it was removed due to something
            if (OptionsCont::getOptions().isSet("edges-min-speed")
                    ||
                    OptionsCont::getOptions().isSet("keep-edges")) {
                WRITE_WARNING("Could not set connection from node '" + from->getID() + "' to node '" + via->getID() + "'.");
            } else {
				if(OptionsCont::getOptions().getBool("visum.verbose-warnings")) {
					WRITE_WARNING("There is no edge from node '" + from->getID() + "' to node '" + via->getID() + "'.");
				}
            }
            return;
        }
        if (dest==0) {
            if (OptionsCont::getOptions().isSet("edges-min-speed")
                    ||
                    OptionsCont::getOptions().isSet("keep-edges")) {
                WRITE_WARNING("Could not set connection from node '" + via->getID() + "' to node '" + to->getID() + "'.");
            } else {
				if(OptionsCont::getOptions().getBool("visum.verbose-warnings")) {
	                WRITE_WARNING("There is no edge from node '" + via->getID() + "' to node '" + to->getID() + "'.");
				}
            }
            return;
        }
        // both edges found
        //  set them into the edge
        src->setTurningDestination(dest);
    }
}


void
NIImporter_VISUM::parse_EdgePolys() {
    // get the from- & to-node and validate them
    NBNode *from = getNamedNode("VonKnot", "VonKnotNr");
    NBNode *to = getNamedNode("NachKnot", "NachKnotNr");
    if (!checkNodes(from, to)) {
        return;
    }
    bool failed = false;
    int index;
    SUMOReal x, y;
    try {
        index = TplConvert<char>::_2int(myLineParser.get("INDEX").c_str());
        x = getNamedFloat("XKoord");
        y = getNamedFloat("YKoord");
    } catch (NumberFormatException&) {
        MsgHandler::getErrorInstance()->inform("Error in geometry description from node '" + from->getID() + "' to node '" + to->getID() + "'.");
        return;
    }
    Position2D pos(x, y);
    if (!GeoConvHelper::x2cartesian(pos)) {
        MsgHandler::getErrorInstance()->inform("Unable to project coordinates for node '" + from->getID() + "'.");
        return;
    }
    NBEdge *e = from->getConnectionTo(to);
    if (e!=0) {
        e->addGeometryPoint(index, pos);
    } else {
        failed = true;
    }
    e = to->getConnectionTo(from);
    if (e!=0) {
        e->addGeometryPoint(-index, pos);
        failed = false;
    }
    // check whether the operation has failed
    if (failed) {
        // we should report this to the warning instance only if we have removed
        //  some nodes or edges...
        if (OptionsCont::getOptions().isSet("edges-min-speed") || OptionsCont::getOptions().isSet("keep-edges")) {
            WRITE_WARNING("Could not set geometry between node '" + from->getID() + "' and node '" + to->getID() + "'.");
        } else {
            // ... in the other case we report this to the error instance
			if(OptionsCont::getOptions().getBool("visum.verbose-warnings")) {
				WRITE_WARNING("There is no edge from node '" + from->getID() + "' to node '" + to->getID() + "'.");
			}
        }
    }
}


void
NIImporter_VISUM::parse_Lanes() {
    // get the node
    NBNode *node = getNamedNode("KNOTNR");
    // get the edge
    NBEdge *baseEdge = getNamedEdge("STRNR");
    NBEdge *edge = getNamedEdgeContinuating("STRNR", node);
    // check
    if (node==0||edge==0) {
        return;
    }
    // get the lane
    std::string laneS = myLineParser.know("FSNR") 
        ? NBHelpers::normalIDRepresentation(myLineParser.get("FSNR")) 
        : NBHelpers::normalIDRepresentation(myLineParser.get("NR"));
    int lane = -1;
    try {
        lane = TplConvert<char>::_2int(laneS.c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("A lane number for edge '" + edge->getID() + "' is not numeric (" + laneS + ").");
        return;
    }
    lane -= 1;
    if (lane<0) {
        MsgHandler::getErrorInstance()->inform("A lane number for edge '" + edge->getID() + "' is not positive (" + laneS + ").");
        return;
    }
    // get the direction
    std::string dirS = NBHelpers::normalIDRepresentation(myLineParser.get("RICHTTYP"));
    int prevLaneNo = baseEdge->getNoLanes();
    if ((dirS=="1"&&!(node->hasIncoming(edge))) || (dirS=="0"&&!(node->hasOutgoing(edge)))) {
        // get the last part of the turnaround direction
        edge = getReversedContinuating(edge, node);
    }
    // get the length
    std::string lengthS = NBHelpers::normalIDRepresentation(myLineParser.get("LAENGE"));
    SUMOReal length = -1;
    try {
        length = TplConvert<char>::_2SUMOReal(lengthS.c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("A lane length for edge '" + edge->getID() + "' is not numeric (" + lengthS + ").");
        return;
    }
    if (length<0) {
        MsgHandler::getErrorInstance()->inform("A lane length for edge '" + edge->getID() + "' is not positive (" + lengthS + ").");
        return;
    }
    //
    if (dirS=="1") {
        lane -= prevLaneNo;
    }
    //
    if (length==0) {
        if ((int) edge->getNoLanes()>lane) {
            // ok, we know this already...
            return;
        }
        // increment by one
        edge->incLaneNo(1);
    } else {
        // check whether this edge already has been created
        if (edge->getID().substr(edge->getID().length()-node->getID().length()-1)=="_" + node->getID()) {
            if (edge->getID().substr(edge->getID().find('_'))=="_" + toString(length) + "_" + node->getID()) {
                if ((int) edge->getNoLanes()>lane) {
                    // ok, we know this already...
                    return;
                }
                // increment by one
                edge->incLaneNo(1);
                return;
            }
        }
        // nope, we have to split the edge...
        //  maybe it is not the proper edge to split - VISUM seems not to sort the splits...
        bool mustRecheck = true;
        NBNode *nextNode = node;
        SUMOReal seenLength = 0;
        while (mustRecheck) {
            if (edge->getID().substr(edge->getID().length()-node->getID().length()-1)=="_" + node->getID()) {
                // ok, we have a previously created edge here
                std::string sub = edge->getID();
                sub = sub.substr(sub.rfind('_', sub.rfind('_')-1));
                sub = sub.substr(1, sub.find('_', 1)-1);
                SUMOReal dist = TplConvert<char>::_2SUMOReal(sub.c_str());
                if (dist<length) {
                    seenLength += edge->getLength();
                    if (dirS=="1") {
                        // incoming -> move back
                        edge = edge->getFromNode()->getIncomingEdges()[0];
                        nextNode = edge->getToNode();
                        nextNode = edge->getFromNode();
                    } else {
                        // outgoing -> move forward
                        edge = edge->getToNode()->getOutgoingEdges()[0];
                        nextNode = edge->getFromNode();
                        nextNode = edge->getToNode();
                    }
                } else {
                    mustRecheck = false;
                }
            } else {
                // we have the center edge - do not continue...
                mustRecheck = false;
            }
        }
        // compute position
        Position2D p;
        SUMOReal useLength = length - seenLength;
        useLength = edge->getLength()-useLength;
        std::string edgeID = edge->getID();
        p = edge->getGeometry().positionAtLengthPosition(useLength);
        if (edgeID.substr(edgeID.length()-node->getID().length()-1)=="_" + node->getID()) {
            edgeID = edgeID.substr(0, edgeID.find('_'));
        }
        NBNode *rn = new NBNode(edgeID + "_" +  toString((size_t) length) + "_" + node->getID(), p);
        if (!myNetBuilder.getNodeCont().insert(rn)) {
            throw ProcessError("Ups - could not insert node!");
        }
        std::string nid = edgeID + "_" +  toString((size_t) length) + "_" + node->getID();
        myNetBuilder.getEdgeCont().splitAt(myNetBuilder.getDistrictCont(), edge, useLength, rn,
                                           edge->getID(), nid, edge->getNoLanes()+0, edge->getNoLanes()+1);
        NBEdge *nedge = myNetBuilder.getEdgeCont().retrieve(nid);
        nedge = nedge->getToNode()->getOutgoingEdges()[0];
        while (nedge->getID().substr(nedge->getID().length()-node->getID().length()-1)=="_" + node->getID()) {
            assert(nedge->getToNode()->getOutgoingEdges().size()>0);
            nedge->incLaneNo(1);
            nedge = nedge->getToNode()->getOutgoingEdges()[0];
        }
    }
}


void
NIImporter_VISUM::parse_TrafficLights() {
    // get the id
    myCurrentID = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
    // cycle time
    SUMOReal CycleTime = getNamedFloat("Umlaufzeit", "UMLZEIT");
    // IntermediateTime
    SUMOReal IntermediateTime = getNamedFloat("StdZwischenzeit", "STDZWZEIT");
    // PhaseBased
    bool PhaseBased = myLineParser.know("PhasenBasiert")
                      ? TplConvert<char>::_2bool(myLineParser.get("PhasenBasiert").c_str())
                      : false;
    // add to the list
    myNIVisumTLs[myCurrentID] = new NIVisumTL(myCurrentID, (SUMOTime) CycleTime, (SUMOTime) IntermediateTime, PhaseBased);
}


void
NIImporter_VISUM::parse_NodesToTrafficLights() {
    std::string Node = myLineParser.get("KnotNr").c_str();
    std::string TrafficLight = myLineParser.get("LsaNr").c_str();
    // add to the list
    myNIVisumTLs[TrafficLight]->GetNodes()->push_back(myNetBuilder.getNodeCont().retrieve(Node));
}


void
NIImporter_VISUM::parse_SignalGroups() {
    // get the id
    myCurrentID = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
    std::string LSAid = NBHelpers::normalIDRepresentation(myLineParser.get("LsaNr"));
    // StartTime
    SUMOReal StartTime = getNamedFloat("GzStart", "GRUENANF");
    // EndTime
    SUMOReal EndTime = getNamedFloat("GzEnd", "GRUENENDE");
    // add to the list
    if (myNIVisumTLs.find(LSAid)==myNIVisumTLs.end()) {
        MsgHandler::getErrorInstance()->inform("Could not find TLS '" + LSAid + "' for setting the signal group.");
        return;
    }
    (*myNIVisumTLs.find(LSAid)).second->AddSignalGroup(myCurrentID, (SUMOTime) StartTime, (SUMOTime) EndTime);
}


void
NIImporter_VISUM::parse_TurnsToSignalGroups() {
    // get the id
    std::string SGid = getNamedString("SGNR", "SIGNALGRUPPENNR");
    std::string LSAid = getNamedString("LsaNr");
    // nodes
    NBNode *from = myLineParser.know("VonKnot") ? getNamedNode("VonKnot") : 0;
    NBNode *via = myLineParser.know("KNOTNR") 
        ? getNamedNode("KNOTNR")
        : getNamedNode("UeberKnot", "UeberKnotNr");
    NBNode *to = myLineParser.know("NachKnot") ? getNamedNode("NachKnot") : 0;
    // edges
    NBEdge *edg1 = 0;
    NBEdge *edg2 = 0;
    if (from==0&&to==0) {
        edg1 = getNamedEdgeContinuating("VONSTRNR", via);
        edg2 = getNamedEdgeContinuating("NACHSTRNR", via);
    } else {
        edg1 = getEdge(from, via);
        edg2 = getEdge(via, to);
    }
    // add to the list
    NIVisumTL::SignalGroup *SG;
    SG = (*myNIVisumTLs.find(LSAid)).second->GetSignalGroup(SGid);
    if (edg1!=0 && edg2!=0) {
        if (!via->hasIncoming(edg1)) {
            std::string sid;
            if (edg1->getID()[0]=='-') {
                sid = edg1->getID().substr(1);
            } else {
                sid = "-" + edg1->getID();
            }
            if (sid.find('_')!=std::string::npos) {
                sid = sid.substr(0, sid.find('_'));
            }
            edg1 = getNamedEdgeContinuating(myNetBuilder.getEdgeCont().retrieve(sid),  via);
        }
        if (!via->hasOutgoing(edg2)) {
            std::string sid;
            if (edg2->getID()[0]=='-') {
                sid = edg2->getID().substr(1);
            } else {
                sid = "-" + edg2->getID();
            }
            if (sid.find('_')!=std::string::npos) {
                sid = sid.substr(0, sid.find('_'));
            }
            edg2 = getNamedEdgeContinuating(myNetBuilder.getEdgeCont().retrieve(sid),  via);
        }
        SG->GetConnections()->push_back(NBConnection(edg1, edg2));
    }
}


void
NIImporter_VISUM::parse_AreaSubPartElement() {
    long id = TplConvert<char>::_2long(myLineParser.get("TFLAECHEID").c_str());
    long edgeid = TplConvert<char>::_2long(myLineParser.get("KANTEID").c_str());
    if (myEdges.find(edgeid)==myEdges.end()) {
        MsgHandler::getErrorInstance()->inform("Unknown edge in TEILFLAECHENELEMENT");
        return;
    }
    std::string dir = myLineParser.get("RICHTUNG");
    std::string indexS = NBHelpers::normalIDRepresentation(myLineParser.get("INDEX"));
    int index = -1;
    try {
        index = TplConvert<char>::_2int(indexS.c_str()) - 1;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("An index for a TEILFLAECHENELEMENT is not numeric (id='" + toString(id) + "').");
        return;
    }
    Position2DVector shape;
    shape.push_back(myPoints[myEdges[edgeid].first]);
    shape.push_back(myPoints[myEdges[edgeid].second]);
    if (dir.length()>0&&dir[0]=='1') {
        shape = shape.reverse();
    }
    if (mySubPartsAreas.find(id)==mySubPartsAreas.end()) {
        MsgHandler::getErrorInstance()->inform("Unkown are for area part '" + myCurrentID + "'.");
        return;
    }

    const std::vector<long> &areas = mySubPartsAreas.find(id)->second;
    for (std::vector<long>::const_iterator i=areas.begin(); i!=areas.end(); ++i) {
        NBDistrict *d = myShapeDistrictMap[*i];
        if (d==0) {
            continue;
        }
        if (myDistrictShapes.find(d)==myDistrictShapes.end()) {
            myDistrictShapes[d] = Position2DVector();
        }
        if (dir.length()>0&&dir[0]=='1') {
            myDistrictShapes[d].push_back(myPoints[myEdges[edgeid].second]);
            myDistrictShapes[d].push_back(myPoints[myEdges[edgeid].first]);
        } else {
            myDistrictShapes[d].push_back(myPoints[myEdges[edgeid].first]);
            myDistrictShapes[d].push_back(myPoints[myEdges[edgeid].second]);
        }
    }
}


void
NIImporter_VISUM::parse_Phases() {
    // get the id
    std::string Phaseid = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
    std::string LSAid = NBHelpers::normalIDRepresentation(myLineParser.get("LsaNr"));
    // StartTime
    SUMOReal StartTime = getNamedFloat("GzStart", "GRUENANF");
    // EndTime
    SUMOReal EndTime = getNamedFloat("GzEnd", "GRUENENDE");
    // add to the list
    (*myNIVisumTLs.find(LSAid)).second->AddPhase(Phaseid, (SUMOTime) StartTime, (SUMOTime) EndTime);
}


void NIImporter_VISUM::parse_SignalGroupsToPhases() {
    // get the id
    std::string Phaseid = NBHelpers::normalIDRepresentation(myLineParser.get("PsNr"));
    std::string LSAid = NBHelpers::normalIDRepresentation(myLineParser.get("LsaNr"));
    std::string SGid = NBHelpers::normalIDRepresentation(myLineParser.get("SGNR"));
    // insert
    NIVisumTL::Phase *PH;
    NIVisumTL::SignalGroup *SG;
    NIVisumTL *LSA;
    LSA = (*myNIVisumTLs.find(LSAid)).second;
    SG = LSA->GetSignalGroup(SGid);
    PH = (*LSA->GetPhases()->find(Phaseid)).second;
    (*SG->GetPhases())[Phaseid] = PH;
}


void NIImporter_VISUM::parse_LanesConnections() {
    // get the node
    NBNode *node = getNamedNode("KNOTNR", "KNOT");
    if (node==0) {
        return;
    }
    // get the from-edge
    NBEdge *fromEdge = getNamedEdgeContinuating("VONSTRNR", "VONSTR", node);
    NBEdge *toEdge = getNamedEdgeContinuating("NACHSTRNR", "NACHSTR", node);
    if (fromEdge==0||toEdge==0) {
        return;
    }

    int fromLaneOffset = 0;
    if (!node->hasIncoming(fromEdge)) {
        fromLaneOffset = fromEdge->getNoLanes();
        fromEdge = getReversedContinuating(fromEdge, node);
    } else {
        fromEdge = getReversedContinuating(fromEdge, node);
        NBEdge *tmp = myNetBuilder.getEdgeCont().retrieve(fromEdge->getID().substr(0, fromEdge->getID().find('_')));
        fromLaneOffset = tmp->getNoLanes();
    }

    int toLaneOffset = 0;
    if (!node->hasOutgoing(toEdge)) {
        toLaneOffset = toEdge->getNoLanes();
        toEdge = getReversedContinuating(toEdge, node);
    } else {
        NBEdge *tmp = myNetBuilder.getEdgeCont().retrieve(toEdge->getID().substr(0, toEdge->getID().find('_')));
        toLaneOffset = tmp->getNoLanes();
    }
    // get the from-lane
    std::string fromLaneS = NBHelpers::normalIDRepresentation(myLineParser.get("VONFSNR"));
    int fromLane = -1;
    try {
        fromLane = TplConvert<char>::_2int(fromLaneS.c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("A from-lane number for edge '" + fromEdge->getID() + "' is not numeric (" + fromLaneS + ").");
        return;
    }
    fromLane -= 1;
    if (fromLane<0) {
        MsgHandler::getErrorInstance()->inform("A from-lane number for edge '" + fromEdge->getID() + "' is not positive (" + fromLaneS + ").");
        return;
    }
    // get the from-lane
    std::string toLaneS = NBHelpers::normalIDRepresentation(myLineParser.get("NACHFSNR"));
    int toLane = -1;
    try {
        toLane = TplConvert<char>::_2int(toLaneS.c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("A to-lane number for edge '" + toEdge->getID() + "' is not numeric (" + toLaneS + ").");
        return;
    }
    toLane -= 1;
    if (toLane<0) {
        MsgHandler::getErrorInstance()->inform("A to-lane number for edge '" + toEdge->getID() + "' is not positive (" + toLaneS + ").");
        return;
    }
    // !!! the next is probably a hack
    if (fromLane-fromLaneOffset<0) {
        fromLaneOffset = 0;
    } else {
        fromLane = fromEdge->getNoLanes() - (fromLane-fromLaneOffset) - 1;
    }
    if (toLane-toLaneOffset<0) {
        toLaneOffset = 0;
    } else {
        toLane = toEdge->getNoLanes() - (toLane-toLaneOffset) - 1;
    }
    //
    if ((int) fromEdge->getNoLanes()<=fromLane) {
        MsgHandler::getErrorInstance()->inform("A from-lane number for edge '" + fromEdge->getID() + "' is larger than the edge's lane number (" + fromLaneS + ").");
        return;
    }
    if ((int) toEdge->getNoLanes()<=toLane) {
        MsgHandler::getErrorInstance()->inform("A to-lane number for edge '" + toEdge->getID() + "' is larger than the edge's lane number (" + toLaneS + ").");
        return;
    }
    //
    fromEdge->addLane2LaneConnection(fromLane, toEdge, toLane, NBEdge::L2L_VALIDATED);
}













SUMOReal
NIImporter_VISUM::getWeightedFloat(const std::string &name) throw() {
    try {
        return TplConvert<char>::_2SUMOReal(myLineParser.get(name).c_str());
    } catch (...) {}
    try {
        return TplConvert<char>::_2SUMOReal(myLineParser.get((name+"(IV)")).c_str());
    } catch (...) {}
    return -1;
}


bool
NIImporter_VISUM::getWeightedBool(const std::string &name) throw() {
    try {
        return TplConvert<char>::_2bool(myLineParser.get(name).c_str());
    } catch (...) {}
    try {
        return TplConvert<char>::_2bool(myLineParser.get((name+"(IV)")).c_str());
    } catch (...) {}
    return false;
}


NBNode *
NIImporter_VISUM::getNamedNode(const std::string &fieldName) throw(OutOfBoundsException, NumberFormatException, UnknownElement) {
    std::string nodeS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
    NBNode *node = myNetBuilder.getNodeCont().retrieve(nodeS);
    if (node==0) {
        MsgHandler::getErrorInstance()->inform("The node '" + nodeS + "' is not known.");
    }
    return node;
}


NBNode *
NIImporter_VISUM::getNamedNode(const std::string &fieldName1, const std::string &fieldName2) throw(OutOfBoundsException, NumberFormatException, UnknownElement) {
    if (myLineParser.know(fieldName1)) {
        return getNamedNode(fieldName1);
    } else {
        return getNamedNode(fieldName2);
    }
}


NBEdge *
NIImporter_VISUM::getNamedEdge(const std::string &fieldName) throw(OutOfBoundsException, NumberFormatException, UnknownElement) {
    std::string edgeS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
    NBEdge *edge = myNetBuilder.getEdgeCont().retrieve(edgeS);
    if (edge==0) {
        MsgHandler::getErrorInstance()->inform("The edge '" + edgeS + "' is not known.");
    }
    return edge;
}


NBEdge *
NIImporter_VISUM::getNamedEdge(const std::string &fieldName1, const std::string &fieldName2) throw(OutOfBoundsException, NumberFormatException, UnknownElement) {
    if (myLineParser.know(fieldName1)) {
        return getNamedEdge(fieldName1);
    } else {
        return getNamedEdge(fieldName2);
    }
}



NBEdge *
NIImporter_VISUM::getReversedContinuating(NBEdge *edge, NBNode *node) throw() {
    std::string sid;
    if (edge->getID()[0]=='-') {
        sid = edge->getID().substr(1);
    } else {
        sid = "-" + edge->getID();
    }
    if (sid.find('_')!=std::string::npos) {
        sid = sid.substr(0, sid.find('_'));
    }
    return getNamedEdgeContinuating(myNetBuilder.getEdgeCont().retrieve(sid),  node);
}


NBEdge *
NIImporter_VISUM::getNamedEdgeContinuating(NBEdge *begin, NBNode *node) throw() {
    if (begin==0) {
        return 0;
    }
    NBEdge *ret = begin;
    std::string edgeID = ret->getID();
    // hangle forward
    while (ret!=0) {
        // ok, this is the edge we are looking for
        if (ret->getToNode()==node) {
            return ret;
        }
        const EdgeVector &nedges = ret->getToNode()->getOutgoingEdges();
        if (nedges.size()!=1) {
            // too many edges follow
            ret = 0;
            continue;
        }
        NBEdge *next = nedges[0];
        if (ret->getID().substr(0, edgeID.length())!=next->getID().substr(0, edgeID.length())) {
            // ok, another edge is next...
            ret = 0;
            continue;
        }
        if (next->getID().substr(next->getID().length()-node->getID().length())!=node->getID()) {
            ret = 0;
            continue;
        }
        ret = next;
    }

    ret = begin;
    // hangle backward
    while (ret!=0) {
        // ok, this is the edge we are looking for
        if (ret->getFromNode()==node) {
            return ret;
        }
        const EdgeVector &nedges = ret->getFromNode()->getIncomingEdges();
        if (nedges.size()!=1) {
            // too many edges follow
            ret = 0;
            continue;
        }
        NBEdge *next = nedges[0];
        if (ret->getID().substr(0, edgeID.length())!=next->getID().substr(0, edgeID.length())) {
            // ok, another edge is next...
            ret = 0;
            continue;
        }
        if (next->getID().substr(next->getID().length()-node->getID().length())!=node->getID()) {
            ret = 0;
            continue;
        }
        ret = next;
    }
    return 0;
}


NBEdge *
NIImporter_VISUM::getNamedEdgeContinuating(const std::string &fieldName, NBNode *node) throw(OutOfBoundsException, NumberFormatException, UnknownElement) {
    std::string edgeS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
    NBEdge *edge = myNetBuilder.getEdgeCont().retrieve(edgeS);
    if (edge==0) {
        MsgHandler::getErrorInstance()->inform("The edge '" + edgeS + "' is not known.");
    }
    return getNamedEdgeContinuating(edge, node);
}


NBEdge *
NIImporter_VISUM::getNamedEdgeContinuating(const std::string &fieldName1, const std::string &fieldName2,
        NBNode *node) throw(OutOfBoundsException, NumberFormatException, UnknownElement) {
    if (myLineParser.know(fieldName1)) {
        return getNamedEdgeContinuating(fieldName1, node);
    } else {
        return getNamedEdgeContinuating(fieldName2, node);
    }
}


NBEdge *
NIImporter_VISUM::getEdge(NBNode *FromNode, NBNode *ToNode) throw() {
    EdgeVector::const_iterator i;
    for (i = FromNode->getOutgoingEdges().begin(); i != FromNode->getOutgoingEdges().end(); i++) {
        if (ToNode == (*i)->getToNode()) {
            return(*i);
        }
    }
    //!!!
    return 0;
}


SUMOReal
NIImporter_VISUM::getNamedFloat(const std::string &fieldName) throw(OutOfBoundsException, NumberFormatException, UnknownElement) {
    std::string valS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
    return TplConvert<char>::_2SUMOReal(valS.c_str());
}


SUMOReal
NIImporter_VISUM::getNamedFloat(const std::string &fieldName, SUMOReal defaultValue) throw() {
    try {
        std::string valS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
        return TplConvert<char>::_2SUMOReal(valS.c_str());
    } catch (...) {
        return defaultValue;
    }
}


SUMOReal
NIImporter_VISUM::getNamedFloat(const std::string &fieldName1, const std::string &fieldName2) throw(OutOfBoundsException, NumberFormatException, UnknownElement) {
    if (myLineParser.know(fieldName1)) {
        return getNamedFloat(fieldName1);
    } else {
        return getNamedFloat(fieldName2);
    }
}


SUMOReal
NIImporter_VISUM::getNamedFloat(const std::string &fieldName1, const std::string &fieldName2,
                                SUMOReal defaultValue) throw() {
    if (myLineParser.know(fieldName1)) {
        return getNamedFloat(fieldName1, defaultValue);
    } else {
        return getNamedFloat(fieldName2, defaultValue);
    }
}


std::string
NIImporter_VISUM::getNamedString(const std::string &fieldName) throw(OutOfBoundsException, NumberFormatException, UnknownElement) {
    return NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
}


std::string
NIImporter_VISUM::getNamedString(const std::string &fieldName1,
                                 const std::string &fieldName2) throw(OutOfBoundsException, NumberFormatException, UnknownElement) {
    if (myLineParser.know(fieldName1)) {
        return getNamedString(fieldName1);
    } else {
        return getNamedString(fieldName2);
    }
}






NBNode *
NIImporter_VISUM::buildDistrictNode(const std::string &id, NBNode *dest,
                                    bool isSource) throw() {
    // get the district
    NBDistrict *dist = myNetBuilder.getDistrictCont().retrieve(id);
    if (dist==0) {
        return 0;
    }
    // build the id
    std::string nid;
    nid = id + "-" + dest->getID();
    if (!isSource) {
        nid = "-" + nid;
    }
    // insert the node
    if (!myNetBuilder.getNodeCont().insert(nid, dist->getPosition())) {
        MsgHandler::getErrorInstance()->inform("Could not build connector node '" + nid + "'.");
    }
    // return the node
    return myNetBuilder.getNodeCont().retrieve(nid);
}


bool
NIImporter_VISUM::checkNodes(NBNode *from, NBNode *to)  throw() {
    if (from==0) {
        MsgHandler::getErrorInstance()->inform(" The from-node was not found within the net");
    }
    if (to==0) {
        MsgHandler::getErrorInstance()->inform(" The to-node was not found within the net");
    }
    if (from==to) {
        MsgHandler::getErrorInstance()->inform(" Both nodes are the same");
    }
    return from!=0&&to!=0&&from!=to;
}


/****************************************************************************/

