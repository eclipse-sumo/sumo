/****************************************************************************/
/// @file    MSContainer.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Thu, 12 Jun 2014
/// @version $Id$
///
// The class for modelling container-movements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSContainer_h
#define MSContainer_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <set>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSEdge;
class MSLane;
class OutputDevice;
class SUMOVehicleParameter;
class MSContainerStop;
class SUMOVehicle;
class MSVehicleType;
class MSCModel_NonInteracting;
class CState;

//typedef std::vector<const MSEdge*> MSEdgeVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSContainer
  *
  * The class holds a simulated container together with its movement stages
  */


class MSContainer {
public:
    enum StageType {
        DRIVING = 0,
        WAITING = 1,
        TRANSHIP = 2
    };

    /// @brief the offset for computing container positions when standing at an edge
    static const SUMOReal ROADSIDE_OFFSET;


    /**
    * The "abstract" class for a single stage of a container movement
    * Contains the destination of the current movement step
    */
    class MSContainerStage {
    public:
        /// constructor
        MSContainerStage(const MSEdge& destination, StageType type);

        /// destructor
        virtual ~MSContainerStage();

        /// returns the destination edge
        const MSEdge& getDestination() const;

        /// Returns the current edge
        virtual const MSEdge* getEdge() const = 0;
        virtual const MSEdge* getFromEdge() const = 0;
        virtual SUMOReal getEdgePos(SUMOTime now) const = 0;

        /// returns the position of the container
        virtual Position getPosition(SUMOTime now) const = 0;

        /// returns the angle of the container
        virtual SUMOReal getAngle(SUMOTime now) const = 0;

        ///
        StageType getStageType() const {
            return myType;
        }

        /// @brief return string representation of the current stage
        virtual std::string getStageDescription() const = 0;

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSContainer* container, SUMOTime now, MSEdge* previousEdge, const SUMOReal at) = 0;

        /// logs end of the step
        void setDeparted(SUMOTime now);

        /// logs end of the step
        void setArrived(SUMOTime now);

        /// Whether the container waits for a vehicle of the line specified.
        virtual bool isWaitingFor(const std::string& line) const;

        /// @brief Whether the container waits for a vehicle
        virtual bool isWaiting4Vehicle() const {
            return false;
        }

        /// @brief the time this container spent waiting
        virtual SUMOTime getWaitingTime(SUMOTime now) const = 0;

        /// @brief the speed of the container
        virtual SUMOReal getSpeed() const = 0;

        /// @brief get position on edge e at length at with orthogonal offset
        Position getEdgePosition(const MSEdge* e, SUMOReal at, SUMOReal offset) const;

        /// @brief get position on lane at length at with orthogonal offset
        Position getLanePosition(const MSLane* lane, SUMOReal at, SUMOReal offset) const;

        /// @brief get angle of the edge at a certain position
        SUMOReal getEdgeAngle(const MSEdge* e, SUMOReal at) const;

        /* @brief Return the current ContainerStop or the destination containe stop
         *
         * returns the current container stop if the stage=Waiting and the
         * container stop from wich the container departs if stage=Driving
         */
        virtual MSContainerStop* getDepartContainerStop() const = 0;

        /** @brief Called on writing tripinfo output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice& os) const = 0;

        /** @brief Called on writing vehroute output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void routeOutput(OutputDevice& os) const = 0;

        /** @brief Called for writing the events output (begin of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void beginEventOutput(const MSContainer& container, SUMOTime t, OutputDevice& os) const = 0;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSContainer& container, SUMOTime t, OutputDevice& os) const = 0;

    protected:
        /// the next edge to reach by getting transported
        const MSEdge& myDestination;

        /// the time at which this stage started
        SUMOTime myDeparted;

        /// the time at which this stage ended
        SUMOTime myArrived;

        /// The type of this stage
        StageType myType;

    private:
        /// @brief Invalidated copy constructor.
        MSContainerStage(const MSContainerStage&);

        /// @brief Invalidated assignment operator.
        MSContainerStage& operator=(const MSContainerStage&);

    };

    /**
     * A "real" stage performing the travelling by a transport system
     * A container is in this stage if it is on a ride or if its waiting for a ride.
     * The given route will be chosen. The travel time is computed by the simulation
     */
    class MSContainerStage_Driving : public MSContainerStage {
    public:
        /// constructor
        MSContainerStage_Driving(const MSEdge& destination, MSContainerStop* toCS,
                                 const std::vector<std::string>& lines);

