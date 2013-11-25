/****************************************************************************/
/// @file    NIImporter_OpenDrive.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in openDrive format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <cmath>
#include <iterator>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <foreign/eulerspiral/euler.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/Boundary.h>
#include "NILoader.h"
#include "NIImporter_OpenDrive.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// definitions
// ===========================================================================
#define C_LENGTH 10.


// ===========================================================================
// static variables
// ===========================================================================
StringBijection<int>::Entry NIImporter_OpenDrive::openDriveTags[] = {
    { "header",           NIImporter_OpenDrive::OPENDRIVE_TAG_HEADER },
    { "road",             NIImporter_OpenDrive::OPENDRIVE_TAG_ROAD },
    { "predecessor",      NIImporter_OpenDrive::OPENDRIVE_TAG_PREDECESSOR },
    { "successor",        NIImporter_OpenDrive::OPENDRIVE_TAG_SUCCESSOR },
    { "geometry",         NIImporter_OpenDrive::OPENDRIVE_TAG_GEOMETRY },
    { "line",             NIImporter_OpenDrive::OPENDRIVE_TAG_LINE },
    { "spiral",           NIImporter_OpenDrive::OPENDRIVE_TAG_SPIRAL },
    { "arc",              NIImporter_OpenDrive::OPENDRIVE_TAG_ARC },
    { "poly3",            NIImporter_OpenDrive::OPENDRIVE_TAG_POLY3 },
    { "laneSection",      NIImporter_OpenDrive::OPENDRIVE_TAG_LANESECTION },
    { "left",             NIImporter_OpenDrive::OPENDRIVE_TAG_LEFT },
    { "center",           NIImporter_OpenDrive::OPENDRIVE_TAG_CENTER },
    { "right",            NIImporter_OpenDrive::OPENDRIVE_TAG_RIGHT },
    { "lane",             NIImporter_OpenDrive::OPENDRIVE_TAG_LANE },
    { "signal",           NIImporter_OpenDrive::OPENDRIVE_TAG_SIGNAL },
    { "junction",         NIImporter_OpenDrive::OPENDRIVE_TAG_JUNCTION },
    { "connection",       NIImporter_OpenDrive::OPENDRIVE_TAG_CONNECTION },
    { "laneLink",         NIImporter_OpenDrive::OPENDRIVE_TAG_LANELINK },
	{ "width",			  NIImporter_OpenDrive::OPENDRIVE_TAG_WIDTH },
	{ "speed",			  NIImporter_OpenDrive::OPENDRIVE_TAG_SPEED },

    { "",                 NIImporter_OpenDrive::OPENDRIVE_TAG_NOTHING }
};


StringBijection<int>::Entry NIImporter_OpenDrive::openDriveAttrs[] = {
    { "revMajor",       NIImporter_OpenDrive::OPENDRIVE_ATTR_REVMAJOR },
    { "revMinor",       NIImporter_OpenDrive::OPENDRIVE_ATTR_REVMINOR },
    { "id",             NIImporter_OpenDrive::OPENDRIVE_ATTR_ID },
    { "length",         NIImporter_OpenDrive::OPENDRIVE_ATTR_LENGTH },
    { "junction",       NIImporter_OpenDrive::OPENDRIVE_ATTR_JUNCTION },
    { "elementType",    NIImporter_OpenDrive::OPENDRIVE_ATTR_ELEMENTTYPE },
    { "elementId",      NIImporter_OpenDrive::OPENDRIVE_ATTR_ELEMENTID },
    { "contactPoint",   NIImporter_OpenDrive::OPENDRIVE_ATTR_CONTACTPOINT },
    { "s",              NIImporter_OpenDrive::OPENDRIVE_ATTR_S },
    { "x",              NIImporter_OpenDrive::OPENDRIVE_ATTR_X },
    { "y",              NIImporter_OpenDrive::OPENDRIVE_ATTR_Y },
    { "hdg",            NIImporter_OpenDrive::OPENDRIVE_ATTR_HDG },
    { "curvStart",      NIImporter_OpenDrive::OPENDRIVE_ATTR_CURVSTART },
    { "curvEnd",        NIImporter_OpenDrive::OPENDRIVE_ATTR_CURVEND },
    { "curvature",      NIImporter_OpenDrive::OPENDRIVE_ATTR_CURVATURE },
    { "a",              NIImporter_OpenDrive::OPENDRIVE_ATTR_A },
    { "b",              NIImporter_OpenDrive::OPENDRIVE_ATTR_B },
    { "c",              NIImporter_OpenDrive::OPENDRIVE_ATTR_C },
    { "d",              NIImporter_OpenDrive::OPENDRIVE_ATTR_D },
    { "type",           NIImporter_OpenDrive::OPENDRIVE_ATTR_TYPE },
    { "level",          NIImporter_OpenDrive::OPENDRIVE_ATTR_LEVEL },
    { "orientation",    NIImporter_OpenDrive::OPENDRIVE_ATTR_ORIENTATION },
    { "dynamic",        NIImporter_OpenDrive::OPENDRIVE_ATTR_DYNAMIC },
    { "incomingRoad",   NIImporter_OpenDrive::OPENDRIVE_ATTR_INCOMINGROAD },
    { "connectingRoad", NIImporter_OpenDrive::OPENDRIVE_ATTR_CONNECTINGROAD },
    { "from",			NIImporter_OpenDrive::OPENDRIVE_ATTR_FROM },
    { "to",  			NIImporter_OpenDrive::OPENDRIVE_ATTR_TO },
	{ "max",            NIImporter_OpenDrive::OPENDRIVE_ATTR_MAX },
    { "sOffset",        NIImporter_OpenDrive::OPENDRIVE_ATTR_SOFFSET },
    { "name",           NIImporter_OpenDrive::OPENDRIVE_ATTR_NAME },

    { "",               NIImporter_OpenDrive::OPENDRIVE_ATTR_NOTHING }
};


