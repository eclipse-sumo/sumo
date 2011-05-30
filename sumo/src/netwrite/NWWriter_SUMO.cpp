/****************************************************************************/
/// @file    NWWriter_SUMO.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the SUMO format
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
#include "NWWriter_SUMO.h"
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBTrafficLightLogic.h>
#include <netbuild/NBDistrict.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/ToString.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NWWriter_SUMO::writeNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether a matsim-file shall be generated
    if (!oc.isSet("output")) {
        return;
    }
	OutputDevice& device = OutputDevice::getDevice(oc.getString("output"));
    device.writeXMLHeader("net");
    device << "\n";
    // write network offsets
    device << "   <location netOffset=\"" << GeoConvHelper::getOffsetBase() << "\""
    << " convBoundary=\"" << GeoConvHelper::getConvBoundary() << "\"";
    if (GeoConvHelper::usingGeoProjection()) {
        device.setPrecision(GEO_OUTPUT_ACCURACY);
        device << " origBoundary=\"" << GeoConvHelper::getOrigBoundary() << "\"";
        device.setPrecision();
    } else {
        device << " origBoundary=\"" << GeoConvHelper::getOrigBoundary() << "\"";
    }
    device << " projParameter=\"" << GeoConvHelper::getProjString() << "\"/>\n\n";

    // get involved container
    const NBNodeCont &nc = nb.getNodeCont();
    const NBEdgeCont &ec = nb.getEdgeCont();
    const NBJunctionLogicCont &jc = nb.getJunctionLogicCont();
    const NBTrafficLightLogicCont &tc = nb.getTLLogicCont();
    const NBDistrictCont &dc = nb.getDistrictCont();

    // write inner lanes
    if (!oc.getBool("no-internal-links")) {
        bool hadAny = false;
        for (std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
            hadAny |= writeInternalEdges(device, *(*i).second);
        }
        if (hadAny) {
            device << "\n";
        }
    }

    // write edges with lanes and connected edges
    for (std::map<std::string, NBEdge*>::const_iterator i=ec.begin(); i!=ec.end(); ++i) {
        writeEdge(device, *(*i).second);
    }
    device << "\n";

    // write right-of-way logics
    for (std::map<std::string, std::string>::const_iterator i=jc.begin(); i!=jc.end(); ++i) {
        device << (*i).second;
    }
    if (jc.size()!=0) {
        device << "\n";
    }

    // write tls logics
    for (std::map<std::string, NBTrafficLightLogic*>::const_iterator i=tc.begin(); i!=tc.end(); ++i) {
        device << "   <tl-logic id=\"" << (*i).second->getID() << "\" type=\"static\""
            << " programID=\"" << (*i).second->getProgramID() 
            << "\" offset=\"" << (*i).second->getOffset() << "\">\n";
        // write the phases
        const std::vector<NBTrafficLightLogic::PhaseDefinition> &phases = (*i).second->getPhases();
        for (std::vector<NBTrafficLightLogic::PhaseDefinition>::const_iterator j=phases.begin(); j!=phases.end(); ++j) {
            device << "      <phase duration=\"" << (*j).duration << "\" state=\"" << (*j).state << "\"/>\n";
        }
        device << "   </tl-logic>\n";
    }
    if (tc.size()!=0) {
        device << "\n";
    }

    // write the nodes (junctions)
    for (std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
        writeJunction(device, *(*i).second);
    }
    device << "\n";
    if (!oc.getBool("no-internal-links")) {
        // ... internal nodes if not unwanted
        bool hadAny = false;
        for (std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
            hadAny |= writeInternalNodes(device, *(*i).second);
        }
        if (hadAny) {
            device << "\n";
        }
    }

    // write the successors of lanes
    bool includeInternal = !oc.getBool("no-internal-links");
    for (std::map<std::string, NBEdge*>::const_iterator i=ec.begin(); i!=ec.end(); ++i) {
        for (unsigned int j=0; j<(*i).second->getLanes().size(); ++j) {
            writeSucceeding(device, *(*i).second, j, includeInternal);
        }
    }
    device << "\n";
    if (!oc.getBool("no-internal-links")) {
        // ... internal successors if not unwanted
        bool hadAny = false;
        for (std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
            hadAny |= writeInternalSucceeding(device, *(*i).second);
        }
        if (hadAny) {
            device << "\n";
        }
    }

    // write roundabout information
	const std::vector<std::set<NBEdge*> > &roundabouts = nb.getRoundabouts();
    for (std::vector<std::set<NBEdge*> >::const_iterator i=roundabouts.begin(); i!=roundabouts.end(); ++i) {
        writeRoundabout(device, *i);
    }
    if (roundabouts.size()!=0) {
        device << "\n";
    }

    // write the districts
    for (std::map<std::string, NBDistrict*>::const_iterator i=dc.begin(); i!=dc.end(); i++) {
        writeDistrict(device, *(*i).second);
    }
    if (dc.size()!=0) {
        device << "\n";
    }
    device.close();
}


