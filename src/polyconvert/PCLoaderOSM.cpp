/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    PCLoaderOSM.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @date    Wed, 19.11.2008
///
// A reader of pois and polygons stored in OSM-format
/****************************************************************************/
#include <config.h>

#include <string>
#include <map>
#include <fstream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StdDefs.h>
#include <utils/common/SysUtils.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <polyconvert/PCPolyContainer.h>
#include "PCLoaderOSM.h"

// static members
// ---------------------------------------------------------------------------
const std::set<std::string> PCLoaderOSM::MyKeysToInclude(PCLoaderOSM::initMyKeysToInclude());

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static interface
// ---------------------------------------------------------------------------
std::set<std::string> PCLoaderOSM::initMyKeysToInclude() {
    std::set<std::string> result;
    result.insert("highway");
    result.insert("railway");
    result.insert("railway:position");
    result.insert("railway:position:exact");
    result.insert("waterway");
    result.insert("aeroway");
    result.insert("aerialway");
    result.insert("power");
    result.insert("man_made");
    result.insert("building");
    result.insert("leisure");
    result.insert("amenity");
    result.insert("shop");
    result.insert("tourism");
    result.insert("historic");
    result.insert("landuse");
    result.insert("natural");
    result.insert("military");
    result.insert("boundary");
    result.insert("admin_level");
    result.insert("sport");
    result.insert("polygon");
    result.insert("place");
    result.insert("population");
    result.insert("barrier");
    result.insert("openGeoDB:population");
    result.insert("openGeoDB:name");
    return result;
}