bool NIImporter_OpenDrive::myImportAllTypes;
bool NIImporter_OpenDrive::myImportWidths;

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_OpenDrive::loadNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether the option is set (properly)
    if (!oc.isUsableFileList("opendrive-files")) {
        return;
    }
	// prepare types
    myImportAllTypes = oc.getBool("opendrive.import-all-lanes");
    myImportWidths = !oc.getBool("opendrive.ignore-widths");
    NBTypeCont& tc = nb.getTypeCont();
    const SUMOReal WIDTH(3.65); // as wanted
    tc.insert("driving", 1, (SUMOReal)(80. / 3.6), 1, ~SVC_PEDESTRIAN, WIDTH, true);
    tc.insert("mwyEntry", 1, (SUMOReal)(80. / 3.6), 1, ~SVC_PEDESTRIAN, WIDTH, true);
    tc.insert("mwyExit", 1, (SUMOReal)(80. / 3.6), 1, ~SVC_PEDESTRIAN, WIDTH, true);
    tc.insert("stop", 1, (SUMOReal)(80. / 3.6), 1, ~SVC_PEDESTRIAN, WIDTH, true);
    tc.insert("special1", 1, (SUMOReal)(80. / 3.6), 1, ~SVC_PEDESTRIAN, WIDTH, true);
    tc.insert("parking", 1, (SUMOReal)(5. / 3.6), 1, ~SVC_PEDESTRIAN, WIDTH, true);
    // build the handler
    std::map<std::string, OpenDriveEdge*> edges;
	NIImporter_OpenDrive handler(tc, edges);
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("opendrive-files");
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        if (!FileHelpers::exists(*file)) {
            WRITE_ERROR("Could not open opendrive file '" + *file + "'.");
            return;
        }
        handler.setFileName(*file);
        PROGRESS_BEGIN_MESSAGE("Parsing opendrive from '" + *file + "'");
        XMLSubSys::runParser(handler, *file);
        PROGRESS_DONE_MESSAGE();
    }
    // split inner/outer edges
    std::map<std::string, OpenDriveEdge*> innerEdges, outerEdges;
    for (std::map<std::string, OpenDriveEdge*>::iterator i = edges.begin(); i != edges.end(); ++i) {
        if ((*i).second->isInner) {
            innerEdges[(*i).first] = (*i).second;
        } else {
            outerEdges[(*i).first] = (*i).second;
        }
    }

    // convert geometries into a discretised representation
    computeShapes(edges);
    // check whether lane sections are valid and whether further must be introduced
    revisitLaneSections(tc, edges);

    // -------------------------
    // node building
    // -------------------------
    // build nodes#1
    //  look at all links which belong to a node, collect their bounding boxes
    //  and place the node in the middle of this bounding box
    std::map<std::string, Boundary> posMap;
    std::map<std::string, std::string> edge2junction;
    //   compute node positions
    for (std::map<std::string, OpenDriveEdge*>::iterator i = innerEdges.begin(); i != innerEdges.end(); ++i) {
        OpenDriveEdge* e = (*i).second;
        assert(e->junction != "-1" && e->junction != "");
        edge2junction[e->id] = e->junction;
        if (posMap.find(e->junction) == posMap.end()) {
            posMap[e->junction] = Boundary();
        }
        posMap[e->junction].add(e->geom.getBoxBoundary());
    }
    //   build nodes
    for (std::map<std::string, Boundary>::iterator i = posMap.begin(); i != posMap.end(); ++i) {
        if (!nb.getNodeCont().insert((*i).first, (*i).second.getCenter())) {
            throw ProcessError("Could not add node '" + (*i).first + "'.");
        }
    }
    //  assign built nodes
    for (std::map<std::string, OpenDriveEdge*>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge* e = (*i).second;
        for (std::vector<OpenDriveLink>::iterator j = e->links.begin(); j != e->links.end(); ++j) {
            OpenDriveLink& l = *j;
            if (l.elementType != OPENDRIVE_ET_ROAD) {
                // set node information
                setNodeSecure(nb.getNodeCont(), *e, l.elementID, l.linkType);
                continue;
            }
            if (edge2junction.find(l.elementID) != edge2junction.end()) {
                // set node information of an internal road
                setNodeSecure(nb.getNodeCont(), *e, edge2junction[l.elementID], l.linkType);
                continue;
            }
        }
    }
    //  we should now have all nodes set for links which are not outer edge-to-outer edge links


    // build nodes#2
    //  build nodes for all outer edge-to-outer edge connections
    for (std::map<std::string, OpenDriveEdge*>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge* e = (*i).second;
        for (std::vector<OpenDriveLink>::iterator j = e->links.begin(); j != e->links.end(); ++j) {
            OpenDriveLink& l = *j;
            if (l.elementType != OPENDRIVE_ET_ROAD || edge2junction.find(l.elementID) != edge2junction.end()) {
                // is a connection to an internal edge, or a node, skip
                continue;
            }
            // we have a direct connection between to external edges
            std::string id1 = e->id;
            std::string id2 = l.elementID;
            if (id1 < id2) {
                std::swap(id1, id2);
            }
            std::string nid = id1 + "." + id2;
            if (nb.getNodeCont().retrieve(nid) == 0) {
                // not yet seen, build
                Position pos = l.linkType == OPENDRIVE_LT_SUCCESSOR ? e->geom[-1] : e->geom[0];
                if (!nb.getNodeCont().insert(nid, pos)) {
                    throw ProcessError("Could not build node '" + nid + "'.");
                }
            }
            /* debug-stuff
            else {
                Position pos = l.linkType==OPENDRIVE_LT_SUCCESSOR ? e.geom[e.geom.size()-1] : e.geom[0];
                cout << nid << " " << pos << " " << nb.getNodeCont().retrieve(nid)->getPosition() << endl;
            }
            */
            setNodeSecure(nb.getNodeCont(), *e, nid, l.linkType);
        }
    }
    // we should now have start/end nodes for all outer edge-to-outer edge connections


    // build nodes#3
    //  assign further nodes generated from inner-edges
    //  these nodes have not been assigned earlier, because the connectiosn are referenced in inner-edges
    for (std::map<std::string, OpenDriveEdge*>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge* e = (*i).second;
        if (e->to != 0 && e->from != 0) {
            continue;
        }
        for (std::map<std::string, OpenDriveEdge*>::iterator j = innerEdges.begin(); j != innerEdges.end(); ++j) {
            OpenDriveEdge* ie = (*j).second;
            for (std::vector<OpenDriveLink>::iterator k = ie->links.begin(); k != ie->links.end(); ++k) {
                OpenDriveLink& il = *k;
                if (il.elementType != OPENDRIVE_ET_ROAD || il.elementID != e->id) {
                    // not conneted to the currently investigated outer edge
                    continue;
                }
                std::string nid = edge2junction[ie->id];
                if (il.contactPoint == OPENDRIVE_CP_START) {
                    setNodeSecure(nb.getNodeCont(), *e, nid, OPENDRIVE_LT_PREDECESSOR);
                } else {
                    setNodeSecure(nb.getNodeCont(), *e, nid, OPENDRIVE_LT_SUCCESSOR);
                }
            }
        }

    }

    // build start/end nodes which were not defined previously
    for (std::map<std::string, OpenDriveEdge*>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge* e = (*i).second;
        if (e->from == 0) {
            const std::string nid = e->id + ".begin";
            e->from = getOrBuildNode(nid, e->geom.front(), nb.getNodeCont());
        }
        if (e->to == 0) {
            const std::string nid = e->id + ".end";
            e->to = getOrBuildNode(nid, e->geom.back(), nb.getNodeCont());
        }
    }


    // -------------------------
    // edge building
    // -------------------------
	SUMOReal defaultSpeed = tc.getSpeed("");
    // build edges
    for (std::map<std::string, OpenDriveEdge*>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge* e = (*i).second;
		bool lanesBuilt = false;

        // go along the lane sections, build a node in between of each pair

        /// @todo: One could think of determining whether lane sections may be joined when being equal in SUMO's sense
        /// Their naming would have to be updated, too, also in TraCI

        /// @todo: probably, the lane offsets to the center are not right
        NBNode* sFrom = e->from;
        NBNode* sTo = e->to;
        int priorityR = e->getPriority(OPENDRIVE_TAG_RIGHT);
        int priorityL = e->getPriority(OPENDRIVE_TAG_LEFT);
        SUMOReal sB = 0;
        SUMOReal sE = e->length;
        SUMOReal cF = e->length / e->geom.length2D();
        NBEdge* prevRight = 0;
        NBEdge* prevLeft = 0;

        // starting at the same node as ending, and no lane sections?
        if (sFrom == sTo && e->laneSections.size() == 1) {
            // --> loop, split!
            OpenDriveLaneSection ls = e->laneSections[0];
            ls.s = e->length / 2.;
            e->laneSections.push_back(ls);
            WRITE_WARNING("Edge '" + e->id + "' has to be split as it connects same junctions.")
        }

        // build along lane sections
        for (std::vector<OpenDriveLaneSection>::iterator j = e->laneSections.begin(); j != e->laneSections.end(); ++j) {
            // add internal node if needed
            if (j == e->laneSections.end() - 1) {
                sTo = e->to;
                sE = e->length / cF;
            } else {
                SUMOReal nextS = (j + 1)->s;
                sTo = new NBNode(e->id + "." + toString(nextS), e->geom.positionAtOffset(nextS));
                if (!nb.getNodeCont().insert(sTo)) {
                    throw ProcessError("Could not add node '" + sTo->getID() + "'.");
                }
                sE = nextS / cF;
            }
            PositionVector geom = e->geom.getSubpart2D(sB, sE);
            std::string id = e->id;
            if (sFrom != e->from || sTo != e->to) {
                id = id + "." + toString((*j).s);
            } else if(e->laneSections.size()==1) {
                id = id + ".0.00";
            }

			// build lanes to right
			NBEdge *currRight = 0;
			if((*j).rightLaneNumber>0) {
				currRight = new NBEdge("-" + id, sFrom, sTo, "", defaultSpeed, (*j).rightLaneNumber, priorityR,
					NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET, geom, "", LANESPREAD_RIGHT, true);
			    if (!nb.getEdgeCont().insert(currRight)) {
				    throw ProcessError("Could not add edge '" + currRight->getID() + "'.");
				}
				lanesBuilt = true;
				const std::vector<OpenDriveLane> &lanes = (*j).lanesByDir[OPENDRIVE_TAG_RIGHT];
				for(std::vector<OpenDriveLane>::const_iterator k=lanes.begin(); k!=lanes.end(); ++k) {
					std::map<int, int>::const_iterator lp = (*j).laneMap.find((*k).id);
					if(lp!=(*j).laneMap.end()) {
						int sumoLaneIndex = lp->second;
						NBEdge::Lane &sumoLane = currRight->getLaneStruct(sumoLaneIndex);
						const OpenDriveLane &odLane = *k;

						sumoLane.origID = e->id + " -" + toString((*k).id);
                        sumoLane.speed = odLane.speed!=0 ? odLane.speed : tc.getSpeed(odLane.type);
                        sumoLane.permissions = tc.getPermissions(odLane.type);
						sumoLane.width = myImportWidths&&odLane.width!=0 ? odLane.width : tc.getWidth(odLane.type);
					}
				}
				// connect lane sections
				if(prevRight!=0) {
					std::map<int, int> connections = (*j).getInnerConnections(OPENDRIVE_TAG_RIGHT, *(j-1));
					for(std::map<int, int>::const_iterator k=connections.begin(); k!=connections.end(); ++k) {
						prevRight->addLane2LaneConnection((*k).first, currRight, (*k).second, NBEdge::L2L_VALIDATED);
					}
				}
				prevRight = currRight;
			}

			// build lanes to left
			NBEdge *currLeft = 0;
			if((*j).leftLaneNumber>0) {
				currLeft = new NBEdge(id, sTo, sFrom, "", defaultSpeed, (*j).leftLaneNumber, priorityL,
					NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET, geom.reverse(), "", LANESPREAD_RIGHT, true);
				if (!nb.getEdgeCont().insert(currLeft)) {
					throw ProcessError("Could not add edge '" + currLeft->getID() + "'.");
				}
				lanesBuilt = true;
				const std::vector<OpenDriveLane> &lanes = (*j).lanesByDir[OPENDRIVE_TAG_LEFT];
				for(std::vector<OpenDriveLane>::const_iterator k=lanes.begin(); k!=lanes.end(); ++k) {
					std::map<int, int>::const_iterator lp = (*j).laneMap.find((*k).id);
					if(lp!=(*j).laneMap.end()) {
						int sumoLaneIndex = lp->second;
						NBEdge::Lane &sumoLane = currLeft->getLaneStruct(sumoLaneIndex);
						const OpenDriveLane &odLane = *k;

						sumoLane.origID = e->id + " " + toString((*k).id);
                        sumoLane.speed = odLane.speed!=0 ? odLane.speed : tc.getSpeed(odLane.type);
                        sumoLane.permissions = tc.getPermissions(odLane.type);
						sumoLane.width = myImportWidths&&odLane.width!=0 ? odLane.width : tc.getWidth(odLane.type);
					}
				}
				// connect lane sections
				if(prevLeft!=0) {
					std::map<int, int> connections = (*j).getInnerConnections(OPENDRIVE_TAG_LEFT, *(j-1));
					for(std::map<int, int>::const_iterator k=connections.begin(); k!=connections.end(); ++k) {
						currLeft->addLane2LaneConnection((*k).first, prevLeft, (*k).second, NBEdge::L2L_VALIDATED);
					}
				}
				prevLeft = currLeft;
			}
			(*j).sumoID = id;


			sB = sE;
			sFrom = sTo;
		}
		if(!lanesBuilt) {
            WRITE_WARNING("Edge '" + e->id + "' has no lanes.");
        }
    }

    // -------------------------
    // connections building
    // -------------------------
    // generate explicit lane-to-lane connections
    for (std::map<std::string, OpenDriveEdge*>::iterator i = edges.begin(); i != edges.end(); ++i) {
        setEdgeLinks2(*(*i).second, edges);
    }
    // compute connections across intersections, if any
    std::vector<Connection> connections2;
    for (std::map<std::string, OpenDriveEdge*>::iterator j = edges.begin(); j != edges.end(); ++j) {
        const std::set<Connection>& conns = (*j).second->connections;

        for (std::set<Connection>::const_iterator i = conns.begin(); i != conns.end(); ++i) {
            if (innerEdges.find((*i).fromEdge) != innerEdges.end()) {
                // connections starting at inner edges are processed by starting from outer edges
                continue;
            }
            if (innerEdges.find((*i).toEdge) != innerEdges.end()) {
                buildConnectionsToOuter(*i, innerEdges, connections2);
            } else {
                connections2.push_back(*i);
            }
        }
    }
    // set connections
    for (std::vector<Connection>::const_iterator i = connections2.begin(); i != connections2.end(); ++i) {
        std::string fromEdge = (*i).fromEdge;
        if (edges.find(fromEdge) == edges.end()) {
            WRITE_WARNING("While setting connections: from-edge '" + fromEdge + "' is not known.");
            continue;
        }
		OpenDriveEdge *odFrom = edges[fromEdge];
		int fromLane = (*i).fromLane;
		bool fromLast = ((*i).fromCP==OPENDRIVE_CP_END) ^ ((*i).fromLane>0&&!(*i).all);
		fromEdge = fromLast ? odFrom->laneSections.back().sumoID : odFrom->laneSections[0].sumoID;

		std::string toEdge = (*i).toEdge;
        if(edges.find(toEdge)==edges.end()) {
            WRITE_WARNING("While setting connections: to-edge '" + toEdge + "' is not known.");
            continue;
        }

		OpenDriveEdge *odTo = edges[toEdge];
		int toLane = (*i).toLane;
		bool toLast = ((*i).toCP==OPENDRIVE_CP_END) || ((*i).toLane>0);
		toEdge = toLast ? odTo->laneSections.back().sumoID : odTo->laneSections[0].sumoID;

		if(fromLane==UNSET_CONNECTION) {
			fromLane = toLast ?  odTo->laneSections.back().laneMap.begin()->first : odTo->laneSections[0].laneMap.begin()->first;
		}
		if(fromLane<0) {
			fromEdge = revertID(fromEdge);
		} 
		if(toLane==UNSET_CONNECTION) {
			toLane = toLast ?  odTo->laneSections.back().laneMap.begin()->first : odTo->laneSections[0].laneMap.begin()->first;
		}
		if(toLane<0) {
			toEdge = revertID(toEdge);
		} 
		fromLane = fromLast ? odFrom->laneSections.back().laneMap[fromLane] : odFrom->laneSections[0].laneMap[fromLane];
		toLane = toLast ?  odTo->laneSections.back().laneMap[toLane] : odTo->laneSections[0].laneMap[toLane];
		NBEdge *from = nb.getEdgeCont().retrieve(fromEdge);
		NBEdge *to = nb.getEdgeCont().retrieve(toEdge);
		if(from==0) { 
            WRITE_WARNING("Could not find fromEdge representation of '" + fromEdge + "' in connection '" + (*i).origID + "'.");
		}
		if(to==0) {
            WRITE_WARNING("Could not find fromEdge representation of '" + toEdge + "' in connection '" + (*i).origID + "'.");
		}
		if(from==0 || to==0) {
			continue;
		}

		from->addLane2LaneConnection(fromLane, to, toLane, NBEdge::L2L_USER);

        if ((*i).origID != "") {
			// @todo: this is the most silly way to determine the connection
			std::vector<NBEdge::Connection>& cons = from->getConnections();
            for (std::vector<NBEdge::Connection>::iterator k = cons.begin(); k != cons.end(); ++k) {
				if ((*k).fromLane == fromLane && (*k).toEdge == to && (*k).toLane == toLane) {
					(*k).origID = (*i).origID + " " + toString((*i).origLane);
                    break;
				}
			}
		}
	}


    // -------------------------
    // traffic lights
    // -------------------------
    std::map<std::string, std::string> tlsControlled;
    for (std::map<std::string, OpenDriveEdge*>::iterator i = edges.begin(); i != edges.end(); ++i) {
        OpenDriveEdge *e = (*i).second;
        for (std::vector<OpenDriveSignal>::const_iterator j = e->signals.begin(); j != e->signals.end(); ++j) {
            if ((*j).type != "1000001") {
                continue;
            }
            std::vector<OpenDriveLaneSection>::iterator k = e->laneSections.begin();
            bool found = false;
            for(; k!=e->laneSections.end()-1 && !found;) {
                if((*j).s>(*k).s && (*j).s<=(*(k+1)).s) {
                    found = true;
                } else {
                    ++k;
                }
            }

            // @todo: major problem, currently, still not completely solved:
            //  inner edges may have traffic lights, too. Nice on one hand, as directions can be recognized
            //  but hard to follow backwards
            std::string id = (*k).sumoID;
            if(id=="") {
                if(e->junction!="") {
                    //WRITE_WARNING("Found a traffic light signal on an internal edge; will not build it (original edge id='" + e->id + "').");
                    std::string fromID, toID;
                    for(std::vector<OpenDriveLink>::const_iterator l=e->links.begin(); l!=e->links.end(); ++l) {
                        if((*l).linkType==OPENDRIVE_LT_PREDECESSOR && (*l).elementType==OPENDRIVE_ET_ROAD) {
                            if(fromID!="") {
                                WRITE_WARNING("Ambigous start of connection.");
                            }
                            fromID = (*l).elementID;
                            OpenDriveEdge *e = edges[fromID];
                            fromID = (*l).contactPoint==OPENDRIVE_CP_START ? e->laneSections[0].sumoID : e->laneSections.back().sumoID;
                        }
                        if((*l).linkType==OPENDRIVE_LT_SUCCESSOR && (*l).elementType==OPENDRIVE_ET_ROAD) {
                            if(toID!="") {
                                WRITE_WARNING("Ambigous end of connection.");
                            }
                            toID = (*l).elementID;
                            OpenDriveEdge *e = edges[toID];
                            toID = (*l).contactPoint==OPENDRIVE_CP_START ? e->laneSections[0].sumoID : e->laneSections.back().sumoID;
                        }
                    }
                    id = fromID + "->" + toID;
                } else {
                    WRITE_WARNING("Found a traffic light signal on an unknown edge (original edge id='" + e->id + "').");
                    continue;
                }
            }

            if ((*j).orientation > 0) {
                id = "-" + id;
            }
            tlsControlled[id] = (*j).name;
        }
    }

    for (std::map<std::string, std::string>::iterator i = tlsControlled.begin(); i != tlsControlled.end(); ++i) {
        std::string id = (*i).first;
        if(id.find("->")!=std::string::npos) {
            id = id.substr(0, id.find("->"));
        }
        NBEdge *e = nb.getEdgeCont().retrieve(id);
        if(e==0) {
            WRITE_WARNING("Could not find edge '" + id + "' while building its traffic light.");
            continue;
        }
        NBNode *toNode = e->getToNode();
        NBTrafficLightDefinition* tlDef = 0;
        if(!toNode->isTLControlled()) {
            TrafficLightType type = SUMOXMLDefinitions::TrafficLightTypes.get(OptionsCont::getOptions().getString("tls.default-type"));
            tlDef = new NBOwnTLDef(toNode->getID(), toNode, 0, type);
            if (!nb.getTLLogicCont().insert(tlDef)) {
                // actually, nothing should fail here
                delete tlDef;
                throw ProcessError();
            }
            toNode->addTrafficLight(tlDef);
            static_cast<NBOwnTLDef*>(tlDef)->setSinglePhase();
        }
        tlDef = *toNode->getControllingTLS().begin();
        tlDef->addParameter("connection:" + id, (*i).second);
    }

    // -------------------------
    // clean up
    // -------------------------
	if(oc.exists("geometry.min-dist")&&oc.isSet("geometry.min-dist")) {
		oc.unSet("geometry.min-dist");
	}
    for (std::map<std::string, OpenDriveEdge*>::iterator i = edges.begin(); i != edges.end(); ++i) {
		delete (*i).second;
    }
}



