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
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>



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

    

    class TraCIScopeWrapper {
    public:
        TraCIScopeWrapper(TraCIAPI &parent) : myParent(parent) {}
        virtual ~TraCIScopeWrapper() {}
    protected:
        TraCIAPI &myParent;
    };




    class VehicleScope : public TraCIScopeWrapper {
    public:
        VehicleScope(TraCIAPI &parent) : TraCIScopeWrapper(parent) {}
        ~VehicleScope() {}

        
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

public:
    EdgeScope edge;
    VehicleScope vehicle;
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

