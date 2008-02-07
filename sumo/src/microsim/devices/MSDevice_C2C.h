/****************************************************************************/
/// @file    MSDevice_C2C.h
/// @author  Michael Behrisch, Daniel Krajzewicz
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// C2C communication and rerouting device
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSDevice_C2C_h
#define MSDevice_C2C_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <vector>
#include <map>
#include "MSDevice.h"
#include <microsim/MSVehicleQuitReminded.h>
#include <utils/common/SUMOTime.h>
#include <microsim/MSVehicle.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSCells;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_C2C
 * @brief C2C communication and rerouting device
 */
class MSDevice_C2C : public MSDevice, public MSVehicleQuitReminded
{
public:
    class Information
    {
    public:
        Information(SUMOReal neededTime_, SUMOTime time_)
                : neededTime(neededTime_), time(time_) { }
        SUMOReal neededTime; // how long needed the vehicle to travel on the edge
        SUMOTime time; // the Time, when the Info was saved
    };

    class C2CConnection
    {
    public:
        C2CConnection(MSDevice_C2C  *connectedVeh_, SUMOTime lastTimeSeen_)
                : connectedVeh(connectedVeh_), lastTimeSeen(lastTimeSeen_) { }
        MSDevice_C2C  *connectedVeh;
        SUMOTime lastTimeSeen;
    };
    typedef std::map<MSDevice_C2C*, C2CConnection*> ConnectionCont;
    typedef std::map<const MSEdge * const, Information *> InfoCont;
    typedef std::vector<C2CConnection*> ClusterCont;



public:
    /** @brief Inserts MSDevice_C2C-options
     */
    static void insertOptions() throw();


    /** @brief Build devices for the given vehicles, if needed
     *
     * The options are read and evaluated whether c2c-devices shall be built
     *  for the given vehicle.
     * Before the first device is built, this implementation builds the list of
     *  neighbor edges, first.
     * For each seen vehicle, the global vehicle index is increased.
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[in] into The vector to store the built device in
     */
    static void buildVehicleDevices(MSVehicle &v, std::vector<MSDevice*> &into) throw();

    /// compute the distances between equipped vehicles
    static void computeCar2Car(SUMOTime t);

private:
    std::map<MSVehicle*, MSDevice_C2C*> myVehiclesToDevicesMap;

    /** @brief Constructor
     */
    MSDevice_C2C(MSVehicle &holder) throw();

    /// Destructor.
    ~MSDevice_C2C() throw();


    // add new neighborhood
    void addNeighbors(std::vector<MSDevice_C2C*>* devices, SUMOTime time);

    /// update the list of neighbors
    void cleanUpConnections(SUMOTime time);

    /// compute the distance between two equipped vehicle
    bool isInDistance(MSVehicle* veh1, MSVehicle* veh2);

    /** C2C: update own information
     * a) insert the current edge if the vehicle is standing for a long period
     * b) remove information older then a specified amount of time (MSGlobals::gLANRefuseOldInfosOffset)
     */
    void updateInfos(SUMOTime time);

    // set the Id of the Cluster to them the vehicle belong
    void setClusterId(int Id);

    // get the Id of the Cluster
    int getClusterId(void) const;

    void sendInfos(SUMOTime time);

    // Build a Cluster for the WLAN simulation
    /* A Cluster is a set of vehicles, where all vehicles or the Neighbor of this Vehicle
     * other the Neighbor of the Neighbor of this Vehicle.
     * A Vehicle cannot belong to two different cluster
     */
    int buildMyCluster(int myStep, int clId);

    void removeOnTripEnd(MSVehicle *veh) throw();




    /** Update of members if vehicle enters a new lane in the move step.
        @param Pointer to the entered Lane. */
    void enterLaneAtMove(MSLane* enteredLane, SUMOReal driven,
                         bool inBetweenJump=false);

    /** Update of members if vehicle enters a new lane in the emit step.
        @param Pointer to the entered Lane. */
    void enterLaneAtEmit(MSLane* enteredLane, const MSVehicle::State &state);

    /** Update of members if vehicle leaves a new lane in the move step. */
    void leaveLaneAtMove(SUMOReal driven);


    void onTripEnd();

    bool knowsEdgeTest(MSEdge &edge) const;

    // enumeration for all type of Connection
    // structure for Car2Car Connection
    const ConnectionCont &getConnections() const;

    SUMOReal getEffort(const MSEdge * const e, SUMOTime t) const;
    void checkReroute(SUMOTime t);

    int getTotalInformationNumber() const {
        return totalNrOfSavedInfos;
    }
    bool hasRouteInformation() const {
        return myHaveRouteInfo;
    }
    SUMOTime getLastInfoTime() const {
        return myLastInfoTime;
    }
    size_t getConnectionsNumber() const {
        return clusterCont.size();
    }
    size_t getInformationNumber() const {
        return infoCont.size();
    }

    size_t getNoGot() const;
    size_t getNoSent() const;
    size_t getNoGotRelevant() const;


    // The time the vehicle waits, may mean the same like myWaitingTime
    int timeSinceStop;

    // the Id of the Cluster
    int clusterId;

    //recent information
    //is saved when the vehicle leaves the lane!!!
    Information *akt;
    SUMOTime myLastInfoTime;
    bool myHaveRouteInfo;

    // count how much Informations this vehicle have saved during the simulation
    int totalNrOfSavedInfos;
    InfoCont infoCont;
    ConnectionCont myNeighbors;
    ClusterCont clusterCont;
    // transfert the N Information in infos into my own InformationsContainer
    void transferInformation(const std::string &senderID, const InfoCont &infos, int N,
                             SUMOTime currentTime);

    //compute accordant the distance, the Number of Infos that can be transmit
    size_t numOfInfos(MSDevice_C2C *veh1, MSDevice_C2C* veh2);


    size_t myNoGot, myNoSent, myNoGotRelevant;


    static int myVehicleIndex;
    static MSCells* myCells;

protected:
    std::string buildID();


private:


    /// Default constructor.
    MSDevice_C2C();

    /// Copy constructor.
    MSDevice_C2C(const MSDevice_C2C&);

    /// Assignment operator.
    MSDevice_C2C& operator=(const MSDevice_C2C&);

};


#endif

/****************************************************************************/

