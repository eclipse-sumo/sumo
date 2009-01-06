/****************************************************************************/
/// @file    NIImporter_SUMO.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id:NIImporter_SUMO.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for networks stored in SUMO format
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
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "NIImporter_SUMO.h"
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_SUMO::loadNetwork(const OptionsCont &oc, NBNetBuilder &nb)
{
    // check whether the option is set (properly)
    if (!oc.isUsableFileList("sumo-net")) {
        return;
    }
    // build the handler
    NIImporter_SUMO handler(nb.getNodeCont());
    // parse file(s)
    vector<string> files = oc.getStringVector("sumo-net");
    for (vector<string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        if (!FileHelpers::exists(*file)) {
            MsgHandler::getErrorInstance()->inform("Could not open sumo-net-file '" + *file + "'.");
            return;
        }
        handler.setFileName(*file);
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing sumo-net from '" + *file + "'...");
        XMLSubSys::runParser(handler, *file);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // build edges
    map<string, EdgeAttrs*> &loadedEdges = handler.myEdges;
    NBNodeCont &nodesCont = nb.getNodeCont();
    NBEdgeCont &edgesCont = nb.getEdgeCont();
    for (map<string, EdgeAttrs*>::const_iterator i=loadedEdges.begin(); i!=loadedEdges.end(); ++i) {
        EdgeAttrs *ed = (*i).second;
        // get and check the nodes
        NBNode *from = nodesCont.retrieve(ed->fromNode);
        NBNode *to = nodesCont.retrieve(ed->toNode);
        if (from==0) {
            MsgHandler::getErrorInstance()->inform("Edge's '" + ed->id + "' from-node '" + ed->fromNode + "' is not known.");
            continue;
        }
        if (to==0) {
            MsgHandler::getErrorInstance()->inform("Edge's '" + ed->id + "' to-node '" + ed->toNode + "' is not known.");
            continue;
        }
        // build and insert the edge
        NBEdge *e = new NBEdge(ed->id, from, to, ed->type, ed->maxSpeed, (unsigned int) ed->lanes.size(), ed->priority);
        if (!edgesCont.insert(e)) {
            MsgHandler::getErrorInstance()->inform("Could not insert edge '" + ed->id + "'.");
            delete e;
            continue;
        }
        ed->builtEdge = edgesCont.retrieve(ed->id);
    }
    // assign lane attributes (edges are built)
    for (map<string, EdgeAttrs*>::const_iterator i=loadedEdges.begin(); i!=loadedEdges.end(); ++i) {
        EdgeAttrs *ed = (*i).second;
        if (ed->builtEdge==0) {
            // earlier errors
            continue;
        }
        for (size_t j=0; j<ed->lanes.size(); ++j) {
            const vector<EdgeLane> &connections = ed->lanes[j]->connections;
            for (std::vector<EdgeLane>::const_iterator k=connections.begin(); k!=connections.end(); ++k) {
                if ((*k).lane!="SUMO_NO_DESTINATION") {
                    string lane = (*k).lane;
                    string edge = lane.substr(0, lane.find('_'));
                    int index = TplConvert<char>::_2int(lane.substr(lane.find('_')+1).c_str());
                    if (loadedEdges.find(edge)==loadedEdges.end()) {
                        MsgHandler::getErrorInstance()->inform("Unknown edge given in succlane (for lane '" + lane + "').");
                        continue;
                    }
                    NBEdge *ce = loadedEdges.find(edge)->second->builtEdge;
                    if (ce==0) {
                        // earlier error or edge removal
                        continue;
                    }
                    ed->builtEdge->addLane2LaneConnection(j, ce, index, NBEdge::L2L_VALIDATED);
                }
            }
        }
    }
    // clean up
    for (map<string, EdgeAttrs*>::const_iterator i=loadedEdges.begin(); i!=loadedEdges.end(); ++i) {
        EdgeAttrs *ed = (*i).second;
        for (vector<LaneAttrs*>::const_iterator j=ed->lanes.begin(); j!=ed->lanes.end(); ++j) {
            delete *j;
        }
        delete ed;
    }
}



// ---------------------------------------------------------------------------
// member methods
// ---------------------------------------------------------------------------
NIImporter_SUMO::NIImporter_SUMO(NBNodeCont &nc)
        : SUMOSAXHandler("sumo-network"),
        myNodeCont(nc), myCurrentEdge(0)
{}


NIImporter_SUMO::~NIImporter_SUMO() throw()
{
}


void
NIImporter_SUMO::myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    switch (element) {
    case SUMO_TAG_EDGE:
        addEdge(attrs);
        break;
    case SUMO_TAG_LANE:
        if (myCurrentEdge!=0) {
            addLane(attrs);
        }
        break;
    case SUMO_TAG_JUNCTION:
        addJunction(attrs);
        break;
    case SUMO_TAG_SUCC:
        addSuccEdge(attrs);
        break;
    case SUMO_TAG_SUCCLANE:
        addSuccLane(attrs);
        break;
    }
}


