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
/// @file    MELoop.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
///
// The main mesocopic simulation loop
/****************************************************************************/
#pragma once
#include <config.h>

#include <atomic>
#include <limits>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <utils/common/SUMOTime.h>
#include <utils/common/RandHelper.h>
#ifdef HAVE_FOX
#include <utils/foxtools/MFXWorkerThread.h>
#endif
#include <microsim/MSMoveReminder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MESegment;
class MEVehicle;
class MSEdge;
class MSJunction;
class MSLink;
class MSVehicleControl;
class OptionsCont;
class OutputDevice;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MELoop
 * @brief The main mesocopic simulation loop
 */
class MELoop {
public:
    /// SUMO constructor
    MELoop(const SUMOTime recheckInterval);

    ~MELoop();

    /** @brief Perform simulation up to the given time
     *
     * Checks all vehicles with an event time less or equal than the given time.
     *
     * @param[in] tMax the end time for the sim step
     */
    void simulate(SUMOTime tMax);

    /** @brief Adds the given car to the leading vehicles
     *
     * @param[in] veh the car which became a leading one
     * @param[in] link the link on which the car shall register its approach
     * @param[in] fromState whether the event is restored from a loaded state (keeping the vehicle's event index)
     */
    void addLeaderCar(MEVehicle* veh, MSLink* link, const bool fromState = false);

    /** @brief Removes the given car from the leading vehicles
     *
     * @param[in] v the car which was a leading one
     */
    void removeLeaderCar(MEVehicle* v);

    /** @brief Removes the approach of the given car from the given link
     *
     * While the threads move the deferred cars, other threads read the approaching vehicles
     *  of the link (MSLink::opened), so the main thread removes it after the moves.
     */
    void removeApproaching(MEVehicle* veh, MSLink* link);

    /** @brief remove the given car and clean up the relevant data structures */
    void vaporizeCar(MEVehicle* v, MSMoveReminder::Notification reason);

    /// @brief whether the given edge is entering a roundabout
    static bool isEnteringRoundabout(const MSEdge& e);

    /** @brief Compute number of segments per edge (best value stay close to the configured segment length) */
    static int numSegmentsFor(const double length, const double slength);

    /** @brief Build the segments for a given edge
     *
     * @param[in] e the edge to build for
     */
    void buildSegmentsFor(const MSEdge& e, const OptionsCont& oc);

    /** @brief Get the segment for a given edge at a given position
     *
     * @param[in] e the edge to get the segment for
     * @param[in] pos the position to get the segment for
     * @return The relevant segment
     */
    MESegment* getSegmentForEdge(const MSEdge& e, double pos = 0);

    /** @brief change to the next segment
     * this handles combinations of the following cases:
     * (ending / continuing route) and (leaving segment / finishing teleport)
     */
    SUMOTime changeSegment(MEVehicle* veh, SUMOTime leaveTime, MESegment* const toSegment,
                           MSMoveReminder::Notification reason, const bool ignoreLink = false) const;

    /** @brief Remove all vehicles before quick-loading state */
    void clearState();

    /// @brief the number of leader events so far (for saving and loading state)
    static long long int getEventCounter() {
        return LeaderEvent::myEventCounter;
    }

    static void setEventCounter(long long int counter) {
        LeaderEvent::myEventCounter = counter;
    }

    /** @brief Split the network into groups whose events are processed in parallel (option --meso-threads)
     *
     * Every edge belongs to the group of the junction it ends at, so all vehicles which want
     *  to pass a junction or enter an edge are handled by the same group.
     */
    void buildGroups(const OptionsCont& oc);

    /// @brief save / load the states of the random number generators and the event counters of the groups
    void saveRNGStates(OutputDevice& out);
    /** @brief load the random number state and the event counter of a group
     * @param[in] eventCounter the saved counter, -1 if the state has none (saved by an older build): the counter then
     *  continues after the highest event index of the loaded vehicles (see addLeaderCar)
     */
    void loadRNGState(int index, const std::string& state, long long int eventCounter);

private:
    /** @brief Check whether the vehicle may move
     *
     * This method is called when the vehicle reaches its event time and checks
     *  whether it may proceed to the next segment.
     * When called for a group (see buildGroups), moves into the segment of another group
     *  and teleports are deferred to the end of the parallel part of the step (see simulate).
     *
     * @param[in] veh The vehicle to check
     * @param[in] group The group processing the event or -1
     */
    void checkCar(MEVehicle* veh, const int group = -1);

    /** @brief Retrieve next segment
     *
     * If the segment is not the last on the current edge, its successor is returned.
     *  Otherwise, the first segment of the edge at which the vehicle continues
     *  his journey is returned.
     *
     * @param[in] s The segment the vehicle is currently at
     * @param[in] v The vehicle to get the next segment for
     * @return The vehicle's next segment
     * @todo Recheck the "quick and dirty" stuff (@see MESegment::saveState, @see MESegment::loadState)
     */
    MESegment* nextSegment(MESegment* s, MEVehicle* v);


