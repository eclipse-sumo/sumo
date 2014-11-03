/****************************************************************************/
/// @file    MSPerson.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The class for modelling person-movements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSPerson_h
#define MSPerson_h


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
class MSBusStop;
class SUMOVehicle;
class MSVehicleType;
class MSPModel;
class PedestrianState;

typedef std::vector<const MSEdge*> MSEdgeVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSPerson
  *
  * The class holds a simulated person together with her movement stages
  */
class MSPerson {
public:
    enum StageType {
        WALKING = 0,
        DRIVING = 1,
        WAITING = 2
    };


    /**
     * The "abstract" class for a single stage of a persons movement
     * Contains the destination of the current movement step
     */
    class MSPersonStage {
    public:
        /// constructor
        MSPersonStage(const MSEdge& destination, StageType type);

        /// destructor
        virtual ~MSPersonStage();

        /// returns the destination edge
        const MSEdge& getDestination() const;

        /// Returns the current edge
        virtual const MSEdge* getEdge() const = 0;
        virtual const MSEdge* getFromEdge() const = 0;
        virtual SUMOReal getEdgePos(SUMOTime now) const = 0;

        ///
        virtual Position getPosition(SUMOTime now) const = 0;
        virtual SUMOReal getAngle(SUMOTime now) const = 0;

        ///
        StageType getStageType() const {
            return myType;
        }

        /// @brief return string representation of the current stage
        virtual std::string getStageDescription() const = 0;

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSPerson* person, SUMOTime now, MSEdge* previousEdge, const SUMOReal at) = 0;

        /// logs end of the step
        void setDeparted(SUMOTime now);

        /// logs end of the step
        void setArrived(SUMOTime now);

        /// Whether the person waits for a vehicle of the line specified.
        virtual bool isWaitingFor(const std::string& line) const;

        /// @brief Whether the person waits for a vehicle
        virtual bool isWaiting4Vehicle() const {
            return false;
        }

        /// @brief the time this person spent waiting
        virtual SUMOTime getWaitingTime(SUMOTime now) const = 0;

        /// @brief the time this person spent waiting
        virtual SUMOReal getSpeed() const = 0;

        /// @brief get position on edge e at length at with orthogonal offset
        Position getEdgePosition(const MSEdge* e, SUMOReal at, SUMOReal offset) const;

        /// @brief get position on lane at length at with orthogonal offset
        Position getLanePosition(const MSLane* lane, SUMOReal at, SUMOReal offset) const;

