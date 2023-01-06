/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
/// @file    Helper.h
/// @author  Robert Hilbrich
/// @author  Leonhard Luecken
/// @date    15.09.2017
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <vector>
#include <memory>
#include <libsumo/Subscription.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/trigger/MSCalibrator.h>
#include <utils/vehicle/SUMOVehicleParameter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Position;
class PositionVector;
class RGBColor;
class MSEdge;
class SUMOTrafficObject;
class MSPerson;
class MSVehicle;
class MSBaseVehicle;
class MSVehicleType;
class MSStoppingPlace;


// ===========================================================================
// class definitions
// ===========================================================================
namespace libsumo {

/**
* @class Helper
* @brief C++ TraCI client API implementation
*/
class Helper {
public:
    static void subscribe(const int commandId, const std::string& id, const std::vector<int>& variables,
                          const double beginTime, const double endTime, const libsumo::TraCIResults& params,
                          const int contextDomain = 0, const double range = 0.);

    static void handleSubscriptions(const SUMOTime t);

    static bool needNewSubscription(libsumo::Subscription& s, std::vector<Subscription>& subscriptions, libsumo::Subscription*& modifiedSubscription);

    static void clearSubscriptions();

    static Subscription* addSubscriptionFilter(SubscriptionFilterType filter);

    /// @brief helper functions
    static TraCIPositionVector makeTraCIPositionVector(const PositionVector& positionVector);
    static TraCIPosition makeTraCIPosition(const Position& position, const bool includeZ = false);
    static Position makePosition(const TraCIPosition& position);

    static PositionVector makePositionVector(const TraCIPositionVector& vector);
    static TraCIColor makeTraCIColor(const RGBColor& color);
    static RGBColor makeRGBColor(const TraCIColor& color);

    static MSEdge* getEdge(const std::string& edgeID);
    static const MSLane* getLaneChecking(const std::string& edgeID, int laneIndex, double pos);
    static std::pair<MSLane*, double> convertCartesianToRoadMap(const Position& pos, const SUMOVehicleClass vClass);
    static double getDrivingDistance(std::pair<const MSLane*, double>& roadPos1, std::pair<const MSLane*, double>& roadPos2);

    static MSBaseVehicle* getVehicle(const std::string& id);
    static MSPerson* getPerson(const std::string& id);
    static SUMOTrafficObject* getTrafficObject(int domain, const std::string& id);
    static const MSVehicleType& getVehicleType(const std::string& vehicleID);
    static MSTLLogicControl::TLSLogicVariants& getTLS(const std::string& id);
    static MSStoppingPlace* getStoppingPlace(const std::string& id, const SumoXMLTag type);

    static SUMOVehicleParameter::Stop buildStopParameters(const std::string& edgeOrStoppingPlaceID,
            double pos, int laneIndex, double startPos, int flags, double duration, double until);

    static TraCINextStopData buildStopData(const SUMOVehicleParameter::Stop& stopPar);

    static void findObjectShape(int domain, const std::string& id, PositionVector& shape);

    static void collectObjectsInRange(int domain, const PositionVector& shape, double range, std::set<const Named*>& into);
    static void collectObjectIDsInRange(int domain, const PositionVector& shape, double range, std::set<std::string>& into);

    /**
     * @brief Filter the given ID-Set (which was obtained from an R-Tree search)
     *        according to the filters set by the subscription or firstly build the object ID list if
     *        the filters rather demand searching along the road network than considering a geometric range.
     * @param[in] s Subscription which holds the filter specification to be applied
     * @param[in/out] objIDs Set of object IDs that is to be filtered. Result is stored in place.
     * @note Currently this assumes that the objects are vehicles.
     */
    static void applySubscriptionFilters(const Subscription& s, std::set<std::string>& objIDs);