bool
NWWriter_SUMO::writeInternalEdges(OutputDevice &into, const NBNode &n) {
    unsigned int noInternalNoSplits = n.countInternalLanes(false);
    if (noInternalNoSplits==0) {
        return false;
    }
    std::string innerID = ":" + n.getID();
    unsigned int lno = 0;
    unsigned int splitNo = 0;
    bool ret = false;
    const EdgeVector &incoming = n.getIncomingEdges();
    for (EdgeVector::const_iterator i=incoming.begin(); i!=incoming.end(); i++) {
        unsigned int noLanesEdge = (*i)->getNoLanes();
        for (unsigned int j=0; j<noLanesEdge; j++) {
            std::vector<NBEdge::Connection> elv = (*i)->getConnectionsFromLane(j);
            for (std::vector<NBEdge::Connection>::iterator k=elv.begin(); k!=elv.end(); ++k) {
                if ((*k).toEdge==0) {
                    continue;
                }
                // compute the maximum speed allowed
                //  see !!! for an explanation (with a_lat_mean ~0.3)
                SUMOReal vmax = (SUMOReal) 0.3 * (SUMOReal) 9.80778 *
                                (*i)->getLaneShape(j).getEnd().distanceTo(
                                    (*k).toEdge->getLaneShape((*k).toLane).getBegin())
                                / (SUMOReal) 2.0 / (SUMOReal) PI;
                vmax = MIN2(vmax, (((*i)->getSpeed()+(*k).toEdge->getSpeed())/(SUMOReal) 2.0));
                vmax = ((*i)->getSpeed()+(*k).toEdge->getSpeed())/(SUMOReal) 2.0;
                //
                Position end = (*k).toEdge->getLaneShape((*k).toLane).getBegin();
                Position beg = (*i)->getLaneShape(j).getEnd();

                PositionVector shape = n.computeInternalLaneShape(*i, j, (*k).toEdge, (*k).toLane);
                assert(shape.size() >= 2);
                SUMOReal length = MAX2(shape.length(), (SUMOReal)POSITION_EPS); // !!! is this needed?

                // get internal splits if any
                std::pair<SUMOReal, std::vector<unsigned int> > cross = n.getCrossingPosition(*i, j, (*k).toEdge, (*k).toLane);
                if (cross.first>=0) {
                    std::pair<PositionVector, PositionVector> split = shape.splitAt(cross.first);
                    writeInternalEdge(into, innerID + "_" + toString(lno), vmax, cross.first, split.first);
                    writeInternalEdge(into, innerID + "_" + toString(splitNo+noInternalNoSplits), vmax, 
                            length - cross.first, split.second);
                    splitNo++;
                } else {
                    writeInternalEdge(into, innerID + "_" + toString(lno), vmax, length, shape);
                }
                lno++;
                ret = true;
            }
        }
    }
    return ret;
}