void
NIImporter_OpenDrive::buildConnectionsToOuter(const Connection& c, const std::map<std::string, OpenDriveEdge*>& innerEdges, std::vector<Connection>& into) {

    OpenDriveEdge* dest = innerEdges.find(c.toEdge)->second;
    if (dest == 0) {
        /// !!! should not, look in all?
        return;
    }
    const std::set<Connection>& conts = dest->connections;
    for (std::set<Connection>::const_iterator i = conts.begin(); i != conts.end(); ++i) {
        if (innerEdges.find((*i).toEdge) != innerEdges.end()) {
            std::vector<Connection> t;
            buildConnectionsToOuter(*i, innerEdges, t);
            for (std::vector<Connection>::const_iterator j = t.begin(); j != t.end(); ++j) {
                // @todo this section is unverified
                Connection cn = (*j);
                cn.fromEdge = c.fromEdge;
                cn.fromLane = c.fromLane;
                cn.fromCP = c.fromCP;
                cn.all = c.all; // @todo "all" is a hack trying to avoid the "from is zero" problem;
                into.push_back(cn);
            }
        } else {
            if ((*i).fromLane == c.toLane) {
                Connection cn = (*i);
                cn.fromEdge = c.fromEdge;
                cn.fromLane = c.fromLane;
                cn.fromCP = c.fromCP;
                cn.all = c.all;
                cn.origID = c.toEdge;
                cn.origLane = c.toLane;
                into.push_back(cn);
            }
        }
    }
}