        SUMOReal getEdgeAngle(const MSEdge* e, SUMOReal at) const;

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
        virtual void beginEventOutput(const MSPerson& p, SUMOTime t, OutputDevice& os) const = 0;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSPerson& p, SUMOTime t, OutputDevice& os) const = 0;


    protected:
        /// the next edge to reach (either by walking or driving)
        const MSEdge& myDestination;

        /// the time at which this stage started
        SUMOTime myDeparted;

        /// the time at which this stage ended
        SUMOTime myArrived;

        /// The type of this stage
        StageType myType;

    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage(const MSPersonStage&);

        /// @brief Invalidated assignment operator.
        MSPersonStage& operator=(const MSPersonStage&);

    };

    /**
     * A "real" stage performing the walking to an edge
     * The walking does not need any route as it is not simulated.
     * Only the duration is needed
     */
    class MSPersonStage_Walking : public MSPersonStage {
        friend class MSPModel;
        friend class GUIPerson; // debugging

    public:
        /// constructor
        MSPersonStage_Walking(const std::vector<const MSEdge*>& route, MSBusStop* toBS, SUMOTime walkingTime, SUMOReal speed, SUMOReal departPos, SUMOReal arrivalPos);

        /// destructor
        ~MSPersonStage_Walking();

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSPerson* person, SUMOTime now, MSEdge* previousEdge, const SUMOReal at);

        /// Returns the current edge
        const MSEdge* getEdge() const;
        const MSEdge* getFromEdge() const;
        SUMOReal getEdgePos(SUMOTime now) const;

        ///
        Position getPosition(SUMOTime now) const;

        SUMOReal getAngle(SUMOTime now) const;

        SUMOTime getWaitingTime(SUMOTime now) const;

        SUMOReal getSpeed() const;

        std::string getStageDescription() const {
            return "walking";
        }

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
        virtual void beginEventOutput(const MSPerson& p, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSPerson& p, SUMOTime t, OutputDevice& os) const;

        /// @brief move forward and return whether the person arrived
        bool moveToNextEdge(MSPerson* person, SUMOTime currentTime, MSEdge* nextInternal = 0);


        /// @brief accessors to be used by MSPModel
        //@{
        inline SUMOReal getMaxSpeed() const {
            return mySpeed;
        }
        inline SUMOReal getDepartPos() const {
            return myDepartPos;
        }
        inline SUMOReal getArrivalPos() const {
            return myArrivalPos;
        }

        inline const MSEdge* getRouteEdge() const {
            return *myRouteStep;
        }
        inline const MSEdge* getNextRouteEdge() const {
            return myRouteStep == myRoute.end() - 1 ? 0 : *(myRouteStep + 1);
        }
        inline const std::vector<const MSEdge*>& getRoute() const {
            return myRoute;
        }

        PedestrianState* getPedestrianState() const {
            return myPedestrianState;
        }
        //@}


    private:

        /* @brief compute average speed if the total walking duration is given
         * @note Must be callled when the previous stage changes myDepartPos from the default*/
        SUMOReal computeAverageSpeed() const;


    private:
        /// the time the person is walking
        SUMOTime myWalkingTime;

        /// @brief The route of the person
        std::vector<const MSEdge*> myRoute;


        std::vector<const MSEdge*>::iterator myRouteStep;

        /// @brief The current internal edge this person is on or 0
        MSEdge* myCurrentInternalEdge;

        /// @brief A vector of computed times an edge is reached
        //std::vector<SUMOTime> myArrivalTimes;

        SUMOReal myDepartPos;
        SUMOReal myArrivalPos;
        MSBusStop* myDestinationBusStop;
        SUMOReal mySpeed;

        /// @brief state that is to be manipulated by MSPModel
        PedestrianState* myPedestrianState;

        class arrival_finder {
        public:
            /// constructor
            explicit arrival_finder(SUMOTime time) : myTime(time) {}

            /// comparison operator
            bool operator()(SUMOReal t) const {
                return myTime > t;
            }

        private:
            /// the searched arrival time
            SUMOTime myTime;
        };

    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage_Walking(const MSPersonStage_Walking&);

        /// @brief Invalidated assignment operator.
        MSPersonStage_Walking& operator=(const MSPersonStage_Walking&);

    };

    /**
     * A "real" stage performing the travelling by a transport system
     * The given route will be chosen. The travel time is computed by the simulation
     */
    class MSPersonStage_Driving : public MSPersonStage {
    public:
        /// constructor
        MSPersonStage_Driving(const MSEdge& destination, MSBusStop* toBS,
                              const std::vector<std::string>& lines);

        /// destructor
        ~MSPersonStage_Driving();

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSPerson* person, SUMOTime now, MSEdge* previousEdge, const SUMOReal at);

        /// Returns the current edge
        const MSEdge* getEdge() const;
        const MSEdge* getFromEdge() const;
        SUMOReal getEdgePos(SUMOTime now) const;

        ///
        Position getPosition(SUMOTime now) const;

        SUMOReal getAngle(SUMOTime now) const;

        std::string getStageDescription() const;

        /// Whether the person waits for a vehicle of the line specified.
        bool isWaitingFor(const std::string& line) const;

        /// @brief Whether the person waits for a vehicle
        bool isWaiting4Vehicle() const;

        /// @brief time spent waiting for a ride
        SUMOTime getWaitingTime(SUMOTime now) const;

        SUMOReal getSpeed() const;

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
        virtual void beginEventOutput(const MSPerson& p, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSPerson& p, SUMOTime t, OutputDevice& os) const;

    private:
        /// the lines  to choose from
        const std::set<std::string> myLines;

        /// @brief The taken vehicle
        SUMOVehicle* myVehicle;

        MSBusStop* myDestinationBusStop;
        SUMOReal myWaitingPos;
        /// @brief The time since which this person is waiting for a ride
        SUMOTime myWaitingSince;
        const MSEdge* myWaitingEdge;

    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage_Driving(const MSPersonStage_Driving&);

        /// @brief Invalidated assignment operator.
        MSPersonStage_Driving& operator=(const MSPersonStage_Driving&);

    };

    /**
     * A "real" stage performing a waiting over the specified time
     */
    class MSPersonStage_Waiting : public MSPersonStage {
    public:
        /// constructor
        MSPersonStage_Waiting(const MSEdge& destination,
                              SUMOTime duration, SUMOTime until, SUMOReal pos, const std::string& actType);

        /// destructor
        ~MSPersonStage_Waiting();

        /// Returns the current edge
        const MSEdge* getEdge() const;
        const MSEdge* getFromEdge() const;
        SUMOReal getEdgePos(SUMOTime now) const;
        SUMOTime getUntil() const;

        ///
        Position getPosition(SUMOTime now) const;

        SUMOReal getAngle(SUMOTime now) const;

        SUMOTime getWaitingTime(SUMOTime now) const;

        SUMOReal getSpeed() const;

        std::string getStageDescription() const {
            return "waiting (" + myActType + ")";
        }

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSPerson* person, SUMOTime now, MSEdge* previousEdge, const SUMOReal at);

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
        virtual void beginEventOutput(const MSPerson& p, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSPerson& p, SUMOTime t, OutputDevice& os) const;

    private:
        /// the time the person is waiting
        SUMOTime myWaitingDuration;

        /// the time until the person is waiting
        SUMOTime myWaitingUntil;

        /// the time the person is waiting
        SUMOTime myWaitingStart;

        /// @brief The type of activity
        std::string myActType;

        SUMOReal myStartPos;


    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage_Waiting(const MSPersonStage_Waiting&);

        /// @brief Invalidated assignment operator.
        MSPersonStage_Waiting& operator=(const MSPersonStage_Waiting&);

    };

