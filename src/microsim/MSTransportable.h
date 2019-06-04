/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <set>
#include <cassert>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Boundary.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/vehicle/SUMOTrafficObject.h>


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
class MSTransportableDevice;

typedef std::vector<const MSEdge*> ConstMSEdgeVector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSTransportable
  *
  * The class holds a simulated moveable object
  */
class MSTransportable : public SUMOTrafficObject {
public:
    enum StageType {
        WAITING_FOR_DEPART = 0,
        WAITING = 1,
        MOVING_WITHOUT_VEHICLE = 2, // walking for persons, tranship for containers
        DRIVING = 3,
        ACCESS = 4,
        TRIP = 5
    };

    /**
    * The "abstract" class for a single stage of a movement
    * Contains the destination of the current movement step
    */
    class Stage {
    public:
        /// constructor
        Stage(const MSEdge* destination, MSStoppingPlace* toStop, const double arrivalPos, StageType type);

        /// destructor
        virtual ~Stage();

        /// returns the destination edge
        const MSEdge* getDestination() const;

        /// returns the destination stop (if any)
        MSStoppingPlace* getDestinationStop() const {
            return myDestinationStop;
        }

        /// returns the origin stop (if any). only needed for Stage_Trip
        virtual const MSStoppingPlace* getOriginStop() const {
            return nullptr;
        }

        double getArrivalPos() const {
            return myArrivalPos;
        }

        /// Returns the current edge
        virtual const MSEdge* getEdge() const;
        virtual const MSEdge* getFromEdge() const;
        virtual double getEdgePos(SUMOTime now) const;

        /// returns the position of the transportable
        virtual Position getPosition(SUMOTime now) const = 0;

        /// returns the angle of the transportable
        virtual double getAngle(SUMOTime now) const = 0;

        ///
        StageType getStageType() const {
            return myType;
        }

        /// @brief return (brief) string representation of the current stage
        virtual std::string getStageDescription() const = 0;

        /// @brief return string summary of the current stage
        virtual std::string getStageSummary() const = 0;

        /// proceeds to this stage
        virtual void proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, Stage* previous) = 0;

        /// abort this stage (TraCI)
        virtual void abort(MSTransportable*) {};

        /// sets the walking speed (ignored in other stages)
        virtual void setSpeed(double) {};

        /// get departure time of stage
        SUMOTime getDeparted() const;

        /// logs end of the step
        void setDeparted(SUMOTime now);

        /// logs end of the step
        virtual void setArrived(MSNet* net, MSTransportable* transportable, SUMOTime now);

        /// Whether the transportable waits for the given vehicle
        virtual bool isWaitingFor(const SUMOVehicle* vehicle) const;

        /// @brief Whether the transportable waits for a vehicle
        virtual bool isWaiting4Vehicle() const {
            return false;
        }

        /// @brief Whether the transportable waits for a vehicle
        virtual SUMOVehicle* getVehicle() const {
            return nullptr;
        }

        /// @brief the time this transportable spent waiting
        virtual SUMOTime getWaitingTime(SUMOTime now) const;

        /// @brief the speed of the transportable
        virtual double getSpeed() const;

        /// @brief the edges of the current stage
        virtual ConstMSEdgeVector getEdges() const;

        /// @brief get position on edge e at length at with orthogonal offset
        Position getEdgePosition(const MSEdge* e, double at, double offset) const;

        /// @brief get position on lane at length at with orthogonal offset
        Position getLanePosition(const MSLane* lane, double at, double offset) const;

        /// @brief get angle of the edge at a certain position
        double getEdgeAngle(const MSEdge* e, double at) const;

        void setDestination(const MSEdge* newDestination, MSStoppingPlace* newDestStop);

        /// @brief get travel distance in this stage
        virtual double getDistance() const = 0;

        /** @brief Called on writing tripinfo output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice& os, const MSTransportable* const transportable) const = 0;

        /** @brief Called on writing vehroute output
         * @param[in] os The stream to write the information into
         * @param[in] withRouteLength whether route length shall be written
         * @exception IOError not yet implemented
         */
        virtual void routeOutput(OutputDevice& os, const bool withRouteLength) const = 0;

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