void
NIImporter_OpenDrive::setEdgeLinks2(OpenDriveEdge& e, const std::map<std::string, OpenDriveEdge*>& edges) {
    for (std::vector<OpenDriveLink>::iterator i = e.links.begin(); i != e.links.end(); ++i) {
        OpenDriveLink& l = *i;
        if (l.elementType != OPENDRIVE_ET_ROAD) {
            // we assume that links to nodes are later given as connections to edges
            continue;
        }
        // get the right direction of the connected edge
        std::string connectedEdge = l.elementID;
        std::string edgeID = e.id;

        OpenDriveLaneSection& laneSection = l.linkType == OPENDRIVE_LT_SUCCESSOR ? e.laneSections.back() : e.laneSections[0];
		const std::map<int, int>& laneMap = laneSection.laneMap;
        if (laneSection.lanesByDir.find(OPENDRIVE_TAG_RIGHT) != laneSection.lanesByDir.end()) {
            const std::vector<OpenDriveLane>& lanes = laneSection.lanesByDir.find(OPENDRIVE_TAG_RIGHT)->second;
            for (std::vector<OpenDriveLane>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
                if (!myImportAllTypes && laneMap.find((*j).id) == laneMap.end()) {
                    continue;
                }
                Connection c; // @todo: give Connection a new name and a constructor
                c.fromEdge = e.id;
                c.fromLane = (*j).id;
                c.fromCP = OPENDRIVE_CP_END;
                c.toLane = l.linkType == OPENDRIVE_LT_SUCCESSOR ? (*j).successor : (*j).predecessor;
                c.toEdge = connectedEdge;
                c.toCP = l.contactPoint;
                c.all = false;
                if (l.linkType != OPENDRIVE_LT_SUCCESSOR) {
                    std::swap(c.fromEdge, c.toEdge);
                    std::swap(c.fromLane, c.toLane);
                    std::swap(c.fromCP, c.toCP);
                }
                if (edges.find(c.fromEdge) == edges.end()) {
                    WRITE_ERROR("While setting connections: incoming road '" + c.fromEdge + "' is not known.");
                } else {
                    OpenDriveEdge* src = edges.find(c.fromEdge)->second;
                    src->connections.insert(c);
                }
            }
        }
        if (laneSection.lanesByDir.find(OPENDRIVE_TAG_LEFT) != laneSection.lanesByDir.end()) {
            const std::vector<OpenDriveLane>& lanes = laneSection.lanesByDir.find(OPENDRIVE_TAG_LEFT)->second;
            for (std::vector<OpenDriveLane>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
                if (!myImportAllTypes && laneMap.find((*j).id) == laneMap.end()) {
                    continue;
                }
                Connection c;
                c.toEdge = e.id;
                c.toLane = (*j).id;
                c.toCP = OPENDRIVE_CP_END;
                c.fromLane = l.linkType == OPENDRIVE_LT_SUCCESSOR ? (*j).successor : (*j).predecessor;
                c.fromEdge = connectedEdge;
                c.fromCP = l.contactPoint;
                c.all = false;
                if (l.linkType != OPENDRIVE_LT_SUCCESSOR) {
                    std::swap(c.fromEdge, c.toEdge);
                    std::swap(c.fromLane, c.toLane);
                    std::swap(c.fromCP, c.toCP);
                }
                if (edges.find(c.fromEdge) == edges.end()) {
                    WRITE_ERROR("While setting connections: incoming road '" + c.fromEdge + "' is not known.");
                } else {
                    OpenDriveEdge* src = edges.find(c.fromEdge)->second;
                    src->connections.insert(c);
                }
            }
        }
    }
}


std::string NIImporter_OpenDrive::revertID(const std::string& id) {
    if (id[0] == '-') {
        return id.substr(1);
    }
    return "-" + id;
}

NBNode*
NIImporter_OpenDrive::getOrBuildNode(const std::string& id, const Position& pos,
                                     NBNodeCont& nc) {
    if (nc.retrieve(id) == 0) {
        // not yet built; build now
        if (!nc.insert(id, pos)) {
            // !!! clean up
            throw ProcessError("Could not add node '" + id + "'.");
        }
    }
    return nc.retrieve(id);
}