    /**
     * @brief Apply the subscription filter "lanes": Only return vehicles on list of lanes relative to ego vehicle.
     *        Search all predecessor and successor lanes along the road network up until upstreamDist and downstreamDist,
     *        respectively,
     * @param[in] s Subscription which holds the filter specification to be applied.
     * @param[in/out] vehs Set of SUMO traffic objects into which the result is inserted.
     * @param[in] filterLanes Lane offsets to consider.
     * @param[in] downstreamDist Downstream distance.
     * @param[in] upstreamDist Upstream distance.
     * @param[in] disregardOppositeDirection Whether vehicles on opposite lanes shall be taken into account.
     */
    static void applySubscriptionFilterLanes(const Subscription& s, std::set<const SUMOTrafficObject*>& vehs, std::vector<int>& filterLanes,
            double downstreamDist, double upstreamDist, bool disregardOppositeDirection);

    /**
     * @brief Apply the subscription filter "turn": Gather upcoming junctions and vialanes within downstream
     *        distance and find approaching foes within foeDistToJunction.
     * @param[in] s Subscription which holds the filter specification to be applied.
     * @param[in/out] vehs Set of SUMO traffic objects into which the result is inserted.
     */
    static void applySubscriptionFilterTurn(const Subscription& s, std::set<const SUMOTrafficObject*>& vehs);

    static void applySubscriptionFilterFieldOfVision(const Subscription& s, std::set<std::string>& objIDs);

    /**
     * @brief Apply the subscription filter "lateral distance": Only return vehicles within the given lateral distance.
     *        Search myRoute (right-most lane) upstream and bestLanesCont downstream up until upstreamDist and
     *        downstreamDist, respectively.
     * @param[in] s Subscription which holds the filter specification to be applied.
     * @param[in/out] vehs Set of SUMO traffic objects into which the result is inserted.
     * @param[in] downstreamDist Downstream distance.
     * @param[in] upstreamDist Upstream distance.
     * @param[in] lateralDist Lateral distance.
     */
    static void applySubscriptionFilterLateralDistance(const Subscription& s, std::set<const SUMOTrafficObject*>& vehs, double downstreamDist,
            double upstreamDist, double lateralDist);

    static void applySubscriptionFilterLateralDistanceSinglePass(const Subscription& s,
            std::set<std::string>& objIDs,
            std::set<const SUMOTrafficObject*>& vehs,
            const std::vector<const MSLane*>& lanes,
            double posOnLane, double posLat, bool isDownstream);

    static void setRemoteControlled(MSVehicle* v, Position xyPos, MSLane* l, double pos, double posLat, double angle,
                                    int edgeOffset, ConstMSEdgeVector route, SUMOTime t);

    static void setRemoteControlled(MSPerson* p, Position xyPos, MSLane* l, double pos, double posLat, double angle,
                                    int edgeOffset, ConstMSEdgeVector route, SUMOTime t);

    /// @brief return number of remote-controlled entities
    static int postProcessRemoteControl();

    static void cleanup();

    static void registerStateListener();

    static const std::vector<std::string>& getVehicleStateChanges(const MSNet::VehicleState state);

    static const std::vector<std::string>& getTransportableStateChanges(const MSNet::TransportableState state);

    static void clearStateChanges();

    static MSCalibrator::AspiredState getCalibratorState(const MSCalibrator* c);

    /// @name functions for moveToXY
    /// @{
    static bool moveToXYMap(const Position& pos, double maxRouteDistance, bool mayLeaveNetwork, const std::string& origID,
                            const double angle, double speed, const ConstMSEdgeVector& currentRoute, const int routePosition,
                            const MSLane* currentLane, double currentLanePos, bool onRoad, SUMOVehicleClass vClass, bool setLateralPos,
                            double& bestDistance, MSLane** lane, double& lanePos, int& routeOffset, ConstMSEdgeVector& edges);

    static bool moveToXYMap_matchingRoutePosition(const Position& pos, const std::string& origID,
            const ConstMSEdgeVector& currentRoute, int routeIndex,
            SUMOVehicleClass vClass, bool setLateralPos,
            double& bestDistance, MSLane** lane, double& lanePos, int& routeOffset);

    static bool findCloserLane(const MSEdge* edge, const Position& pos, SUMOVehicleClass vClass, double& bestDistance, MSLane** lane);

