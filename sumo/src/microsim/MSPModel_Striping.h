/****************************************************************************/
/// @file    MSPModel_Striping.h
/// @author  Jakob Erdmann
/// @date    Mon, 13 Jan 2014
/// @version $Id$
///
// The pedestrian following model (prototype)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSPModel_Striping_h
#define	MSPModel_Striping_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <limits>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSPerson.h>
#include <microsim/MSPModel.h>
#include <microsim/MSLane.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLink;
class MSJunction;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPModel_Striping
 * @brief The pedestrian following model
 *
 */
class MSPModel_Striping : public MSPModel {

    friend class GUIPerson; // for debugging

public:

    /// @brief Constructor (it should not be necessary to construct more than one instance)
    MSPModel_Striping(const OptionsCont& oc, MSNet* net);

    ~MSPModel_Striping();

    /// @brief register the given person as a pedestrian
    PedestrianState* add(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, SUMOTime now);

    /// @brief whether a pedestrian is blocking the crossing of lane at offset distToCrossing
    bool blockedAtDist(const MSLane* lane, SUMOReal distToCrossing, std::vector<const MSPerson*>* collectBlockers);

    /// @brief remove state at simulation end
    void cleanupHelper();

    /// @brief model parameters
    ///@{

    // @brief the width of a pedstrian stripe
    static SUMOReal stripeWidth;

    // @brief the factor for random slow-down
    static SUMOReal dawdling;

    // @brief the distance to look ahead for changing stripes
    static const SUMOReal LOOKAHEAD_SAMEDIR;
    // @brief the distance to look ahead for changing stripes (regarding oncoming pedestrians)
    static const SUMOReal LOOKAHEAD_ONCOMING;

    // @brief the speed penalty for moving sideways
    static const SUMOReal LATERAL_PENALTY;

    // @brief the factor by which pedestrian width is reduced when sqeezing past each other
    static const SUMOReal SQUEEZE;

    // @brief the maximum distance at which oncoming pedestrians block right turning traffic
    static const SUMOReal BLOCKER_LOOKAHEAD;

    // @brief fraction of the leftmost lanes to reserve for oncoming traffic
    static const SUMOReal RESERVE_FOR_ONCOMING_FACTOR;

    // @brief the time pedestrians take to reach maximum impatience
    static const SUMOReal MAX_WAIT_TOLERANCE;

    // @brief the fraction of forward speed to be used for lateral movemenk
    static const SUMOReal LATERAL_SPEED_FACTOR;

    ///@}


protected:
    class lane_by_numid_sorter {
    public:
        /// comparing operation
        bool operator()(const MSLane* l1, const MSLane* l2) const {
            return l1->getNumericalID() < l2->getNumericalID();
        }
    };

    struct Obstacle;
    struct WalkingAreaPath;
    class PState;
    typedef std::vector<PState*> Pedestrians;
    typedef std::map<const MSLane*, Pedestrians, lane_by_numid_sorter> ActiveLanes;
    typedef std::vector<Obstacle> Obstacles;
    typedef std::map<const MSLane*, Obstacles, lane_by_numid_sorter> NextLanesObstacles;
    typedef std::map<std::pair<const MSLane*, const MSLane*>, WalkingAreaPath> WalkingAreaPaths;

    struct NextLaneInfo {
        NextLaneInfo(const MSLane* _lane, const MSLink* _link, int _dir) :
            lane(_lane),
            link(_link),
            dir(_dir)
        { }

        NextLaneInfo() :
            lane(0),
            link(0),
            dir(UNDEFINED_DIRECTION)
        { }

        // @brief the next lane to be used
        const MSLane* lane;
        // @brief the link from the current lane to the next lane
        const MSLink* link;
        // @brief the direction on the next lane
        int dir;
    };

    /// @brief information regarding surround Pedestrians (and potentially other things)
    struct Obstacle {
        /// @brief create No-Obstacle
        Obstacle(int dir);
        /// @brief create an obstacle from ped for ego moving in dir
        Obstacle(const PState& ped, int dir);
        /// @brief create an obstacle from explict values
        Obstacle(SUMOReal _x, SUMOReal _speed, const std::string& _description) : x(_x), speed(_speed), description(_description) {};

        /// @brief position on the current lane
        SUMOReal x;
        /// @brief speed relative to ego direction (positive means in the same direction)
        SUMOReal speed;
        /// @brief the id / description of the obstacle
        std::string description;
    };

    struct WalkingAreaPath {
        WalkingAreaPath(const MSLane* _from, const MSLane* _walkingArea, const MSLane* _to, const PositionVector& _shape) :
            from(_from),
            to(_to),
            lane(_walkingArea),
            shape(_shape),
            length(_shape.length())
        {}

        WalkingAreaPath(): from(0), to(0), lane(0) {};

        const MSLane* from;
        const MSLane* to;
        const MSLane* lane; // the walkingArea;
        // actually const but needs to be copyable by some stl code
        PositionVector shape;
        SUMOReal length;

    };

    /**
     * @class PState
     * @brief Container for pedestrian state and individual position update function
     */
    class PState : public PedestrianState {
    public:

        /// @brief abstract methods inherited from PedestrianState
        /// @{
        SUMOReal getEdgePos(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        Position getPosition(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        SUMOReal getAngle(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        SUMOTime getWaitingTime(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        SUMOReal getSpeed(const MSPerson::MSPersonStage_Walking& stage) const;
        /// @}

        PState(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, const MSLane* lane);
        ~PState() {};
        MSPerson* myPerson;
        MSPerson::MSPersonStage_Walking* myStage;
        /// @brief the current lane of this pedestrian
        const MSLane* myLane;
        /// @brief the advancement along the current lane
        SUMOReal myRelX;
        /// @brief the orthogonal shift on the current lane
        SUMOReal myRelY;
        /// @brief the walking direction on the current lane (1 forward, -1 backward)
        int myDir;
        /// @brief the current walking speed
        SUMOReal mySpeed;
        /// @brief whether the pedestrian is blocked by an oncoming pedestrian
        int myBlockedByOncoming;
        /// @brief whether the pedestrian is waiting to start its walk
        bool myWaitingToEnter;
        /// @brief the consecutive time spent at speed 0
        SUMOTime myWaitingTime;
        /// @brief information about the upcoming lane
        NextLaneInfo myNLI;
        /// @brief the current walkingAreaPath or 0
        WalkingAreaPath* myWalkingAreaPath;

        /// @brief return the length of the pedestrian
        SUMOReal getLength() const;

        /// @brief the absolute distance to the end of the lane in walking direction (or to the arrivalPos)
        SUMOReal distToLaneEnd() const;

        /// @brief return whether this pedestrian has passed the end of the current lane and update myRelX if so
        bool moveToNextLane(SUMOTime currentTime);

        /// @brief perform position update
        void walk(const Obstacles& obs, SUMOTime currentTime);

        /// @brief returns the impatience
        SUMOReal getImpatience(SUMOTime now) const; // XXX

        /// @brief return the speed-dependent minGap of the pedestrian
        SUMOReal getMingap() const;

        int stripe() const;
        int otherStripe() const;

    };

    class MovePedestrians : public Command {
    public:
        MovePedestrians(MSPModel_Striping* model) : myModel(model) {};
        ~MovePedestrians() {};
        SUMOTime execute(SUMOTime currentTime);
    private:
        MSPModel_Striping* const myModel;
    private:
        /// @brief Invalidated assignment operator.
        MovePedestrians& operator=(const MovePedestrians&);
    };

    class by_xpos_sorter {
    public:
        /// constructor
        by_xpos_sorter(int dir): myDir(dir) {}

    public:
        /// comparing operation
        bool operator()(const PState* p1, const PState* p2) const {
            if (p1->myRelX != p2->myRelX) {
                return myDir * p1->myRelX > myDir * p2->myRelX;
            }
            return p1->myPerson->getID() < p2->myPerson->getID();
        }

    private:
        const int myDir;

    private:
        /// @brief Invalidated assignment operator.
        by_xpos_sorter& operator=(const by_xpos_sorter&);
    };


    /// @brief move all pedestrians forward and advance to the next lane if applicable
    void moveInDirection(SUMOTime currentTime, std::set<MSPerson*>& changedLane, int dir);

    const ActiveLanes& getActiveLanes() {
        return myActiveLanes;
    }

private:
    static void DEBUG_PRINT(const Obstacles& obs);

    /// @brief returns the direction in which these lanes are connectioned or 0 if they are not
    static int connectedDirection(const MSLane* from, const MSLane* to);

    /** @brief computes the successor lane for the given pedestrian and sets the
     * link as well as the direction to use on the succesor lane
     * @param[in] currentLane The lane the pedestrian is currently on
     * @param[in] ped The pedestrian for which to compute the next lane
     */
    static NextLaneInfo getNextLane(const PState& ped, const MSLane* currentLane, const MSLane* prevLane);

    /// @brief return the next walkingArea in the given direction
    static const MSLane* getNextWalkingArea(const MSLane* currentLane, const int dir, MSLink*& link);

    static void initWalkingAreaPaths(const MSNet* net);

    /// @brief return the maximum number of pedestrians walking side by side
    static int numStripes(const MSLane* lane);

    static Obstacles mergeObstacles(const Obstacles& obs1, const Obstacles& obs2, int dir);

    static Obstacles getNeighboringObstacles(const Pedestrians& pedestrians, int egoIndex, int stripes);

    const Obstacles& getNextLaneObstacles(NextLanesObstacles& nextLanesObs, const MSLane* nextLane, int stripes,
                                          SUMOReal nextLength, int nextDir, SUMOReal currentLength, int currentDir);

    /// @brief retrieves the pedestian vector for the given lane (may be empty)
    Pedestrians& getPedestrians(const MSLane* lane);


private:
    /// @brief the MovePedestrians command that is registered
    MovePedestrians* myCommand;

    /// @brief the total number of active pedestrians
    int myNumActivePedestrians;

    /// @brief store of all lanes which have pedestrians on them
    ActiveLanes myActiveLanes;

    /// @brief store for walkinArea elements
    static WalkingAreaPaths myWalkingAreaPaths;

    /// @brief empty pedestrian vector
    static Pedestrians noPedestrians;

};


#endif	/* MSPModel_Striping_h */

