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
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
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
//

#include <string>
#include <vector>

#define WALKING 0
#define PUBLIC 1
#define PRIVATE 2
#define WAITING 3

class MSNet;
class MSEdge;


using namespace std;
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
  protected:
    /// the next edge to reach (either by walking or driving)
    MSEdge  *m_pDestination;
  public:
    /// constructor
    MSPersonStage(MSEdge *destination);
    /// destructor
    virtual ~MSPersonStage();
    /// returns the destination edge
    MSEdge *getDestination();
    /// returns the type of the stage (faster than type_id)
    virtual int getType() = 0;
    /// proceeds to the next step
    virtual void proceed(MSNet *net, MSPerson *person, unsigned int now, MSEdge *previousEdge) = 0;
  };

  /**
    * A "real" stage performing the walking to an edge
    * The walking does not need any route as it is not simulated. Only the duration is needed
    */
  class MSPersonStage_Walking : MSPersonStage {
  private:
    /// the time the person is walking
    unsigned int m_uiWalkingTime;
  public:
    /// constructor
    MSPersonStage_Walking(MSEdge *destination, unsigned int walkingTime);
    /// destructor
    ~MSPersonStage_Walking();
    /// returns the time the person is walking
    unsigned int getWalkingTime();
    /// returns the type of the stage
    int getType() { return WALKING; }
    /// proceeds to the next step
    virtual void proceed(MSNet *net, MSPerson *person, unsigned int now, MSEdge *previousEdge);
  };

  /**
    * A "real" stage performing the travelling by a public transport
    * This class holds the id of the used public transport system. The route is stored in this transport system and will be proceeded until the destination edge is reached. The travel time is computed by the simulation
    */
  class MSPersonStage_PublicVehicle : MSPersonStage {
  private:
    /// the line of the public traffic the person is using
    string m_LineId;
  public:
    /// constructor
    MSPersonStage_PublicVehicle(MSEdge *destination, string lineId);
    /// destructor
    ~MSPersonStage_PublicVehicle();
    /// returns the id of the line to use
    string getLineId();
    /// returns the type of the stage
    int getType() { return PUBLIC; }
    /// proceeds to the next step
    virtual void proceed(MSNet *net, MSPerson *person, unsigned int now, MSEdge *previousEdge);
  };

  /**
    * A "real" stage performing the travelling by a private transport system
    * The given route will be chosen. The travle time is computed by the simulation
    */ 
  class MSPersonStage_PrivateVehicle : MSPersonStage {
  private:
    /// the id of the chosen route 
    string m_RouteId;
    /// the id of the vehicle
    string m_VehicleId;
    /// the type of the vehicle
    string m_VehicleType;
  public:
    /// constructor
    MSPersonStage_PrivateVehicle(MSEdge *destination, string routeId, string vehicleId, string vehicleType);
    /// destructor
    ~MSPersonStage_PrivateVehicle();
    /// returns the id of the chosen route
    string getRouteId();
    /// returns the id of the vehicle
    string getVehicleId();
    /// return the type of the vehicle
    string getVehicleType();
    /// returns the type of the stage
    int getType() { return PRIVATE; }
    /// proceeds to the next step
    virtual void proceed(MSNet *net, MSPerson *person, unsigned int now, MSEdge *previousEdge);
  };

  /**
    * A "real" stage performing a waiting over the specified time
    * The time is not being added to the travel time?
    */
  class MSPersonStage_Waiting : MSPersonStage {
  private:
    /// the time the person is waiting
    unsigned int m_uiWaitingTime;
  public:
    /// constructor
    MSPersonStage_Waiting(MSEdge *destination, unsigned int waitingTime);
    /// destructor
    ~MSPersonStage_Waiting();
    /// returns the time th eperson is waiting
    unsigned int getWaitingTime();
    /// returns the type of the stage
    int getType() { return WAITING; }
    /// proceeds to the next step
    virtual void proceed(MSNet *net, MSPerson *person, unsigned int now, MSEdge *previousEdge);
  };

public:
  /// the structure holding the route of a person
  typedef vector<MSPersonStage> MSPersonRoute;
private:
  /// the travel time (without the time waited)
  unsigned int m_uiTravelTime;
  /// the id of the person
  string m_Id;
  /// the route of the person
  MSPersonRoute *m_pRoute;
  /// the iterator over the route
  MSPersonRoute::iterator m_pStep;
public:
  /// constructor
  MSPerson(string id, MSPersonRoute *route);
  /// destructor
  ~MSPerson();
  /// the current route item
  MSPersonStage &getCurrentStage();
  /// proceeds to the next step of the route
  void proceed(MSNet *net, unsigned int time);
  /// returns the information whether the persons route is over
  bool endReached();
  /** Inserts edge into the static dictionary and returns true if the key
    id isn't already in the dictionary. Otherwise returns false. */
  static bool dictionary(string id, MSPerson* person);
  /** Returns the MSEdge associated to the key id if exists, otherwise
      returns 0. */
  static MSPerson* dictionary(string id);
private:
  /// Static dictionary to associate string-ids with objects.
  typedef map<string, MSPerson*> DictType;
  static DictType myDict;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "MSPerson.iC"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