void 
NWWriter_SUMO::writeInternalEdge(OutputDevice &into, 
        const std::string &id, SUMOReal vmax, SUMOReal length, const PositionVector &shape) {
    into << "   <edge id=\"" << id << "\" function=\"internal\">\n";
    into << "      <lane id=\"" << id << "_0\" depart=\"0\" "
        << "maxspeed=\"" << vmax << "\" "
        << "length=\"" << toString(length) << "\" "
        << "shape=\"" << shape << "\"/>\n";
    into << "   </edge>\n";
}


void
NWWriter_SUMO::writeEdge(OutputDevice &into, const NBEdge &e) {
    // write the edge's begin
    into << "   <edge id=\"" << e.getID() <<
    "\" from=\"" << e.getFromNode()->getID() <<
    "\" to=\"" << e.getToNode()->getID() <<
    "\" priority=\"" << e.getPriority() << "\"";
    if(e.getTypeName()!="") {
        into << " type=\"" << e.getTypeName() << "\"";
    }
    if (e.isMacroscopicConnector()) {
        into << " function=\"connector\"";
    }
    // write the spread type if not default ("right")
    if (e.getLaneSpreadFunction()!=LANESPREAD_RIGHT) {
        into << " spread_type=\"" << toString(e.getLaneSpreadFunction()) << "\"";
    }
    if (!e.hasDefaultGeometry()) {
        into << " " << toString(SUMO_ATTR_SHAPE) <<  "=\"" << toString(e.getGeometry()) << "\"";
    }
    into << ">\n";
    // write the lanes
    const std::vector<NBEdge::Lane> &lanes = e.getLanes();
    SUMOReal length = e.getLoadedLength();
    if (length<=0) {
        length = (SUMOReal) .1;
    }
    for (unsigned int i=0; i<(unsigned int) lanes.size(); i++) {
        writeLane(into, e.getID(), e.getLaneID(i), lanes[i], length, i);
    }
    // close the edge
    into << "   </edge>\n";
}


void
NWWriter_SUMO::writeLane(OutputDevice &into, const std::string &eID, const std::string &lID, const NBEdge::Lane &lane, SUMOReal length, unsigned int index) {
    // output the lane's attributes
    into << "      <lane id=\"" << lID << "\"";
    // the first lane of an edge will be the depart lane
    if (index==0) {
        into << " depart=\"1\"";
    } else {
        into << " depart=\"0\"";
    }
    // write the list of allowed/disallowed vehicle classes
    if (lane.allowed.size() > 0) {
        into << " allow=\"" << getVehicleClassNames(lane.allowed) << '\"';
    }
    if (lane.notAllowed.size() > 0) {
        into << " disallow=\"" << getVehicleClassNames(lane.notAllowed) << '\"';
    }
    if (lane.preferred.size() > 0) {
        into << " prefer=\"" << getVehicleClassNames(lane.preferred) << '\"';
    }
    // some further information
    if (lane.speed==0) {
        WRITE_WARNING("Lane #" + toString(index) + " of edge '" + eID + "' has a maximum velocity of 0.");
    } else if (lane.speed<0) {
        throw ProcessError("Negative velocity (" + toString(lane.speed) + " on edge '" + eID + "' lane#" + toString(index) + ".");
    }
    if(lane.offset>0) {
        length = length - lane.offset;
    }
    into << " maxspeed=\"" << lane.speed << "\" length=\"" << length << "\"";
    if (lane.offset > 0) {
        into << " endOffset=\"" << lane.offset << '\"';
    }
    if (lane.width > 0) {
        into << " width=\"" << lane.width << '\"';
    }
    PositionVector shape = lane.shape;
    if(lane.offset>0) {
        shape = shape.getSubpart(0, shape.length()-lane.offset);
    }
    into << " shape=\"" << shape << "\"/>\n";
}