void
NIImporter_OpenDrive::setNodeSecure(NBNodeCont& nc, OpenDriveEdge& e,
                                    const std::string& nodeID, NIImporter_OpenDrive::LinkType lt) {
    NBNode* n = nc.retrieve(nodeID);
    if (n == 0) {
        throw ProcessError("Could not find node '" + nodeID + "'.");
    }
    if (lt == OPENDRIVE_LT_SUCCESSOR) {
        if (e.to != 0 && e.to != n) {
            throw ProcessError("Edge '" + e.id + "' has two end nodes.");
        }
        e.to = n;
    } else {
        if (e.from != 0 && e.from != n) {
            throw ProcessError("Edge '" + e.id + "' has two start nodes.");
        }
        e.from = n;
    }
}







void
NIImporter_OpenDrive::computeShapes(std::map<std::string, OpenDriveEdge*>& edges) {
    OptionsCont& oc = OptionsCont::getOptions();
    for (std::map<std::string, OpenDriveEdge*>::iterator i = edges.begin(); i != edges.end(); ++i) {
        OpenDriveEdge& e = *(*i).second;
        for (std::vector<OpenDriveGeometry>::iterator j = e.geometries.begin(); j != e.geometries.end(); ++j) {
            OpenDriveGeometry& g = *j;
            std::vector<Position> geom;
            switch (g.type) {
                case OPENDRIVE_GT_UNKNOWN:
                    break;
                case OPENDRIVE_GT_LINE:
                    geom = geomFromLine(e, g);
                    break;
                case OPENDRIVE_GT_SPIRAL:
                    geom = geomFromSpiral(e, g);
                    break;
                case OPENDRIVE_GT_ARC:
                    geom = geomFromArc(e, g);
                    break;
                case OPENDRIVE_GT_POLY3:
                    geom = geomFromPoly(e, g);
                    break;
                default:
                    break;
            }
            for (std::vector<Position>::iterator k = geom.begin(); k != geom.end(); ++k) {
                e.geom.push_back_noDoublePos(*k);
            }
        }
        if (oc.exists("geometry.min-dist") && oc.isSet("geometry.min-dist")) {
            e.geom.removeDoublePoints(oc.getFloat("geometry.min-dist"), true);
        }
        for (unsigned int j = 0; j < e.geom.size(); ++j) {
            if (!NBNetBuilder::transformCoordinates(e.geom[j])) {
                WRITE_ERROR("Unable to project coordinates for.");
            }
        }
    }
}


void
NIImporter_OpenDrive::revisitLaneSections(const NBTypeCont &tc, std::map<std::string, OpenDriveEdge*>& edges) {
    for (std::map<std::string, OpenDriveEdge*>::iterator i = edges.begin(); i != edges.end(); ++i) {
        OpenDriveEdge& e = *(*i).second;
        std::vector<OpenDriveLaneSection> &laneSections = e.laneSections;
        // split by speed limits
        std::vector<OpenDriveLaneSection> newSections;
        for(std::vector<OpenDriveLaneSection>::iterator j=laneSections.begin(); j!=laneSections.end(); ++j) {
            std::vector<OpenDriveLaneSection> splitSections;
            bool splitBySpeed = (*j).buildSpeedChanges(tc, splitSections);
            if(!splitBySpeed) {
                newSections.push_back(*j);
            } else {
                std::copy(splitSections.begin(), splitSections.end(), back_inserter(newSections));
            }
        }

        e.laneSections = newSections;
        laneSections = e.laneSections;
        SUMOReal lastS = -1;
        // check whether the lane sections are in the right order
        bool sorted = true;
        for(std::vector<OpenDriveLaneSection>::const_iterator j=laneSections.begin(); j!=laneSections.end() && sorted; ++j) {
            if((*j).s<=lastS) {
                sorted = false;
            }
            lastS = (*j).s;
        }
        if(!sorted) {
            WRITE_WARNING("The sections of edge '" + e.id + "' are not sorted properly.");
            sort(e.laneSections.begin(), e.laneSections.end(), sections_by_s_sorter());
        }
        // check whether no duplicates of s-value occure
        lastS = -1;
        laneSections = e.laneSections;
        for(std::vector<OpenDriveLaneSection>::iterator j=laneSections.begin(); j!=laneSections.end();) {
            bool simlarToLast = fabs((*j).s-lastS)<POSITION_EPS;
            lastS = (*j).s;
            if(simlarToLast) {
                WRITE_WARNING("Almost duplicate s-value '" + toString(lastS) + "' for lane sections occured at edge '" + e.id + "'; second entry was removed.");
                j = laneSections.erase(j);
            } else {
                ++j;
            }
        }
    }
}


std::vector<Position>
NIImporter_OpenDrive::geomFromLine(const OpenDriveEdge& e, const OpenDriveGeometry& g) {
    UNUSED_PARAMETER(e);
    std::vector<Position> ret;
    ret.push_back(Position(g.x, g.y));
    ret.push_back(calculateStraightEndPoint(g.hdg, g.length, Position(g.x, g.y)));
    return ret;
}


std::vector<Position>
NIImporter_OpenDrive::geomFromSpiral(const OpenDriveEdge& e, const OpenDriveGeometry& g) {
    UNUSED_PARAMETER(e);
    std::vector<Position> ret;
    SUMOReal curveStart = g.params[0];
    SUMOReal curveEnd = g.params[1];
    Point2D<double> end;
    EulerSpiral s(Point2D<double>(g.x, g.y), g.hdg, curveStart, (curveEnd - curveStart) / g.length, g.length);
    std::vector<Point2D<double> > into;
    s.computeSpiral(into, 1.);
    for (std::vector<Point2D<double> >::iterator i = into.begin(); i != into.end(); ++i) {
        ret.push_back(Position((*i).getX(), (*i).getY()));
    }
    return ret;
}


std::vector<Position>
NIImporter_OpenDrive::geomFromArc(const OpenDriveEdge& e, const OpenDriveGeometry& g) {
    UNUSED_PARAMETER(e);
    std::vector<Position> ret;
    SUMOReal dist = 0.0;
    SUMOReal centerX = g.x;
    SUMOReal centerY = g.y;
    // left: positive value
    SUMOReal curvature = g.params[0];
    SUMOReal radius = 1. / curvature;
    // center point
    calculateCurveCenter(&centerX, &centerY, radius, g.hdg);
    SUMOReal endX = g.x;
    SUMOReal endY = g.y;
    SUMOReal startX = g.x;
    SUMOReal startY = g.y;
    SUMOReal geo_posS = g.s;
    SUMOReal geo_posE = g.s;
    bool end = false;
    do {
        geo_posE += C_LENGTH;
        if (geo_posE - g.s > g.length) {
            geo_posE = g.s + g.length;
        }
        if (geo_posE - g.s > g.length) {
            geo_posE = g.s + g.length;
        }
        calcPointOnCurve(&endX, &endY, centerX, centerY, radius, geo_posE - geo_posS);

        dist += (geo_posE - geo_posS);
        //
        ret.push_back(Position(startX, startY));
        //
        startX = endX;
        startY = endY;
        geo_posS = geo_posE;

        if (geo_posE  - (g.s + g.length) < 0.001 && geo_posE  - (g.s + g.length) > -0.001) {
            end = true;
        }
    } while (!end);
    return ret;
}


std::vector<Position>
NIImporter_OpenDrive::geomFromPoly(const OpenDriveEdge& e, const OpenDriveGeometry& g) {
    UNUSED_PARAMETER(e);
    std::vector<Position> ret;
    for (SUMOReal off = 0; off < g.length + 2.; off += 2.) {
        SUMOReal x = off;
        SUMOReal y = g.params[0] + g.params[1] * off + g.params[2] * pow(off, 2.) + g.params[3] * pow(off, 3.);
        SUMOReal s = sin(g.hdg);
        SUMOReal c = cos(g.hdg);
        SUMOReal xnew = x * c - y * s;
        SUMOReal ynew = x * s + y * c;
        ret.push_back(Position(g.x + xnew, g.y + ynew));
    }
    return ret;
}


Position
NIImporter_OpenDrive::calculateStraightEndPoint(double hdg, double length, const Position& start) {
    double normx = 1.0f;
    double normy = 0.0f;
    double x2 = normx * cos(hdg) - normy * sin(hdg);
    double y2 = normx * sin(hdg) + normy * cos(hdg);
    normx = x2 * length;
    normy = y2 * length;
    return Position(start.x() + normx, start.y() + normy);
}