        /// destructor
        ~MSContainerStage_Driving();

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSContainer* container, SUMOTime now, MSEdge* previousEdge, const SUMOReal at);

        /// Returns the current edge
        const MSEdge* getEdge() const;
        const MSEdge* getFromEdge() const;
        SUMOReal getEdgePos(SUMOTime now) const;

        ///
        Position getPosition(SUMOTime now) const;

        /// @brief the angle of the vehicle or the angle of the edge + 90deg
        SUMOReal getAngle(SUMOTime now) const;

        /// @brief returns the stage description as a string
        std::string getStageDescription() const;

        /// Whether the container waits for a vehicle of the line specified.
        bool isWaitingFor(const std::string& line) const;

        /// @brief Whether the container waits for a vehicle
        bool isWaiting4Vehicle() const;

        /// @brief time spent waiting for a ride
        SUMOTime getWaitingTime(SUMOTime now) const;

        /** @brief the speed of the container
         *
         * If the container is still waiting for a ride zero will be returned.
         * If the container is on a ride the speed of the corresponding vehicle
         * will be returned.
         */
        SUMOReal getSpeed() const;

        /// @brief returns the container stop from which the container departs
        MSContainerStop* getDepartContainerStop() const;

        /// @brief assign a vehicle to the container
        void setVehicle(SUMOVehicle* v) {
            myVehicle = v;
        }

        /** @brief Called on writing tripinfo output
         *
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice& os) const;

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
        virtual void beginEventOutput(const MSContainer& container, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSContainer& container, SUMOTime t, OutputDevice& os) const;

    private:
        /// the lines  to choose from
        const std::set<std::string> myLines;

        /// @brief The taken vehicle
        SUMOVehicle* myVehicle;

        /// @brief The destination container stop
        MSContainerStop* myDestinationContainerStop;

        /// @brief The container stop from which the container departs
        MSContainerStop* myDepartContainerStop;

        SUMOReal myWaitingPos;

        /// @brief The time since which this container is waiting for a ride
        SUMOTime myWaitingSince;

        const MSEdge* myWaitingEdge;

    private:
        /// @brief Invalidated copy constructor.
        MSContainerStage_Driving(const MSContainerStage_Driving&);

        /// @brief Invalidated assignment operator.
        MSContainerStage_Driving& operator=(const MSContainerStage_Driving&);

    };

    /**
     * A "real" stage performing a waiting over the specified time
     * A container is in this stage if it is not on a ride or waiting for a ride, e.g.
     * if it is stored, or if gets filled or emptied.
     */
    class MSContainerStage_Waiting : public MSContainerStage {
    public:
        /// constructor
        MSContainerStage_Waiting(const MSEdge& destination,
                                 SUMOTime duration, SUMOTime until, SUMOReal pos, const std::string& actType);

        /// destructor
        ~MSContainerStage_Waiting();

        /// Returns the current edge
        const MSEdge* getEdge() const;

        /// Returns the current edge
        const MSEdge* getFromEdge() const;

        SUMOReal getEdgePos(SUMOTime now) const;

        /// Returns time until the container waits
        SUMOTime getUntil() const;

        Position getPosition(SUMOTime now) const;

        /// @brief the angle of the edge minus 90deg
        SUMOReal getAngle(SUMOTime now) const;

        SUMOTime getWaitingTime(SUMOTime now) const;

