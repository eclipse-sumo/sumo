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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSEdge;
class OutputDevice;
class SUMOVehicleParameter;
class MSBusStop;
class SUMOVehicle;
class MSVehicleType;

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
        virtual std::string getStageTypeName() const = 0;

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

        /// @brief the time this person spent waiting for a vehicle
        virtual SUMOTime timeWaiting4Vehicle(SUMOTime /*now*/) const {
            return false;
        }

        /// @brief get position on edge e at length at with orthogonal offset
        Position getEdgePosition(const MSEdge* e, SUMOReal at, SUMOReal offset) const;

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

        std::string getStageTypeName() const {
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

        SUMOTime moveToNextEdge(MSPerson* person, SUMOTime currentTime);


        class MoveToNextEdge : public Command {
        public:
            MoveToNextEdge(MSPerson* person, MSPersonStage_Walking& walk) : myParent(walk), myPerson(person) {}
            ~MoveToNextEdge() {}
            SUMOTime execute(SUMOTime currentTime) {
                return myParent.moveToNextEdge(myPerson, currentTime);
            }
        private:
            MSPersonStage_Walking& myParent;
            MSPerson* myPerson;
        private:
            /// @brief Invalidated assignment operator.
            MoveToNextEdge& operator=(const MoveToNextEdge&);

        };


    private:
        void computeWalkingTime(const MSEdge* const e, SUMOReal fromPos, SUMOReal toPos, MSBusStop* bs);


    private:
        /// the time the person is walking
        SUMOTime myWalkingTime;

        /// @brief The route of the person
        std::vector<const MSEdge*> myRoute;


        std::vector<const MSEdge*>::iterator myRouteStep;

        /// @brief A vector of computed times an edge is reached
        //std::vector<SUMOTime> myArrivalTimes;

        SUMOReal myDepartPos;
        SUMOReal myArrivalPos;
        MSBusStop* myDestinationBusStop;
        SUMOTime myLastEntryTime;
        SUMOReal mySpeed;

        SUMOReal myCurrentBeginPos, myCurrentLength, myCurrentDuration;
        //bool myDurationWasGiven;
        //SUMOReal myOverallLength;

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

        std::string getStageTypeName() const;

        /// Whether the person waits for a vehicle of the line specified.
        bool isWaitingFor(const std::string& line) const;

        /// @brief Whether the person waits for a vehicle
        bool isWaiting4Vehicle() const;

        /// @brief time spent waiting for a ride
        SUMOTime timeWaiting4Vehicle(SUMOTime now) const;

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

        std::string getStageTypeName() const {
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

    SUMOReal getEdgePos(SUMOTime now) const {
        return (*myStep)->getEdgePos(now);
    }

    ///
    virtual Position getPosition(SUMOTime now) const {
        return (*myStep)->getPosition(now);
    }


    SUMOReal getAngle(SUMOTime now) const {
        return (*myStep)->getAngle(now);
    }

    ///
    StageType getCurrentStageType() const {
        return (*myStep)->getStageType();
    }


    std::string getCurrentStageTypeName() const {
        return (*myStep)->getStageTypeName();
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


    /// @brief the time this person spent waiting for a vehicle
    SUMOTime timeWaiting4Vehicle(SUMOTime now) const {
        return (*myStep)->timeWaiting4Vehicle(now);
    }

    const SUMOVehicleParameter& getParameter() const {
        return *myParameter;
    }


    inline const MSVehicleType& getVehicleType() const {
        return *myVType;
    }


    /// @brief the offset for computing person positions when walking
    static const SUMOReal SIDEWALK_OFFSET;

private:
    /// @brief Invalidated copy constructor.
    MSPerson(const MSPerson&);

    /// @brief Invalidated assignment operator.
    MSPerson& operator=(const MSPerson&);

};


#endif

/****************************************************************************/