void
NWWriter_SUMO::writeJunction(OutputDevice &into, const NBNode &n) {
    // write the attributes
    into << "   <junction id=\"" << n.getID() << '\"';
    SumoXMLNodeType type = n.getType();
    if (n.getIncomingEdges().size()==0 || n.getOutgoingEdges().size()==0) {
        type = NODETYPE_DEAD_END;
    }
    into << " type=\"" << toString(type) << "\"";
    into << " x=\"" << n.getPosition().x() << "\" y=\"" << n.getPosition().y() << "\"";
    into << " incLanes=\"";
    // write the incoming lanes
    const std::vector<NBEdge*> &incoming = n.getIncomingEdges();
    for (std::vector<NBEdge*>::const_iterator i=incoming.begin(); i!=incoming.end(); ++i) {
        unsigned int noLanes = (*i)->getNoLanes();
        std::string id = (*i)->getID();
        for (unsigned int j=0; j<noLanes; j++) {
            into << id << '_' << j;
            if (i!=incoming.end()-1 || j<noLanes-1) {
                into << ' ';
            }
        }
    }
    into << "\"";
    // write the internal lanes
    into << " intLanes=\"";
    if (!OptionsCont::getOptions().getBool("no-internal-links")) {
        unsigned int l = 0;
        unsigned int o = n.countInternalLanes(false);
        for (std::vector<NBEdge*>::const_iterator i=incoming.begin(); i!=incoming.end(); i++) {
            unsigned int noLanesEdge = (*i)->getNoLanes();
            for (unsigned int j=0; j<noLanesEdge; j++) {
                std::vector<NBEdge::Connection> elv = (*i)->getConnectionsFromLane(j);
                for (std::vector<NBEdge::Connection>::iterator k=elv.begin(); k!=elv.end(); ++k) {
                    if ((*k).toEdge==0) {
                        continue;
                    }
                    if (l!=0) {
                        into << ' ';
                    }
                    std::pair<SUMOReal, std::vector<unsigned int> > cross = n.getCrossingPosition(*i, j, (*k).toEdge, (*k).toLane);
                    if (cross.first<=0) {
                        into << ':' << n.getID() << '_' << l << "_0";
                    } else {
                        into << ':' << n.getID() << '_' << o << "_0";
                        o++;
                    }
                    l++;
                }
            }
        }
    }
    into << "\"";
    // close writing
    into << " shape=\"" << n.getShape() << "\"/>\n";
}


bool
NWWriter_SUMO::writeInternalNodes(OutputDevice &into, const NBNode &n) {
    unsigned int noInternalNoSplits = n.countInternalLanes(false);
    if (noInternalNoSplits==0) {
        return false;
    }
    bool ret = false;
    unsigned int lno = 0;
    unsigned int splitNo = 0;
    std::string innerID = ":" + n.getID();
    const std::vector<NBEdge*> &incoming = n.getIncomingEdges();
    for (std::vector<NBEdge*>::const_iterator i=incoming.begin(); i!=incoming.end(); i++) {
        unsigned int noLanesEdge = (*i)->getNoLanes();
        for (unsigned int j=0; j<noLanesEdge; j++) {
            std::vector<NBEdge::Connection> elv = (*i)->getConnectionsFromLane(j);
            for (std::vector<NBEdge::Connection>::iterator k=elv.begin(); k!=elv.end(); ++k) {
                if ((*k).toEdge==0) {
                    continue;
                }
                std::pair<SUMOReal, std::vector<unsigned int> > cross = n.getCrossingPosition(*i, j, (*k).toEdge, (*k).toLane);
                if (cross.first<=0) {
                    lno++;
                    continue;
                }
                // write the attributes
                std::string sid = innerID + "_" + toString(splitNo+noInternalNoSplits) + "_0";
                std::string iid = innerID + "_" + toString(lno) + "_0";
                PositionVector shape = n.computeInternalLaneShape(*i, j, (*k).toEdge, (*k).toLane);
                Position pos = shape.positionAtLengthPosition(cross.first);
                into << "   <junction id=\"" << sid << '\"';
                into << " type=\"" << toString(NODETYPE_INTERNAL) << "\"";
                into << " x=\"" << pos.x() << "\" y=\"" << pos.y() << "\"";
                into << " incLanes=\"";
                std::string furtherIncoming = n.getCrossingSourcesNames_dividedBySpace(*i, j, (*k).toEdge, (*k).toLane);
                if (furtherIncoming.length()!=0) {
                    into << iid << " " << furtherIncoming;
                } else {
                    into << iid;
                }
                into << "\"";
                into << " intLanes=\"" << n.getCrossingNames_dividedBySpace(*i, j, (*k).toEdge, (*k).toLane) << "\"";
                into << " shape=\"\"/>\n";
                splitNo++;
                lno++;
                ret = true;
            }
        }
    }
    return ret;
}


