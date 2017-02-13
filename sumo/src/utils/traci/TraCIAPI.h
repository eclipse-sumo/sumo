/****************************************************************************/
/// @file    TraCIAPI.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCIAPI_h
#define TraCIAPI_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>
#include <foreign/tcpip/socket.h>
#include <traci-server/TraCIConstants.h>

// ===========================================================================
// global definitions
// ===========================================================================
#define DEFAULT_VIEW "View #0"
#define PRECISION 2

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIAPI
 * @brief C++ TraCI client API implementation
 */


typedef long long int SUMOTime; // <utils/common/SUMOTime.h>
#define SUMOTime_MAX std::numeric_limits<SUMOTime>::max()


class TraCIAPI {
public:
    /// @name Structures definitions
    /// @{

    /** @struct TraCIPosition
     * @brief A 3D-position
     */
    struct TraCIPosition {
        double x, y, z;
    };

    /** @struct TraCIPosition
     * @brief A color
     */
    struct TraCIColor {
        int r, g, b, a;
    };

    /** @struct TraCIPositionVector
     * @brief A list of positions
     */
    typedef std::vector<TraCIPosition> TraCIPositionVector;

    /** @struct TraCIBoundary
     * @brief A 3D-bounding box
     */
    struct TraCIBoundary {
        double xMin, yMin, zMin;
        double xMax, yMax, zMax;
    };

    struct TraCIValue {
        union {
            double scalar;
            TraCIPosition position;
            TraCIColor color;
        };
        std::string string;
    };

    class TraCIPhase {
    public:
        TraCIPhase(const SUMOTime _duration, const SUMOTime _duration1, const SUMOTime _duration2, const std::string& _phase)
            : duration(_duration), duration1(_duration1), duration2(_duration2), phase(_phase) {}
        ~TraCIPhase() {}

        SUMOTime duration, duration1, duration2;
        std::string phase;
    };


    class TraCILogic {
    public:
        TraCILogic(const std::string& _subID, int _type, const std::map<std::string, SUMOReal>& _subParameter, int _currentPhaseIndex, const std::vector<TraCIPhase>& _phases)
            : subID(_subID), type(_type), subParameter(_subParameter), currentPhaseIndex(_currentPhaseIndex), phases(_phases) {}
        ~TraCILogic() {}

        std::string subID;
        int type;
        std::map<std::string, SUMOReal> subParameter;
        int currentPhaseIndex;
        std::vector<TraCIPhase> phases;
    };

    class TraCILink {
    public:
        TraCILink(const std::string& _from, const std::string& _via, const std::string& _to)
            : from(_from), via(_via), to(_to) {}
        ~TraCILink() {}

        std::string from;
        std::string via;
        std::string to;
    };

    /// @}



    /** @brief Constructor
     */
    TraCIAPI();


    /// @brief Destructor
    ~TraCIAPI();


    /// @name Connection handling
    /// @{

    /** @brief Connects to the specified SUMO server
     * @param[in] host The name of the host to connect to
     * @param[in] port The port to connect to
     * @exception tcpip::SocketException if the connection fails
     */
    void connect(const std::string& host, int port);


    /// @brief ends the simulation and closes the connection
    void close();
    /// @}

    /// @brief Advances by one step (or up to the given time)
    void simulationStep(SUMOTime time = 0);


    /// @name Atomar getter
    /// @{

