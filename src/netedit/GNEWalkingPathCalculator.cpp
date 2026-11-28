/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEWalkingPathCalculator.cpp
/// @author  Josh
/// @date    2026-11-26
///
/****************************************************************************/
#include <config.h>

#include <algorithm>
#include <netbuild/NBNetBuilder.h>
#include <netwrite/NWWriter_SUMO.h>
#include <router/ROAbstractEdgeBuilder.h>
#include <router/ROEdge.h>
#include <router/RONet.h>
#include <router/RONetHandler.h>
#include <router/RONode.h>
#include <router/ROVehicle.h>
#include <utils/iodevices/OutputDevice_String.h>
#include <utils/options/OptionsCont.h>
#include <utils/router/PedestrianRouter.h>
#include <utils/xml/SUMOSAXReader.h>
#include <utils/xml/XMLSubSys.h>
#include <xercesc/sax/SAXException.hpp>

#include "GNEWalkingPathCalculator.h"

namespace {

class WalkingEdgeBuilder : public ROAbstractEdgeBuilder {
public:
    ROEdge* buildEdge(const std::string& name, RONode* from, RONode* to, const int priority,
                      const std::string& type, const std::string& routingType) override {
        return new ROEdge(name, from, to, getNextIndex(), priority, type, routingType, false);
    }
};

}

class GNEWalkingPathCalculator::Network {
public:
    using Graph = IntermodalNetwork<ROEdge, ROLane, RONode, ROVehicle>;
    using Router = PedestrianRouter<ROEdge, ROLane, RONode, ROVehicle>;

    Network(NBNetBuilder& builder, const OptionsCont& options) : edges(false) {
        OutputDevice_String xml;
        NWWriter_SUMO::writeNetworkSnapshot(xml, options, builder);
        WalkingEdgeBuilder edgeBuilder;
        RONetHandler handler(edges, edgeBuilder, options.getBool("no-internal-links"), 0, 0, 0, false);
        SUMOSAXReader reader(handler, "never", nullptr);
        reader.parseString(xml.getString());
        std::vector<ROEdge*> graphEdges;
        graphEdges.reserve(edges.getEdgeMap().size());
        for (const auto& edge : edges.getEdgeMap()) {
            // A permission-only connection placeholder is not a road edge.
            if (edge.second->getNumericalID() >= 0) {
                graphEdges.push_back(edge.second);
            }
        }
        std::sort(graphEdges.begin(), graphEdges.end(), [](const ROEdge* a, const ROEdge* b) {
            return a->getNumericalID() < b->getNumericalID();
        });
        graph.reset(new Graph(graphEdges, true));
        router.reset(new Router(graph.get()));
    }

    RONet edges;
    std::unique_ptr<Graph> graph;
    std::unique_ptr<Router> router;
};


GNEWalkingPathCalculator::GNEWalkingPathCalculator(NBNetBuilder& builder, const OptionsCont& options) {
    if (!options.getBool("no-internal-links")) {
        for (const auto& edge : builder.getEdgeCont()) {
            const auto& connections = edge.second->getConnections();
            // Netedit can ask for a road path before generated junction lanes exist.
            if (std::any_of(connections.begin(), connections.end(), [](const NBEdge::Connection& connection) {
                return connection.toEdge == nullptr || connection.id.empty() || connection.shape.size() < 2;
            })) {
                return;
            }
        }
    }
    try {
        myNetwork = std::make_unique<Network>(builder, options);
    } catch (const ProcessError& e) {
        WRITE_WARNINGF(TL("Could not prepare walking paths: %"), e.what());
    } catch (const XERCES_CPP_NAMESPACE::SAXException& e) {
        WRITE_WARNINGF(TL("Could not prepare walking paths: %"), XMLSubSys::transcode(e.getMessage()));
    }
}


GNEWalkingPathCalculator::~GNEWalkingPathCalculator() {}


bool
GNEWalkingPathCalculator::isAvailable() const {
    return myNetwork != nullptr;
}


std::vector<GNEWalkingPathCalculator::Segment>
GNEWalkingPathCalculator::compute(const std::string& fromID, const std::string& toID,
                                  double departPos, double arrivalPos) const {
    if (!myNetwork) {
        return {};
    }
    const ROEdge* from = myNetwork->edges.getEdge(fromID);
    const ROEdge* to = myNetwork->edges.getEdge(toID);
    if (getSidewalk<ROEdge, ROLane>(from) == nullptr || getSidewalk<ROEdge, ROLane>(to) == nullptr) {
        return {};
    }
    const SUMOVTypeParameter person(DEFAULT_PEDTYPE_ID, SVC_PEDESTRIAN);
    std::vector<const ROEdge*> path;
    std::vector<Network::Router::PathSegment> segments;
    myNetwork->router->compute(from, to, departPos, arrivalPos, person.maxSpeed, 0, nullptr, person, path, false, &segments);
    std::vector<Segment> result;
    result.reserve(segments.size());
    for (const auto& segment : segments) {
        if (segment.edge->isWalkingArea()) {
            // The lane shape is the area outline. Join its adjacent paths in the display.
            continue;
        }
        const ROLane* lane = getSidewalk<ROEdge, ROLane>(segment.edge);
        const PositionVector& shape = lane->getShape();
        const double fromOffset = lane->interpolateLanePosToGeometryPos(segment.startPos);
        const double toOffset = lane->interpolateLanePosToGeometryPos(segment.endPos);
        PositionVector subpart = shape.getSubpart2D(MIN2(fromOffset, toOffset), MAX2(fromOffset, toOffset));
        if (fromOffset > toOffset) {
            subpart = subpart.reverse();
        }
        result.push_back({lane->getID(), std::move(subpart)});
    }
    return result;
}
