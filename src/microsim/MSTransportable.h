/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSTransportable.h
/// @author  Michael Behrisch
/// @date    Tue, 21 Apr 2015
/// @version $Id$
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/
#ifndef MSTransportable_h
#define MSTransportable_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <cassert>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Boundary.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSLane;
class MSNet;
class MSStoppingPlace;
class MSVehicleType;
class OutputDevice;
class SUMOVehicleParameter;
class SUMOVehicle;

typedef std::vector<const MSEdge*> ConstMSEdgeVector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSTransportable
  *
  * The class holds a simulated moveable object
  */
class MSTransportable {
public:
    enum StageType {
        WAITING_FOR_DEPART = 0,
        WAITING = 1,
        MOVING_WITHOUT_VEHICLE = 2, // walking for persons, tranship for containers
        DRIVING = 3
    };

    /**
    * The "abstract" class for a single stage of a movement
    * Contains the destination of the current movement step
    */
    class Stage {
    public:
        /// constructor
        Stage(const MSEdge& destination, MSStoppingPlace* toStop, const double arrivalPos, StageType type);

        /// destructor
        virtual ~Stage();

        /// returns the destination edge
        const MSEdge& getDestination() const;

        /// returns the destination stop (if any)
        const MSStoppingPlace* getDestinationStop() const {
            return myDestinationStop;
        }

        double getArrivalPos() const {
            return myArrivalPos;
        }

        /// Returns the current edge
        virtual const MSEdge* getEdge() const = 0;
        virtual const MSEdge* getFromEdge() const = 0;
        virtual double getEdgePos(SUMOTime now) const = 0;

        /// returns the position of the transportable
        virtual Position getPosition(SUMOTime now) const = 0;

        /// returns the angle of the transportable
        virtual double getAngle(SUMOTime now) const = 0;

        ///
        StageType getStageType() const {
            return myType;
        }

        /// @brief return string representation of the current stage
        virtual std::string getStageDescription() const = 0;

        /// proceeds to this stage
        virtual void proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, Stage* previous) = 0;

        /// abort this stage (TraCI)
        virtual void abort(MSTransportable*) {};

        /// sets the walking speed (ignored in other stages)
        virtual void setSpeed(double) {};

        /// logs end of the step
        void setDeparted(SUMOTime now);

        /// logs end of the step
        virtual void setArrived(SUMOTime now);

        /// Whether the transportable waits for a vehicle of the line specified.
        virtual bool isWaitingFor(const std::string& line) const;

        /// @brief Whether the transportable waits for a vehicle
        virtual bool isWaiting4Vehicle() const {
            return false;
        }

        /// @brief Whether the transportable waits for a vehicle
        virtual SUMOVehicle* getVehicle() const {
            return 0;
        }

        /// @brief the time this transportable spent waiting
        virtual SUMOTime getWaitingTime(SUMOTime now) const = 0;

        /// @brief the speed of the transportable
        virtual double getSpeed() const = 0;

        /// @brief the edges of the current stage
        virtual ConstMSEdgeVector getEdges() const = 0;

        /// @brief get position on edge e at length at with orthogonal offset
        Position getEdgePosition(const MSEdge* e, double at, double offset) const;

        /// @brief get position on lane at length at with orthogonal offset
        Position getLanePosition(const MSLane* lane, double at, double offset) const;

        /// @brief get angle of the edge at a certain position
        double getEdgeAngle(const MSEdge* e, double at) const;

        /** @brief Called on writing tripinfo output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice& os, MSTransportable* transportable) const = 0;

        /** @brief Called on writing vehroute output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void routeOutput(OutputDevice& os) const = 0;

        /** @brief Called for writing the events output (begin of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void beginEventOutput(const MSTransportable& transportable, SUMOTime t, OutputDevice& os) const = 0;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSTransportable& transportable, SUMOTime t, OutputDevice& os) const = 0;

    protected:
        /// the next edge to reach by getting transported
        const MSEdge& myDestination;

        /// the stop to reach by getting transported (if any)
        MSStoppingPlace* const myDestinationStop;

        /// the position at which we want to arrive
        double myArrivalPos;

        /// the time at which this stage started
        SUMOTime myDeparted;

        /// the time at which this stage ended
        SUMOTime myArrived;

        /// The type of this stage
        StageType myType;

    private:
        /// @brief Invalidated copy constructor.
        Stage(const Stage&);

        /// @brief Invalidated assignment operator.
        Stage& operator=(const Stage&);

    };

    /**
    * A "real" stage performing a waiting over the specified time
    */
    class Stage_Waiting : public Stage {
    public:
        /// constructor
        Stage_Waiting(const MSEdge& destination, SUMOTime duration, SUMOTime until,
                      double pos, const std::string& actType, const bool initial);

        /// destructor
        virtual ~Stage_Waiting();

        /// abort this stage (TraCI)
        void abort(MSTransportable*);

