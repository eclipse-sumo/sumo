/****************************************************************************/
/// @file    NWWriter_DlrNavteq.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    26.10.2012
/// @version $Id$
///
// Exporter writing networks using DlrNavteq (Elmar) format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2014 DLR (http://www.dlr.de/) and contributors
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
#include <algorithm>
#include <ctime>
#include <cmath>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/geom/GeoConvHelper.h>
#include "NWFrame.h"
#include "NWWriter_DlrNavteq.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ---------------------------------------------------------------------------
// static members
// ---------------------------------------------------------------------------
const std::string NWWriter_DlrNavteq::UNDEFINED("-1");

// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NWWriter_DlrNavteq::writeNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether a matsim-file shall be generated
    if (!oc.isSet("dlr-navteq-output")) {
        return;
    }
    writeNodesUnsplitted(oc, nb.getNodeCont(), nb.getEdgeCont());
    writeLinksUnsplitted(oc, nb.getEdgeCont());
    writeTrafficSignals(oc, nb.getNodeCont());
}


void NWWriter_DlrNavteq::writeHeader(OutputDevice& device, const OptionsCont& oc) {
    time_t rawtime;
    time(&rawtime);
    char buffer [80];
    strftime(buffer, 80, "on %c", localtime(&rawtime));
    device << "# Generated " << buffer << " by " << oc.getFullName() << "\n";
    device << "# Format matches Extraction version: V6.0 \n";
    std::stringstream tmp;
    oc.writeConfiguration(tmp, true, false, false);
    tmp.seekg(std::ios_base::beg);
    std::string line;
    while (!tmp.eof()) {
        std::getline(tmp, line);
        device << "# " << line << "\n";
    }
    device << "#\n";
}

void
NWWriter_DlrNavteq::writeNodesUnsplitted(const OptionsCont& oc, NBNodeCont& nc, NBEdgeCont& ec) {
    // For "real" nodes we simply use the node id.
    // For internal nodes (geometry vectors describing edge geometry in the parlance of this format)
    // we use the id of the edge and do not bother with
    // compression (each direction gets its own internal node).
    // XXX add option for generating numerical ids in case the input network has string ids and the target process needs integers
    OutputDevice& device = OutputDevice::getDevice(oc.getString("dlr-navteq-output") + "_nodes_unsplitted.txt");
    writeHeader(device, oc);
    const GeoConvHelper& gch = GeoConvHelper::getFinal();
    const bool haveGeo = gch.usingGeoProjection();
    const SUMOReal geoScale = pow(10.0f, haveGeo ? 5 : 2); // see NIImporter_DlrNavteq::GEO_SCALE
    device.setPrecision(0);
    if (!haveGeo) {
        WRITE_WARNING("DlrNavteq node data will be written in (floating point) cartesian coordinates");
    }
    // write format specifier
    device << "# NODE_ID\tIS_BETWEEN_NODE\tamount_of_geocoordinates\tx1\ty1\t[x2 y2  ... xn  yn]\n";
    // write normal nodes
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        NBNode* n = (*i).second;
        Position pos = n->getPosition();
        gch.cartesian2geo(pos);
        pos.mul(geoScale);
        device << n->getID() << "\t0\t1\t" << pos.x() << "\t" << pos.y() << "\n";
    }
    // write "internal" nodes
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        NBEdge* e = (*i).second;
        const PositionVector& geom = e->getGeometry();
        if (geom.size() > 2) {
            std::string internalNodeID = e->getID();
            if (internalNodeID == UNDEFINED ||
                    (nc.retrieve(internalNodeID) != 0)) {
                // need to invent a new name to avoid clashing with the id of a 'real' node or a reserved name
                internalNodeID += "_geometry";
            }
            device << internalNodeID << "\t1\t" << geom.size() - 2;
            for (size_t ii = 1; ii < geom.size() - 1; ++ii) {
                Position pos = geom[(int)ii];
                gch.cartesian2geo(pos);
                pos.mul(geoScale);
                device << "\t" << pos.x() << "\t" << pos.y();
            }
            device << "\n";
        }
    }
    device.close();
}


