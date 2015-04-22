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
#include <microsim/MSTransportable.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSEdge;
class MSLane;
class OutputDevice;
class SUMOVehicleParameter;
class MSStoppingPlace;
class SUMOVehicle;
class MSVehicleType;
class MSPModel;
class PedestrianState;

typedef std::vector<const MSEdge*> ConstMSEdgeVector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSPerson
  *
  * The class holds a simulated person together with her movement stages
  */
class MSPerson : public MSTransportable {
public:
    /**
     * A "real" stage performing the walking to an edge
     * The walking does not need any route as it is not simulated.
     * Only the duration is needed
     */
    class MSPersonStage_Walking : public MSTransportable::Stage {
        friend class MSPModel;
        friend class GUIPerson; // debugging

    public:
        /// constructor
        MSPersonStage_Walking(const ConstMSEdgeVector& route, MSStoppingPlace* toBS, SUMOTime walkingTime, SUMOReal speed, SUMOReal departPos, SUMOReal arrivalPos);

        /// destructor
        ~MSPersonStage_Walking();

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* person, SUMOTime now, MSEdge* previousEdge, const SUMOReal at);

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
        virtual void beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const;

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
        inline const ConstMSEdgeVector& getRoute() const {
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
        ConstMSEdgeVector myRoute;


        ConstMSEdgeVector::iterator myRouteStep;

        /// @brief The current internal edge this person is on or 0
        MSEdge* myCurrentInternalEdge;

        /// @brief A vector of computed times an edge is reached
        //std::vector<SUMOTime> myArrivalTimes;

        SUMOReal myDepartPos;
        SUMOReal myArrivalPos;
        MSStoppingPlace* myDestinationBusStop;
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
    class MSPersonStage_Driving : public MSTransportable::Stage {
    public:
        /// constructor
        MSPersonStage_Driving(const MSEdge& destination, MSStoppingPlace* toBS,
                              const std::vector<std::string>& lines);

        /// destructor
        ~MSPersonStage_Driving();

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* person, SUMOTime now, MSEdge* previousEdge, const SUMOReal at);

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
        virtual void beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const;

    private:
        /// the lines  to choose from
        const std::set<std::string> myLines;

        /// @brief The taken vehicle
        SUMOVehicle* myVehicle;

        MSStoppingPlace* myDestinationBusStop;
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
    class MSPersonStage_Waiting : public MSTransportable::Stage {
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
        virtual void proceed(MSNet* net, MSTransportable* person, SUMOTime now, MSEdge* previousEdge, const SUMOReal at);

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
    /// constructor
    MSPerson(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan);

    /// destructor
    virtual ~MSPerson();

    /* @brief proceeds to the next step of the route,
     * @return Whether the persons plan continues  */
    bool proceed(MSNet* net, SUMOTime time);

    /** @brief Called on writing vehroute output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    void routeOutput(OutputDevice& os) const;

    /// @brief return the list of internal edges if this person is walking and the pedestrian model allows it
    const std::string& getNextEdge() const; 

private:
    /// @brief Invalidated copy constructor.
    MSPerson(const MSPerson&);

    /// @brief Invalidated assignment operator.
    MSPerson& operator=(const MSPerson&);

};


#endif

/****************************************************************************/
