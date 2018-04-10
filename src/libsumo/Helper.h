/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Helper.h
/// @author  Robert Hilbrich
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef Helper_h
#define Helper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Position;
class PositionVector;
class RGBColor;
class MSEdge;
class MSLane;
class MSPerson;


// ===========================================================================
// class definitions
// ===========================================================================

class LaneStoringVisitor {
public:
    /// @brief Constructor
    LaneStoringVisitor(std::set<std::string>& ids, const PositionVector& shape,
                       const double range, const int domain)
        : myIDs(ids), myShape(shape), myRange(range), myDomain(domain) {}

    /// @brief Destructor
    ~LaneStoringVisitor() {}

    /// @brief Adds the given object to the container
    void add(const MSLane* const l) const;

    /// @brief The container
    std::set<std::string>& myIDs;
    const PositionVector& myShape;
    const double myRange;
    const int myDomain;

private:
    /// @brief invalidated copy constructor
    LaneStoringVisitor(const LaneStoringVisitor& src);

    /// @brief invalidated assignment operator
    LaneStoringVisitor& operator=(const LaneStoringVisitor& src);
};

#define LANE_RTREE_QUAL RTree<MSLane*, MSLane, float, 2, LaneStoringVisitor>
template<>
inline float LANE_RTREE_QUAL::RectSphericalVolume(Rect* a_rect) {
    ASSERT(a_rect);
    const float extent0 = a_rect->m_max[0] - a_rect->m_min[0];
    const float extent1 = a_rect->m_max[1] - a_rect->m_min[1];
    return .78539816f * (extent0 * extent0 + extent1 * extent1);
}

template<>
inline LANE_RTREE_QUAL::Rect LANE_RTREE_QUAL::CombineRect(Rect* a_rectA, Rect* a_rectB) {
    ASSERT(a_rectA && a_rectB);
    Rect newRect;
    newRect.m_min[0] = rtree_min(a_rectA->m_min[0], a_rectB->m_min[0]);
    newRect.m_max[0] = rtree_max(a_rectA->m_max[0], a_rectB->m_max[0]);
    newRect.m_min[1] = rtree_min(a_rectA->m_min[1], a_rectB->m_min[1]);
    newRect.m_max[1] = rtree_max(a_rectA->m_max[1], a_rectB->m_max[1]);
    return newRect;
}

/**
 * @class Helper
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {

/** @class StoringVisitor
 * @brief Allows to store the object; used as context while traveling the rtree in TraCI
 */

class Helper {
public:
    /** @brief Connects to the specified SUMO server
    * @param[in] host The name of the host to connect to
    * @param[in] port The port to connect to
    * @exception tcpip::SocketException if the connection fails
    */
    //void connect(const std::string& host, int port);


    /// @brief ends the Helper and closes the connection
    void close();
    /// @}

    /// @brief load a Helper with the given arguments
    static void load(const std::vector<std::string>& args);

    /// @brief Advances by one step (or up to the given time)
    static void HelperStep(const SUMOTime time = 0);

    /// @brief {object->{variable->value}}
    typedef std::map<int, TraCIValue> TraCIValues;
    typedef std::map<std::string, TraCIValues> SubscribedValues;
    typedef std::map<std::string, SubscribedValues> SubscribedContextValues;

    //void subscribe(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars) const;
    //void subscribeContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<int>& vars) const;

    const SubscribedValues& getSubscriptionResults() const;
    const TraCIValues& getSubscriptionResults(const std::string& objID) const;

    const SubscribedContextValues& getContextSubscriptionResults() const;
    const SubscribedValues& getContextSubscriptionResults(const std::string& objID) const;

    /// @brief helper functions
    static TraCIPositionVector makeTraCIPositionVector(const PositionVector& positionVector);
    static TraCIPosition makeTraCIPosition(const Position& position);
    static Position makePosition(const TraCIPosition& position);

    static PositionVector makePositionVector(const TraCIPositionVector& vector);
    static TraCIColor makeTraCIColor(const RGBColor& color);
    static RGBColor makeRGBColor(const TraCIColor& color);