void
NWWriter_DlrNavteq::writeLinksUnsplitted(const OptionsCont& oc, NBEdgeCont& ec) {
    OutputDevice& device = OutputDevice::getDevice(oc.getString("dlr-navteq-output") + "_links_unsplitted.txt");
    writeHeader(device, oc);
    // write format specifier
    device << "# LINK_ID\tNODE_ID_FROM\tNODE_ID_TO\tBETWEEN_NODE_ID\tLENGTH\tVEHICLE_TYPE\tFORM_OF_WAY\tBRUNNEL_TYPE\tFUNCTIONAL_ROAD_CLASS\tSPEED_CATEGORY\tNUMBER_OF_LANES\tSPEED_LIMIT\tSPEED_RESTRICTION\tNAME_ID1_REGIONAL\tNAME_ID2_LOCAL\tHOUSENUMBERS_RIGHT\tHOUSENUMBERS_LEFT\tZIP_CODE\tAREA_ID\tSUBAREA_ID\tTHROUGH_TRAFFIC\tSPECIAL_RESTRICTIONS\tEXTENDED_NUMBER_OF_LANES\tISRAMP\tCONNECTION\n";
    // write edges
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        NBEdge* e = (*i).second;
        const int kph = speedInKph(e->getSpeed());
        const std::string& betweenNodeID = (e->getGeometry().size() > 2) ? e->getID() : UNDEFINED;
        device << e->getID() << "\t"
               << e->getFromNode()->getID() << "\t"
               << e->getToNode()->getID() << "\t"
               << betweenNodeID << "\t"
               << getGraphLength(e) << "\t"
               << getAllowedTypes(e->getPermissions()) << "\t"
               << "3\t" // Speed Category 1-8 XXX refine this
               << UNDEFINED << "\t" // no special brunnel type (we don't know yet)
               << getRoadClass(e) << "\t"
               << getSpeedCategory(kph) << "\t"
               << getNavteqLaneCode(e->getNumLanes()) << "\t"
               << getSpeedCategoryUpperBound(kph) << "\t"
               << kph << "\t"
               << UNDEFINED << "\t" // NAME_ID1_REGIONAL XXX
               << UNDEFINED << "\t" // NAME_ID2_LOCAL XXX
               << UNDEFINED << "\t" // housenumbers_right
               << UNDEFINED << "\t" // housenumbers_left
               << UNDEFINED << "\t" // ZIP_CODE
               << UNDEFINED << "\t" // AREA_ID
               << UNDEFINED << "\t" // SUBAREA_ID
               << "1\t" // through_traffic (allowed)
               << UNDEFINED << "\t" // special_restrictions
               << UNDEFINED << "\t" // extended_number_of_lanes
               << UNDEFINED << "\t" // isRamp
               << "0\t" // connection (between nodes always in order)
               << "\n";
    }
}


std::string
NWWriter_DlrNavteq::getAllowedTypes(SVCPermissions permissions) {
    if (permissions == SVCAll) {
        return "100000000000";
    }
    std::ostringstream oss;
    oss << "0";
    oss << ((permissions & SVC_PASSENGER)              > 0 ? 1 : 0);
    oss << ((permissions & SVC_PASSENGER)              > 0 ? 1 : 0); // residential
    oss << ((permissions & SVC_HOV)                    > 0 ? 1 : 0);
    oss << ((permissions & SVC_EMERGENCY)              > 0 ? 1 : 0);
    oss << ((permissions & SVC_TAXI)                   > 0 ? 1 : 0);
    oss << ((permissions & (SVC_BUS | SVC_COACH))      > 0 ? 1 : 0);
    oss << ((permissions & SVC_DELIVERY)               > 0 ? 1 : 0);
    oss << ((permissions & (SVC_TRUCK | SVC_TRAILER))  > 0 ? 1 : 0);
    oss << ((permissions & SVC_MOTORCYCLE)             > 0 ? 1 : 0);
    oss << ((permissions & SVC_BICYCLE)                > 0 ? 1 : 0);
    oss << ((permissions & SVC_PEDESTRIAN)             > 0 ? 1 : 0);
    return oss.str();
}


