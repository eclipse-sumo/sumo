/****************************************************************************/
/// @file    MSPerson.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The class for modelling person-movements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <set>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSEdge;
class OutputDevice;

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

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, const MSEdge &previousEdge) = 0;

        /// logs end of the step
        void setDeparted(SUMOTime now);

        /// logs end of the step
        void setArrived(SUMOTime now);

        /// Whether the person waits for a vehicle of the line specified.
        virtual bool isWaitingFor(const std::string &line) const;

        /** @brief Called on writing tripinfo output
         *
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice &os) const throw(IOError) = 0;


    protected:
        /// the next edge to reach (either by walking or driving)
        const MSEdge &myDestination;

        /// the time at which this stage started
        SUMOTime myDeparted;

        /// the time at which this stage ended
        SUMOTime myArrived;

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

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, const MSEdge &previousEdge);

        /** @brief Called on writing tripinfo output
         *
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice &os) const throw(IOError);

    private:
        /// the time the person is walking
        SUMOTime myWalkingTime;

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

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, const MSEdge &previousEdge);

        /// Whether the person waits for a vehicle of the line specified.
        bool isWaitingFor(const std::string &line) const;

        /** @brief Called on writing tripinfo output
         *
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice &os) const throw(IOError);

    private:
        /// the lines  to choose from
        const std::set<std::string> myLines;

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
        MSPersonStage_Waiting(const MSEdge &destination, SUMOTime duration, SUMOTime until);

        /// destructor
        ~MSPersonStage_Waiting();

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, const MSEdge &previousEdge);

        /** @brief Called on writing tripinfo output
         *
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice &os) const throw(IOError);

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

    /// returns the person id
    const std::string& getID() const throw();

    /// proceeds to the next step of the route
    void proceed(MSNet *net, SUMOTime time);

    /// Returns the desired departure time.
    SUMOTime getDesiredDepart() const throw();

    /// logs end of the step
    void setDeparted(SUMOTime now);

    /// Returns the current destination.
    const MSEdge &getDestination() const;

    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    void tripInfoOutput(OutputDevice &os) const throw(IOError);

    /// Whether the person waits for a vehicle of the line specified.
    bool isWaitingFor(const std::string &line) const;

private:
    /// @brief Invalidated copy constructor.
    MSPerson(const MSPerson&);

    /// @brief Invalidated assignment operator.
    MSPerson& operator=(const MSPerson&);

};


#endif

/****************************************************************************/