        /// Returns the current edge
        const MSEdge* getEdge() const;
        const MSEdge* getFromEdge() const;
        double getEdgePos(SUMOTime now) const;
        SUMOTime getUntil() const;

        ///
        Position getPosition(SUMOTime now) const;

        double getAngle(SUMOTime now) const;

        SUMOTime getWaitingTime(SUMOTime now) const;

        double getSpeed() const;

        ConstMSEdgeVector getEdges() const;

        std::string getStageDescription() const {
            return "waiting (" + myActType + ")";
        }

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, Stage* previous);

        /** @brief Called on writing tripinfo output
        *
        * @param[in] os The stream to write the information into
        * @exception IOError not yet implemented
        */
        virtual void tripInfoOutput(OutputDevice& os, MSTransportable* transportable) const;

        /** @brief Called on writing vehroute output
        *
        * @param[in] os The stream to write the information into
        * @exception IOError not yet implemented
        */
        virtual void routeOutput(OutputDevice& os) const;

        /** @brief Called for writing the events output
        * @param[in] os The stream to write the information into
        * @exception IOError not yet implemented
        */
        virtual void beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
        * @param[in] os The stream to write the information into
        * @exception IOError not yet implemented
        */
        virtual void endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const;

    private:
        /// the time the person is waiting
        SUMOTime myWaitingDuration;

        /// the time until the person is waiting
        SUMOTime myWaitingUntil;

        /// @brief The type of activity
        std::string myActType;

    private:
        /// @brief Invalidated copy constructor.
        Stage_Waiting(const Stage_Waiting&);

