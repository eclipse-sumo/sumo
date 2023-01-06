/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    NWWriter_OpenDrive.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 04.05.2011
///
// Exporter writing networks using the openDRIVE format
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/StringBijection.h>
#include <utils/common/SUMOVehicleClass.h>
#include <netbuild/NBEdge.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class NBEdge;
class OptionsCont;
class PositionVector;
class OutputDevice;
class OutputDevice_String;
class ShapeContainer;
class PointOfInterest;
class SUMOPolygon;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NWWriter_OpenDrive
 * @brief Exporter writing networks using the openDRIVE format
 *
 */
class NWWriter_OpenDrive {
public:
    /** @brief Writes the network into a openDRIVE-file
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void writeNetwork(const OptionsCont& oc, NBNetBuilder& nb);

protected:
    /// @brief signalID -> (lanes, dirs)
    typedef std::map<std::string, std::pair<std::set<int>, std::set<LinkDirection> > > SignalLanes;

    /// @brief retrieve divider type
    static std::string getDividerType(const NBEdge* e);

    /// @brief write normal edge to device
    static void writeNormalEdge(OutputDevice& device, const NBEdge* e,
                                int edgeID, int fromNodeID, int toNodeID,
                                const bool origNames,
                                const double straightThresh,
                                const ShapeContainer& shc,
                                SignalLanes& signalLanes,
                                const std::vector<std::string>& crossings);

    /// @brief write internal edge to device, return next connectionID
    static int writeInternalEdge(OutputDevice& device, OutputDevice& junctionDevice,
                                 const NBEdge* inEdge, int nodeID,
                                 int edgeID, int inEdgeID, int outEdgeID,
                                 int connectionID,
                                 const std::vector<NBEdge::Connection>& parallel,
                                 const bool isOuterEdge,
                                 const double straightThresh,
                                 const std::string& centerMark,
                                 SignalLanes& signalLanes);

    static void addPedestrianConnection(const NBEdge* inEdge, const NBEdge* outEdge, std::vector<NBEdge::Connection>& parallel);

    /// @brief write geometry as sequence of lines (sumo style)
    static double writeGeomLines(const PositionVector& shape, OutputDevice& device, OutputDevice& elevationDevice, double offset = 0);

    /* @brief write geometry as sequence of lines and bezier curves
     *
     * @param[in] straightThresh angular changes below threshold are considered to be straight and no curve will be fitted between the segments
     * @param[out] length Return the total length of the reference line
     */
    static bool writeGeomSmooth(const PositionVector& shape, double speed, OutputDevice& device, OutputDevice& elevationDevice, double straightThresh, double& length);

    /// @brief write geometry as a single bezier curve (paramPoly3)
    static double writeGeomPP3(OutputDevice& device,
                               OutputDevice& elevationDevice,
                               PositionVector init,
                               double length,
                               double offset = 0);

    static void writeElevationProfile(const PositionVector& shape, OutputDevice& device, const OutputDevice_String& elevationDevice);

    static void writeEmptyCenterLane(OutputDevice& device, const std::string& mark, double markWidth);
    static int getID(const std::string& origID, StringBijection<int>& map, int& lastID);

    static std::string getLaneType(SVCPermissions permissions);

    /// @brief get the lane border that is closer to the reference line (center line of the edge)
    static PositionVector getInnerLaneBorder(const NBEdge* edge, int laneIndex = -1, double widthOffset = 0);
    /// @brief get the lane border that is further away from the reference line (center line of the edge)
    static PositionVector getOuterLaneBorder(const NBEdge* edge, int laneIndex = -1);

    /// @brief check if the lane geometries are compatible with OpenDRIVE assumptions (colinear stop line)
    static void checkLaneGeometries(const NBEdge* e);

    /// @brief write road objects referenced as edge parameters
    static void writeRoadObjects(OutputDevice& device, const NBEdge* e, const ShapeContainer& shc, const std::vector<std::string>& crossings);

    /// @brief write signal record for traffic light
    static void writeSignals(OutputDevice& device, const NBEdge* e, double length, SignalLanes& signalLanes, const ShapeContainer& shc);

    /// @brief convert sumo lane index to xodr lane index
    static int s2x(int sumoIndex, int numLanes);

    /// @brief map pois and polygons to the closes edge
    static void mapmatchRoadObjects(const ShapeContainer& shc,  const NBEdgeCont& ec);

    static void writeRoadObjectPOI(OutputDevice& device, const NBEdge* e, const PositionVector& roadShape, const PointOfInterest* poi);

    static void writeRoadObjectPoly(OutputDevice& device, const NBEdge* e, const PositionVector& roadShape, const SUMOPolygon* p);

    struct TrafficSign {
        std::string country;
        std::string type;
        std::string subtype;
        std::string value;
    };

    static std::vector<TrafficSign> parseTrafficSign(const std::string& trafficSign, PointOfInterest* poi);
    static TrafficSign parseTrafficSignId(const std::string& trafficSign);

    // @brief return road postion in s,t coordinates
    static double getRoadSideOffset(const NBEdge* e);


protected:
    /// @brief whether a lefthand network is being written
    static bool lefthand;

    /* @brief whether a the lanes in a lefthand network shall be written to the
     * left of the reference line (positive indices)
     * This style is not support by some older programs.
     * */
    static bool LHLL;

    // lefthand but lanes written as right lanes
    static bool LHRL;

};