void
NIImporter_SUMO::myCharacters(SumoXMLTag element,
                              const std::string &chars) throw(ProcessError)
{
    switch (element) {
    case SUMO_TAG_LANE:
        if (myCurrentLane!=0) {
            myCurrentLane->shape = GeomConvHelper::parseShape(chars);
        }
        break;
    }
}



void
NIImporter_SUMO::myEndElement(SumoXMLTag element) throw(ProcessError)
{
    switch (element) {
    case SUMO_TAG_EDGE:
        if (myEdges.find(myCurrentEdge->id)!=myEdges.end()) {
            MsgHandler::getErrorInstance()->inform("Edge '" + myCurrentEdge->id + "' occured at least twice in the input.");
        } else {
            myEdges[myCurrentEdge->id] = myCurrentEdge;
        }
        myCurrentEdge = 0;
        break;
    case SUMO_TAG_LANE:
        if (myCurrentEdge!=0) {
            myCurrentEdge->maxSpeed = MAX2(myCurrentEdge->maxSpeed, myCurrentLane->maxSpeed);
            myCurrentEdge->lanes.push_back(myCurrentLane);
        }
        myCurrentLane = 0;
        break;
    }
}


void
NIImporter_SUMO::addEdge(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if (!attrs.setIDFromAttributes("edge", id)) {
        return;
    }
    myCurrentEdge = new EdgeAttrs;
    myCurrentEdge->id = id;
    // get the type
    myCurrentEdge->type = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
    // get the origin and the destination node
    myCurrentEdge->fromNode = attrs.getStringSecure(SUMO_ATTR_FROM, "");
    myCurrentEdge->toNode = attrs.getStringSecure(SUMO_ATTR_TO, "");
    myCurrentEdge->priority = attrs.getIntSecure(SUMO_ATTR_PRIORITY, -1);
    myCurrentEdge->maxSpeed = 0;
    myCurrentEdge->builtEdge = 0;
}


void
NIImporter_SUMO::addLane(const SUMOSAXAttributes &attrs)
{
    myCurrentLane = new LaneAttrs;
    myCurrentLane->depart = attrs.getBoolSecure(SUMO_ATTR_DEPART, false);
    myCurrentLane->vclasses = attrs.getStringSecure(SUMO_ATTR_VCLASSES, "");
    myCurrentLane->maxSpeed = attrs.getFloatSecure(SUMO_ATTR_MAXSPEED, -1);
}


void
NIImporter_SUMO::addJunction(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if (!attrs.setIDFromAttributes("junction", id)) {
        return;
    }
    if (id[0]==':') {
        return;
    }
    SUMOReal x = attrs.getFloatSecure(SUMO_ATTR_X, -1);
    SUMOReal y = attrs.getFloatSecure(SUMO_ATTR_Y, -1);
    if (x==-1||y==-1) {
        MsgHandler::getErrorInstance()->inform("Junction '" + id + "' has an invalid position.");
        return;
    }
    Position2D pos(x, y);
    GeoConvHelper::x2cartesian(pos);
    string type = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
    NBNode *node = new NBNode(id, pos/* !!!, type */);
    if (!myNodeCont.insert(node)) {
        MsgHandler::getErrorInstance()->inform("Problems on adding junction '" + id + "'.");
        delete node;
        return;
    }
}


void
NIImporter_SUMO::addSuccEdge(const SUMOSAXAttributes &attrs)
{
//    string edge = attrs.getStringSecure(SUMO_ATTR_EDGE, ""); // !!! never used?
    string lane = attrs.getStringSecure(SUMO_ATTR_LANE, "");
    string edge = lane.substr(0, lane.find('_'));
    int index = TplConvert<char>::_2int(lane.substr(lane.find('_')+1).c_str());
    myCurrentEdge = 0;
    myCurrentLane = 0;
    if (myEdges.find(edge)==myEdges.end()) {
        MsgHandler::getErrorInstance()->inform("Unknown edge '" + edge + "' given in succedge.");
        return;
    }
    myCurrentEdge = myEdges.find(edge)->second;
    // !!! externalize retrieval of lane index by name
    if (myCurrentEdge->lanes.size()<(size_t) index) {
        MsgHandler::getErrorInstance()->inform("Unknown lane '" + lane + "' given in succedge.");
        return;
    }
    myCurrentLane = myCurrentEdge->lanes[(size_t) index];
}


void
NIImporter_SUMO::addSuccLane(const SUMOSAXAttributes &attrs)
{
    if (myCurrentLane==0) {
        // had error
        return;
    }
    string lane = attrs.getStringSecure(SUMO_ATTR_LANE, "");
    EdgeLane el;
    el.lane = lane;
    myCurrentLane->connections.push_back(el);
}



/****************************************************************************/