        /// Returns the speed of the container which is always zero in that stage
        SUMOReal getSpeed() const;

        /// Returns the current stage description as a string
        std::string getStageDescription() const {
            return "waiting (" + myActType + ")";
        }

        /* @brief returns the container stop at which the container is waiting
         *
         * this method was added to have a method 'getDepartContainerStop'
         * for MSContainer.
         */
        MSContainerStop* getDepartContainerStop() const;

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSContainer* container, SUMOTime now, MSEdge* previousEdge, const SUMOReal at);

        /** @brief Called on writing tripinfo output
         *
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice& os) const;

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
        virtual void beginEventOutput(const MSContainer& container, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSContainer& container, SUMOTime t, OutputDevice& os) const;

    private:
        /// the time the container is waiting
        SUMOTime myWaitingDuration;

        /// the time until the container is waiting
        SUMOTime myWaitingUntil;

        /// the time the container started waiting
        SUMOTime myWaitingStart;

        /// @brief The type of activity
        std::string myActType;

        SUMOReal myStartPos;

        /// @brief the container stop at which the container is waiting
        MSContainerStop* myCurrentContainerStop;


    private:
        /// @brief Invalidated copy constructor.
        MSContainerStage_Waiting(const MSContainerStage_Waiting&);

        /// @brief Invalidated assignment operator.
        MSContainerStage_Waiting& operator=(const MSContainerStage_Waiting&);

    };

    /**
    * A "real" stage performing the tranship of a container
    * A container is in this stage if it gets transhipred between two stops that are
    * assumed to be connected.
    */
    class MSContainerStage_Tranship : public MSContainerStage {
        friend class MSCModel_NonInteracting;

    public:
        /// constructor
        MSContainerStage_Tranship(const std::vector<const MSEdge*>& route, MSContainerStop* toCS, SUMOReal speed, SUMOReal departPos, SUMOReal arrivalPos);

        /// destructor
        ~MSContainerStage_Tranship();

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSContainer* container, SUMOTime now, MSEdge* previousEdge, const SUMOReal at);

        /// Returns the current edge
        const MSEdge* getEdge() const;

        /// Returns first edge of the containers route
        const MSEdge* getFromEdge() const;

        /// Returns last edge of the containers route
        const MSEdge* getToEdge() const;

        /// Returns the offset from the start of the current edge measured in its natural direction
        SUMOReal getEdgePos(SUMOTime now) const;

        /// Returns the position of the container
        Position getPosition(SUMOTime now) const;

        /// Returns the angle of the container
        SUMOReal getAngle(SUMOTime now) const;

        /// Returns the time the container spent waiting
        SUMOTime getWaitingTime(SUMOTime now) const;

        /// Returns the speed of the container
        SUMOReal getSpeed() const;

        /// Returns the current stage description as a string
        std::string getStageDescription() const {
            return "tranship";
        }

        /// @brief returns the container stop from which the container departs
        MSContainerStop* getDepartContainerStop() const;