public:
    /// the structure holding the plan of a person
    typedef std::vector<MSPersonStage*> MSPersonPlan;

protected:
    /// the plan of the person
    const SUMOVehicleParameter* myParameter;

    /// @brief This Persons's type. (mainly used for drawing related information
    const MSVehicleType* myVType;

    /// the plan of the person
    MSPersonPlan* myPlan;

    /// the iterator over the route
    MSPersonPlan::iterator myStep;

    /// @brief Whether events shall be written
    bool myWriteEvents;

public:
    /// constructor
    MSPerson(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSPersonPlan* plan);

    /// destructor
    virtual ~MSPerson();

    /// returns the person id
    const std::string& getID() const;

    /* @brief proceeds to the next step of the route,
     * @return Whether the persons plan continues  */
    bool proceed(MSNet* net, SUMOTime time);

    /// Returns the desired departure time.
    SUMOTime getDesiredDepart() const;

    /// logs end of the step
    void setDeparted(SUMOTime now);

    /// Returns the current destination.
    const MSEdge& getDestination() const {
        return (*myStep)->getDestination();
    }

    /// @brief Returns the current edge
    const MSEdge* getEdge() const {
        return (*myStep)->getEdge();
    }

    /// @brief Returns the departure edge
    const MSEdge* getFromEdge() const {
        return (*myStep)->getFromEdge();
    }

    /// @brief return the offset from the start of the current edge
    virtual SUMOReal getEdgePos() const;

    /// @brief return the Network coordinate of the person
    virtual Position getPosition() const;

    /// @brief return the current angle of the person
    virtual SUMOReal getAngle() const;

    /// @brief the time this person spent waiting in seconds
    virtual SUMOReal getWaitingSeconds() const;

    /// @brief the current speed of the person
    virtual SUMOReal getSpeed() const;

    ///
    StageType getCurrentStageType() const {
        return (*myStep)->getStageType();
    }


    std::string getCurrentStageDescription() const {
        return (*myStep)->getStageDescription();
    }

    MSPersonStage* getCurrentStage() const {
        return *myStep;
    }

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

    /// Whether the person waits for a vehicle of the line specified.
    bool isWaitingFor(const std::string& line) const {
        return (*myStep)->isWaitingFor(line);
    }

    /// Whether the person waits for a vehicle
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
    MSPerson(const MSPerson&);

    /// @brief Invalidated assignment operator.
    MSPerson& operator=(const MSPerson&);

};


#endif

/****************************************************************************/