void
PCLoaderOSM::loadIfSet(OptionsCont& oc, PCPolyContainer& toFill, PCTypeMap& tm) {
    if (!oc.isSet("osm-files")) {
        return;
    }
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("osm-files");
    // load nodes, first
    std::map<long long int, PCOSMNode*> nodes;
    bool withAttributes = oc.getBool("all-attributes");
    MsgHandler* m = OptionsCont::getOptions().getBool("ignore-errors") ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance();
    NodesHandler nodesHandler(nodes, withAttributes, *m);
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // nodes
        if (!FileHelpers::isReadable(*file)) {
            WRITE_ERRORF(TL("Could not open osm-file '%'."), *file);
            return;
        }
        const long before = PROGRESS_BEGIN_TIME_MESSAGE("Parsing nodes from osm-file '" + *file + "'");
        if (!XMLSubSys::runParser(nodesHandler, *file)) {
            for (std::map<long long int, PCOSMNode*>::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
                delete (*i).second;
            }
            throw ProcessError();
        }
        PROGRESS_TIME_MESSAGE(before);
    }
    // load relations to see which additional ways may be relevant
    Relations relations;
    RelationsMap additionalWays;
    std::set<long long int> innerEdges;
    RelationsHandler relationsHandler(additionalWays, relations, innerEdges, withAttributes, *m);
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // edges
        const long before = PROGRESS_BEGIN_TIME_MESSAGE("Parsing relations from osm-file '" + *file + "'");
        XMLSubSys::runParser(relationsHandler, *file);
        PROGRESS_TIME_MESSAGE(before);
    }

    // load ways
    EdgeMap edges;
    EdgesHandler edgesHandler(nodes, edges, additionalWays, withAttributes, *m);
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // edges
        const long before = PROGRESS_BEGIN_TIME_MESSAGE("Parsing edges from osm-file '" + *file + "'");
        XMLSubSys::runParser(edgesHandler, *file);
        PROGRESS_TIME_MESSAGE(before);
    }

    // build all
    const bool useName = oc.getBool("osm.use-name");
    const double mergeRelationsThreshold = OptionsCont::getOptions().getFloat("osm.merge-relations");
    // create polygons from relations
    if (mergeRelationsThreshold >= 0) {
        for (PCOSMRelation* rel : relations) {
            if (!rel->keep || rel->myWays.empty()) {
                continue;
            }
            // filter unknown and empty ways
            int numNodes = 0;
            for (auto it = rel->myWays.begin(); it != rel->myWays.end();) {
                if (edges.count(*it) == 0 || edges[*it]->myCurrentNodes.empty()) {
                    it = rel->myWays.erase(it);
                } else if (innerEdges.count(*it) > 0) {
                    // @note: it would be a good idea to merge inner shapes but
                    // it's difficult since there may be more than one
                    // independent inner shape
                    edges[*it]->standalone = true;
                    it = rel->myWays.erase(it);
                } else {
                    numNodes += (int)edges[*it]->myCurrentNodes.size();
                    it++;
                }
            }
            if (numNodes == 0) {
                WRITE_WARNINGF(TL("Could not import polygon from relation '%' (missing ways)"), rel->id);
                continue;
            }
            PCOSMEdge* e = new PCOSMEdge();
            e->id = rel->id;
            e->name = rel->name;
            e->myAttributes = rel->myAttributes;
            e->myIsClosed = false;
            e->standalone = true;

            std::vector<std::vector<long long int> > snippets;
            for (const long long int wayID : rel->myWays) {
                PCOSMEdge* edge = edges[wayID];
                snippets.push_back(edge->myCurrentNodes);
            }
            double maxDist = 0.;
            bool ok = true;
            while (snippets.size() > 1) {
                maxDist = MAX2(maxDist, mergeClosest(nodes, snippets));
                if (maxDist > mergeRelationsThreshold) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                e->myCurrentNodes = snippets.front();
                edges[e->id] = e;
                double frontBackDist = 0;
                if (e->myCurrentNodes.front() != e->myCurrentNodes.back()) {
                    // should be filled
                    const Position posFront = convertNodePosition(nodes[e->myCurrentNodes.front()]);
                    const Position posBack = convertNodePosition(nodes[e->myCurrentNodes.back()]);
                    frontBackDist = posFront.distanceTo2D(posBack);
                    if (frontBackDist < mergeRelationsThreshold) {
                        e->myCurrentNodes.push_back(e->myCurrentNodes.front());
                        frontBackDist = 0;
                    }
                }
                std::string frontBackMsg = "";
                if (frontBackDist > 0) {
                    frontBackMsg = TLF(", (front-to-back dist: %)", frontBackDist);
                }
                WRITE_MESSAGEF(TL("Assembled polygon from relation '%' (name:%)%"), toString(rel->id), e->name, frontBackMsg);
            } else {
                WRITE_WARNINGF(TL("Could not import polygon from relation '%' (name:% reason: found gap of %m)."), rel->id, rel->name, maxDist)
                delete e;
                // export ways by themselves
                for (long long int wayID : rel->myWays) {
                    PCOSMEdge* part = edges[wayID];
                    part->standalone = true;
                }
            }
        }
    }

    // instantiate polygons
    for (EdgeMap::iterator i = edges.begin(); i != edges.end(); ++i) {
        PCOSMEdge* e = (*i).second;
        if (e->myAttributes.size() == 0) {
            // cannot be relevant as a polygon
            continue;
        }
        if (!e->standalone && mergeRelationsThreshold >= 0) {
            // part of a relation
            continue;
        }
        if (e->myCurrentNodes.size() == 0) {
            WRITE_WARNINGF(TL("Polygon '%' has no shape."), toString(e->id));
            continue;
        }
        // compute shape
        PositionVector vec;
        for (std::vector<long long int>::iterator j = e->myCurrentNodes.begin(); j != e->myCurrentNodes.end(); ++j) {
            PCOSMNode* n = nodes.find(*j)->second;
            Position pos(n->lon, n->lat);
            if (!GeoConvHelper::getProcessing().x2cartesian(pos)) {
                WRITE_WARNINGF(TL("Unable to project coordinates for polygon '%'."), e->id);
            }
            vec.push_back_noDoublePos(pos);
        }
        const bool ignorePruning = OptionsCont::getOptions().isInStringVector("prune.keep-list", toString(e->id));
        // add as many polygons as keys match defined types
        int index = 0;
        std::string unknownPolyType = "";
        bool isInner = mergeRelationsThreshold >= 0 && innerEdges.count(e->id) != 0 && tm.has("inner");
        for (std::map<std::string, std::string>::iterator it = e->myAttributes.begin(); it != e->myAttributes.end(); ++it) {
            const std::string& key = it->first;
            const std::string& value = it->second;
            const std::string fullType = key + "." + value;
            if (tm.has(key + "." + value)) {
                auto def = tm.get(isInner ? "inner" : fullType);
                index = addPolygon(e, vec, def, fullType, index, useName, toFill, ignorePruning, withAttributes);
            } else if (tm.has(key)) {
                auto def = tm.get(isInner ? "inner" : key);
                index = addPolygon(e, vec, def, fullType, index, useName, toFill, ignorePruning, withAttributes);
            } else if (MyKeysToInclude.count(key) > 0) {
                unknownPolyType = fullType;
            }
        }
        const PCTypeMap::TypeDef& def = tm.getDefault();
        if (index == 0 && !def.discard && unknownPolyType != "") {
            addPolygon(e, vec, def, unknownPolyType, index, useName, toFill, ignorePruning, withAttributes);
        }
    }


    // instantiate pois
    for (std::map<long long int, PCOSMNode*>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
        PCOSMNode* n = (*i).second;
        if (n->myAttributes.size() == 0) {
            // cannot be relevant as a poi
            continue;
        }
        Position pos(n->lon, n->lat);
        if (!GeoConvHelper::getProcessing().x2cartesian(pos)) {
            WRITE_WARNINGF(TL("Unable to project coordinates for POI '%'."), n->id);
        }
        const bool ignorePruning = OptionsCont::getOptions().isInStringVector("prune.keep-list", toString(n->id));
        // add as many POIs as keys match defined types
        int index = 0;
        std::string unKnownPOIType = "";
        for (std::map<std::string, std::string>::iterator it = n->myAttributes.begin(); it != n->myAttributes.end(); ++it) {
            const std::string& key = it->first;
            const std::string& value = it->second;
            const std::string fullType = key + "." + value;
            if (tm.has(key + "." + value)) {
                index = addPOI(n, pos, tm.get(fullType), fullType, index, useName, toFill, ignorePruning, withAttributes);
            } else if (tm.has(key)) {
                index = addPOI(n, pos, tm.get(key), fullType, index, useName, toFill, ignorePruning, withAttributes);
            } else if (MyKeysToInclude.count(key) > 0) {
                unKnownPOIType = fullType;
            }
        }
        const PCTypeMap::TypeDef& def = tm.getDefault();
        if (index == 0 && !def.discard && unKnownPOIType != "") {
            addPOI(n, pos, def, unKnownPOIType, index, useName, toFill, ignorePruning, withAttributes);
        }
    }
    // delete nodes
    for (std::map<long long int, PCOSMNode*>::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
        delete (*i).second;
    }
    // delete edges
    for (EdgeMap::iterator i = edges.begin(); i != edges.end(); ++i) {
        delete (*i).second;
    }
    // delete relations
    for (Relations::iterator i = relations.begin(); i != relations.end(); ++i) {
        delete (*i);
    }
}