    static MSEdge* getEdge(const std::string& edgeID);
    static const MSLane* getLaneChecking(const std::string& edgeID, int laneIndex, double pos);
    static std::pair<MSLane*, double> convertCartesianToRoadMap(Position pos);

    static SUMOTime getCurrentTime();

    static SUMOTime getDeltaT();

    static TraCIBoundary getNetBoundary();

    static int getMinExpectedNumber();

    static TraCIStage findRoute(const std::string& from, const std::string& to, const std::string& typeID, const SUMOTime depart, const int routingMode);

    static std::vector<TraCIStage> findIntermodalRoute(const std::string& from, const std::string& to, const std::string& modes,
            const SUMOTime depart, const int routingMode, const double speed, const double walkFactor,
            const double departPos, const double arrivalPos, const double departPosLat,
            const std::string& pType, const std::string& vehType);

    static std::string getParameter(const std::string& objectID, const std::string& key);

    static void collectObjectsInRange(int domain, const PositionVector& shape, double range, std::set<std::string>& into);

    static void setRemoteControlled(MSVehicle* v, Position xyPos, MSLane* l, double pos, double posLat, double angle,
                                    int edgeOffset, ConstMSEdgeVector route, SUMOTime t);

    static void setRemoteControlled(MSPerson* p, Position xyPos, MSLane* l, double pos, double posLat, double angle,
                                    int edgeOffset, ConstMSEdgeVector route, SUMOTime t);

    static void postProcessRemoteControl();

    static void cleanup();

    /// @name functions for moveToXY
    /// @{
    static bool moveToXYMap(const Position& pos, double maxRouteDistance, bool mayLeaveNetwork, const std::string& origID, const double angle,
                            double speed, const ConstMSEdgeVector& currentRoute, const int routePosition, MSLane* currentLane, double currentLanePos, bool onRoad,
                            double& bestDistance, MSLane** lane, double& lanePos, int& routeOffset, ConstMSEdgeVector& edges);

    static bool moveToXYMap_matchingRoutePosition(const Position& pos, const std::string& origID,
            const ConstMSEdgeVector& currentRoute, int routeIndex,
            double& bestDistance, MSLane** lane, double& lanePos, int& routeOffset);

    static bool findCloserLane(const MSEdge* edge, const Position& pos, double& bestDistance, MSLane** lane);

    class LaneUtility {
    public:
        LaneUtility(double dist_, double perpendicularDist_, double lanePos_, double angleDiff_, bool ID_,
                    bool onRoute_, bool sameEdge_, const MSEdge* prevEdge_, const MSEdge* nextEdge_) :
            dist(dist_), perpendicularDist(perpendicularDist_), lanePos(lanePos_), angleDiff(angleDiff_), ID(ID_),
            onRoute(onRoute_), sameEdge(sameEdge_), prevEdge(prevEdge_), nextEdge(nextEdge_) {}
        LaneUtility() {}
        ~LaneUtility() {}

        double dist;
        double perpendicularDist;
        double lanePos;
        double angleDiff;
        bool ID;
        bool onRoute;
        bool sameEdge;
        const MSEdge* prevEdge;
        const MSEdge* nextEdge;
    };
    /// @}

private:

    SubscribedValues mySubscribedValues;
    SubscribedContextValues mySubscribedContextValues;

    /// @brief A storage of objects
    static std::map<int, NamedRTree*> myObjects;

    /// @brief A storage of lanes
    static LANE_RTREE_QUAL* myLaneTree;

    static std::map<std::string, MSVehicle*> myRemoteControlledVehicles;
    static std::map<std::string, MSPerson*> myRemoteControlledPersons;

    /// @brief invalidated standard constructor
    Helper();

    /// @brief invalidated copy constructor
    Helper(const Helper& src);

    /// @brief invalidated assignment operator
    Helper& operator=(const Helper& src);
};

}


#endif

/****************************************************************************/