        /// @brief Invalidated assignment operator.
        Stage_Waiting& operator=(const Stage_Waiting&);

    };

    /**
    * A "real" stage performing the travelling by a transport system
    * The given route will be chosen. The travel time is computed by the simulation
    */
    class Stage_Driving : public Stage {
    public:
        /// constructor
        Stage_Driving(const MSEdge& destination, MSStoppingPlace* toStop,
                      const double arrivalPos, const std::vector<std::string>& lines);

        /// destructor
        virtual ~Stage_Driving();

        /// abort this stage (TraCI)
        void abort(MSTransportable*);

        /// Returns the current edge
        const MSEdge* getEdge() const;
        const MSEdge* getFromEdge() const;
        double getEdgePos(SUMOTime now) const;

        ///
        Position getPosition(SUMOTime now) const;

        double getAngle(SUMOTime now) const;

        /// Whether the person waits for a vehicle of the line specified.
        bool isWaitingFor(const std::string& line) const;

        /// @brief Whether the person waits for a vehicle
        bool isWaiting4Vehicle() const;

        /// @brief Return where the person waits and for what
        std::string getWaitingDescription() const;

        SUMOVehicle* getVehicle() const {
            return myVehicle;
        }

        /// @brief time spent waiting for a ride
        SUMOTime getWaitingTime(SUMOTime now) const;

        double getSpeed() const;

        ConstMSEdgeVector getEdges() const;

        void setVehicle(SUMOVehicle* v);

        /// @brief marks arrival time and records driven distance
        void setArrived(SUMOTime now);

        /** @brief Called for writing the events output
        * @param[in] os The stream to write the information into
        * @exception IOError not yet implemented
        */
        virtual void beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
        * @param[in] os The stream to write the information into
        * @exception IOError not yet implemented
        */
        virtual void endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const;

    protected:
        /// the lines  to choose from
        const std::set<std::string> myLines;

        /// @brief The taken vehicle
        SUMOVehicle* myVehicle;
        /// @brief cached vehicle data for output after the vehicle has been removed
        std::string myVehicleID;
        std::string myVehicleLine;
        SUMOVehicleClass myVehicleVClass;
        double myVehicleDistance;

        double myWaitingPos;
        /// @brief The time since which this person is waiting for a ride
        SUMOTime myWaitingSince;
        const MSEdge* myWaitingEdge;
        Position myStopWaitPos;

    private:
        /// @brief Invalidated copy constructor.
        Stage_Driving(const Stage_Driving&);

        /// @brief Invalidated assignment operator.
        Stage_Driving& operator=(const Stage_Driving&);

    };

    /// the structure holding the plan of a transportable
    typedef std::vector<MSTransportable::Stage*> MSTransportablePlan;

    /// constructor
    MSTransportable(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportablePlan* plan);

    /// destructor
    virtual ~MSTransportable();

    /* @brief proceeds to the next step of the route,
     * @return Whether the transportables plan continues  */
    virtual bool proceed(MSNet* net, SUMOTime time) = 0;

    /// returns the id of the transportable
    const std::string& getID() const;

    inline const SUMOVehicleParameter& getParameter() const {
        return *myParameter;
    }

    inline const MSVehicleType& getVehicleType() const {
        return *myVType;
    }

    /// Returns the desired departure time.
    SUMOTime getDesiredDepart() const;

    /// logs depart time of the current stage
    void setDeparted(SUMOTime now);

    /// Returns the current destination.
    const MSEdge& getDestination() const {
        return (*myStep)->getDestination();
    }

    /// Returns the destination after the current destination.
    const MSEdge& getNextDestination() const {
        return (*(myStep + 1))->getDestination();
    }

    /// @brief Returns the current edge
    const MSEdge* getEdge() const {
        return (*myStep)->getEdge();
    }

    /// @brief Returns the departure edge
    const MSEdge* getFromEdge() const {
        return (*myStep)->getFromEdge();
    }

    /// @brief Return the position on the edge
    virtual double getEdgePos() const;

    /// @brief Return the Network coordinate of the transportable
    virtual Position getPosition() const;

    /// @brief return the current angle of the transportable
    virtual double getAngle() const;

    /// @brief the time this transportable spent waiting in seconds
    virtual double getWaitingSeconds() const;

    /// @brief the current speed of the transportable
    virtual double getSpeed() const;

    /// @brief the current speed factor of the transportable (where applicable)
    virtual double getSpeedFactor() const {
        return 1;
    }

    /// @brief the current stage type of the transportable
    StageType getCurrentStageType() const {
        return (*myStep)->getStageType();
    }

    /// @brief the stage type for the nth next stage
    StageType getStageType(int next) const {
        assert(myStep + next < myPlan->end());
        assert(myStep + next >= myPlan->begin());
        return (*(myStep + next))->getStageType();
    }

    /// Returns the current stage description as a string
    std::string getCurrentStageDescription() const {
        return (*myStep)->getStageDescription();
    }

    /// @brief Return the current stage
    MSTransportable::Stage* getCurrentStage() const {
        return *myStep;
    }

    /// @brief Return the edges of the nth next stage
    ConstMSEdgeVector getEdges(int next) const {
        assert(myStep + next < myPlan->end());
        assert(myStep + next >= myPlan->begin());
        return (*(myStep + next))->getEdges();
    }

    /// @brief Return the number of remaining stages (including the current)
    int getNumRemainingStages() const;

    /// @brief Return the total number stages in this persons plan
    int getNumStages() const;

    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    virtual void tripInfoOutput(OutputDevice& os, MSTransportable* transportable) const = 0;

    /** @brief Called on writing vehroute output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    virtual void routeOutput(OutputDevice& os) const = 0;

    /// @brief Whether the transportable waits for a vehicle of the line specified.
    bool isWaitingFor(const std::string& line) const {
        return (*myStep)->isWaitingFor(line);
    }

    /// @brief Whether the transportable waits for a vehicle
    bool isWaiting4Vehicle() const {
        return (*myStep)->isWaiting4Vehicle();
    }

    /// @brief The vehicle associated with this transportable
    SUMOVehicle* getVehicle() const {
        return (*myStep)->getVehicle();
    }

    /// @brief Appends the given stage to the current plan
    void appendStage(Stage* stage, int next = -1);

    /// @brief removes the nth next stage
    void removeStage(int next);

    /// sets the walking speed (ignored in other stages)
    void setSpeed(double speed);

    /// @brief returns the final arrival pos
    double getArrivalPos() const {
        return myPlan->back()->getArrivalPos();
    }

    /// @brief returns the final arrival edge
    const MSEdge* getArrivalEdge() const {
        return myPlan->back()->getEdges().back();
    }

    /** @brief Replaces the current vehicle type by the one given
    *
    * If the currently used vehicle type is marked as being used by this vehicle
    *  only, it is deleted, first. The new, given type is then assigned to
    *  "myVType".
    * @param[in] type The new vehicle type
    * @see MSTransportable::myVType
    */
    void replaceVehicleType(MSVehicleType* type);


    /** @brief Replaces the current vehicle type with a new one used by this vehicle only
    *
    * If the currently used vehicle type is already marked as being used by this vehicle
    *  only, no new type is created.
    * @return The new modifiable vehicle type
    * @see MSTransportable::myVType
    */
    MSVehicleType& getSingularType();


    /// @brief return the bounding box of the person
    PositionVector getBoundingBox() const;

protected:
    /// @brief the offset for computing positions when standing at an edge
    static const double ROADSIDE_OFFSET;

    /// the plan of the transportable
    const SUMOVehicleParameter* myParameter;

    /// @brief This transportable's type. (mainly used for drawing related information
    /// Note sure if it is really necessary
    MSVehicleType* myVType;

    /// @brief Whether events shall be written
    bool myWriteEvents;

    /// the plan of the transportable
    MSTransportablePlan* myPlan;

    /// the iterator over the route
    MSTransportablePlan::iterator myStep;

private:
    /// @brief Invalidated copy constructor.
    MSTransportable(const MSTransportable&);

    /// @brief Invalidated assignment operator.
    MSTransportable& operator=(const MSTransportable&);

};


#endif

/****************************************************************************/