        virtual Stage* clone() const = 0;

    protected:
        /// the next edge to reach by getting transported
        const MSEdge* myDestination;

        /// the stop to reach by getting transported (if any)
        MSStoppingPlace* myDestinationStop;

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
    * A "placeholder" stage storing routing info which will result in real stages when routed
    */
    class Stage_Trip : public Stage {
    public:
        /// constructor
        Stage_Trip(const MSEdge* origin, MSStoppingPlace* fromStop,
                   const MSEdge* destination, MSStoppingPlace* toStop,
                   const SUMOTime duration, const SVCPermissions modeSet,
                   const std::string& vTypes, const double speed, const double walkFactor,
                   const double departPosLat, const bool hasArrivalPos, const double arrivalPos);

        /// destructor
        virtual ~Stage_Trip();

        Stage* clone() const;

        const MSEdge* getEdge() const;

        const MSStoppingPlace* getOriginStop() const {
            return myOriginStop;
        }

        double getEdgePos(SUMOTime now) const;

        Position getPosition(SUMOTime now) const;

        double getAngle(SUMOTime now) const;

        double getDistance() const {
            // invalid
            return -1;
        }

        std::string getStageDescription() const {
            return "trip";
        }

        std::string getStageSummary() const;

        /// logs end of the step
        virtual void setArrived(MSNet* net, MSTransportable* transportable, SUMOTime now);

        /// change origin for parking area rerouting
        void setOrigin(const MSEdge* origin) {
            myOrigin = origin;
        }

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, Stage* previous);

        /** @brief Called on writing tripinfo output
        *
        * @param[in] os The stream to write the information into
        * @exception IOError not yet implemented
        */
        virtual void tripInfoOutput(OutputDevice& os, const MSTransportable* const transportable) const;

        /** @brief Called on writing vehroute output
        *
        * @param[in] os The stream to write the information into
        * @exception IOError not yet implemented
        */
        virtual void routeOutput(OutputDevice& os, const bool withRouteLength) const;

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
        /// the origin edge
        const MSEdge* myOrigin;

        /// the origin edge
        const MSStoppingPlace* myOriginStop;

        /// the time the trip should take (applies to only walking)
        SUMOTime myDuration;

        /// @brief The allowed modes of transportation
        const SVCPermissions myModeSet;

        /// @brief The possible vehicles to use
        const std::string myVTypes;

        /// @brief The walking speed
        const double mySpeed;

        /// @brief The factor to apply to walking durations
        const double myWalkFactor;

        /// @brief The depart position
        double myDepartPos;

        /// @brief The lateral depart position
        const double myDepartPosLat;

        /// @brief whether an arrivalPos was in the input
        const bool myHaveArrivalPos;

    private:
        /// @brief Invalidated copy constructor.
        Stage_Trip(const Stage_Trip&);

