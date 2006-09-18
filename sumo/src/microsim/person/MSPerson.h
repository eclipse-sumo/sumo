#ifndef MSPerson_h
#define MSPerson_h
/***************************************************************************
                          MSPerson.h
			  THe class for modelling person-movements
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.1  2006/09/18 10:04:43  dkrajzew
// first steps towards a person-based multimodal simulation
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:18  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.3  2001/11/15 17:12:14  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.2  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/23 09:32:25  traffic
// person route implementation
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>

#define WALKING 0
#define PUBLIC 1
#define PRIVATE 2
#define WAITING 3


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class MSEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
  * The class holds a simulated person together with her movement stages
  */
class MSPerson {
public:
    /**
     * The "abstract" class for a single stage of a persons movement
     * Contains the destination of the curent movement step
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
        virtual int getType() const = 0;

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, MSEdge *previousEdge) = 0;

    protected:
        /// the next edge to reach (either by walking or driving)
        const MSEdge &m_pDestination;

  };

    /**
     * A "real" stage performing the walking to an edge
     * The walking does not need any route as it is not simulated.
     * Only the duration is needed
     */
    class MSPersonStage_Walking : MSPersonStage {
    public:
        /// constructor
        MSPersonStage_Walking(const MSEdge &destination, SUMOTime walkingTime);

        /// destructor
        ~MSPersonStage_Walking();

        /// returns the time the person is walking
        SUMOTime getWalkingTime();

        /// returns the type of the stage
        int getType() const { return WALKING; }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, MSEdge *previousEdge);

    private:
        /// the time the person is walking
        SUMOTime m_uiWalkingTime;

    };

    /**
     * A "real" stage performing the travelling by a public transport
     * This class holds the id of the used public transport system.
     *  The route is stored in this transport system and will be proceeded
     *  until the destination edge is reached. The travel time is computed
     *  by the simulation.
     */
    class MSPersonStage_PublicVehicle : MSPersonStage {
    public:
        /// constructor
        MSPersonStage_PublicVehicle(const MSEdge &destination, const std::string &lineId);

        /// destructor
        ~MSPersonStage_PublicVehicle();

        /// returns the id of the line to use
        const std::string &getLineId() const;

        /// returns the type of the stage
        int getType() const { return PUBLIC; }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, MSEdge *previousEdge);

    private:
        /// the line of the public traffic the person is using
        std::string m_LineId;

    };

    /**
     * A "real" stage performing the travelling by a private transport system
     * The given route will be chosen. The travel time is computed by the simulation
     */
    class MSPersonStage_PrivateVehicle : MSPersonStage {
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
        int getType() const { return PRIVATE; }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, MSEdge *previousEdge);

    private:
        /// the id of the chosen route
        std::string m_RouteId;

        /// the id of the vehicle
        std::string m_VehicleId;

        /// the type of the vehicle
        std::string m_VehicleType;

    };

    /**
     * A "real" stage performing a waiting over the specified time
     * The time is not being added to the travel time?
     */
    class MSPersonStage_Waiting : MSPersonStage {
    public:
        /// constructor
        MSPersonStage_Waiting(const MSEdge &destination, SUMOTime waitingTime);

        /// destructor
        ~MSPersonStage_Waiting();

        /// returns the time th eperson is waiting
        SUMOTime getWaitingTime() const;

        /// returns the type of the stage
        int getType() const { return WAITING; }

        /// proceeds to the next step
        virtual void proceed(MSNet *net, MSPerson *person, SUMOTime now, MSEdge *previousEdge);

    private:
        /// the time the person is waiting
        SUMOTime m_uiWaitingTime;

    };

public:
    /// the structure holding the route of a person
    typedef std::vector<MSPersonStage> MSPersonRoute;

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

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
