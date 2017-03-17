/****************************************************************************/
/// @file    MSDevice_SSM.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    11.06.2013
/// @version $Id: MSDevice_SSM.h 23150 2017-02-27 12:08:30Z behrisch $
///
// A device which stands as an implementation example and which outputs movereminder calls
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 200132014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSDevice_SSM_h
#define MSDevice_SSM_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDevice.h"
#include <utils/common/SUMOTime.h>
#include <utils/iodevices/OutputDevice_File.h>
#include <utils/geom/Position.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_SSM
 * @brief A device which collects info on the vehicle trip (mainly on departure and arrival)
 *
 * Each device collects departure time, lane and speed and the same for arrival.
 *
 * @see MSDevice
 */

class MSCrossSection;

class MSDevice_SSM : public MSDevice {

private:
    /// All currently existing SSM devices
    static std::set<MSDevice*>* instances;

    /// @brief Different types of encounters corresponding to relative positions of the vehicles.
    ///        The name describes the type from the ego perspective
    enum EncounterType {
        // Other vehicle is closer than range, but not on a conflicting lane
        ENCOUNTER_TYPE_NOCONFLICT,       //!< ENCOUNTER_TYPE_NOCONFLICT
        // Ego and foe vehicles' edges form a part of a consecutive sequence of edges
        // This type may be specified further by ENCOUNTER_TYPE_FOLLOWING_LEADER or ENCOUNTER_TYPE_FOLLOWING_FOLLOWER
        ENCOUNTER_TYPE_FOLLOWING,       //!< ENCOUNTER_TYPE_FOLLOWING
        // Ego vehicle is on an edge that has a sequence of successors connected to the other vehicle's edge
        ENCOUNTER_TYPE_FOLLOWING_FOLLOWER,       //!< ENCOUNTER_TYPE_FOLLOWING_FOLLOWER
        // Other vehicle is on an edge that has a sequence of successors connected to the ego vehicle's current edge
        ENCOUNTER_TYPE_FOLLOWING_LEADER,         //!< ENCOUNTER_TYPE_FOLLOWING_LEADER
        // Ego and foe share an upcoming edge of their routes while the merging point for the routes is still ahead
        // This type may be specified further by ENCOUNTER_TYPE_MERGING_LEADER or ENCOUNTER_TYPE_MERGING_FOLLOWER
        ENCOUNTER_TYPE_MERGING,  //!< ENCOUNTER_TYPE_MERGING
        // Other vehicle is on an edge that has a sequence of successors connected to an edge on the ego vehicle's route
        // and the estimated arrival vehicle at the merge point is earlier for the ego than for the foe
        ENCOUNTER_TYPE_MERGING_LEADER,  //!< ENCOUNTER_TYPE_MERGING_LEADER
        // Other vehicle is on an edge that has a sequence of successors connected to an edge on the ego vehicle's route
        // and the estimated arrival vehicle at the merge point is earlier for the foe than for the ego
        ENCOUNTER_TYPE_MERGING_FOLLOWER,//!< ENCOUNTER_TYPE_MERGING_FOLLOWER
        // Ego's and foe's routes have crossing edges
        // This type may be specified further by ENCOUNTER_TYPE_CROSSING_LEADER or ENCOUNTER_TYPE_CROSSING_FOLLOWER
        ENCOUNTER_TYPE_CROSSING,  //!< ENCOUNTER_TYPE_CROSSING
        // Other vehicle is on an edge that has a sequence of successors leading to an internal edge that crosses the ego vehicle's edge at a junction
        // and the estimated arrival vehicle at the merge point is earlier for the ego than for the foe
        ENCOUNTER_TYPE_CROSSING_LEADER, //!< ENCOUNTER_TYPE_CROSSING_LEADER
        // Other vehicle is on an edge that has a sequence of successors leading to an internal edge that crosses the ego vehicle's edge at a junction
        // and the estimated arrival vehicle at the merge point is earlier for the foe than for the ego
        ENCOUNTER_TYPE_CROSSING_FOLLOWER//!< ENCOUNTER_TYPE_CROSSING_FOLLOWER
    };



private:
    /// @brief An encounter is an episode involving two vehicles,
    ///        which are closer to each other than some specified distance.
    class Encounter {
    private:
        /// @brief A trajectory encloses a series of positions x and speeds v for one vehicle
        /// (the times are stored only once in the enclosing encounter)
        struct Trajectory {
            // positions
            PositionVector x;
            // momentary speeds
            PositionVector v;
        };

    public:
        /// @brief Constructor
        Encounter(const MSVehicle* ego, const MSVehicle* const foe, double begin);
        /// @brief Destructor
        ~Encounter();
        /// @brief add a new data point
        void add(double time, Position egoX, Position egoV, Position foeX, Position foeV);
        /// @brief Compares encounters regarding to their start time
        struct compare {
            typedef bool value_type;
            bool operator() (Encounter* e1, Encounter* e2) {
                return e1->begin <= e2->begin;
            };
        };



