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

#include <vector>
#include <queue>
#include <map>
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