void
NIImporter_OpenDrive::calculateCurveCenter(SUMOReal* ad_x, SUMOReal* ad_y, SUMOReal ad_radius, SUMOReal ad_hdg) {
    SUMOReal normX = 1.0;
    SUMOReal normY = 0.0;
    SUMOReal tmpX;
    SUMOReal turn;
    if (ad_radius > 0) {
        turn = -1.0;
    } else {
        turn = 1.0;
    }

    tmpX = normX;
    normX = normX * cos(ad_hdg) + normY * sin(ad_hdg);
    normY = tmpX * sin(ad_hdg) + normY * cos(ad_hdg);

    tmpX = normX;
    normX = turn * normY;
    normY = -turn * tmpX;

    normX = fabs(ad_radius) * normX;
    normY = fabs(ad_radius) * normY;

    *ad_x += normX;
    *ad_y += normY;
}


void
NIImporter_OpenDrive::calcPointOnCurve(SUMOReal* ad_x, SUMOReal* ad_y, SUMOReal ad_centerX, SUMOReal ad_centerY,
                                       SUMOReal ad_r, SUMOReal ad_length) {
    double rotAngle = ad_length / fabs(ad_r);
    double vx = *ad_x - ad_centerX;
    double vy = *ad_y - ad_centerY;
    double tmpx;

    double turn;
    if (ad_r > 0) {
        turn = -1;    //left
    } else {
        turn = 1;    //right
    }
    tmpx = vx;
    vx = vx * cos(rotAngle) + turn * vy * sin(rotAngle);
    vy = -1 * turn * tmpx * sin(rotAngle) + vy * cos(rotAngle);
    *ad_x = vx + ad_centerX;
    *ad_y = vy + ad_centerY;
}


// ---------------------------------------------------------------------------
// section
// ---------------------------------------------------------------------------
NIImporter_OpenDrive::OpenDriveLaneSection::OpenDriveLaneSection(SUMOReal sArg) : s(sArg) {
    lanesByDir[OPENDRIVE_TAG_LEFT] = std::vector<OpenDriveLane>();
    lanesByDir[OPENDRIVE_TAG_RIGHT] = std::vector<OpenDriveLane>();
    lanesByDir[OPENDRIVE_TAG_CENTER] = std::vector<OpenDriveLane>();
}


void
NIImporter_OpenDrive::OpenDriveLaneSection::buildLaneMapping(const NBTypeCont &tc) {
    unsigned int sumoLane = 0;
    const std::vector<OpenDriveLane>& dirLanesR = lanesByDir.find(OPENDRIVE_TAG_RIGHT)->second;
    for (std::vector<OpenDriveLane>::const_reverse_iterator i = dirLanesR.rbegin(); i != dirLanesR.rend(); ++i) {
		if (myImportAllTypes || (tc.knows((*i).type) && !tc.getShallBeDiscarded((*i).type))) {
            laneMap[(*i).id] = sumoLane++;
        }
    }
	rightLaneNumber = sumoLane;
    sumoLane = 0;
    const std::vector<OpenDriveLane>& dirLanesL = lanesByDir.find(OPENDRIVE_TAG_LEFT)->second;
    for (std::vector<OpenDriveLane>::const_iterator i = dirLanesL.begin(); i != dirLanesL.end(); ++i) {
        if (myImportAllTypes || (tc.knows((*i).type) && !tc.getShallBeDiscarded((*i).type))) {
            laneMap[(*i).id] = sumoLane++;
        }
    }
	leftLaneNumber = sumoLane;
}


std::map<int, int>
NIImporter_OpenDrive::OpenDriveLaneSection::getInnerConnections(OpenDriveXMLTag dir, const OpenDriveLaneSection& prev) {
    std::map<int, int> ret;
    const std::vector<OpenDriveLane>& dirLanes = lanesByDir.find(dir)->second;
    for (std::vector<OpenDriveLane>::const_reverse_iterator i = dirLanes.rbegin(); i != dirLanes.rend(); ++i) {
        std::map<int, int>::const_iterator toP = laneMap.find((*i).id);
        if (toP == laneMap.end()) {
            // the current lane is not available in SUMO
            continue;
        }
        int to = (*toP).second;
        int from = UNSET_CONNECTION;
        if ((*i).predecessor != UNSET_CONNECTION) {
            from = (*i).predecessor;
        }
        if (from != UNSET_CONNECTION) {
            std::map<int, int>::const_iterator fromP = prev.laneMap.find(from);
            if (fromP != prev.laneMap.end()) {
                from = (*fromP).second;
            } else {
                from = UNSET_CONNECTION;
            }
        }
        if (from != UNSET_CONNECTION && to != UNSET_CONNECTION) {
            if (ret.find(from) != ret.end()) {
//				WRITE_WARNING("double connection");
            }
            if (dir == OPENDRIVE_TAG_LEFT) {
                std::swap(from, to);
            }
            ret[from] = to;
        } else {
//			WRITE_WARNING("missing connection");
        }
    }
    return ret;
}


NIImporter_OpenDrive::OpenDriveLaneSection
NIImporter_OpenDrive::OpenDriveLaneSection::buildLaneSection(SUMOReal startPos) {
    OpenDriveLaneSection ret(*this);
    ret.s += startPos;
    for(unsigned int k=0; k!=ret.lanesByDir[OPENDRIVE_TAG_RIGHT].size(); ++k) {
        OpenDriveLane &l = ret.lanesByDir[OPENDRIVE_TAG_RIGHT][k];
        l.speed = 0;
        std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator i = std::find_if(l.speeds.begin(), l.speeds.end(), same_position_finder(startPos));
        if(i!=l.speeds.end()) {
            l.speed = (*i).second;
        }
    }
    for(unsigned int k=0; k!=ret.lanesByDir[OPENDRIVE_TAG_LEFT].size(); ++k) {
        OpenDriveLane &l = ret.lanesByDir[OPENDRIVE_TAG_LEFT][k];
        std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator i = std::find_if(l.speeds.begin(), l.speeds.end(), same_position_finder(startPos));
        l.speed = 0;
        if(i!=l.speeds.end()) {
            l.speed = (*i).second;
        }
    }
    return ret;
}


bool
NIImporter_OpenDrive::OpenDriveLaneSection::buildSpeedChanges(const NBTypeCont &tc, std::vector<OpenDriveLaneSection> &newSections) {
    std::set<SUMOReal> speedChangePositions;
    // collect speed change positions and apply initial speed to the begin
    for(std::vector<OpenDriveLane>::iterator k=lanesByDir[OPENDRIVE_TAG_RIGHT].begin(); k!=lanesByDir[OPENDRIVE_TAG_RIGHT].end(); ++k) {
        for(std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator l=(*k).speeds.begin(); l!=(*k).speeds.end(); ++l) {
            speedChangePositions.insert((*l).first);
            if((*l).first==0) {
                (*k).speed = (*l).second;
            }
        }
    }
    for(std::vector<OpenDriveLane>::iterator k=lanesByDir[OPENDRIVE_TAG_LEFT].begin(); k!=lanesByDir[OPENDRIVE_TAG_LEFT].end(); ++k) {
        for(std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator l=(*k).speeds.begin(); l!=(*k).speeds.end(); ++l) {
            speedChangePositions.insert((*l).first);
            if((*l).first==0) {
                (*k).speed = (*l).second;
            }
        }
    }
    // do nothing if there is none
    if(speedChangePositions.size()==0) {
        return false;
    }
    if(*speedChangePositions.begin()>0) {
        speedChangePositions.insert(0);
    }
    //
    for(std::set<SUMOReal>::iterator i=speedChangePositions.begin(); i!=speedChangePositions.end(); ++i) {
        if(i==speedChangePositions.begin()) {
            newSections.push_back(*this);
        } else {
            newSections.push_back(buildLaneSection(*i));
        }
    }
    // propagate speeds
    for(int i = 0; i != (int)newSections.size(); ++i) {
        OpenDriveLaneSection &ls = newSections[i];
        std::map<OpenDriveXMLTag, std::vector<OpenDriveLane> > &lanesByDir = ls.lanesByDir;
        for(std::map<OpenDriveXMLTag, std::vector<OpenDriveLane> >::iterator k=lanesByDir.begin(); k!=lanesByDir.end(); ++k) {
            std::vector<OpenDriveLane> &lanes = (*k).second;
            for(int j = 0; j != (int)lanes.size(); ++j) {
                OpenDriveLane &l = lanes[j];
                if(l.speed!=0) {
                    continue;
                }
                if(i>0) {
                    l.speed = newSections[i-1].lanesByDir[(*k).first][j].speed;
                } else {
                    tc.getSpeed(l.type);
                }
            }
        }
    }
    return true;
}



