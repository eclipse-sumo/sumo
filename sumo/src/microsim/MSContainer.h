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
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
class MSCModel_NonInteracting;
class CState;


// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSContainer
  *
  * The class holds a simulated container together with its movement stages
  */


class MSContainer : public MSTransportable {
public:


    /**
     * A "real" stage performing the travelling by a transport system
     * A container is in this stage if it is on a ride or if its waiting for a ride.
     * The given route will be chosen. The travel time is computed by the simulation
     */
    class MSContainerStage_Driving : public MSTransportable::Stage_Driving {
    public:
        /// constructor
        MSContainerStage_Driving(const MSEdge& destination, MSStoppingPlace* toStop,
                                 const SUMOReal arrivalPos, const std::vector<std::string>& lines);

        /// destructor
        ~MSContainerStage_Driving();

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* container, SUMOTime now, Stage* previous);

        /// @brief returns the stage description as a string
        std::string getStageDescription() const;

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
    };

    /**
    * A "real" stage performing the tranship of a container
    * A container is in this stage if it gets transhipred between two stops that are
    * assumed to be connected.
    */
    class MSContainerStage_Tranship : public MSTransportable::Stage {
        friend class MSCModel_NonInteracting;

    public:
        /// constructor
        MSContainerStage_Tranship(const std::vector<const MSEdge*>& route, MSStoppingPlace* toStop, SUMOReal speed, SUMOReal departPos, SUMOReal arrivalPos);

        /// destructor
        ~MSContainerStage_Tranship();

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* container, SUMOTime now, Stage* previous);

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

        /// @brief the edges of the current stage
        ConstMSEdgeVector getEdges() const;

        /// Returns the current stage description as a string
        std::string getStageDescription() const {
            return "tranship";
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
        virtual void beginEventOutput(const MSTransportable& c, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSTransportable& c, SUMOTime t, OutputDevice& os) const;

        /// @brief move forward and return whether the container arrived
        bool moveToNextEdge(MSTransportable* container, SUMOTime currentTime, MSEdge* nextInternal = 0);


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
    /// constructor
    MSContainer(const SUMOVehicleParameter* pars, const MSVehicleType* vtype,  MSTransportablePlan* plan);

    /// destructor
    virtual ~MSContainer();

    /* @brief proceeds to the next step of the route,
     * @return Whether the persons plan continues  */
    virtual bool proceed(MSNet* net, SUMOTime time);

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

private:
    /// @brief Invalidated copy constructor.
    MSContainer(const MSContainer&);

    /// @brief Invalidated assignment operator.
    MSContainer& operator=(const MSContainer&);

};


#endif

/****************************************************************************/
