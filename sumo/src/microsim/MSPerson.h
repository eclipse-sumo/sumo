/****************************************************************************/
/// @file    MSPerson.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The class for modelling person-movements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSEdge;

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
    /**
     * @enum ModeType
     * @brief Defines possible person modes
     */
    enum ModeType {
        /// @brief The person walks
        PERSON_WALKING = 0,
        /// @brief The person waits (stops)
        PERSON_WAITING = 1,
        /// @brief The person drives either by car or public transport
        PERSON_DRIVING = 2
    };
    /**
     * The "abstract" class for a single stage of a persons movement
     * Contains the destination of the current movement step
     */
    class MSPersonStage {
    public:
        /// constructor
        MSPersonStage(const MSEdge &destination);

        /// destructor
        virtual ~MSPersonStage();

        /// returns the destination edge
        const MSEdge &getDestination() const;

        /// returns the type of the stage (faster than type_id)
        virtual ModeType getMode() const = 0;

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, const MSEdge &previousEdge) = 0;

    protected:
        /// the next edge to reach (either by walking or driving)
        const MSEdge &myDestination;

        /// the time at which this stage started
        SUMOTime myStart;

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
        MSPersonStage_Walking(MSEdgeVector route, SUMOTime walkingTime, SUMOReal speed);

        /// destructor
        ~MSPersonStage_Walking();

        /// returns the time the person is walking
        SUMOTime getWalkingTime();

        /// returns the mode of the stage
        ModeType getMode() const {
            return PERSON_WALKING;
        }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, const MSEdge &previousEdge);

    private:
        /// the time the person is walking
        MSEdgeVector myRoute;

        /// the time the person is walking
        SUMOTime myWalkingTime;

        /// the speed at which the person is walking
        SUMOReal myWalkingSpeed;

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
        MSPersonStage_Driving(const MSEdge &destination,
                              const std::vector<std::string> &lines);

        /// destructor
        ~MSPersonStage_Driving();

        /// returns the type of the stage
        ModeType getMode() const {
            return PERSON_DRIVING;
        }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, const MSEdge &previousEdge);

    private:
        /// the lines  to choose from
        const std::vector<std::string> &myLines;

    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage_Driving(const MSPersonStage_Driving&);

        /// @brief Invalidated assignment operator.
        MSPersonStage_Driving& operator=(const MSPersonStage_Driving&);

    };

    /**
     * A "real" stage performing a waiting over the specified time
     * The time is not being added to the travel time?
     */
    class MSPersonStage_Waiting : public MSPersonStage {
    public:
        /// constructor
        MSPersonStage_Waiting(const MSEdge &destination, SUMOTime duration, SUMOTime until);

        /// destructor
        ~MSPersonStage_Waiting();

        /// returns the time th eperson is waiting
        SUMOTime getWaitingTime() const;

        /// returns the type of the stage
        ModeType getMode() const {
            return PERSON_WAITING;
        }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, const MSEdge &previousEdge);

    private:
        /// the time the person is waiting
        SUMOTime myWaitingDuration;

        /// the time until the person is waiting
        SUMOTime myWaitingUntil;

    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage_Waiting(const MSPersonStage_Waiting&);

        /// @brief Invalidated assignment operator.
        MSPersonStage_Waiting& operator=(const MSPersonStage_Waiting&);

    };

public:
    /// the structure holding the plan of a person
    typedef std::vector<MSPersonStage*> MSPersonPlan;

private:
    /// the plan of the person
    const SUMOVehicleParameter *myParameter;

    /// the plan of the person
    MSPersonPlan *myPlan;

    /// the iterator over the route
    MSPersonPlan::iterator myStep;

public:
    /// constructor
    MSPerson(const SUMOVehicleParameter* pars, MSPersonPlan *plan);

    /// destructor
    ~MSPerson();

    /// proceeds to the next step of the route
    void proceed(MSNet *net, SUMOTime time);

    /// returns the information whether the persons route is over
    bool endReached() const;

    /// Returns the desired departure time.
    SUMOTime getDesiredDepart() const throw() {
        return myParameter->depart;
    }

private:
    /// @brief Invalidated copy constructor.
    MSPerson(const MSPerson&);

    /// @brief Invalidated assignment operator.
    MSPerson& operator=(const MSPerson&);

};


#endif

/****************************************************************************/