// ---------------------------------------------------------------------------
// edge
// ---------------------------------------------------------------------------
int
NIImporter_OpenDrive::OpenDriveEdge::getPriority(OpenDriveXMLTag dir) const {
    int prio = 1;
    for (std::vector<OpenDriveSignal>::const_iterator i = signals.begin(); i != signals.end(); ++i) {
        int tmp = 1;
        if ((*i).type == "301" || (*i).type == "306") {
            tmp = 2;
        }
        if ((*i).type == "205") {
            tmp = 0;
        }
        if (tmp != 1 && dir == OPENDRIVE_TAG_RIGHT && (*i).orientation > 0) {
            prio = tmp;
        }
        if (tmp != 1 && dir == OPENDRIVE_TAG_LEFT && (*i).orientation < 0) {
            prio = tmp;
        }

    }
    return prio;
}



// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_OpenDrive::NIImporter_OpenDrive(const NBTypeCont &tc, std::map<std::string, OpenDriveEdge*>& edges)
    : GenericSAXHandler(openDriveTags, OPENDRIVE_TAG_NOTHING, openDriveAttrs, OPENDRIVE_ATTR_NOTHING, "opendrive"),
      myTypeContainer(tc), myCurrentEdge("", "", -1), myEdges(edges) {
}


NIImporter_OpenDrive::~NIImporter_OpenDrive() {
}