        /// @brief Invalidated assignment operator.
        Stage_Trip& operator=(const Stage_Trip&);

    };

    /**
    * A "real" stage performing a waiting over the specified time
    */
    class Stage_Waiting : public Stage {
    public:
        /// constructor
        Stage_Waiting(const MSEdge* destination, MSStoppingPlace* toStop, SUMOTime duration, SUMOTime until,
                      double pos, const std::string& actType, const bool initial);

        /// destructor
        virtual ~Stage_Waiting();

        Stage* clone() const;

        /// abort this stage (TraCI)
        void abort(MSTransportable*);

        SUMOTime getUntil() const;

        ///
        Position getPosition(SUMOTime now) const;

        double getAngle(SUMOTime now) const;

        /// @brief get travel distance in this stage
        double getDistance() const {
            return 0;
        }

        SUMOTime getWaitingTime(SUMOTime now) const;

        std::string getStageDescription() const {
            return "waiting (" + myActType + ")";
        }

        std::string getStageSummary() const;

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, Stage* previous);

        /** @brief Called on writing tripinfo output
        *
        * @param[in] os The stream to write the information into
        * @exception IOError not yet implemented
        */
        virtual void tripInfoOutput(OutputDevice& os, const MSTransportable* const transportable) const;

        /** @brief Called on writing vehroute output
        *
        * @param[in] os The stream to write the information into
        * @exception IOError not yet implemented
        */
        virtual void routeOutput(OutputDevice& os, const bool withRouteLength) const;

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
        Stage_Driving(const MSEdge* destination, MSStoppingPlace* toStop,
                      const double arrivalPos, const std::vector<std::string>& lines,
                      const std::string& intendedVeh = "", SUMOTime intendedDepart = -1);

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

        /// @brief get travel distance in this stage
        double getDistance() const {
            return myVehicleDistance;
        }

        /// Whether the person waits for the given vehicle
        bool isWaitingFor(const SUMOVehicle* vehicle) const;

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
        void setArrived(MSNet* net, MSTransportable* transportable, SUMOTime now);

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

        const std::set<std::string>& getLines() const {
            return myLines;
        }

        std::string getIntendedVehicleID() const {
            return myIntendedVehicleID;
        }

        SUMOTime getIntendedDepart() const {
            return myIntendedDepart;
        }

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

        std::string myIntendedVehicleID;
        SUMOTime myIntendedDepart;

    private:
        /// @brief Invalidated copy constructor.
        Stage_Driving(const Stage_Driving&);

        /// @brief Invalidated assignment operator.
        Stage_Driving& operator=(const Stage_Driving&);

    };

    /// @name inherited from SUMOTrafficObject
    /// @{
    bool isVehicle() const {
        return false;
    }

    bool isStopped() const {
        return getCurrentStageType() == WAITING;
    }

    double getSlope() const;

    double getChosenSpeedFactor() const {
        return 1.0;
    }

    SUMOVehicleClass getVClass() const;

    double getMaxSpeed() const;

    SUMOTime getWaitingTime() const;

    double getPreviousSpeed() const {
        return getSpeed();
    }

    double getAcceleration() const {
        return 0.0;
    }

    double getPositionOnLane() const {
        return getEdgePos();
    }

    double getBackPositionOnLane(const MSLane* /*lane*/) const {
        return getEdgePos();
    }

    Position getPosition(const double /*offset*/) const {
        return getPosition();
    }
    /// @}

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
    const MSEdge* getDestination() const {
        return (*myStep)->getDestination();
    }

    /// Returns the destination after the current destination.
    const MSEdge* getNextDestination() const {
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

    /// @brief return textual summary for the given stage
    std::string getStageSummary(int stageIndex) const;

    /// Returns the current stage description as a string
    std::string getCurrentStageDescription() const {
        return (*myStep)->getStageDescription();
    }

    /// @brief Return the current stage
    MSTransportable::Stage* getCurrentStage() const {
        return *myStep;
    }

    /// @brief Return the current stage
    MSTransportable::Stage* getNextStage(int next) const {
        assert(myStep + next >= myPlan->begin());
        assert(myStep + next < myPlan->end());
        return *(myStep + next);
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
    virtual void tripInfoOutput(OutputDevice& os) const = 0;

    /** @brief Called on writing vehroute output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    virtual void routeOutput(OutputDevice& os, const bool withRouteLength) const = 0;

    /// Whether the transportable waits for the given vehicle in the current step
    bool isWaitingFor(const SUMOVehicle* vehicle) const {
        return (*myStep)->isWaitingFor(vehicle);
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

    /// @brief return whether the person has reached the end of its plan
    bool hasArrived() const;

    /// @brief return whether the transportable has started it's plan
    bool hasDeparted() const;

    /// @brief adapt plan when the vehicle reroutes and now stops at replacement instead of orig
    void rerouteParkingArea(MSStoppingPlace* orig, MSStoppingPlace* replacement);

    /// @brief Returns a device of the given type if it exists or 0
    MSTransportableDevice* getDevice(const std::type_info& type) const;

    /** @brief Returns this vehicle's devices
     * @return This vehicle's devices
     */
    inline const std::vector<MSTransportableDevice*>& getDevices() const {
        return myDevices;
    }

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

    /// @brief The devices this transportable has
    std::vector<MSTransportableDevice*> myDevices;

private:
    /// @brief Invalidated copy constructor.
    MSTransportable(const MSTransportable&);

    /// @brief Invalidated assignment operator.
    MSTransportable& operator=(const MSTransportable&);

};


#endif

/****************************************************************************/