void
NWWriter_SUMO::writeSucceeding(OutputDevice &into, const NBEdge &e, unsigned int lane, bool includeInternal) {
    into << "   <succ edge=\"" << e.getID() << "\" lane=\"" << e.getLaneID(lane) << "\" junction=\"" << e.getToNode()->getID() << "\">\n";
    // output list of connected lanes
    const std::vector<NBEdge::Connection> &connections = e.getConnections();
    for (std::vector<NBEdge::Connection>::const_iterator i=connections.begin(); i!=connections.end(); ++i) {
        const NBEdge::Connection &c = *i;
        if (c.fromLane!=static_cast<int>(lane)) {
            continue;
        }
        assert(c.toEdge != 0);
        into << "      <succlane lane=\"" << c.toEdge->getLaneID(c.toLane) << '\"'; // !!! classe LaneEdge mit getLaneID
        if (includeInternal) {
            into << " via=\"" << e.getToNode()->getInternalLaneID(&e, c.fromLane, c.toEdge, c.toLane) << "_0\"";
        }
        // set information about the controlling tl if any
        if (c.tlID!="") {
            into << " tl=\"" << c.tlID << "\"";
            into << " linkno=\"" << c.tlLinkNo << "\"";
        }
        // write the direction information
        NBMMLDirection dir = e.getToNode()->getMMLDirection(&e, c.toEdge);
        into << " dir=\"";
        switch (dir) {
        case MMLDIR_STRAIGHT:
            into << "s";
            break;
        case MMLDIR_LEFT:
            into << "l";
            break;
        case MMLDIR_RIGHT:
            into << "r";
            break;
        case MMLDIR_TURN:
            into << "t";
            break;
        case MMLDIR_PARTLEFT:
            into << "L";
            break;
        case MMLDIR_PARTRIGHT:
            into << "R";
            break;
        default:
            // should not happen
            assert(false);
            break;
        }
        into << "\" ";
        // write the state information
        if (c.tlID!="") {
            into << "state=\"t";
        } else {
            into << "state=\"" << e.getToNode()->stateCode(&e, c.toEdge, c.toLane, c.mayDefinitelyPass);
        }
        // close
        into << "\"/>\n";
    }
    into << "   </succ>\n";
}