int
NWWriter_DlrNavteq::getRoadClass(NBEdge* edge) {
    // quoting the navteq manual:
    // As a general rule, Functional Road Class assignments have no direct
    // correlation with other road attributes like speed, controlled access, route type, etc.
    //
    // we do a simple speed / lane-count mapping anyway
    // XXX the resulting functional road class layers probably won't be connected as required
    const int kph = speedInKph(edge->getSpeed());
    if ((kph) > 100) {
        return 0;
    }
    if ((kph) > 70) {
        return 1;
    }
    if ((kph) > 50) {
        return (edge->getNumLanes() > 1 ? 2 : 3);
    }
    if ((kph) > 30) {
        return 3;
    }
    return 4;
}


int
NWWriter_DlrNavteq::getSpeedCategory(int kph) {
    if ((kph) > 130) {
        return 1;
    }
    if ((kph) > 100) {
        return 2;
    }
    if ((kph) > 90) {
        return 3;
    }
    if ((kph) > 70) {
        return 4;
    }
    if ((kph) > 50) {
        return 5;
    }
    if ((kph) > 30) {
        return 6;
    }
    if ((kph) > 10) {
        return 7;
    }
    return 8;
}


int
NWWriter_DlrNavteq::getSpeedCategoryUpperBound(int kph) {
    if ((kph) > 130) {
        return 131;
    }
    if ((kph) > 100) {
        return 130;
    }
    if ((kph) > 90) {
        return 100;
    }
    if ((kph) > 70) {
        return 90;
    }
    if ((kph) > 50) {
        return 70;
    }
    if ((kph) > 30) {
        return 50;
    }
    if ((kph) > 10) {
        return 30;
    }
    return 10;
}


unsigned int
NWWriter_DlrNavteq::getNavteqLaneCode(const unsigned int numLanes) {
    const unsigned int code = (numLanes == 1 ? 1 :
                               (numLanes < 4 ?  2 : 3));
    return numLanes * 10 + code;
}


SUMOReal
NWWriter_DlrNavteq::getGraphLength(NBEdge* edge) {
    PositionVector geom = edge->getGeometry();
    geom.push_back_noDoublePos(edge->getToNode()->getPosition());
    geom.push_front_noDoublePos(edge->getFromNode()->getPosition());
    return geom.length();
}


void
NWWriter_DlrNavteq::writeTrafficSignals(const OptionsCont& oc, NBNodeCont& nc) {
    OutputDevice& device = OutputDevice::getDevice(oc.getString("dlr-navteq-output") + "_traffic_signals.txt");
    writeHeader(device, oc);
    const GeoConvHelper& gch = GeoConvHelper::getFinal();
    const bool haveGeo = gch.usingGeoProjection();
    const SUMOReal geoScale = pow(10.0f, haveGeo ? 5 : 2); // see NIImporter_DlrNavteq::GEO_SCALE
    device.setPrecision(0);
    // write format specifier
    device << "#Traffic signal related to LINK_ID and NODE_ID with location relative to driving direction.\n#column format like pointcollection.\n#DESCRIPTION->LOCATION: 1-rechts von LINK; 2-links von LINK; 3-oberhalb LINK -1-keineAngabe\n#RELATREC_ID\tPOICOL_TYPE\tDESCRIPTION\tLONGITUDE\tLATITUDE\tLINK_ID\n";
    // write record for every edge incoming to a tls controlled node
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        NBNode* n = (*i).second;
        if (n->isTLControlled()) {
            Position pos = n->getPosition();
            gch.cartesian2geo(pos);
            pos.mul(geoScale);
            const EdgeVector& incoming = n->getIncomingEdges();
            for (EdgeVector::const_iterator it = incoming.begin(); it != incoming.end(); ++it) {
                NBEdge* e = *it;
                device << e->getID() << "\t"
                       << "12\t" // POICOL_TYPE
                       << "LSA;NODEIDS#" << n->getID() << "#;LOCATION#-1#;\t"
                       << pos.x() << "\t"
                       << pos.y() << "\t"
                       << e->getID() << "\n";
            }
        }
    }
}


/****************************************************************************/

