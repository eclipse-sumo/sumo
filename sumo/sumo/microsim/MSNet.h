/***************************************************************************
                          MSNet.h  -  We will simulate on this object. 
                          Holds all necessary objects for micro-simulation.
                             -------------------
    begin                : Mon, 12 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

#ifndef MSNet_H
#define MSNet_H

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.4  2002/04/11 15:25:56  croessel
// Changed float to double.
//
// Revision 1.3  2002/04/11 10:33:25  dkrajzew
// Addition of detectors added
//
// Revision 1.2  2002/04/10 16:19:34  croessel
// Modifications due to detector-implementation.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.2  2002/03/14 08:09:26  traffic
// Option for no raw output added
//
// Revision 2.1  2002/03/07 07:55:02  traffic
// implemented the usage of stdout as the default raw output
//
// Revision 2.0  2002/02/14 14:43:18  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.14  2002/02/13 16:30:54  croessel
// Output goes in a file now.
//
// Revision 1.13  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.12  2002/01/16 10:03:34  croessel
// New method "static double deltaT()" and member "static double myDeltaT"
// added. DeltaT is the length of a timestep in seconds.
//
// Revision 1.11  2001/12/20 14:35:54  croessel
// using namespace std replaced by std::
//
// Revision 1.10  2001/12/19 16:36:25  croessel
// Moved methods (assignment, copy-ctor) to private and removed outcommented 
// code.
//
// Revision 1.9  2001/11/15 17:12:14  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.8  2001/11/14 11:45:54  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.7  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.6  2001/10/23 09:31:29  traffic
// parser bugs removed
//
// Revision 1.4  2001/09/06 15:39:12  croessel
// Added simple text output to simulation-loop.
//
// Revision 1.3  2001/07/16 16:00:52  croessel
// Changed Route-Container type to map<string, Route*>. Added static dictionary
// methods to access it (same as id-handling).
//
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer 
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

#include <typeinfo>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "MSPerson.h"

class MSEdge;
class MSEdgeControl;
class MSJunctionControl;
class MSEmitControl;
class MSEventControl;
class Event;
class MSPersonControl;
class MSDetector;

/**
 */
class MSNet
{
    friend class MSPerson;

public:
    /// Container for Edges. This are the routes.
    typedef std::vector< const MSEdge* > Route;

    /// Const-iterator to Route-elements, i.e. edges.
    typedef Route::const_iterator RouteIterator;
     
    /// Container for persons
    typedef std::vector< MSPerson* > PersonCont;

    /// Type for time (seconds).
    typedef unsigned int Time;

    /// Detector-container type.
    typedef vector< MSDetector* > DetectorCont;

    /// Use this constructor only.
    MSNet( string id,
           MSEdgeControl* ec,
           MSJunctionControl* jc,
           MSEmitControl* emc,
           MSEventControl* evc,
           MSPersonControl* wpc,
           DetectorCont* detectors );

    /// Use this to add separetly loaded vehicles
    void addVehicles( MSEmitControl* cont );

    /// Use this to add seperatly loaded detectors
    void addDetectors( DetectorCont *cont);

    /// Destructor.
    ~MSNet();

    /** Simulates from timestep start to stop. start and stop in
        timesteps.  In each timestep we emit Vehicles, move Vehicles,
        the Vehicles change Lanes.  The method returns true when the
        simulation could be finished without errors, otherwise
        false. */
    bool simulate( std::ostream *craw, Time start, Time stop );
   
    /** Inserts a MSNet into the static dictionary and returns true if
        the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool dictionary( std::string id, MSNet* net );

    /** Returns the MSNet associated to the key id if exists,
        otherwise returns 0. */
    static MSNet* dictionary( std::string id );

    /** Inserts a MSNet::Route into the static dictionary and returns true
        if the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool routeDict( std::string id, Route* route );

    /** Returns the MSNet associated to the key id if exists,
        otherwise returns 0. */
    static const MSNet::Route* routeDict( std::string id );

    /// Returns the timestep-length in seconds.
    static double deltaT();

    /** Returns the current simulation time in seconds. Current means
        start-time plus runtime. */
    double simSeconds();

protected:

private:
    void processWaitingPersons(unsigned int time);
    friend void MSPerson::MSPersonStage::proceed(MSNet *net, MSPerson *person, MSNet::Time now, MSEdge *previousEdge);
    friend void MSPerson::MSPersonStage_Walking::proceed(MSNet *net, MSPerson *person, MSNet::Time now, MSEdge *previousEdge);
    friend void MSPerson::MSPersonStage_PublicVehicle::proceed(MSNet *net, MSPerson *person, MSNet::Time now, MSEdge *previousEdge);
    friend void MSPerson::MSPersonStage_PrivateVehicle::proceed(MSNet *net, MSPerson *person, MSNet::Time now, MSEdge *previousEdge);
    friend void MSPerson::MSPersonStage_Waiting::proceed(MSNet *net, MSPerson *person, MSNet::Time now, MSEdge *previousEdge);
  
    /// Copy constructor.
    MSNet( const MSNet& );

    /// Assignment operator.
    MSNet& operator=( const MSNet& );
  
    /// Unique ID.
    std::string myID;

    /** Lane-changing is done by this object. */
    MSEdgeControl* myEdges;

    /// Sets the right-of-way rules and moves first cars.
    MSJunctionControl* myJunctions;

    /// Emits cars into the lanes.
    MSEmitControl* myEmitter;

    /** Time-dependant events like traffic-light-changes, output
        generation etc. */
    MSEventControl* myEvents; 

    /// schedules alking persons
    MSPersonControl *myPersons;

    /// Static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSNet* > DictType;
    static DictType myDict;

    /// Container for routes.
    typedef std::map< std::string, const Route* > RouteDict;
    static RouteDict myRoutes;

    /// Timestep [sec]
    static double myDeltaT;

    /// Current time step.
    Time myStep;

    /// Container of detectors.
    DetectorCont* myDetectors;

    /** Default constructor. It makes no sense to build a net without
        initialisation. */
    MSNet();
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "MSNet.iC"
//#endif

#endif

// Local Variables:
// mode:C++