    SUMOTime getSUMOTime(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    int getUnsignedByte(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    int getByte(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    int getInt(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    SUMOReal getFloat(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    SUMOReal getDouble(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    TraCIBoundary getBoundingBox(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    TraCIPositionVector getPolygon(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    TraCIPosition getPosition(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    std::string getString(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    std::vector<std::string> getStringVector(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    TraCIColor getColor(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    /// @}


    static const SUMOReal DEPART_NOW;

    /** @class TraCIScopeWrapper
     * @brief An abstract interface for accessing type-dependent values
     *
     * Must be derived by interfaces which implement access methods to certain object types
     */
    class TraCIScopeWrapper {
    public:
        /** @brief Constructor
         * @param[in] parent The parent TraCI client which offers the connection
         */
        TraCIScopeWrapper(TraCIAPI& parent) : myParent(parent) {}

        /// @brief Destructor
        virtual ~TraCIScopeWrapper() {}


    protected:
        /// @brief The parent TraCI client which offers the connection
        TraCIAPI& myParent;


    private:
        /// @brief invalidated copy constructor
        TraCIScopeWrapper(const TraCIScopeWrapper& src);

        /// @brief invalidated assignment operator
        TraCIScopeWrapper& operator=(const TraCIScopeWrapper& src);

    };





    /** @class EdgeScope
     * @brief Scope for interaction with edges
     */
    class EdgeScope : public TraCIScopeWrapper {
    public:
        EdgeScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~EdgeScope() {}

        std::vector<std::string> getIDList() const;
        int getIDCount() const;
        SUMOReal getAdaptedTraveltime(const std::string& edgeID, SUMOReal time) const;
        SUMOReal getEffort(const std::string& edgeID, SUMOTime time) const;
        SUMOReal getCO2Emission(const std::string& edgeID) const;
        SUMOReal getCOEmission(const std::string& edgeID) const;
        SUMOReal getHCEmission(const std::string& edgeID) const;
        SUMOReal getPMxEmission(const std::string& edgeID) const;
        SUMOReal getNOxEmission(const std::string& edgeID) const;
        SUMOReal getFuelConsumption(const std::string& edgeID) const;
        SUMOReal getNoiseEmission(const std::string& edgeID) const;
        SUMOReal getElectricityConsumption(const std::string& edgeID) const;
        SUMOReal getLastStepMeanSpeed(const std::string& edgeID) const;
        SUMOReal getLastStepOccupancy(const std::string& edgeID) const;
        SUMOReal getLastStepLength(const std::string& edgeID) const;
        SUMOReal getTraveltime(const std::string& edgeID) const;
        int getLastStepVehicleNumber(const std::string& edgeID) const;
        SUMOReal getLastStepHaltingNumber(const std::string& edgeID) const;
        std::vector<std::string> getLastStepVehicleIDs(const std::string& edgeID) const;

        void adaptTraveltime(const std::string& edgeID, SUMOReal time, SUMOReal begin = 0, SUMOReal end = SUMOTime_MAX / 1000.0) const;
        void setEffort(const std::string& edgeID, SUMOReal effort, SUMOTime begin = 0, SUMOTime end = SUMOTime_MAX) const;
        void setMaxSpeed(const std::string& edgeID, SUMOReal speed) const;

    private:
        /// @brief invalidated copy constructor
        EdgeScope(const EdgeScope& src);

        /// @brief invalidated assignment operator
        EdgeScope& operator=(const EdgeScope& src);

    };





    /** @class GUIScope
     * @brief Scope for interaction with the gui
     */
    class GUIScope : public TraCIScopeWrapper {
    public:
        GUIScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~GUIScope() {}

        std::vector<std::string> getIDList() const;
        SUMOReal getZoom(const std::string& viewID = DEFAULT_VIEW) const;
        TraCIPosition getOffset(const std::string& viewID = DEFAULT_VIEW) const;
        std::string getSchema(const std::string& viewID = DEFAULT_VIEW) const;
        TraCIBoundary getBoundary(const std::string& viewID = DEFAULT_VIEW) const;
        void setZoom(const std::string& viewID, SUMOReal zoom) const;
        void setOffset(const std::string& viewID, SUMOReal x, SUMOReal y) const;
        void setSchema(const std::string& viewID, const std::string& schemeName) const;
        void setBoundary(const std::string& viewID, SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax) const;
        void screenshot(const std::string& viewID, const std::string& filename) const;
        void trackVehicle(const std::string& viewID, const std::string& vehID) const;

    private:
        /// @brief invalidated copy constructor
        GUIScope(const GUIScope& src);

        /// @brief invalidated assignment operator
        GUIScope& operator=(const GUIScope& src);

    };





    /** @class InductionLoopScope
     * @brief Scope for interaction with inductive loops
     */
    class InductionLoopScope : public TraCIScopeWrapper {
    public:
        InductionLoopScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~InductionLoopScope() {}

        struct VehicleData {
            /* @brief Constructor
              (mirrors MSInductLoop::VehicleData) */
            VehicleData() {}
            /// @brief The id of the vehicle
            std::string id;
            /// @brief Length of the vehicle
            SUMOReal length;
            /// @brief Entry-time of the vehicle in [s]
            SUMOReal entryTime;
            /// @brief Leave-time of the vehicle in [s]
            SUMOReal leaveTime;
            /// @brief Type of the vehicle in
            std::string typeID;
        };

        std::vector<std::string> getIDList() const;
        SUMOReal  getPosition(const std::string& loopID) const;
        std::string getLaneID(const std::string& loopID) const;
        int getLastStepVehicleNumber(const std::string& loopID) const;
        SUMOReal getLastStepMeanSpeed(const std::string& loopID) const;
        std::vector<std::string> getLastStepVehicleIDs(const std::string& loopID) const;
        SUMOReal getLastStepOccupancy(const std::string& loopID) const;
        SUMOReal getLastStepMeanLength(const std::string& loopID) const;
        SUMOReal getTimeSinceDetection(const std::string& loopID) const;
        std::vector<VehicleData> getVehicleData(const std::string& loopID) const;


    private:
        /// @brief invalidated copy constructor
        InductionLoopScope(const InductionLoopScope& src);

        /// @brief invalidated assignment operator
        InductionLoopScope& operator=(const InductionLoopScope& src);

    };





    /** @class JunctionScope
     * @brief Scope for interaction with junctions
     */
    class JunctionScope : public TraCIScopeWrapper {
    public:
        JunctionScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~JunctionScope() {}

        std::vector<std::string> getIDList() const;
        TraCIPosition getPosition(const std::string& junctionID) const;

    private:
        /// @brief invalidated copy constructor
        JunctionScope(const JunctionScope& src);

        /// @brief invalidated assignment operator
        JunctionScope& operator=(const JunctionScope& src);

    };





    /** @class LaneScope
     * @brief Scope for interaction with lanes
     */
    class LaneScope : public TraCIScopeWrapper {
    public:
        LaneScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~LaneScope() {}

        std::vector<std::string> getIDList() const;
        SUMOReal getLength(const std::string& laneID) const;
        SUMOReal getMaxSpeed(const std::string& laneID) const;
        SUMOReal getWidth(const std::string& laneID) const;
        std::vector<std::string> getAllowed(const std::string& laneID) const;
        std::vector<std::string> getDisallowed(const std::string& laneID) const;
        int getLinkNumber(const std::string& laneID) const;
        TraCIPositionVector getShape(const std::string& laneID) const;
        std::string getEdgeID(const std::string& laneID) const;
        SUMOReal getCO2Emission(const std::string& laneID) const;
        SUMOReal getCOEmission(const std::string& laneID) const;
        SUMOReal getHCEmission(const std::string& laneID) const;
        SUMOReal getPMxEmission(const std::string& laneID) const;
        SUMOReal getNOxEmission(const std::string& laneID) const;
        SUMOReal getFuelConsumption(const std::string& laneID) const;
        SUMOReal getNoiseEmission(const std::string& laneID) const;
        SUMOReal getElectricityConsumption(const std::string& laneID) const;
        SUMOReal getLastStepMeanSpeed(const std::string& laneID) const;
        SUMOReal getLastStepOccupancy(const std::string& laneID) const;
        SUMOReal getLastStepLength(const std::string& laneID) const;
        SUMOReal getTraveltime(const std::string& laneID) const;
        int getLastStepVehicleNumber(const std::string& laneID) const;
        int getLastStepHaltingNumber(const std::string& laneID) const;
        std::vector<std::string> getLastStepVehicleIDs(const std::string& laneID) const;

        void setAllowed(const std::string& laneID, const std::vector<std::string>& allowedClasses) const;
        void setDisallowed(const std::string& laneID, const std::vector<std::string>& disallowedClasses) const;
        void setMaxSpeed(const std::string& laneID, SUMOReal speed) const;
        void setLength(const std::string& laneID, SUMOReal length) const;

    private:
        /// @brief invalidated copy constructor
        LaneScope(const LaneScope& src);

        /// @brief invalidated assignment operator
        LaneScope& operator=(const LaneScope& src);

    };


    /** @class AreaScope
    * @brief Scope for interaction with areal detectors
    */
    class AreaScope : public TraCIScopeWrapper {
    public:
        AreaScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~AreaScope() {}

        std::vector<std::string> getIDList() const;
        int getJamLengthVehicle(const std::string& laneID) const;
        SUMOReal getJamLengthMeters(const std::string& laneID) const;

    private:
        /// @brief invalidated copy constructor
        AreaScope(const AreaScope& src);

        /// @brief invalidated assignment operator
        AreaScope& operator=(const AreaScope& src);

    };


    /** @class MeMeScope
     * @brief Scope for interaction with multi entry/-exit detectors
     */
    class MeMeScope : public TraCIScopeWrapper {
    public:
        MeMeScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~MeMeScope() {}

        std::vector<std::string> getIDList() const;
        int getLastStepVehicleNumber(const std::string& detID) const;
        SUMOReal getLastStepMeanSpeed(const std::string& detID) const;
        std::vector<std::string> getLastStepVehicleIDs(const std::string& detID) const;
        int getLastStepHaltingNumber(const std::string& detID) const;

    private:
        /// @brief invalidated copy constructor
        MeMeScope(const MeMeScope& src);

        /// @brief invalidated assignment operator
        MeMeScope& operator=(const MeMeScope& src);

    };





    /** @class POIScope
     * @brief Scope for interaction with POIs
     */
    class POIScope : public TraCIScopeWrapper {
    public:
        POIScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~POIScope() {}

        std::vector<std::string> getIDList() const;
        std::string getType(const std::string& poiID) const;
        TraCIPosition getPosition(const std::string& poiID) const;
        TraCIColor getColor(const std::string& poiID) const;

        void setType(const std::string& poiID, const std::string& setType) const;
        void setPosition(const std::string& poiID, SUMOReal x, SUMOReal y) const;
        void setColor(const std::string& poiID, const TraCIColor& c) const;
        void add(const std::string& poiID, SUMOReal x, SUMOReal y, const TraCIColor& c, const std::string& type, int layer) const;
        void remove(const std::string& poiID, int layer = 0) const;

    private:
        /// @brief invalidated copy constructor
        POIScope(const POIScope& src);

        /// @brief invalidated assignment operator
        POIScope& operator=(const POIScope& src);

    };





    /** @class PolygonScope
     * @brief Scope for interaction with polygons
     */
    class PolygonScope : public TraCIScopeWrapper {
    public:
        PolygonScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~PolygonScope() {}

        std::vector<std::string> getIDList() const;
        std::string getType(const std::string& polygonID) const;
        TraCIPositionVector getShape(const std::string& polygonID) const;
        TraCIColor getColor(const std::string& polygonID) const;
        void setType(const std::string& polygonID, const std::string& setType) const;
        void setShape(const std::string& polygonID, const TraCIPositionVector& shape) const;
        void setColor(const std::string& polygonID, const TraCIColor& c) const;
        void add(const std::string& polygonID, const TraCIPositionVector& shape, const TraCIColor& c, bool fill, const std::string& type, int layer) const;
        void remove(const std::string& polygonID, int layer = 0) const;

    private:
        /// @brief invalidated copy constructor
        PolygonScope(const PolygonScope& src);

        /// @brief invalidated assignment operator
        PolygonScope& operator=(const PolygonScope& src);

    };





    /** @class RouteScope
     * @brief Scope for interaction with routes
     */
    class RouteScope : public TraCIScopeWrapper {
    public:
        RouteScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~RouteScope() {}

        std::vector<std::string> getIDList() const;
        std::vector<std::string> getEdges(const std::string& routeID) const;

        void add(const std::string& routeID, const std::vector<std::string>& edges) const;

    private:
        /// @brief invalidated copy constructor
        RouteScope(const RouteScope& src);

        /// @brief invalidated assignment operator
        RouteScope& operator=(const RouteScope& src);

    };



    /// @brief {object->{variable->value}}
    typedef std::map<int, TraCIValue> TraCIValues;
    typedef std::map<std::string, TraCIValues> SubscribedValues;
    typedef std::map<std::string, SubscribedValues> SubscribedContextValues;


    /** @class SimulationScope
     * @brief Scope for interaction with the simulation
     */
    class SimulationScope : public TraCIScopeWrapper {
    public:
        SimulationScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~SimulationScope() {}

        SUMOTime getCurrentTime() const;
        int getLoadedNumber() const;
        std::vector<std::string> getLoadedIDList() const;
        int getDepartedNumber() const;
        std::vector<std::string> getDepartedIDList() const;
        int getArrivedNumber() const;
        std::vector<std::string> getArrivedIDList() const;
        int getStartingTeleportNumber() const;
        std::vector<std::string> getStartingTeleportIDList() const;
        int getEndingTeleportNumber() const;
        std::vector<std::string> getEndingTeleportIDList() const;
        SUMOTime getDeltaT() const;
        TraCIBoundary getNetBoundary() const;
        int getMinExpectedNumber() const;

        void subscribe(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars) const;
        void subscribeContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, SUMOReal range, const std::vector<int>& vars) const;

        SubscribedValues getSubscriptionResults();
        TraCIValues getSubscriptionResults(const std::string& objID);

        SubscribedContextValues getContextSubscriptionResults();
        SubscribedValues getContextSubscriptionResults(const std::string& objID);

    private:
        /// @brief invalidated copy constructor
        SimulationScope(const SimulationScope& src);

        /// @brief invalidated assignment operator
        SimulationScope& operator=(const SimulationScope& src);

    };





    /** @class TrafficLightScope
     * @brief Scope for interaction with traffic lights
     */
    class TrafficLightScope : public TraCIScopeWrapper {
    public:
        TrafficLightScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~TrafficLightScope() {}

        std::vector<std::string> getIDList() const;
        std::string getRedYellowGreenState(const std::string& tlsID) const;
        std::vector<TraCIAPI::TraCILogic> getCompleteRedYellowGreenDefinition(const std::string& tlsID) const;
        std::vector<std::string> getControlledLanes(const std::string& tlsID) const;
        std::vector<TraCIAPI::TraCILink> getControlledLinks(const std::string& tlsID) const;
        std::string getProgram(const std::string& tlsID) const;
        int getPhase(const std::string& tlsID) const;
        int getNextSwitch(const std::string& tlsID) const;

        void setRedYellowGreenState(const std::string& tlsID, const std::string& state) const;
        void setPhase(const std::string& tlsID, int index) const;
        void setProgram(const std::string& tlsID, const std::string& programID) const;
        void setPhaseDuration(const std::string& tlsID, int phaseDuration) const;
        void setCompleteRedYellowGreenDefinition(const std::string& tlsID, const TraCIAPI::TraCILogic& logic) const;

    private:
        /// @brief invalidated copy constructor
        TrafficLightScope(const TrafficLightScope& src);

        /// @brief invalidated assignment operator
        TrafficLightScope& operator=(const TrafficLightScope& src);

    };





    /** @class VehicleTypeScope
     * @brief Scope for interaction with vehicle types
     */
    class VehicleTypeScope : public TraCIScopeWrapper {
    public:
        VehicleTypeScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~VehicleTypeScope() {}

        std::vector<std::string> getIDList() const;
        SUMOReal getLength(const std::string& typeID) const;
        SUMOReal getMaxSpeed(const std::string& typeID) const;
        SUMOReal getSpeedFactor(const std::string& typeID) const;
        SUMOReal getSpeedDeviation(const std::string& typeID) const;
        SUMOReal getAccel(const std::string& typeID) const;
        SUMOReal getDecel(const std::string& typeID) const;
        SUMOReal getImperfection(const std::string& typeID) const;
        SUMOReal getTau(const std::string& typeID) const;
        std::string getVehicleClass(const std::string& typeID) const;
        std::string getEmissionClass(const std::string& typeID) const;
        std::string getShapeClass(const std::string& typeID) const;
        SUMOReal getMinGap(const std::string& typeID) const;
        SUMOReal getWidth(const std::string& typeID) const;
        SUMOReal getHeight(const std::string& typeID) const;
        TraCIColor getColor(const std::string& typeID) const;

        void setLength(const std::string& typeID, SUMOReal length) const;
        void setMaxSpeed(const std::string& typeID, SUMOReal speed) const;
        void setVehicleClass(const std::string& typeID, const std::string& clazz) const;
        void setSpeedFactor(const std::string& typeID, SUMOReal factor) const;
        void setSpeedDeviation(const std::string& typeID, SUMOReal deviation) const;
        void setEmissionClass(const std::string& typeID, const std::string& clazz) const;
        void setShapeClass(const std::string& typeID, const std::string& shapeClass) const;
        void setWidth(const std::string& typeID, SUMOReal width) const;
        void setHeight(const std::string& typeID, SUMOReal height) const;
        void setMinGap(const std::string& typeID, SUMOReal minGap) const;
        void setAccel(const std::string& typeID, SUMOReal accel) const;
        void setDecel(const std::string& typeID, SUMOReal decel) const;
        void setImperfection(const std::string& typeID, SUMOReal imperfection) const;
        void setTau(const std::string& typeID, SUMOReal tau) const;
        void setColor(const std::string& typeID, const TraCIColor& c) const;

    private:
        /// @brief invalidated copy constructor
        VehicleTypeScope(const VehicleTypeScope& src);

        /// @brief invalidated assignment operator
        VehicleTypeScope& operator=(const VehicleTypeScope& src);

    };





    /** @class VehicleScope
     * @brief Scope for interaction with vehicles
     */
    class VehicleScope : public TraCIScopeWrapper {
    public:
        VehicleScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~VehicleScope() {}


        struct NextTLSData {
            /* @brief Constructor */
            NextTLSData() {}
            /// @brief The id of the next tls
            std::string id;
            /// @brief The tls index of the controlled link
            int tlIndex;
            /// @brief The distance to the tls
            SUMOReal dist;
            /// @brief The current state of the tls
            char state;
        };


        std::vector<std::string> getIDList() const;
        int getIDCount() const;
        SUMOReal getSpeed(const std::string& vehicleID) const;
        TraCIPosition getPosition(const std::string& vehicleID) const;
        SUMOReal getAngle(const std::string& vehicleID) const;
        std::string getRoadID(const std::string& vehicleID) const;
        std::string getLaneID(const std::string& vehicleID) const;
        int getLaneIndex(const std::string& vehicleID) const;
        std::string getTypeID(const std::string& vehicleID) const;
        std::string getRouteID(const std::string& vehicleID) const;
        int getRouteIndex(const std::string& vehicleID) const;
        std::vector<std::string> getEdges(const std::string& vehicleID) const;
        TraCIColor getColor(const std::string& vehicleID) const;
        SUMOReal getLanePosition(const std::string& vehicleID) const;
        SUMOReal getCO2Emission(const std::string& vehicleID) const;
        SUMOReal getCOEmission(const std::string& vehicleID) const;
        SUMOReal getHCEmission(const std::string& vehicleID) const;
        SUMOReal getPMxEmission(const std::string& vehicleID) const;
        SUMOReal getNOxEmission(const std::string& vehicleID) const;
        SUMOReal getFuelConsumption(const std::string& vehicleID) const;
        SUMOReal getNoiseEmission(const std::string& vehicleID) const;
        SUMOReal getElectricityConsumption(const std::string& vehicleID) const;
        int getSignalStates(const std::string& vehicleID) const;
        SUMOReal getWaitingTime(const std::string& vehicleID) const;
        std::vector<NextTLSData> getNextTLS(const std::string& vehID) const;
        int getSpeedMode(const std::string& vehicleID) const;
        SUMOReal getSlope(const std::string& vehicleID) const;
        std::string getLine(const std::string& vehicleID) const;
        std::vector<std::string> getVia(const std::string& vehicleID) const;
        std::string getEmissionClass(const std::string& vehicleID) const;
        std::string getShapeClass(const std::string& vehicleID) const;

        /* /// not yet implemented
        SUMOReal getCO2Emissions(const std::string& vehicleID) const;
        SUMOReal getCOEmissions(const std::string& vehicleID) const;
        SUMOReal getHCEmissions(const std::string& vehicleID) const;
        SUMOReal getPMxEmissions(const std::string& vehicleID) const;
        SUMOReal getNOxEmissions(const std::string& vehicleID) const;
        SUMOReal getFuelConsumption(const std::string& vehicleID) const;
        SUMOReal getNoiseEmission(const std::string& vehicleID) const;
        int getBestLanes(const std::string& vehicleID) const;
        int getStopState(const std::string& vehicleID) const;
        SUMOReal getLength(const std::string& vehicleID) const;
        SUMOReal getMaxSpeed(const std::string& vehicleID) const;
        SUMOReal getAccel(const std::string& vehicleID) const;
        SUMOReal getDecel(const std::string& vehicleID) const;
        SUMOReal getTau(const std::string& vehicleID) const;
        SUMOReal getImperfection(const std::string& vehicleID) const;
        SUMOReal getSpeedFactor(const std::string& vehicleID) const;
        SUMOReal getSpeedDeviation(const std::string& vehicleID) const;
        std::string getVClass(const std::string& vehicleID) const;
        SUMOReal getMinGap(const std::string& vehicleID) const;
        SUMOReal getWidth(const std::string& vehicleID) const;
        */

        void add(const std::string& vehicleID,
                 const std::string& routeID,
                 const std::string& typeID = "DEFAULT_VEHTYPE",
                 std::string depart = "-1",
                 const std::string& departLane = "first",
                 const std::string& departPos = "base",
                 const std::string& departSpeed = "0",
                 const std::string& arrivalLane = "current",
                 const std::string& arrivalPos = "max",
                 const std::string& arrivalSpeed = "current",
                 const std::string& fromTaz = "",
                 const std::string& toTaz = "",
                 const std::string& line = "",
                 int personCapacity = 0,
                 int personNumber = 0) const;

        void moveTo(const std::string& vehicleID, const std::string& laneID, SUMOReal position) const;
        void moveToXY(const std::string& vehicleID, const std::string& edgeID, const int lane, const SUMOReal x, const SUMOReal y, const SUMOReal angle, const int keepRoute) const;
        void slowDown(const std::string& vehicleID, SUMOReal speed, int duration) const;
        void setSpeed(const std::string& vehicleID, SUMOReal speed) const;
        void remove(const std::string& vehicleID, char reason = REMOVE_VAPORIZED) const;
        void setColor(const std::string& vehicleID, const TraCIColor& c) const;
        void setLine(const std::string& vehicleID, const std::string& line) const;
        void setVia(const std::string& vehicleID, const std::vector<std::string>& via) const;
        void setShapeClass(const std::string& vehicleID, const std::string& clazz) const;
        void setEmissionClass(const std::string& vehicleID, const std::string& clazz) const;

    private:
        /// @brief invalidated copy constructor
        VehicleScope(const VehicleScope& src);

        /// @brief invalidated assignment operator
        VehicleScope& operator=(const VehicleScope& src);

    };

    /** @class PersonScope
     * * @brief Scope for interaction with vehicles
     * */
    class PersonScope : public TraCIScopeWrapper {
    public:
        PersonScope(TraCIAPI& parent) : TraCIScopeWrapper(parent) {}
        virtual ~PersonScope() {}

        std::vector<std::string> getIDList() const;
        int getIDCount() const;
        SUMOReal getSpeed(const std::string& personID) const;
        TraCIPosition getPosition(const std::string& personID) const;
        std::string getRoadID(const std::string& personID) const;
        std::string getTypeID(const std::string& personID) const;
        SUMOReal getWaitingTime(const std::string& personID) const;
        std::string getNextEdge(const std::string& personID) const;
        std::string getVehicle(const std::string& personID) const;
        int getRemainingStages(const std::string& personID) const;
        int getStage(const std::string& personID, int nextStageIndex = 0) const;
        std::vector<std::string> getEdges(const std::string& personID, int nextStageIndex = 0) const;

        void removeStages(const std::string& personID) const;
        void add(const std::string& personID, const std::string& edgeID, SUMOReal pos, SUMOReal depart = DEPART_NOW, const std::string typeID = "DEFAULT_PEDTYPE");
        void appendWaitingStage(const std::string& personID, SUMOReal duration, const std::string& description = "waiting", const std::string& stopID = "");
        void appendWalkingStage(const std::string& personID, const std::vector<std::string>& edges, SUMOReal arrivalPos, SUMOReal duration = -1, SUMOReal speed = -1, const std::string& stopID = "");
        void appendDrivingStage(const std::string& personID, const std::string& toEdge, const std::string& lines, const std::string& stopID = "");
        void removeStage(const std::string& personID, int nextStageIndex) const;
        void setSpeed(const std::string& personID, SUMOReal speed) const;
        void setType(const std::string& personID, const std::string& typeID) const;
        void setLength(const std::string& personID, SUMOReal length) const;
        void setWidth(const std::string& personID, SUMOReal width) const;
        void setHeight(const std::string& personID, SUMOReal height) const;
        void setMinGap(const std::string& personID, SUMOReal minGap) const;
        void setColor(const std::string& personID, const TraCIColor& c) const;

    private:
        /// @brief invalidated copy constructor
        PersonScope(const PersonScope& src);

        /// @brief invalidated assignment operator
        PersonScope& operator=(const PersonScope& src);
    };



public:
    /// @brief Scope for interaction with edges
    EdgeScope edge;
    /// @brief Scope for interaction with the gui
    GUIScope gui;
    /// @brief Scope for interaction with inductive loops
    InductionLoopScope inductionloop;
    /// @brief Scope for interaction with junctions
    JunctionScope junction;
    /// @brief Scope for interaction with lanes
    LaneScope lane;
    /// @brief Scope for interaction with multi-entry/-exit detectors
    MeMeScope multientryexit;
    /// @brief Scope for interaction with POIs
    POIScope poi;
    /// @brief Scope for interaction with polygons
    PolygonScope polygon;
    /// @brief Scope for interaction with routes
    RouteScope route;
    /// @brief Scope for interaction with the simulation
    SimulationScope simulation;
    /// @brief Scope for interaction with traffic lights
    TrafficLightScope trafficlights;
    /// @brief Scope for interaction with vehicle types
    VehicleTypeScope vehicletype;
    /// @brief Scope for interaction with vehicles
    VehicleScope vehicle;
    /// @brief Scope for interaction with persons
    PersonScope person;


protected:
    /// @name Command sending methods
    /// @{

    /** @brief Sends a SimulationStep command
     */
    void send_commandSimulationStep(SUMOTime time) const;


    /** @brief Sends a Close command
     */
    void send_commandClose() const;


    /** @brief Sends a GetVariable request
     * @param[in] domID The domain of the variable
     * @param[in] varID The variable to retrieve
     * @param[in] objID The object to retrieve the variable from
     * @param[in] add Optional additional parameter
     */
    void send_commandGetVariable(int domID, int varID, const std::string& objID, tcpip::Storage* add = 0) const;


    /** @brief Sends a SetVariable request
     * @param[in] domID The domain of the variable
     * @param[in] varID The variable to set
     * @param[in] objID The object to change
     * @param[in] content The value of the variable
     */
    void send_commandSetValue(int domID, int varID, const std::string& objID, tcpip::Storage& content) const;


    /** @brief Sends a SubscribeVariable request
     * @param[in] domID The domain of the variable
     * @param[in] objID The object to subscribe the variables from
     * @param[in] beginTime The begin time step of subscriptions
     * @param[in] endTime The end time step of subscriptions
     * @param[in] vars The variables to subscribe
     */
    void send_commandSubscribeObjectVariable(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars) const;


    /** @brief Sends a SubscribeContext request
     * @param[in] domID The domain of the variable
     * @param[in] objID The object to subscribe the variables from
     * @param[in] beginTime The begin time step of subscriptions
     * @param[in] endTime The end time step of subscriptions
     * @param[in] domain The domain of the objects which values shall be returned
     * @param[in] range The range around the obj to investigate
     * @param[in] vars The variables to subscribe
     */
    void send_commandSubscribeObjectContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime,
                                            int domain, SUMOReal range, const std::vector<int>& vars) const;
    /// @}


    void send_commandMoveToXY(const std::string& vehicleID, const std::string& edgeID, const int lane,
                              const SUMOReal x, const SUMOReal y, const SUMOReal angle, const int keepRoute) const;


    /// @name Command sending methods
    /// @{

    /** @brief Validates the result state of a command
     * @param[in] inMsg The buffer to read the message from
     * @param[in] command The original command id
     * @param[in] ignoreCommandId Whether the returning command id shall be validated
     * @param[in] acknowledgement Pointer to an existing string into which the acknowledgement message shall be inserted
     */
    void check_resultState(tcpip::Storage& inMsg, int command, bool ignoreCommandId = false, std::string* acknowledgement = 0) const;

    /** @brief Validates the result state of a command
     * @return The command Id
     */
    int check_commandGetResult(tcpip::Storage& inMsg, int command, int expectedType = -1, bool ignoreCommandId = false) const;

    void processGET(tcpip::Storage& inMsg, int command, int expectedType, bool ignoreCommandId = false) const;
    /// @}

    void readVariableSubscription(tcpip::Storage& inMsg);
    void readContextSubscription(tcpip::Storage& inMsg);
    void readVariables(tcpip::Storage& inMsg, const std::string& objectID, int variableCount, SubscribedValues& into);

    template <class T>
    static inline std::string toString(const T& t, std::streamsize accuracy = PRECISION) {
        std::ostringstream oss;
        oss.setf(std::ios::fixed , std::ios::floatfield);
        oss << std::setprecision(accuracy);
        oss << t;
        return oss.str();
    }

    /// @brief Closes the connection
    void closeSocket();

protected:
    /// @brief The socket
    tcpip::Socket* mySocket;

    SubscribedValues mySubscribedValues;
    SubscribedContextValues mySubscribedContextValues;
};


#endif

/****************************************************************************/