void
NIImporter_OpenDrive::myStartElement(int element,
                                     const SUMOSAXAttributes& attrs) {
    bool ok = true;
    switch (element) {
        case OPENDRIVE_TAG_HEADER: {
            int majorVersion = attrs.get<int>(OPENDRIVE_ATTR_REVMAJOR, 0, ok);
            int minorVersion = attrs.get<int>(OPENDRIVE_ATTR_REVMINOR, 0, ok);
            if (majorVersion != 1 || minorVersion != 2) {
                WRITE_WARNING("Given openDrive file '" + getFileName() + "' uses version " + toString(majorVersion) + "." + toString(minorVersion) + ";\n Version 1.2 is supported.");
            }
        }
        break;
        case OPENDRIVE_TAG_ROAD: {
            std::string id = attrs.get<std::string>(OPENDRIVE_ATTR_ID, 0, ok);
            std::string junction = attrs.get<std::string>(OPENDRIVE_ATTR_JUNCTION, id.c_str(), ok);
            SUMOReal length = attrs.get<SUMOReal>(OPENDRIVE_ATTR_LENGTH, id.c_str(), ok);
            myCurrentEdge = OpenDriveEdge(id, junction, length);
        }
        break;
        case OPENDRIVE_TAG_PREDECESSOR: {
            if (myElementStack.size() >= 2 && myElementStack[myElementStack.size() - 2] == OPENDRIVE_TAG_ROAD) {
                std::string elementType = attrs.get<std::string>(OPENDRIVE_ATTR_ELEMENTTYPE, myCurrentEdge.id.c_str(), ok);
                std::string elementID = attrs.get<std::string>(OPENDRIVE_ATTR_ELEMENTID, myCurrentEdge.id.c_str(), ok);
                std::string contactPoint = attrs.hasAttribute(OPENDRIVE_ATTR_CONTACTPOINT)
                                           ? attrs.get<std::string>(OPENDRIVE_ATTR_CONTACTPOINT, myCurrentEdge.id.c_str(), ok)
                                           : "end";
                addLink(OPENDRIVE_LT_PREDECESSOR, elementType, elementID, contactPoint);
            }
            if (myElementStack.size() >= 2 && myElementStack[myElementStack.size() - 2] == OPENDRIVE_TAG_LANE) {
                int no = attrs.get<int>(OPENDRIVE_ATTR_ID, myCurrentEdge.id.c_str(), ok);
                OpenDriveLane& l = myCurrentEdge.laneSections.back().lanesByDir[myCurrentLaneDirection].back();
                l.predecessor = no;
            }
        }
        break;
        case OPENDRIVE_TAG_SUCCESSOR: {
            if (myElementStack.size() >= 2 && myElementStack[myElementStack.size() - 2] == OPENDRIVE_TAG_ROAD) {
                std::string elementType = attrs.get<std::string>(OPENDRIVE_ATTR_ELEMENTTYPE, myCurrentEdge.id.c_str(), ok);
                std::string elementID = attrs.get<std::string>(OPENDRIVE_ATTR_ELEMENTID, myCurrentEdge.id.c_str(), ok);
                std::string contactPoint = attrs.hasAttribute(OPENDRIVE_ATTR_CONTACTPOINT)
                                           ? attrs.get<std::string>(OPENDRIVE_ATTR_CONTACTPOINT, myCurrentEdge.id.c_str(), ok)
                                           : "start";
                addLink(OPENDRIVE_LT_SUCCESSOR, elementType, elementID, contactPoint);
            }
            if (myElementStack.size() >= 2 && myElementStack[myElementStack.size() - 2] == OPENDRIVE_TAG_LANE) {
                int no = attrs.get<int>(OPENDRIVE_ATTR_ID, myCurrentEdge.id.c_str(), ok);
                OpenDriveLane& l = myCurrentEdge.laneSections.back().lanesByDir[myCurrentLaneDirection].back();
                l.successor = no;
            }
        }
        break;
        case OPENDRIVE_TAG_GEOMETRY: {
            SUMOReal length = attrs.get<SUMOReal>(OPENDRIVE_ATTR_LENGTH, myCurrentEdge.id.c_str(), ok);
            SUMOReal s = attrs.get<SUMOReal>(OPENDRIVE_ATTR_S, myCurrentEdge.id.c_str(), ok);
            SUMOReal x = attrs.get<SUMOReal>(OPENDRIVE_ATTR_X, myCurrentEdge.id.c_str(), ok);
            SUMOReal y = attrs.get<SUMOReal>(OPENDRIVE_ATTR_Y, myCurrentEdge.id.c_str(), ok);
            SUMOReal hdg = attrs.get<SUMOReal>(OPENDRIVE_ATTR_HDG, myCurrentEdge.id.c_str(), ok);
            myCurrentEdge.geometries.push_back(OpenDriveGeometry(length, s, x, y, hdg));
        }
        break;
        case OPENDRIVE_TAG_LINE: {
            std::vector<SUMOReal> vals;
            addGeometryShape(OPENDRIVE_GT_LINE, vals);
        }
        break;
        case OPENDRIVE_TAG_SPIRAL: {
            std::vector<SUMOReal> vals;
            vals.push_back(attrs.get<SUMOReal>(OPENDRIVE_ATTR_CURVSTART, myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.get<SUMOReal>(OPENDRIVE_ATTR_CURVEND, myCurrentEdge.id.c_str(), ok));
            addGeometryShape(OPENDRIVE_GT_SPIRAL, vals);
        }
        break;
        case OPENDRIVE_TAG_ARC: {
            std::vector<SUMOReal> vals;
            vals.push_back(attrs.get<SUMOReal>(OPENDRIVE_ATTR_CURVATURE, myCurrentEdge.id.c_str(), ok));
            addGeometryShape(OPENDRIVE_GT_ARC, vals);
        }
        break;
        case OPENDRIVE_TAG_POLY3: {
            std::vector<SUMOReal> vals;
            vals.push_back(attrs.get<SUMOReal>(OPENDRIVE_ATTR_A, myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.get<SUMOReal>(OPENDRIVE_ATTR_B, myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.get<SUMOReal>(OPENDRIVE_ATTR_C, myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.get<SUMOReal>(OPENDRIVE_ATTR_D, myCurrentEdge.id.c_str(), ok));
            addGeometryShape(OPENDRIVE_GT_POLY3, vals);
        }
        break;
        case OPENDRIVE_TAG_LANESECTION: {
            SUMOReal s = attrs.get<SUMOReal>(OPENDRIVE_ATTR_S, myCurrentEdge.id.c_str(), ok);
            myCurrentEdge.laneSections.push_back(OpenDriveLaneSection(s));
        }
        break;
        case OPENDRIVE_TAG_LEFT:
            myCurrentLaneDirection = OPENDRIVE_TAG_LEFT;
            break;
        case OPENDRIVE_TAG_CENTER:
            myCurrentLaneDirection = OPENDRIVE_TAG_CENTER;
            break;
        case OPENDRIVE_TAG_RIGHT:
            myCurrentLaneDirection = OPENDRIVE_TAG_RIGHT;
            break;
        case OPENDRIVE_TAG_LANE: {
            std::string type = attrs.get<std::string>(OPENDRIVE_ATTR_TYPE, myCurrentEdge.id.c_str(), ok);
            int id = attrs.get<int>(OPENDRIVE_ATTR_ID, myCurrentEdge.id.c_str(), ok);
            std::string level = attrs.hasAttribute(OPENDRIVE_ATTR_LEVEL)
                                ? attrs.get<std::string>(OPENDRIVE_ATTR_LEVEL, myCurrentEdge.id.c_str(), ok)
                                : "";
            OpenDriveLaneSection& ls = myCurrentEdge.laneSections.back();
            ls.lanesByDir[myCurrentLaneDirection].push_back(OpenDriveLane(id, level, type));
        }
        break;
        case OPENDRIVE_TAG_SIGNAL: {
            int id = attrs.get<int>(OPENDRIVE_ATTR_ID, myCurrentEdge.id.c_str(), ok);
            std::string type = attrs.get<std::string>(OPENDRIVE_ATTR_TYPE, myCurrentEdge.id.c_str(), ok);
            std::string name = attrs.getOpt<std::string>(OPENDRIVE_ATTR_NAME, myCurrentEdge.id.c_str(), ok, "", false);
            int orientation = attrs.get<std::string>(OPENDRIVE_ATTR_ORIENTATION, myCurrentEdge.id.c_str(), ok) == "-" ? -1 : 1;
            SUMOReal s = attrs.get<SUMOReal>(OPENDRIVE_ATTR_S, myCurrentEdge.id.c_str(), ok);
            bool dynamic = attrs.get<std::string>(OPENDRIVE_ATTR_DYNAMIC, myCurrentEdge.id.c_str(), ok) == "no" ? false : true;
            myCurrentEdge.signals.push_back(OpenDriveSignal(id, type, name, orientation, dynamic, s));
        }
        break;
        case OPENDRIVE_TAG_JUNCTION:
            myCurrentJunctionID = attrs.get<std::string>(OPENDRIVE_ATTR_ID, myCurrentJunctionID.c_str(), ok);
            break;
        case OPENDRIVE_TAG_CONNECTION: {
            std::string id = attrs.get<std::string>(OPENDRIVE_ATTR_ID, myCurrentJunctionID.c_str(), ok);
            myCurrentIncomingRoad = attrs.get<std::string>(OPENDRIVE_ATTR_INCOMINGROAD, myCurrentJunctionID.c_str(), ok);
            myCurrentConnectingRoad = attrs.get<std::string>(OPENDRIVE_ATTR_CONNECTINGROAD, myCurrentJunctionID.c_str(), ok);
            std::string cp = attrs.get<std::string>(OPENDRIVE_ATTR_CONTACTPOINT, myCurrentJunctionID.c_str(), ok);
            myCurrentContactPoint = cp == "start" ? OPENDRIVE_CP_START : OPENDRIVE_CP_END;
            myConnectionWasEmpty = true;
        }
        break;
        case OPENDRIVE_TAG_LANELINK: {
            int from = attrs.get<int>(OPENDRIVE_ATTR_FROM, myCurrentJunctionID.c_str(), ok);
            int to = attrs.get<int>(OPENDRIVE_ATTR_TO, myCurrentJunctionID.c_str(), ok);
            Connection c;
            c.fromEdge = myCurrentIncomingRoad;
            c.toEdge = myCurrentConnectingRoad;
            c.fromLane = from;
            c.toLane = to;
            c.fromCP = OPENDRIVE_CP_END;
            c.toCP = myCurrentContactPoint;
            c.all = false;
            if (myEdges.find(c.fromEdge) == myEdges.end()) {
                WRITE_ERROR("In laneLink-element: incoming road '" + c.fromEdge + "' is not known.");
            } else {
                OpenDriveEdge* e = myEdges.find(c.fromEdge)->second;
                e->connections.insert(c);
                myConnectionWasEmpty = false;
            }
        }
        break;
        case OPENDRIVE_TAG_WIDTH: {
            if (myElementStack.size() >= 2 && myElementStack[myElementStack.size() - 1] == OPENDRIVE_TAG_LANE) {
                SUMOReal width = attrs.get<SUMOReal>(OPENDRIVE_ATTR_A, myCurrentEdge.id.c_str(), ok);
                OpenDriveLane& l = myCurrentEdge.laneSections.back().lanesByDir[myCurrentLaneDirection].back();
                l.width = MAX2(l.width, width);
            }
        }
        break;
        case OPENDRIVE_TAG_SPEED: {
            if (myElementStack.size() >= 2 && myElementStack[myElementStack.size() - 1] == OPENDRIVE_TAG_LANE) {
                SUMOReal speed = attrs.get<SUMOReal>(OPENDRIVE_ATTR_MAX, myCurrentEdge.id.c_str(), ok);
                SUMOReal pos = attrs.get<SUMOReal>(OPENDRIVE_ATTR_SOFFSET, myCurrentEdge.id.c_str(), ok);
				myCurrentEdge.laneSections.back().lanesByDir[myCurrentLaneDirection].back().speeds.push_back(std::make_pair(pos, speed));
            }
        }
        break;
        default:
            break;
    }
    myElementStack.push_back(element);
}


void
NIImporter_OpenDrive::myEndElement(int element) {
    myElementStack.pop_back();
    switch (element) {
        case OPENDRIVE_TAG_ROAD:
            myEdges[myCurrentEdge.id] = new OpenDriveEdge(myCurrentEdge);
            break;
        case OPENDRIVE_TAG_CONNECTION:
            if (myConnectionWasEmpty) {
                Connection c;
                c.fromEdge = myCurrentIncomingRoad;
                c.toEdge = myCurrentConnectingRoad;
                c.fromLane = 0;
                c.toLane = 0;
                c.fromCP = OPENDRIVE_CP_END;
                c.toCP = myCurrentContactPoint;
                c.all = true;
                if (myEdges.find(c.fromEdge) == myEdges.end()) {
                    WRITE_ERROR("In laneLink-element: incoming road '" + c.fromEdge + "' is not known.");
                } else {
                    OpenDriveEdge* e = myEdges.find(c.fromEdge)->second;
                    e->connections.insert(c);
                }
            }
            break;
        case OPENDRIVE_TAG_LANESECTION: {
			myCurrentEdge.laneSections.back().buildLaneMapping(myTypeContainer);
        }
        break;
        default:
            break;
    }
}



void
NIImporter_OpenDrive::addLink(LinkType lt, const std::string& elementType,
                              const std::string& elementID,
                              const std::string& contactPoint) {
    OpenDriveLink l(lt, elementID);
    // elementType
    if (elementType == "road") {
        l.elementType = OPENDRIVE_ET_ROAD;
    } else if (elementType == "junction") {
        l.elementType = OPENDRIVE_ET_JUNCTION;
    }
    // contact point
    if (contactPoint == "start") {
        l.contactPoint = OPENDRIVE_CP_START;
    } else if (contactPoint == "end") {
        l.contactPoint = OPENDRIVE_CP_END;
    }
    // add
    myCurrentEdge.links.push_back(l);
}


void
NIImporter_OpenDrive::addGeometryShape(GeometryType type, const std::vector<SUMOReal>& vals) {
    // checks
    if (myCurrentEdge.geometries.size() == 0) {
        throw ProcessError("Mismatching paranthesis in geometry definition for road '" + myCurrentEdge.id + "'");
    }
    OpenDriveGeometry& last = myCurrentEdge.geometries.back();
    if (last.type != OPENDRIVE_GT_UNKNOWN) {
        throw ProcessError("Double geometry information for road '" + myCurrentEdge.id + "'");
    }
    // set
    last.type = type;
    last.params = vals;
}


bool
operator<(const NIImporter_OpenDrive::Connection& c1, const NIImporter_OpenDrive::Connection& c2) {
    if (c1.fromEdge != c2.fromEdge) {
        return c1.fromEdge   < c2.fromEdge;
    }
    if (c1.toEdge     != c2.toEdge) {
        return c1.toEdge     < c2.toEdge;
    }
    if (c1.fromLane != c2.fromLane) {
        return c1.fromLane < c2.fromLane;
    }
    return c1.toLane < c2.toLane;
}



/****************************************************************************/