Position
PCLoaderOSM::convertNodePosition(PCOSMNode* n) {
    Position pos(n->lon, n->lat);
    GeoConvHelper::getProcessing().x2cartesian(pos);
    return pos;
}


int
PCLoaderOSM::addPolygon(const PCOSMEdge* edge, const PositionVector& vec, const PCTypeMap::TypeDef& def, const std::string& fullType, int index, bool useName, PCPolyContainer& toFill, bool ignorePruning, bool withAttributes) {
    if (def.discard) {
        return index;
    } else {
        const bool closedShape = vec.front() == vec.back();
        const std::string idSuffix = (index == 0 ? "" : "#" + toString(index));
        const std::string id = def.prefix + (useName && edge->name != "" ? edge->name : toString(edge->id)) + idSuffix;
        bool fill = def.allowFill == PCTypeMap::Filltype::FORCE || (def.allowFill == PCTypeMap::Filltype::FILL && closedShape);
        SUMOPolygon* poly = new SUMOPolygon(
            StringUtils::escapeXML(id),
            StringUtils::escapeXML(OptionsCont::getOptions().getBool("osm.keep-full-type") ? fullType : def.id),
            def.color, vec, false, fill, 1, def.layer);
        if (withAttributes) {
            poly->updateParameters(edge->myAttributes);
        }
        if (!toFill.add(poly, ignorePruning)) {
            return index;
        } else {
            return index + 1;
        }
    }
}


