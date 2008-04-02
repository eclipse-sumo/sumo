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
#include <utils/common/StaticCommand.h>
#include <microsim/MSVehicle.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSCells;
class MSLane;
class MSEdge;
class Position2DVector;


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
    /** @brief Inserts MSDevice_C2C-options
     */
    static void insertOptions() throw();


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether c2c-devices shall be built
     *  for the given vehicle.
     *
     * Before the first device is built, this implementation builds the list of
     *  neighbor edges, first.
     *
     * For each seen vehicle, the global vehicle index is increased.
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[in, filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(MSVehicle &v, std::vector<MSDevice*> &into) throw();


public:
    /** 
     * @class Information
     * @brief An (exchanged) information about an edge
     */
    class Information
    {
    public:
        /// @brief Constructor
        Information(SUMOReal neededTime_, SUMOTime time_)
                : neededTime(neededTime_), time(time_) { }

        /// @brief How long the vehicle needed to travel on the edge
        SUMOReal neededTime;
        /// @brief The time at which the information was generated
        SUMOTime time;
    };


    /** 
     * @class C2CConnection
     * @brief Description of a connection to another vehicle
     */
    class C2CConnection
    {
    public:
        /// @brief Constructor
        C2CConnection(MSDevice_C2C *connectedVeh_, SUMOTime lastTimeSeen_)
                : connectedVeh(connectedVeh_), lastTimeSeen(lastTimeSeen_) { }

        /// @brief The connected vehicle
        MSDevice_C2C  *connectedVeh;
        /// @brief The last time step the vehicle was visible
        SUMOTime lastTimeSeen;
    };


    /// @brief Definition of a container of connections (connected device -> connection information)
    typedef std::map<MSDevice_C2C*, C2CConnection*> ConnectionCont;

    /// @brief Definition of a container of edge information (edge -> effort information)
    typedef std::map<const MSEdge * const, Information *> InfoCont;

    /// @brief Definition of a container of a connection cluster
    typedef std::vector<C2CConnection*> ClusterCont;


public:
    /// compute the distances between equipped vehicles
    static SUMOTime computeC2CExecute(SUMOTime t);


public:
    void removeOnTripEnd(MSVehicle *veh) throw();



    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Update if vehicle enters a new lane in the move step.
     *
     * Deletes the last information ("akt") and builds a new one for
     *  the current lane in "akt".
     *
     * @param[in] enteredLane The lane the vehicle enters (unused)
     * @param[in] driven The distance driven by the vehicle within this time step (unused)
     */
    void enterLaneAtMove(MSLane* enteredLane, SUMOReal driven);


    /** @brief Update of members if vehicle enters a new lane in the emit step
     *
     * Deletes the last information ("akt") and builds a new one for
     *  the current lane in "akt".
     *
     * @param[in] enteredLane The lane the vehicle enters (unused)
     * @param[in] state The vehicle's state during the emission (unused)
     */
    void enterLaneAtEmit(MSLane* enteredLane, const MSVehicle::State &state);


    /** @brief Update of members if vehicle leaves a new lane in the move step.
     *
     * Computes the factor by which the edge was passed slower than estimated.
     *  If the factor is higher than the one defined in "device.c2x.insert-info-factor"
     *  and the travel time was larger than 10s, then the information about the 
     *  passed edge is inserted into the information container (if an information
     *  about the edge was existing, it is updated). Otherwise, the information
     *  about the edge is removed from the container.
     *
     * Deletes the last information ("akt").
     *
     * @param[in] driven The distance driven by the vehicle within this time step
     * @todo Make the 10s variable
     */
    void leaveLaneAtMove(SUMOReal driven);


    /** @brief Called when the vehicle leaves the lane 
     *
     * Removes all information stored in clusters, connections, and edge information
     *  ("clusterCont", "myNeighbors", "infoCont").
     */
    void onTripEnd();
    // @}



    /** @brief Returns the effort to pass an edge
     *
     * This method is given to the used router in order to obtain the efforts
     *  to pass an edge. If the given vehicle knows the edge (either because
     *  it has collected or because it has received an information about it), 
     *  this information is used. Otherwise, we ask the edge for the effort.
     *
     * !!! which edge information is used?
     *
     * @param[in] e The edge for which the effort to be passed shall be returned
     * @param[in] v The vehicle that is rerouted
     * @param[in] t The time for which the effort shall be returned
     * @return The effort (time to pass in this case) for an edge
     * @see SUMODijkstraRouter_ByProxi
     */
    SUMOReal getEffort(const MSEdge * const e, const MSVehicle * const v, SUMOReal t) const;


private:
    /** @brief Constructor
     * 
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_C2C(MSVehicle &holder, const std::string &id) throw();


    /// @brief Destructor.
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


    bool knowsEdgeTest(MSEdge &edge) const;

    // enumeration for all type of Connection
    // structure for Car2Car Connection
    const ConnectionCont &getConnections() const;

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
    void addVehNeighbors(MSDevice_C2C *veh, SUMOTime time);

    size_t getNoGot() const;
    size_t getNoSent() const;
    size_t getNoGotRelevant() const;

private:
    struct Cell {
        /// The Index of this Cell in the Grid
        size_t index;
        /// The sizes of the cells
        SUMOReal xcellsize, ycellsize;
        /// Definition of a Edges container
        typedef std::vector<const MSEdge*> Cont;
        /// Definition of a Cells container
        typedef std::vector<Cell*> CellCont;
        /// The container holding the Egdes
        Cont ownEdges;
        /// 
        Cont neighborEdges;
        /// all the Neighbors of the Cell
        CellCont _neighbors;
    };
private:


    /// @name Methods for building speed-up cell/edge neighborhood
    /// @{

    /** @brief Builds the look-up information 
     *
     * The look-up information is an array of cells (Cell) where
     *  each cell contains the edges that are within it and edges
     *  of the 4-neighbor cells. These are the edges at which a
     *  second vehicle must be in order to communicate with a vehicle
     *  within the cell. Each cell has the height and width of the 
     *  simulated C2C range. Additionally, the information in which
     *  cells each edge lies is stored in myEdgeCells.
     *
     * At first, the cell vector is built, then filled by assigning the
     *  edges that are within each cell to these cells. Afterwards, the
     *  cell neighbor edges are computed.
     *
     * "computeEdgeCells" is responsible for computing in which cells
     *  an edge lies, "getNeighbors" for getting all neighbor cells of
     *  a cell.
     *
     * @see Cell
     * @see computeEdgeCells
     * @see getNeighbors
     */
    static void buildLookUpInformation();


    /** @brief Stores the given edge within all cells the edge lies in
     *
     * The method goes all of the edge's lanes and calls "computeLaneCells"
     *  for assigning the edge to cells it passes.
     *
     * @param[in] edge The edge to compute the passed cells for
     */
    static void computeEdgeCells(const MSEdge *edge);


    /** @brief Stores the given edge within all cells the given shape lies in
     *
     * @param[in] shape The shape of one of the edge's lanes to determine which cells the edge crosses
     * @param[in] edge The edge to compute the passed cells for
     */
    static void computeLaneCells(const Position2DVector &shape, const MSEdge *edge);


    /** @brief Returns the neighbors of a cell
     *
     * @param[in] i The index of the cell
     * @return The neighbors of the cell
     */
    static std::vector<Cell*> getNeighbors(size_t i);


    /** @brief Saves information about neighbor edges
     *
     * @param[in] od The device to write into
     */
    static void writeNearEdges(OutputDevice &od);
    /// @}

private:


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



    static std::vector<Cell*> myCells;
    static unsigned int myLookupXSize, myLookupYSize;
    static SUMOReal myXCellSize, myYCellSize;
    static std::vector<MSDevice_C2C*> myConnected;
    static std::vector<MSDevice_C2C*> myClusterHeaders;
    static std::vector<MSEdge*> myAllEdges;
    static std::map<const MSEdge*, std::vector<size_t> > myEdgeCells;
    static std::map<const MSEdge*, std::vector<const MSEdge*> > myEdgeNeighbors;
    static std::map<const MSEdge*, std::vector<MSDevice_C2C*> > myEdgeVehicles;
    static StaticCommand< MSDevice_C2C > *myC2CComputationCommand;


private:
    static std::map<const MSVehicle*, MSDevice_C2C*> myVehiclesToDevicesMap;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_C2C(const MSDevice_C2C&);

    /// @brief Invalidated assignment operator.
    MSDevice_C2C& operator=(const MSDevice_C2C&);


};


#endif

/****************************************************************************/