    public:
        EncounterType type;

        double begin, end;
        const MSVehicle* ego;
        const MSVehicle* const foe;

//        /// @brief tuple time x point of entry to the potential conflict
//        std::pair<double, MSCrossSection> egoConflictEntry;
//        std::pair<double, MSCrossSection> foeConflictEntry;
//        std::pair<double, MSCrossSection> egoConflictExit;
//        std::pair<double, MSCrossSection> foeConflictExit;

        /// @brief time points corresponding to the trajectories
        std::vector<double> timespan;
        /// @brief Trajectory of the ego vehicle
        Trajectory egoTrajectory;
        /// @brief Trajectory of the foe vehicle
        Trajectory foeTrajectory;

        /// @brief All values for TTC
        std::vector<double> TTCspan;
        /// @brief All values for DRAC
        std::vector<double> DRACspan;

        /// @name Extremal values for the SSMs (as <time,value>-pairs)
        /// @{
        std::pair<double, double> maxDRAC;
        std::pair<double, double> minTTC;
        std::pair<double, double> PET;
        /// @}

    private:
        /// @brief Invalidated Constructor.
        Encounter(const Encounter&);
        /// @brief Invalidated assignment operator.
        MSDevice_SSM& operator=(const MSDevice_SSM&);
        ///
    };

    /// A new FoeInfo is created during findSurroundingVehicles() to memorize, where the potential conflict
    /// corresponding to the encounter might occur. Each FoeInfo ends up in a call to updateEncounter() and
    /// is deleted there.
    struct FoeInfo {
        const MSLane* egoConflictLane;
        double egoDistToConflictLane;
    };
    // TODO: consider introducing a class foeCollector, which holds the foe info content
    //       plus a vehicle container to be used in findSurrounding vehicles.
    //       findSurroundingVehicles() would then deliver a vector of such foeCollectors
    //       (one for each possible egoConflictLane) instead of a map vehicle->foeInfo


    typedef std::priority_queue<Encounter*, std::vector<Encounter*>, Encounter::compare> EncounterQueue;
    typedef std::vector<Encounter*> EncounterVector;
    typedef std::map<const MSVehicle*, FoeInfo*> FoeInfoMap;
public:

    /** @brief Inserts MSDevice_SSM-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a example-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into);


    /** @brief returns all currently existing SSM devices
     */
    static const std::set<MSDevice*>& getInstances();

    /** @brief This is called once per time step in MSNet::writeOutput() and
     *         collects the surrounding vehicles, updates information on encounters
     *         and flushes the encounters qualified as conflicts (@see thresholds)
     *         to the output file.
     */
    void updateAndWriteOutput();

private:
    void update();
    void writeOutConflict(Encounter* e);

public:
    /** @brief Clean up remaining devices instances
     */
    static void cleanup();


public:
    /// @brief Destructor.
    ~MSDevice_SSM();


    /** @brief Returns all vehicles, which are within the given range of the given vehicle.
     *  @note all vehicles behind and in front are collected,
     *  including vehicles on confluent edges. If the range is 20 m. and
     *  a junction lies 10 m. ahead, an upstream scan of 20 m. is performed
     *  for all incoming edges.
     *
     * @param veh   The ego vehicle, that forms the origin for the scan
     * @param range The range to be scanned.
     * @param[in/out] foeCollector container for all collected vehicles
     * @return All vehicles within range from veh
     */
    static void findSurroundingVehicles(const MSVehicle& veh, double range, FoeInfoMap& foeCollector);

    /** @brief Collects all vehicles within range 'range' upstream of the position 'pos' on the edge 'edge' into foeCollector
     */
    static void getUpstreamVehicles(const MSEdge* edge, double pos, double range, double egoDistToConflictLane, const MSLane* const egoConflictLane, FoeInfoMap& foeCollector);

    /** @brief Collects all vehicles on the junction into foeCollector
     */
    static void getVehiclesOnJunction(const MSJunction*, double egoDistToConflictLane, const MSLane* const egoConflictLane, FoeInfoMap& foeCollector);


    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Checks for waiting steps when the vehicle moves
     *
     * @param[in] veh Vehicle that notifies.
     * @param[in] oldPos Position before move.
     * @param[in] newPos Position after move with newSpeed.
     * @param[in] newSpeed Moving speed.
     *
     * @return Always true to keep the device as it cannot be thrown away
     */
    bool notifyMove(SUMOVehicle& veh, double oldPos,
                    double newPos, double newSpeed);


    /** @brief Called whenever the holder enteres a lane
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason Reason for leaving the lane
     * @param[in] enteredLane The lane entered.
     * @return Always true to keep the device as it cannot be thrown away
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason, const MSLane* enteredLane = 0);


    /** @brief Called whenever the holder leaves a lane
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] reason Reason for leaving the lane
     * @param[in] enteredLane The lane entered.
     * @return True if it did not leave the net.
     */
    bool notifyLeave(SUMOVehicle& veh, double lastPos,
                     MSMoveReminder::Notification reason, const MSLane* enteredLane = 0);
    /// @}