int
PCLoaderOSM::addPOI(const PCOSMNode* node, const Position& pos, const PCTypeMap::TypeDef& def, const std::string& fullType,
                    int index, bool useName, PCPolyContainer& toFill, bool ignorePruning, bool withAttributes) {
    if (def.discard) {
        return index;
    } else {
        const std::string idSuffix = (index == 0 ? "" : "#" + toString(index));
        const std::string id = def.prefix + (useName && node->name != "" ? node->name : toString(node->id)) + idSuffix;
        PointOfInterest* poi = new PointOfInterest(
            StringUtils::escapeXML(id),
            StringUtils::escapeXML(OptionsCont::getOptions().getBool("osm.keep-full-type") ? fullType : def.id),
            def.color, pos, false, "", 0, false, 0, def.icon, def.layer);
        if (withAttributes) {
            poi->updateParameters(node->myAttributes);
        }
        if (!toFill.add(poi, ignorePruning)) {
            return index;
        } else {
            return index + 1;
        }
    }
}


// ---------------------------------------------------------------------------
// definitions of PCLoaderOSM::NodesHandler-methods
// ---------------------------------------------------------------------------
PCLoaderOSM::NodesHandler::NodesHandler(std::map<long long int, PCOSMNode*>& toFill,
                                        bool withAttributes, MsgHandler& errorHandler) :
    SUMOSAXHandler("osm - file"), myWithAttributes(withAttributes), myErrorHandler(errorHandler),
    myToFill(toFill), myLastNodeID(-1) {}


PCLoaderOSM::NodesHandler::~NodesHandler() {}


void
PCLoaderOSM::NodesHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    myParentElements.push_back(element);
    if (element == SUMO_TAG_NODE) {
        bool ok = true;
        long long int id = attrs.get<long long int>(SUMO_ATTR_ID, nullptr, ok);
        if (!ok) {
            return;
        }
        myLastNodeID = -1;
        if (myToFill.find(id) == myToFill.end()) {
            myLastNodeID = id;
            // assume we are loading multiple files...
            //  ... so we won't report duplicate nodes
            PCOSMNode* toAdd = new PCOSMNode();
            toAdd->id = id;
            toAdd->lon = attrs.get<double>(SUMO_ATTR_LON, toString(id).c_str(), ok);
            toAdd->lat = attrs.get<double>(SUMO_ATTR_LAT, toString(id).c_str(), ok);
            if (!ok) {
                delete toAdd;
                return;
            }
            myToFill[toAdd->id] = toAdd;
        }
    }
    if (element == SUMO_TAG_TAG && myParentElements.size() > 2 && myParentElements[myParentElements.size() - 2] == SUMO_TAG_NODE
            && myLastNodeID != -1) {
        bool ok = true;
        std::string key = attrs.getOpt<std::string>(SUMO_ATTR_K, toString(myLastNodeID).c_str(), ok, "", false);
        std::string value = attrs.getOpt<std::string>(SUMO_ATTR_V, toString(myLastNodeID).c_str(), ok, "", false);
        if (key == "name") {
            myToFill[myLastNodeID]->name = value;
        } else if (key == "") {
            myErrorHandler.inform("Empty key in a a tag while parsing node '" + toString(myLastNodeID) + "' occurred.");
            ok = false;
        }
        if (!ok) {
            return;
        }
        myToFill[myLastNodeID]->myAttributes[key] = value;
    }
}


void
PCLoaderOSM::NodesHandler::myEndElement(int element) {
    if (element == SUMO_TAG_NODE) {
        myLastNodeID = -1;
    }
    myParentElements.pop_back();
}


// ---------------------------------------------------------------------------
// definitions of PCLoaderOSM::RelationsHandler-methods
// ---------------------------------------------------------------------------
PCLoaderOSM::RelationsHandler::RelationsHandler(RelationsMap& additionalWays,
        Relations& relations,
        std::set<long long int>& innerEdges,
        bool withAttributes,
        MsgHandler& errorHandler) :
    SUMOSAXHandler("osm - file"),
    myAdditionalWays(additionalWays),
    myRelations(relations),
    myInnerEdges(innerEdges),
    myWithAttributes(withAttributes),
    myErrorHandler(errorHandler),
    myCurrentRelation(nullptr) {
}


PCLoaderOSM::RelationsHandler::~RelationsHandler() {
}