    /** @brief teleports a vehicle or continues a teleport
     * @param[in] veh The vehicle to teleport
     * @param[in] toSegment The first segment where the vehicle may reenter the network
     */
    void teleportVehicle(MEVehicle* veh, MESegment* const toSegment, bool disconnected);

private:

    struct LeaderEvent {
        LeaderEvent(MEVehicle* v);

        SUMOTime time;
        // eventIndex ensures stable sorting of vehicles with the same event time
        long long int eventIndex;
        // nid allows safely comparing events for deleted vehicles (myInvalidatedLeaderCars)
        long long int nid;
        MEVehicle* veh;

        bool operator<(const LeaderEvent& b) const {
            if (time == b.time) {
                return eventIndex > b.eventIndex;
            }
            return time > b.time;
        }

        bool operator==(const LeaderEvent& b) const {
            return time == b.time && nid == b.nid;
        }

        static long long int myEventCounter;
    };

    /// @brief leader cars in the segments sorted by exit time
    typedef std::priority_queue<LeaderEvent> LeaderEventQueue;
    LeaderEventQueue myLeaderCars;
    LeaderEventQueue myInvalidatedLeaderCars;

    /// @brief check whether the current car was invalidated and remove it from the invalidated cars
    static bool wasInvalidated(const LeaderEvent& e, LeaderEventQueue& invalidated);

    /// @brief check all cars with an event time up to tMax (group -1 for the queues without groups)
    void processEvents(LeaderEventQueue& leaders, LeaderEventQueue& invalidated, SUMOTime tMax, const int group);

    /// @brief the group whose queue holds the event of the given car
    int getGroup(const MEVehicle* veh) const;

    /** @brief Computes the groups of the junctions (see buildGroups)
     *
     * Junctions connected by edges which can be passed within one step stay together, the clusters are ordered along a
     *  z-order curve and cut into groups with similar numbers of segments.
     */
    void computeGroups(const int numGroups, std::map<const MSJunction*, int>& junctionGroups);

    /** @brief Loads the groups of the junctions from a file of taz elements (option --meso-threads.partition-file)
     *
     * Every edge belongs to the group of the junction it ends at, so the edges of a taz determine the group of their end
     *  junctions (the group of most of them where they differ). Junctions without listed edges take the group of a neighbour.
     * @return the number of groups
     */
    int loadPartition(const std::string& file, std::map<const MSJunction*, int>& junctionGroups) const;

    /// @brief writes the groups as taz elements with their edges (option --meso-threads.partition-output)
    void writePartition() const;

    /// @brief the junctions at which edges end: only they need a group since every edge belongs to the group of its end
    static std::set<const MSJunction*> getEndJunctions();

    /// @brief reads the taz elements of a partition file
    class PartitionHandler;

    /// @brief whether the move of the vehicle touches only the state of its group (no stops, passengers or reminders bound to the main thread)
    static bool isGroupSafe(const MEVehicle* veh, const MESegment* on, const MESegment* to);

    /// @brief whether the move of the vehicle into the segment of another group may run on the threads of the two groups (no teleport due, no arrival)
    static bool isParallelMove(const MEVehicle* veh, const MESegment* on, const MESegment* to);

    /// @brief whether the car is the leader of its queue (or teleporting or parking): a deferred car which was overtaken waits for its next turn
    static bool isQueueLeader(const MEVehicle* veh);

    /** @brief A move into the segment of another group, see simulate
     *
     * The thread of the group entered decides the move and prepares the departure (MESegment::prepareSend),
     *  the main thread sets the event time and the thread of the group left completes the departure (MESegment::completeSend).
     */
    struct DeferredMove {
        MEVehicle* veh;
        MESegment* from;
        int qIdx;
        MSLink* link;
        SUMOTime time;
        SUMOTime headway;
        SUMOTime eventTime;
        bool queued;
    };

    /// @brief an event the main thread schedules after the moves: the vehicle, the link it registers on, the time of the move and the new event time (-1: unchanged)
    struct DeferredLeader {
        MEVehicle* veh;
        MSLink* link;
        SUMOTime time;
        SUMOTime eventTime;
    };

    /// @brief what a thread records while it moves the deferred cars into its group instead of changing what other threads read
    struct DeferredOps {
        std::vector<DeferredMove> moves;
        std::vector<DeferredLeader> leaders;
        /// @brief cars refused because the leader of their target is deferred as well: the main thread moves them after the departures
        std::vector<MEVehicle*> serial;
        /// @brief cars overtaken by an arrival which no longer approach their link: the main thread removes them after the moves
        std::vector<std::pair<MEVehicle*, MSLink*> > unapproached;
        void clear() {
            moves.clear();
            leaders.clear();
            serial.clear();
            unapproached.clear();
        }
    };

