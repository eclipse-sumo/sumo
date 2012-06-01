/****************************************************************************/
/// @file    TraCIAPI.h
/// @author  Daniel Krajzewicz
/// @date    30.05.2012
/// @version $Id:$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include <string>
#include <foreign/tcpip/socket.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Boundary;
class PositionVector;
class RGBColor;
class Position;


// ===========================================================================
// global definitions
// ===========================================================================
#define DEFAULT_VIEW "View #0"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIAPI
 * @brief C++ TraCI client API implementation
 */
class TraCIAPI {
public:
    /** @brief Constructor
     */
    TraCIAPI();


    /// @brief Destructor
    ~TraCIAPI();


    void connect(const std::string &host, int port) throw(tcpip::SocketException);

    void close();



    SUMOTime getSUMOTime(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    int getUnsignedByte(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    int getByte(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    int getInt(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    SUMOReal getFloat(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    SUMOReal getDouble(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    Boundary getBoundingBox(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    PositionVector getPolygon(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    Position getPosition(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    std::string getString(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    std::vector<std::string> getStringVector(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);
    RGBColor getColor(int cmd, int var, const std::string &id, tcpip::Storage* add=0) throw(tcpip::SocketException);

    

    class TraCIScopeWrapper {
    public:
        TraCIScopeWrapper(TraCIAPI &parent) : myParent(parent) {}
        virtual ~TraCIScopeWrapper() {}
    protected:
        TraCIAPI &myParent;
    };








    class EdgeScope : public TraCIScopeWrapper {
    public:
        EdgeScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        virtual ~EdgeScope() {}

        std::vector<std::string> getIDList() const throw(tcpip::SocketException);
        unsigned int getIDCount() const throw(tcpip::SocketException);
        SUMOReal getAdaptedTraveltime(const std::string &edgeID, SUMOTime time) const throw(tcpip::SocketException);
        SUMOReal getEffort(const std::string &edgeID, SUMOTime time) const throw(tcpip::SocketException);
        SUMOReal getCO2Emission(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getCOEmission(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getHCEmission(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getPMxEmission(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getNOxEmission(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getFuelConsumption(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getNoiseEmission(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getLastStepMeanSpeed(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getLastStepOccupancy(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getLastStepLength(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getTraveltime(const std::string &edgeID) const throw(tcpip::SocketException);
        unsigned int getLastStepVehicleNumber(const std::string &edgeID) const throw(tcpip::SocketException);
        SUMOReal getLastStepHaltingNumber(const std::string &edgeID) const throw(tcpip::SocketException);
        std::vector<std::string> getLastStepVehicleIDs(const std::string &edgeID) const throw(tcpip::SocketException);

        void adaptTraveltime(const std::string &edgeID, SUMOReal time) const throw(tcpip::SocketException);
        void setEffort(const std::string &edgeID, SUMOReal effort) const throw(tcpip::SocketException);
        void setMaxSpeed(const std::string &edgeID, SUMOReal speed) const throw(tcpip::SocketException);

    };


    class GUIScope : public TraCIScopeWrapper {
    public:
        GUIScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        virtual ~GUIScope() {}

        std::vector<std::string> getIDList() const throw(tcpip::SocketException);
        SUMOReal getZoom(const std::string &viewID=DEFAULT_VIEW) const throw(tcpip::SocketException);
        Position getOffset(const std::string &viewID=DEFAULT_VIEW) const throw(tcpip::SocketException);
        std::string getSchema(const std::string &viewID=DEFAULT_VIEW) const throw(tcpip::SocketException);
        Boundary getBoundary(const std::string &viewID=DEFAULT_VIEW) const throw(tcpip::SocketException);
        void setZoom(const std::string &viewID, SUMOReal zoom) const throw(tcpip::SocketException);
        void setOffset(const std::string &viewID, SUMOReal x, SUMOReal y) const throw(tcpip::SocketException);
        void setSchema(const std::string &viewID, const std::string &schemeName) const throw(tcpip::SocketException);
        void setBoundary(const std::string &viewID, SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax) const throw(tcpip::SocketException);
        void screenshot(const std::string &viewID, const std::string &filename) const throw(tcpip::SocketException);
        void trackVehicle(const std::string &viewID, const std::string &vehID) const throw(tcpip::SocketException);

    };


    class InductionLoopScope : public TraCIScopeWrapper {
    public:
        InductionLoopScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        virtual ~InductionLoopScope() {}

        std::vector<std::string> getIDList() const throw(tcpip::SocketException);
        SUMOReal  getPosition(const std::string &loopID) const throw(tcpip::SocketException);
        std::string getLaneID(const std::string &loopID) const throw(tcpip::SocketException);
        unsigned int getLastStepVehicleNumber(const std::string &loopID) const throw(tcpip::SocketException);
        SUMOReal getLastStepMeanSpeed(const std::string &loopID) const throw(tcpip::SocketException);
        std::vector<std::string> getLastStepVehicleIDs(const std::string &loopID) const throw(tcpip::SocketException);
        SUMOReal getLastStepOccupancy(const std::string &loopID) const throw(tcpip::SocketException);
        SUMOReal getLastStepMeanLength(const std::string &loopID) const throw(tcpip::SocketException);
        SUMOReal getTimeSinceDetection(const std::string &loopID) const throw(tcpip::SocketException);
        unsigned int getVehicleData(const std::string &loopID) const throw(tcpip::SocketException);

    };


    class JunctionScope : public TraCIScopeWrapper {
    public:
        JunctionScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        virtual ~JunctionScope() {}

        std::vector<std::string> getIDList() const throw(tcpip::SocketException);
        Position getPosition(const std::string &junctionID) const throw(tcpip::SocketException);

    };


    class LaneScope : public TraCIScopeWrapper {
    public:
        LaneScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        virtual ~LaneScope() {}

        std::vector<std::string> getIDList() const throw(tcpip::SocketException);
        SUMOReal getLength(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getMaxSpeed(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getWidth(const std::string &laneID) const throw(tcpip::SocketException);
        std::vector<std::string> getAllowed(const std::string &laneID) const throw(tcpip::SocketException);
        std::vector<std::string> getDisallowed(const std::string &laneID) const throw(tcpip::SocketException);
        unsigned int getLinkNumber(const std::string &laneID) const throw(tcpip::SocketException);
        PositionVector getShape(const std::string &laneID) const throw(tcpip::SocketException);
        std::string getEdgeID(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getCO2Emission(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getCOEmission(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getHCEmission(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getPMxEmission(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getNOxEmission(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getFuelConsumption(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getNoiseEmission(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getLastStepMeanSpeed(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getLastStepOccupancy(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getLastStepLength(const std::string &laneID) const throw(tcpip::SocketException);
        SUMOReal getTraveltime(const std::string &laneID) const throw(tcpip::SocketException);
        unsigned int getLastStepVehicleNumber(const std::string &laneID) const throw(tcpip::SocketException);
        unsigned int getLastStepHaltingNumber(const std::string &laneID) const throw(tcpip::SocketException);
        std::vector<std::string> getLastStepVehicleIDs(const std::string &laneID) const throw(tcpip::SocketException);

        void setAllowed(const std::string &laneID, const std::vector<std::string> &allowedClasses) const throw(tcpip::SocketException);
        void setDisallowed(const std::string &laneID, const std::vector<std::string> &disallowedClasses) const throw(tcpip::SocketException);
        void setMaxSpeed(const std::string &laneID, SUMOReal speed) const throw(tcpip::SocketException);
        void setLength(const std::string &laneID, SUMOReal length) const throw(tcpip::SocketException);
    };

    class MeMeScope : public TraCIScopeWrapper {
    public:
        MeMeScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        virtual ~MeMeScope() {}

        std::vector<std::string> getIDList() const throw(tcpip::SocketException);
        unsigned int getLastStepVehicleNumber(const std::string &detID) const throw(tcpip::SocketException);
        SUMOReal getLastStepMeanSpeed(const std::string &detID) const throw(tcpip::SocketException);
        std::vector<std::string> getLastStepVehicleIDs(const std::string &detID) const throw(tcpip::SocketException);
        unsigned int getLastStepHaltingNumber(const std::string &detID) const throw(tcpip::SocketException);

    };

    class POIScope : public TraCIScopeWrapper {
    public:
        POIScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        virtual ~POIScope() {}

        std::vector<std::string> getIDList() const throw(tcpip::SocketException);
        std::string getType(const std::string &poiID) const throw(tcpip::SocketException);
        Position getPosition(const std::string &poiID) const throw(tcpip::SocketException);
        RGBColor getColor(const std::string &poiID) const throw(tcpip::SocketException);
    
        void setType(const std::string &poiID, const std::string &setType) const throw(tcpip::SocketException);
        void setPosition(const std::string &poiID, SUMOReal x, SUMOReal y) const throw(tcpip::SocketException);
        void setColor(const std::string &poiID, const RGBColor &c) const throw(tcpip::SocketException);
        void add(const std::string &poiID, SUMOReal x, SUMOReal y, const RGBColor &c, const std::string &type, int layer) const throw(tcpip::SocketException);
        void remove(const std::string &poiID, int layer=0) const throw(tcpip::SocketException);

    };

    class PolygonScope : public TraCIScopeWrapper {
    public:
        PolygonScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        virtual ~PolygonScope() {}

        std::vector<std::string> getIDList() const throw(tcpip::SocketException);
        std::string getType(const std::string &polygonID) const throw(tcpip::SocketException);
        PositionVector getShape(const std::string &polygonID) const throw(tcpip::SocketException);
        RGBColor getColor(const std::string &polygonID) const throw(tcpip::SocketException);
        void setType(const std::string &polygonID, const std::string &setType) const throw(tcpip::SocketException);
        void setShape(const std::string &polygonID, const PositionVector &shape) const throw(tcpip::SocketException);
        void setColor(const std::string &polygonID, const RGBColor &c) const throw(tcpip::SocketException);
        void add(const std::string &polygonID, const PositionVector &shape, const RGBColor &c, bool fill, const std::string &type, int layer) const throw(tcpip::SocketException);
        void remove(const std::string &polygonID, int layer=0) const throw(tcpip::SocketException);

    };


    class RouteScope : public TraCIScopeWrapper {
    public:
        RouteScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        virtual ~RouteScope() {}

        std::vector<std::string> getIDList() const throw(tcpip::SocketException);
        std::vector<std::string> getEdges(const std::string &routeID) const throw(tcpip::SocketException);

        void add(const std::string &routeID, const std::vector<std::string> &edges) const throw(tcpip::SocketException);

    };


    
    class SimulationScope : public TraCIScopeWrapper {
    public:
        SimulationScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        virtual ~SimulationScope() {}

        SUMOTime getCurrentTime() const;
        unsigned int getLoadedNumber() const;
        std::vector<std::string> getLoadedIDList() const;
        unsigned int getDepartedNumber() const;
        std::vector<std::string> getDepartedIDList() const;
        unsigned int getArrivedNumber() const;
        std::vector<std::string> getArrivedIDList() const;
        unsigned int getStartingTeleportNumber() const;
        std::vector<std::string> getStartingTeleportIDList() const;
        unsigned int getEndingTeleportNumber() const;
        std::vector<std::string> getEndingTeleportIDList() const;
        SUMOTime getDeltaT() const;
        Boundary getNetBoundary() const;
        unsigned int getMinExpectedNumber() const;

    };

public:
    EdgeScope edge;
    GUIScope gui;
    InductionLoopScope inductionloop;
    JunctionScope junction;
    LaneScope lane;
    MeMeScope multientryexit;
    POIScope poi;
    PolygonScope polygon;
    RouteScope route;
    SimulationScope simulation;

protected:
    void check_resultState(tcpip::Storage& inMsg, int command, bool ignoreCommandId=false) const throw(tcpip::SocketException);
    void check_commandGetResult(tcpip::Storage& inMsg, int command, int expectedType, bool ignoreCommandId) const throw(tcpip::SocketException);

    void send_commandGetVariable(int domID, int varID, const std::string& objID, tcpip::Storage* add) const throw(tcpip::SocketException);
    void processGET(tcpip::Storage& inMsg, int command, int expectedType, bool ignoreCommandId=false) const throw(tcpip::SocketException);

    void send_commandSetValue(int domID, int varID, const std::string& objID, tcpip::Storage& content) const throw(tcpip::SocketException);

protected:
    tcpip::Socket *mySocket;

};


#endif

/****************************************************************************/