void
PCLoaderOSM::RelationsHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    myParentElements.push_back(element);
    // parse "relation" elements
    if (element == SUMO_TAG_RELATION) {
        myCurrentWays.clear();
        bool ok = true;
        const std::string& action = attrs.getOpt<std::string>(SUMO_ATTR_ACTION, nullptr, ok);
        if (action == "delete" || !ok) {
            myCurrentRelation = nullptr;
        } else {
            myCurrentRelation = new PCOSMRelation();
            myCurrentRelation->keep = false;
            myCurrentRelation->id = attrs.get<long long int>(SUMO_ATTR_ID, nullptr, ok);
            myRelations.push_back(myCurrentRelation);
        }
        return;
    } else if (myCurrentRelation == nullptr) {
        return;
    }
    // parse member elements
    if (element == SUMO_TAG_MEMBER) {
        bool ok = true;
        std::string role = attrs.hasAttribute("role") ? attrs.getStringSecure("role", "") : "";
        long long int ref = attrs.get<long long int>(SUMO_ATTR_REF, nullptr, ok);
        if (role == "outer" || role == "inner") {
            std::string memberType = attrs.get<std::string>(SUMO_ATTR_TYPE, nullptr, ok);
            if (memberType == "way") {
                myCurrentWays.push_back(ref);
                if (role == "inner") {
                    myInnerEdges.insert(ref);
                }
            }
        }
        return;
    }
    // parse values
    if (element == SUMO_TAG_TAG && myParentElements.size() > 2 && myParentElements[myParentElements.size() - 2] == SUMO_TAG_RELATION
            && myCurrentRelation != nullptr) {
        bool ok = true;
        std::string key = attrs.getOpt<std::string>(SUMO_ATTR_K, toString(myCurrentRelation).c_str(), ok, "", false);
        std::string value = attrs.getOpt<std::string>(SUMO_ATTR_V, toString(myCurrentRelation).c_str(), ok, "", false);
        if (key == "") {
            myErrorHandler.inform("Empty key in a a tag while parsing way '" + toString(myCurrentRelation) + "' occurred.");
            ok = false;
        }
        if (!ok) {
            return;
        }
        if (key == "name") {
            myCurrentRelation->name = value;
        } else if (MyKeysToInclude.count(key) > 0) {
            myCurrentRelation->keep = true;
            for (std::vector<long long int>::iterator it = myCurrentWays.begin(); it != myCurrentWays.end(); ++it) {
                myAdditionalWays[*it] = myCurrentRelation;
            }
        }
        myCurrentRelation->myAttributes[key] = value;
    }
}


void
PCLoaderOSM::RelationsHandler::myEndElement(int element) {
    myParentElements.pop_back();
    if (element == SUMO_TAG_RELATION) {
        myCurrentRelation->myWays = myCurrentWays;
        myCurrentRelation = nullptr;
        myCurrentWays.clear();
    }
}


// ---------------------------------------------------------------------------
// definitions of PCLoaderOSM::EdgesHandler-methods
// ---------------------------------------------------------------------------
PCLoaderOSM::EdgesHandler::EdgesHandler(const std::map<long long int, PCOSMNode*>& osmNodes,
                                        EdgeMap& toFill,
                                        const RelationsMap& additionalWays,
                                        bool withAttributes, MsgHandler& errorHandler) :
    SUMOSAXHandler("osm - file"),
    myWithAttributes(withAttributes),
    myErrorHandler(errorHandler),
    myOSMNodes(osmNodes),
    myEdgeMap(toFill),
    myAdditionalWays(additionalWays) {
}


PCLoaderOSM::EdgesHandler::~EdgesHandler() {
}