    /// @brief the records of the calling thread while it moves deferred cars, nullptr otherwise
    static thread_local DeferredOps* myDeferred;

    /// @brief lets the calling thread record its moves for the lifetime of the object
    struct DeferredScope;

    /// @brief a set of junctions with the edges ending there, see buildGroups
    struct Group {
        Group(const std::string& id) : rng(id) {}
        LeaderEventQueue leaders;
        LeaderEventQueue invalidated;
        long long int eventCounter = 0;
        SumoRNG rng;
        /// @brief cars which enter the segment of another group at the end of the parallel part of the step
        std::vector<MEVehicle*> deferred;
        /// @brief cars whose event needs the main thread: teleports, stops, passengers, serial reminders, the followers of deferred cars
        std::vector<MEVehicle*> serial;
        /// @brief the deferred cars entering this group in the order of their events
        std::vector<MEVehicle*> arrivals;
        /// @brief the moves out of this group to complete
        std::vector<DeferredMove*> departures;
        /// @brief the records of the thread moving the arrivals
        DeferredOps ops;
        /// @brief per other group (and all groups as the last entry) the times from which on the pending events may change it (see addHolds)
        std::vector<std::multiset<SUMOTime> > holds;
        /// @brief the groups whose events may change this group within a step (sorted)
        std::vector<int> neighbours;
    };
    std::vector<Group> myGroups;

    /// @brief the parts of a step which run in parallel over the groups: the events, the moves into other groups, the departures they cause
    enum Phase { EVENTS, ARRIVALS, DEPARTURES };

    /// @brief runs the phase for the group
    void runGroup(const int group, const Phase phase);

    /// @brief runs the phase for all groups (on the threads with FOX)
    void runPhase(const Phase phase);

    /// @brief the number of moves between groups below which they run on the main thread (the synchronisation would cost more than it saves)
    static const int MIN_PARALLEL_MOVES;

    /// @brief the group of each edge by numerical id
    std::vector<int> myEdgeGroups;

    /// @brief the end of the current simulation step
    SUMOTime myStepEnd = 0;

    /// @brief how many parallel rounds a step may run, the events still due then run serially (0: until no event is due)
    int myRounds = 8;

    /// @brief whether a group has an event due until the given time
    bool anyEventDue(SUMOTime tMax) const;

    /// @name synchronisation of the groups at the events which concern two groups (option --meso-threads.event-sync)
    /// @{

    /// @brief whether the groups synchronise at these events instead of deferring the moves between groups to the end of the step
    bool myEventSync = false;

    /** @brief the holds which the pending event of a vehicle placed (see MEVehicle::getEventHolds)
     *
     * An event holds back the groups which its vehicle, or a follower it promotes, may enter within the step:
     *  they do not run their events beyond the time of such a move until the event is done.
     */
    enum EventHolds {
        /// @brief none: other groups may be beyond the event already, so it runs in the serial part of the step if it concerns them
        NO_HOLDS = 0,
        /// @brief the groups which a vehicle may enter from its segment within the step (see MESegment::getReach)
        REACH_HOLDS = 1,
        /// @brief these and all groups at the time of the event (teleports, stops, passengers, reminders bound to the main thread)
        ALL_HOLDS = 2
    };

    /// @brief the event which a thread runs and the groups it changes (see isForeseen)
    struct RunningEvent {
        SUMOTime time;
        int group;
        /// @brief the other group the event changes, -1 for none and the number of groups for all
        int partner;
        /// @brief the holds the event placed
        int holds;
        const MESegment* segment;
    };

    /// @brief the event the calling thread runs, nullptr outside the parallel part of the step
    static thread_local const RunningEvent* myRunningEvent;

    /// @brief a key shared between the threads, alone in its cache line
    struct SharedKey {
        std::atomic<long long int> value{std::numeric_limits<long long int>::max()};
        char padding[64 - sizeof(std::atomic<long long int>)];
    };

    /// @brief per group and target (the other groups, then all groups) the key of the smallest hold
    std::vector<SharedKey> myHoldKeys;

    /// @brief per group the key of its next event while it waits for others, RUNNING while its thread runs its events
    std::vector<SharedKey> myParkedKeys;

    /// @brief the number of groups which have events left in the current step
    std::atomic<int> myActiveGroups{0};

    /// @brief set when a thread failed so that the others stop waiting for it
    std::atomic<bool> myAbort{false};

    /// @brief the parked key of a group while its thread runs its events
    static const long long int RUNNING;

