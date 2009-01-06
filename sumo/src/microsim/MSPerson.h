/****************************************************************************/
/// @file    MSPerson.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The class for modelling person-movements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#define WALKING 0
#define PUBLIC 1
#define PRIVATE 2
#define WAITING 3


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSPerson
  *
  * The class holds a simulated person together with her movement stages
  */
class MSPerson
{
public:
    /**
     * The "abstract" class for a single stage of a persons movement
     * Contains the destination of the curent movement step
     */
    class MSPersonStage
    {
    public:
        /// constructor
        MSPersonStage(const MSEdge &destination);

        /// destructor
        virtual ~MSPersonStage();

        /// returns the destination edge
        const MSEdge &getDestination() const;

        /// returns the type of the stage (faster than type_id)
        virtual int getType() const = 0;

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, MSEdge *previousEdge) = 0;

    protected:
        /// the next edge to reach (either by walking or driving)
        const MSEdge &m_pDestination;

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
class MSPersonStage_Walking : MSPersonStage
    {
    public:
        /// constructor
        MSPersonStage_Walking(const MSEdge &destination, SUMOTime walkingTime);

        /// destructor
        ~MSPersonStage_Walking();

        /// returns the time the person is walking
        SUMOTime getWalkingTime();

        /// returns the type of the stage
        int getType() const {
            return WALKING;
        }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, MSEdge *previousEdge);

    private:
        /// the time the person is walking
        SUMOTime m_uiWalkingTime;

    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage_Walking(const MSPersonStage_Walking&);

        /// @brief Invalidated assignment operator.
        MSPersonStage_Walking& operator=(const MSPersonStage_Walking&);

    };

    /**
     * A "real" stage performing the travelling by a public transport
     * This class holds the id of the used public transport system.
     *  The route is stored in this transport system and will be proceeded
     *  until the destination edge is reached. The travel time is computed
     *  by the simulation.
     */
class MSPersonStage_PublicVehicle : MSPersonStage
    {
    public:
        /// constructor
        MSPersonStage_PublicVehicle(const MSEdge &destination, const std::string &lineId);

        /// destructor
        ~MSPersonStage_PublicVehicle();

        /// returns the id of the line to use
        const std::string &getLineId() const;

        /// returns the type of the stage
        int getType() const {
            return PUBLIC;
        }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, MSEdge *previousEdge);

    private:
        /// the line of the public traffic the person is using
        std::string m_LineId;

    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage_PublicVehicle(const MSPersonStage_PublicVehicle&);

        /// @brief Invalidated assignment operator.
        MSPersonStage_PublicVehicle& operator=(const MSPersonStage_PublicVehicle&);

    };

    /**
     * A "real" stage performing the travelling by a private transport system
     * The given route will be chosen. The travel time is computed by the simulation
     */
class MSPersonStage_PrivateVehicle : MSPersonStage
    {
    public:
        /// constructor
        MSPersonStage_PrivateVehicle(const MSEdge &destination,
                                     const std::string &routeId, const std::string &vehicleId,
                                     const std::string &vehicleType);

        /// destructor
        ~MSPersonStage_PrivateVehicle();

        /// returns the id of the chosen route
        const std::string &getRouteId() const;

        /// returns the id of the vehicle
        const std::string &getVehicleId() const;

        /// return the type of the vehicle
        const std::string &getVehicleType() const;

        /// returns the type of the stage
        int getType() const {
            return PRIVATE;
        }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, MSEdge *previousEdge);

    private:
        /// the id of the chosen route
        std::string m_RouteId;

        /// the id of the vehicle
        std::string m_VehicleId;

        /// the type of the vehicle
        std::string m_VehicleType;

    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage_PrivateVehicle(const MSPersonStage_PrivateVehicle&);

        /// @brief Invalidated assignment operator.
        MSPersonStage_PrivateVehicle& operator=(const MSPersonStage_PrivateVehicle&);

    };

    /**
     * A "real" stage performing a waiting over the specified time
     * The time is not being added to the travel time?
     */
class MSPersonStage_Waiting : MSPersonStage
    {
    public:
        /// constructor
        MSPersonStage_Waiting(const MSEdge &destination, SUMOTime waitingTime);

        /// destructor
        ~MSPersonStage_Waiting();

        /// returns the time th eperson is waiting
        SUMOTime getWaitingTime() const;

        /// returns the type of the stage
        int getType() const {
            return WAITING;
        }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, MSEdge *previousEdge);

    private:
        /// the time the person is waiting
        SUMOTime m_uiWaitingTime;

    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage_Waiting(const MSPersonStage_Waiting&);

        /// @brief Invalidated assignment operator.
        MSPersonStage_Waiting& operator=(const MSPersonStage_Waiting&);

    };

public:
    /// the structure holding the route of a person
    typedef std::vector<MSPersonStage*> MSPersonRoute;

private:
    /// the travel time (without the time waited)
    SUMOTime m_uiTravelTime;

    /// the id of the person
    std::string m_Id;

    /// the route of the person
    MSPersonRoute *m_pRoute;

    /// the iterator over the route
    MSPersonRoute::iterator m_pStep;

public:
    /// constructor
    MSPerson(const std::string &id, MSPersonRoute *route);

    /// destructor
    ~MSPerson();

    /// the current route item
    const MSPersonStage &getCurrentStage() const;

    /// proceeds to the next step of the route
    void proceed(MSNet *net, SUMOTime time);

    /// returns the information whether the persons route is over
    bool endReached() const;

    /** Inserts edge into the static dictionary and returns true if the key
        id isn't already in the dictionary. Otherwise returns false. */
    static bool dictionary(const std::string &id, MSPerson* person);

    /** Returns the MSEdge associated to the key id if exists, otherwise
        returns 0. */
    static MSPerson* dictionary(const std::string &id);

private:
    /// Static dictionary to associate string-ids with objects.
    typedef std::map<std::string, MSPerson*> DictType;

    static DictType myDict;

private:
    /// @brief Invalidated copy constructor.
    MSPerson(const MSPerson&);

    /// @brief Invalidated assignment operator.
    MSPerson& operator=(const MSPerson&);

};


#endif

/****************************************************************************/