    class LaneUtility {
    public:
        LaneUtility(double dist_, double perpendicularDist_, double lanePos_, double angleDiff_, bool ID_,
                    bool onRoute_, bool sameEdge_, const MSEdge* prevEdge_, const MSEdge* nextEdge_) :
            dist(dist_), perpendicularDist(perpendicularDist_), lanePos(lanePos_), angleDiff(angleDiff_), ID(ID_),
            onRoute(onRoute_), sameEdge(sameEdge_), prevEdge(prevEdge_), nextEdge(nextEdge_) {}
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

    class SubscriptionWrapper final : public VariableWrapper {
    public:
        SubscriptionWrapper(VariableWrapper::SubscriptionHandler handler, SubscriptionResults& into, ContextSubscriptionResults& context);
        void setContext(const std::string* const refID);
        void clear();
        bool wrapDouble(const std::string& objID, const int variable, const double value);
        bool wrapInt(const std::string& objID, const int variable, const int value);
        bool wrapString(const std::string& objID, const int variable, const std::string& value);
        bool wrapStringList(const std::string& objID, const int variable, const std::vector<std::string>& value);
        bool wrapDoubleList(const std::string& objID, const int variable, const std::vector<double>& value);
        bool wrapPosition(const std::string& objID, const int variable, const TraCIPosition& value);
        bool wrapPositionVector(const std::string& objID, const int variable, const TraCIPositionVector& value);
        bool wrapColor(const std::string& objID, const int variable, const TraCIColor& value);
        bool wrapStringDoublePair(const std::string& objID, const int variable, const std::pair<std::string, double>& value);
        bool wrapStringPair(const std::string& objID, const int variable, const std::pair<std::string, std::string>& value);
        void empty(const std::string& objID);
    private:
        SubscriptionResults& myResults;
        ContextSubscriptionResults& myContextResults;
        SubscriptionResults* myActiveResults;
    private:
        /// @brief Invalidated assignment operator
        SubscriptionWrapper& operator=(const SubscriptionWrapper& s) = delete;
    };

private:
    static void handleSingleSubscription(const Subscription& s);

    /// @brief Adds lane coverage information from newLaneCoverage into aggregatedLaneCoverage
    /// @param[in/out] aggregatedLaneCoverage - aggregated lane coverage info, to which the new will be added
    /// @param[in] newLaneCoverage - new lane coverage to be added
    /// @todo Disjunct ranges are not handled (LaneCoverageInfo definition would need to allow several intervals per lane) but
    ///       the intermediate range is simply assimilated.
    static void fuseLaneCoverage(std::shared_ptr<LaneCoverageInfo> aggregatedLaneCoverage, const std::shared_ptr<LaneCoverageInfo> newLaneCoverage);

    static void debugPrint(const SUMOTrafficObject* veh);

private:
    class VehicleStateListener : public MSNet::VehicleStateListener {
    public:
        void vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& info = "");
        /// @brief Changes in the states of simulated vehicles
        std::map<MSNet::VehicleState, std::vector<std::string> > myVehicleStateChanges;
    };

    class TransportableStateListener : public MSNet::TransportableStateListener {
    public:
        void transportableStateChanged(const MSTransportable* const transportable, MSNet::TransportableState to, const std::string& info = "");
        /// @brief Changes in the states of simulated transportables
        std::map<MSNet::TransportableState, std::vector<std::string> > myTransportableStateChanges;
    };

    /// @brief The list of known, still valid subscriptions
    static std::vector<Subscription> mySubscriptions;

    /// @brief The last context subscription
    static Subscription* myLastContextSubscription;

    /// @brief Map of commandIds -> their executors; applicable if the executor applies to the method footprint
    static std::map<int, std::shared_ptr<VariableWrapper> > myWrapper;

    /// @brief Changes in the states of simulated vehicles
    static VehicleStateListener myVehicleStateListener;

    /// @brief Changes in the states of simulated transportables
    static TransportableStateListener myTransportableStateListener;

    /// @brief A lookup tree of lanes
    static LANE_RTREE_QUAL* myLaneTree;

    static std::map<std::string, MSVehicle*> myRemoteControlledVehicles;
    static std::map<std::string, MSPerson*> myRemoteControlledPersons;

    /// @brief invalidated standard constructor
    Helper() = delete;
};

}