        /** @brief Called on writing tripinfo output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice& os) const;

        /** @brief Called on writing vehroute output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void routeOutput(OutputDevice& os) const;

        /** @brief Called for writing the events output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void beginEventOutput(const MSContainer& c, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSContainer& c, SUMOTime t, OutputDevice& os) const;

        /// @brief move forward and return whether the container arrived
        bool moveToNextEdge(MSContainer* container, SUMOTime currentTime, MSEdge* nextInternal = 0);


        /// @brief accessors to be used by MSCModel_NonInteracting
        inline SUMOReal getMaxSpeed() const {
            return mySpeed;
        }

        inline SUMOReal getDepartPos() const {
            return myDepartPos;
        }

        inline SUMOReal getArrivalPos() const {
            return myArrivalPos;
        }

        inline const MSEdge* getNextRouteEdge() const {
            return myRouteStep == myRoute.end() - 1 ? 0 : *(myRouteStep + 1);
        }

        CState* getContainerState() const {
            return myContainerState;
        }

    private:
        /// @brief The route of the container
        std::vector<const MSEdge*> myRoute;

        /// @brief current step
        std::vector<const MSEdge*>::iterator myRouteStep;

        /// @brief the depart position
        SUMOReal myDepartPos;

        /// @brief the arrival position
        SUMOReal myArrivalPos;

        /// @brief the destination container stop
        MSContainerStop* myDestinationContainerStop;

        /// @brief The container stop from which the container departs
        MSContainerStop* myDepartContainerStop;

        /// @brief the speed of the container
        SUMOReal mySpeed;

        /// @brief state that is to be manipulated by MSCModel
        CState* myContainerState;

        /// @brief The current internal edge this container is on or 0
        MSEdge* myCurrentInternalEdge;

    private:
        /// @brief Invalidated copy constructor.
        MSContainerStage_Tranship(const MSContainerStage_Tranship&);

        /// @brief Invalidated assignment operator.
        MSContainerStage_Tranship& operator=(const MSContainerStage_Tranship&);

    };

public:
    /// the structure holding the plan of a container
    typedef std::vector<MSContainerStage*> MSContainerPlan;

    /// the last destination of the route of the container
    const MSEdge* lastDestination;

protected:
    /// the plan of the container
    const SUMOVehicleParameter* myParameter;

    /// @brief This container's type. (mainly used for drawing related information
    /// Note sure if it is really necessary
    const MSVehicleType* myVType;

    /// the plan of the container
    MSContainerPlan* myPlan;

    /// the iterator over the route
    MSContainerPlan::iterator myStep;

    /// @brief Whether events shall be written
    bool myWriteEvents;

public:
    /// constructor
    MSContainer(const SUMOVehicleParameter* pars, const MSVehicleType* vtype,  MSContainerPlan* plan);

    /// destructor
    virtual ~MSContainer();

    /// returns the container id
    const std::string& getID() const;

    /* @brief proceeds to the next step of the route,
     * @return Whether the containers plan continues  */
    bool proceed(MSNet* net, SUMOTime time);

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
    virtual SUMOReal getEdgePos() const;

    /// @brief Return the Network coordinate of the container
    virtual Position getPosition() const;

    /// @brief return the current angle of the container
    virtual SUMOReal getAngle() const;

    /// @brief the time this container spent waiting in seconds
    virtual SUMOReal getWaitingSeconds() const;

    /// @brief the current speed of the container
    virtual SUMOReal getSpeed() const;

    /// @brief the current stage type of the container
    StageType getCurrentStageType() const {
        return (*myStep)->getStageType();
    }

    /// Returns the current stage description as a string
    std::string getCurrentStageDescription() const {
        return (*myStep)->getStageDescription();
    }

    /// @brief Return the current stage
    MSContainerStage* getCurrentStage() const {
        return *myStep;
    }

    /* @brief Return the current ContainerStop or the destination containe stop
     *
     * returns the current container stop if the stage=Waiting and the
     * container stop from wich the container departs if stage=Driving
     */
    virtual MSContainerStop* getDepartContainerStop() const;


    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    void tripInfoOutput(OutputDevice& os) const;

    /** @brief Called on writing vehroute output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    void routeOutput(OutputDevice& os) const;

    /// Whether the container waits for a vehicle of the line specified.
    bool isWaitingFor(const std::string& line) const {
        return (*myStep)->isWaitingFor(line);
    }

    /// Whether the container waits for a vehicle
    bool isWaiting4Vehicle() const {
        return (*myStep)->isWaiting4Vehicle();
    }

    const SUMOVehicleParameter& getParameter() const {
        return *myParameter;
    }

    inline const MSVehicleType& getVehicleType() const {
        return *myVType;
    }

private:
    /// @brief Invalidated copy constructor.
    MSContainer(const MSContainer&);

    /// @brief Invalidated assignment operator.
    MSContainer& operator=(const MSContainer&);

};


#endif

/****************************************************************************/