    /** @brief the reach of the segments assumes that vehicles drive at most this factor times the speed limit (see buildReach)
     *
     * The default speed distribution is capped there. A move of a faster vehicle may come earlier than foreseen and then waits for
     *  the serial part of the step.
     */
    static const double MAX_SPEED_FACTOR;

    /// @brief the position of an event of the given group at the given time in the order of the events of all groups
    inline long long int getKey(SUMOTime time, int group) const {
        return time * (long long int)myGroups.size() + group;
    }

    /// @brief the end of the step in which an event at the given time runs
    SUMOTime getStepEnd(SUMOTime time) const;

    /// @brief computes for every segment which other groups a vehicle may enter from there within one step (see MESegment::getReach)
    void buildReach();

    /// @brief places the holds of the new event of the vehicle in the given group
    void addHolds(MEVehicle* veh, const int group);

    /// @brief removes the holds of an event of the given group
    void removeHolds(const int group, const MESegment* segment, SUMOTime time, int holds);

    /// @brief adds or removes a single hold and publishes the smallest one
    void insertHold(const int group, const int target, SUMOTime time);
    void eraseHold(const int group, const int target, SUMOTime time);

    /// @brief whether the event of the vehicle holds back all groups (see EventHolds)
    bool holdsAll(MEVehicle* veh, MESegment* segment);

    /// @brief whether the move changes state outside the groups of the two segments (stops, arrivals with passengers, reminders bound to the main thread)
    static bool isGlobalMove(const MEVehicle* veh, const MESegment* on, const MESegment* to);

    /// @brief whether the vehicle teleports if it is still blocked at the given time (see checkCar)
    static bool mayTeleport(const MEVehicle* veh, SUMOTime time);

    /// @brief whether the running event placed the holds which a new event with the given holds needs (no other group got beyond them)
    bool isForeseen(const MESegment* segment, SUMOTime time, bool all, int group) const;

    /// @brief the other group which the event of the vehicle changes (-1: none, the number of groups: all), -2 if its holds do not suffice
    int getPartner(MEVehicle* veh, const int group);

    /// @brief the smallest key at which another group may change the given group
    long long int getBound(const int group) const;

    /// @brief whether the partner (or every group) waits beyond the given key and no other group may change it before
    bool isPartnerReady(const int group, const int partner, long long int key) const;

    /// @brief removes the invalidated events from the top of the queue of the group
    static void skipInvalidated(Group& g);

    /// @brief runs the events of the step on the threads, each group in the order of its events
    void runEventSync();

    /// @brief runs the events of the groups of one thread
    void runEventSyncThread(const int thread, const int numThreads);
    /// @}

#ifdef HAVE_FOX
    /// @brief the task running one phase for one group
    class GroupTask : public MFXWorkerThread::Task {
    public:
        GroupTask(MELoop& loop, int group, Phase phase) : myLoop(loop), myGroup(group), myPhase(phase) {}
        void run(MFXWorkerThread* /*context*/) {
            myLoop.runGroup(myGroup, myPhase);
        }
    private:
        MELoop& myLoop;
        const int myGroup;
        const Phase myPhase;
    private:
        /// @brief Invalidated assignment operator.
        GroupTask& operator=(const GroupTask&) = delete;
    };
    /// @brief the tasks by phase and group
    std::vector<std::vector<GroupTask*> > myGroupTasks;

    /// @brief the task running the events of the groups of one thread (option --meso-threads.event-sync)
    class EventSyncTask : public MFXWorkerThread::Task {
    public:
        EventSyncTask(MELoop& loop, int thread, int numThreads) : myLoop(loop), myThread(thread), myNumThreads(numThreads) {}
        void run(MFXWorkerThread* /*context*/) {
            myLoop.runEventSyncThread(myThread, myNumThreads);
        }
        int getThread() const {
            return myThread;
        }
    private:
        MELoop& myLoop;
        const int myThread;
        const int myNumThreads;
    private:
        /// @brief Invalidated assignment operator.
        EventSyncTask& operator=(const EventSyncTask&) = delete;
    };
    /// @brief one task per thread
    std::vector<EventSyncTask*> myEventSyncTasks;
    MFXWorkerThread::Pool myThreadPool;
#endif

    /// @brief mapping from internal edge ids to their initial segments
    std::vector<MESegment*> myEdges2FirstSegments;

    /// @brief the interval at which to recheck at full segments (<=0 means asap)
    const SUMOTime myFullRecheckInterval;

    /// @brief the interval at which to recheck at blocked junctions (<=0 means asap)
    const SUMOTime myLinkRecheckInterval;

    bool myInterpolateMesoPos;

private:
    /// @brief Invalidated copy constructor.
    MELoop(const MELoop&);

    /// @brief Invalidated assignment operator.
    MELoop& operator=(const MELoop&);
};