void
PCLoaderOSM::EdgesHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    myParentElements.push_back(element);
    // parse "way" elements
    if (element == SUMO_TAG_WAY) {
        bool ok = true;
        const long long int id = attrs.get<long long int>(SUMO_ATTR_ID, nullptr, ok);
        const std::string& action = attrs.getOpt<std::string>(SUMO_ATTR_ACTION, nullptr, ok);
        if (action == "delete" || !ok) {
            myCurrentEdge = nullptr;
            return;
        }
        myCurrentEdge = new PCOSMEdge();
        myCurrentEdge->id = id;
        myCurrentEdge->myIsClosed = false;
        myCurrentEdge->standalone = false;
        myKeep = (myAdditionalWays.find(id) != myAdditionalWays.end());
    }
    // parse "nd" (node) elements
    if (element == SUMO_TAG_ND && myCurrentEdge != nullptr) {
        bool ok = true;
        const long long int ref = attrs.get<long long int>(SUMO_ATTR_REF, nullptr, ok);
        if (ok) {
            if (myOSMNodes.find(ref) == myOSMNodes.end()) {
                WRITE_WARNINGF(TL("The referenced geometry information (ref='%') is not known"), ref);
                return;
            }
            myCurrentEdge->myCurrentNodes.push_back(ref);
        }
    }
    // parse values
    if (element == SUMO_TAG_TAG && myParentElements.size() > 2 && myParentElements[myParentElements.size() - 2] == SUMO_TAG_WAY
            && myCurrentEdge != nullptr) {
        bool ok = true;
        std::string key = attrs.getOpt<std::string>(SUMO_ATTR_K, toString(myCurrentEdge->id).c_str(), ok, "", false);
        std::string value = attrs.getOpt<std::string>(SUMO_ATTR_V, toString(myCurrentEdge->id).c_str(), ok, "", false);
        if (key == "") {
            myErrorHandler.inform("Empty key in a a tag while parsing way '" + toString(myCurrentEdge->id) + "' occurred.");
            ok = false;
        }
        if (!ok) {
            return;
        }
        if (key == "name") {
            myCurrentEdge->name = value;
        } else if (MyKeysToInclude.count(key) > 0) {
            myKeep = true;
            myCurrentEdge->standalone = true;
        }
        myCurrentEdge->myAttributes[key] = value;
    }
}


void
PCLoaderOSM::EdgesHandler::myEndElement(int element) {
    myParentElements.pop_back();
    if (element == SUMO_TAG_WAY && myCurrentEdge != nullptr) {
        if (myKeep) {
            RelationsMap::const_iterator it = myAdditionalWays.find(myCurrentEdge->id);
            if (it != myAdditionalWays.end()) {
                myCurrentEdge->myAttributes.insert((*it).second->myAttributes.begin(), (*it).second->myAttributes.end());
            }
            myEdgeMap[myCurrentEdge->id] = myCurrentEdge;
        } else {
            delete myCurrentEdge;
        }
        myCurrentEdge = nullptr;
    }
}


double
PCLoaderOSM::mergeClosest(const std::map<long long int, PCOSMNode*>& nodes, std::vector<std::vector<long long int> >& snippets) {
    double best = std::numeric_limits<double>::max();
    int best_i = 0;
    int best_j = 1;
    bool iFW = true;
    bool jFW = true;

    for (int i = 0; i < (int)snippets.size(); i++) {
        for (int j = i + 1; j < (int)snippets.size(); j++) {
            Position front1(convertNodePosition(nodes.find(snippets[i].front())->second));
            Position back1(convertNodePosition(nodes.find(snippets[i].back())->second));
            Position front2(convertNodePosition(nodes.find(snippets[j].front())->second));
            Position back2(convertNodePosition(nodes.find(snippets[j].back())->second));
            double dist1 = front1.distanceTo2D(front2);
            double dist2 = front1.distanceTo2D(back2);
            double dist3 = back1.distanceTo2D(front2);
            double dist4 = back1.distanceTo2D(back2);
            if (dist1 < best) {
                best = dist1;
                best_i = i;
                best_j = j;
                iFW = false;
                jFW = true;
            }
            if (dist2 < best) {
                best = dist2;
                best_i = i;
                best_j = j;
                iFW = false;
                jFW = false;
            }
            if (dist3 < best) {
                best = dist3;
                best_i = i;
                best_j = j;
                iFW = true;
                jFW = true;
            }
            if (dist4 < best) {
                best = dist4;
                best_i = i;
                best_j = j;
                iFW = true;
                jFW = false;
            }
        }
    }
    std::vector<long long int> merged;
    if (iFW) {
        merged.insert(merged.end(), snippets[best_i].begin(), snippets[best_i].end());
    } else {
        merged.insert(merged.end(), snippets[best_i].rbegin(), snippets[best_i].rend());
    }
    if (jFW) {
        merged.insert(merged.end(), snippets[best_j].begin(), snippets[best_j].end());
    } else {
        merged.insert(merged.end(), snippets[best_j].rbegin(), snippets[best_j].rend());
    }
    snippets.erase(snippets.begin() + best_j);
    snippets.erase(snippets.begin() + best_i);
    snippets.push_back(merged);
    return best;
}

/****************************************************************************/