bool
NWWriter_SUMO::writeInternalSucceeding(OutputDevice &into, const NBNode &n) {
    unsigned int noInternalNoSplits = n.countInternalLanes(false);
    if (noInternalNoSplits==0) {
        return false;
    }
    bool ret = false;
    unsigned int lno = 0;
    unsigned int splitNo = 0;
    std::string innerID = ":" + n.getID();
    const std::vector<NBEdge*> &incoming = n.getIncomingEdges();
    for (std::vector<NBEdge*>::const_iterator i=incoming.begin(); i!=incoming.end(); i++) {
        unsigned int noLanesEdge = (*i)->getNoLanes();
        for (unsigned int j=0; j<noLanesEdge; j++) {
            std::vector<NBEdge::Connection> elv = (*i)->getConnectionsFromLane(j);
            for (std::vector<NBEdge::Connection>::iterator k=elv.begin(); k!=elv.end(); ++k) {
                if ((*k).toEdge==0) {
                    continue;
                }
                std::string id = innerID + "_" + toString(lno);
                std::string sid = innerID + "_" + toString(splitNo+noInternalNoSplits);
                std::pair<SUMOReal, std::vector<unsigned int> > cross = n.getCrossingPosition(*i, j, (*k).toEdge, (*k).toLane);

                // get internal splits if any
                into << "   <succ edge=\"" << id << "\" "
                << "lane=\"" << id << "_"
                << 0 << "\" junction=\"" << n.getID() << "\">\n";
                if (cross.first>=0) {
                    into << "      <succlane lane=\""
                    //<< sid << "_" << 0 ()
                    << (*k).toEdge->getID() << "_" << (*k).toLane << "\""
                    << " via=\"" << sid << "_" << 0 << "\""
                    << " tl=\"" << "" << "\" linkno=\""
                    << "" << "\" dir=\"s\" state=\"M\""; // !!! yield or not depends on whether it is tls controlled or not
                } else {
                    into << "      <succlane lane=\""
                    << (*k).toEdge->getID() << "_" << (*k).toLane
                    << "\" tl=\"" << "" << "\" linkno=\""
                    << "" << "\" dir=\"s\" state=\"M\"";
                }
                into << "/>\n";
                into << "   </succ>\n";

                if (cross.first>=0) {
                    into << "   <succ edge=\"" << sid << "\" "
                    << "lane=\"" << sid << "_" << 0
                    << "\" junction=\"" << sid << "\">\n";
                    into << "      <succlane lane=\""
                    << (*k).toEdge->getID() << "_" << (*k).toLane
                    << "\" tl=\"" << "" << "\" linkno=\""
                    << "0" << "\" dir=\"s\" state=\"M\"";
                    into << "/>\n";
                    into << "   </succ>\n";
                    splitNo++;
                }
                lno++;
                ret = true;
            }
        }
    }
    return ret;
}


void 
NWWriter_SUMO::writeRoundabout(OutputDevice &into, const std::set<NBEdge*> &r) {
        std::vector<NBNode*> nodes;
        for (std::set<NBEdge*>::const_iterator j=r.begin(); j!=r.end(); ++j) {
            NBNode *n = (*j)->getToNode();
            if (find(nodes.begin(), nodes.end(), n)==nodes.end()) {
                nodes.push_back(n);
            }
        }
		sort(nodes.begin(), nodes.end(), NBNode::nodes_by_id_sorter());
        into << "   <roundabout nodes=\"";
        int k = 0;
        for (std::vector<NBNode*>::iterator j=nodes.begin(); j!=nodes.end(); ++j, ++k) {
            if (k!=0) {
                into << ' ';
            }
            into << (*j)->getID();
        }
        into << "\"/>\n";
}


void
NWWriter_SUMO::writeDistrict(OutputDevice &into, const NBDistrict &d) {
    std::vector<SUMOReal> sourceW = d.getSourceWeights();
    VectorHelper<SUMOReal>::normaliseSum(sourceW, 1.0);
    std::vector<SUMOReal> sinkW = d.getSinkWeights();
    VectorHelper<SUMOReal>::normaliseSum(sinkW, 1.0);
    // write the head and the id of the district
    into << "   <district id=\"" << d.getID() << "\"";
    if (d.getShape().size()>0) {
        into << " shape=\"" << d.getShape() << "\"";
    }
    into << ">\n";
    size_t i;
    // write all sources
    const std::vector<NBEdge*> &sources = d.getSourceEdges();
    for (i=0; i<sources.size(); i++) {
        // write the head and the id of the source
        into << "      <dsource id=\"" << sources[i]->getID() << "\" weight=\"" << sourceW[i] << "\"/>\n";
    }
    // write all sinks
    const std::vector<NBEdge*> &sinks = d.getSinkEdges();
    for (i=0; i<sinks.size(); i++) {
        // write the head and the id of the sink
        into << "      <dsink id=\"" << sinks[i]->getID() << "\" weight=\"" << sinkW[i] << "\"/>\n";
    }
    // write the tail
    into << "   </district>\n";
}


/****************************************************************************/