    /** @brief Finalizes output. Called on vehicle removal
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     * @see MSDevice::generateOutput
     */
    void generateOutput() const;



private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     * @param measures Vector of Surrogate Safety Measure IDs
     * @param thresholds Vector of corresponding thresholds
     * @param trajectories Flag indicating whether complete trajectories should be saved for an encounter (if false only extremal values are logged)
     * @param frequency Maximal length of a single encounter.
     * @param range Detection range. For vehicles closer than this distance from the ego vehicle, SSMs are traced
     */
    MSDevice_SSM(SUMOVehicle& holder, const std::string& id, std::string outputFilename, std::vector<std::string> measures, std::vector<double> thresholds,
            bool trajectories, double frequency, double range);


    /** @brief Finds encounters for which the foe vehicle has disappeared from range and
     *         if an ended encounter is qualified as a conflict, it is transferred to myPastConflicts
     *         All vehicles for which an encounter instance already exists (vehicle is already tracked)
     *         are removed from 'foes' during processing.
     */
    void processEncounters(FoeInfoMap& foes);

    /** @brief Makes new encounters for all given vehicles (these should be the ones entering the device's range in the current timestep)
     */
    void createEncounters(FoeInfoMap& foes);


    /** @brief Closes all current Encounters and moves conflicts to myPastConflicts, @see processEncounters
     */
    void resetEncounters();

    /** @brief Writes out all past conflicts that have begun earlier than time t-myFrequency (i.e. no active encounter can have an earlier begin)
     * @param[in] all Whether all conflicts should be flushed or only those for which no active encounters with earlier begin can exist
     */
    void flushConflicts(bool all = false);

    /** @brief Updates the encounter (adds a new trajectory point) and deletes the foeInfo.
     */
    void updateEncounter(Encounter* e, FoeInfo* foeInfo);

    /** @brief Computes the conflict lane for the foe
     *
     * @param foe Foe vehicle
     * @param egoConflictLane Lane, on which the ego would enter the possible conflict
     * @param routeToConflict, Series of edges, that were traced back from egoConflictLane during findSurrounding Vehicles, when collecting the foe vehicle
     * @param[out] distToConflictLane, distance to conflictlane entry link (may be negative if foe is already on the conflict lane)
     * @return Lane, on which the foe would enter the possible conflict, if foe is not on conflict course, Null-pointer is returned.
     */
    const MSLane* findFoeConflictLane(const MSVehicle* foe, const MSLane* egoConflictLane, double& distToConflictLane);

    /** @brief Finalizes the encounter and calculates SSM values.
     */
    void closeEncounter(Encounter* e);

    /** @brief Tests if the SSM values exceed the threshold for qualification as conflict.
     */
    bool qualifiesAsConflict(Encounter* e);

    /** Compute current values of the logged SSMs (myMeasures) for the given encounter 'e'
     *  and update 'e' accordingly (add point to SSM time-series, update maximal/minimal value)
     *  This is called just after adding the current vehicle positions and velocity vectors to the encounter.
     */
    void computeSSMs(Encounter* e);


    /// @name parameter load helpers (introduced for readability of buildVehicleDevices())
    /// @{
    static std::string getOutputFilename(const SUMOVehicle& v, std::string deviceID);
    static double getDetectionRange(const SUMOVehicle& v);
    static double getLoggingFrequency(const SUMOVehicle& v);
    static bool requestsTrajectories(const SUMOVehicle& v);
    static bool getMeasuresAndThresholds(const SUMOVehicle& v, std::string deviceID,
            std::vector<double>& thresholds, std::vector<std::string>& measures);
    ///@}

private:
    /// @name Device parameters
    /// @{
    std::vector<std::string> myMeasures;
    std::vector<double> myThresholds;
    bool mySaveTrajectories;
    /// @brief Maximal timespan duration for a single encounter
    /// @todo rename to maxEncounterLength
    double myFrequency;
    double myRange;
    /// @brief Corresponding maximal trajectory size in points, derived from myFrequency
    size_t maxTrajectorySize;
    /// Flags for switching on / off comutation of different SSMs, derived from myMeasures
    bool myComputeTTC, myComputeDRAC, myComputePET;
    MSVehicle* myHolderMS;
    /// @}


    /// @name Internal storage for encounters/conflicts
    /// @{
    /// @brief Currently observed encounters/conflicts
    EncounterVector myActiveEncounters;
    /// @brief Past encounters that where qualified as conflicts and are not yet flushed to the output file
    EncounterQueue myPastConflicts;
    /// @}

    /// Output device
    OutputDevice* myOutputFile;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_SSM(const MSDevice_SSM&);

    /// @brief Invalidated assignment operator.
    MSDevice_SSM& operator=(const MSDevice_SSM&);


};

#endif

/****************************************************************************/

