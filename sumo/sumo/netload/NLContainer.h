#ifndef NLContainer_h
#define NLContainer_h
/***************************************************************************
                          NLContainer.h
			  Holds the builded structures or structures that
			  hold these
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.7  2002/07/31 17:34:50  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.7  2002/07/22 12:44:32  dkrajzew
// Source loading structures added
//
// Revision 1.6  2002/06/11 14:39:24  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.3  2002/04/17 11:17:01  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:05:35  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:21  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:41  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:05  traffic
// moved from netbuild
//
// Revision 1.4  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>
#include "../microsim/MSEmitControl.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NLEdgeControlBuilder;
class NLJunctionControlBuilder;
class NLRoutesBuilder;
class NLSucceedingLaneBuilder;
class MSEdge;
class MSNet;
class MSRouteCont;
class MSModel;
class MSVehicleType;
class MSVehicle;
class MSJunctionLogic;
class MSEventControl;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLContainer
 * NLContainer is used as storage for structures from which the simulation
 * structures will be build.
 * As not all of these structures are atomic, the container holds pointers to
 * other structure-holding classes i.e. the NLEdgeControlBuilder, the
 * "NLJunctionControlBuilder" and the "NLRoutesBuilder".
 * All methods are called from NLHandler* classes where each is a derivation
 * of the NLSAXHandler-class which as the name says is a XML-SAX-Handler
 * (see there).
 * When parameter checking is on, most of the classes throw exceptions on
 * false description which are caught by the NLHandler*-classes where errors
 * are generated and reported.
 * As a result of parsing, the MSNet-class containing all structures is
 * returned.
 */
class NLContainer {
private:
    /// pointer to the NLEdgeControlBuilder (storage for edges during building)
    NLEdgeControlBuilder      *m_pECB;
    /** pointer to the NLJunctionControlBuilder
         (storage for junctions during building) */
    NLJunctionControlBuilder  *m_pJCB;
    /// pointer to the NLRoutesBuilder (storage for routes during building)
    NLRoutesBuilder           *m_pRCB;
    /** pointer to the NLSucceedingLaneBuilder
         (storage for building succeeding lanes) */
    NLSucceedingLaneBuilder   *m_pSLB;
    /// pointer to the used model
    MSModel                   *m_pModel;
    /** pointer to a list of vehicles
         (used during building, later stored inside the net) */
    MSEmitControl::VehCont    *m_pVehicles;
    /** pointer to the NLDetectorBuilder
        (storage for building detectors) */
    MSNet::DetectorCont       *m_pDetectors;
    /// the id of the build net (not yet used!!!)
    std::string                    m_Id; // !!! not yet set
    /** definition of a container for junction logic keys
         the value is not really needed, we use a map to avoid double loading */
    typedef std::map<std::string, int> LogicKeyCont;
    /// the list of junction keys used
    LogicKeyCont            m_LogicKeys;
    /// the later-used event controller
    MSEventControl          *m_EventControl;

    /// the number of edges inside the net
    int noEdges;
    /// the number of lanes inside the net
    int noLanes;
    /// the number of junctions inside the net
    int noJunctions;
    /// the number of vehicles inside the net
    int noVehicles;
    /// the number of vehicle types inside the net
    int noVehicleTypes;
    /// the number of routes inside the net
    int noRoutes;
    /// the number of routes inside the net
    int noDetectors;
private:

  typedef std::map<std::string, MSJunctionLogic*> LogicCont;
  static LogicCont _logics;

public:
    /// standard constructor
    NLContainer();
    /// standard destructor
    ~NLContainer();

    // data storage preallocation
    /// increments the number of found edges
    void incEdges();
    /// increments the number of found lanes
    void incLanes();
    /// increments the number of found junctions
    void incJunctions();
    /// increments the number of found vehicles
    void incVehicles();
    /// increments the number of found types
    void incVehicleTypes();
    /// increments the number of found routes
    void incRoutes();
    /// increments the number of found detectors
    void incDetectors();

    /// preallocates the needed space (vectors)
    void preallocate();
    /// preallocates the space for the vehicles
    void preallocateVehicles();
    /// inserts a new junction type key
    void addKey(std::string key);

    // interface to use the edge control builder
    /** adds an edge (allocates it); this method may throw an exception if
        an edge with the given id already exists */
    void addEdge(const std::string &id);
    /// rechooses a previously allocated edge
    void chooseEdge(const std::string &id);
    /** adds a lane to the last chosen edge; this method may throw exceptions
        if a) no edge was chosen b) a lane with the given id already existed
        and c) another lane was already set as the depart-lane */
    void addLane(const std::string &id, const bool isDepartLane,
		 const float maxSpeed, const float length,
		 const float changeUrge);
    /// closes the addition of an edges lanes
    void closeLanes();
    /** starts the building of a connection to the next edge;
        this method may throw an exception if no edge with the given id
        exists */
    void openAllowedEdge(const std::string &id);
    /** adds a connecting lane of the current edge to the list of connection
        to the next edge; this method may throw an exception when a) no lane
        with this id exists or b) the lane with the given id is not a part of
        the current (chosen) edge */
    void addAllowed(const std::string &id);
    /// closes the building of connections to a destination edge
    void closeAllowedEdge();
    /// closes the building of the current edge
    void closeEdge();

    /// interfaces for the building of succeeding lanes
    /** opens the computation of a container holding the succeding lanes of a
        lane */
    void openSuccLane(const std::string &laneId);
    /// sets the succeeding junction
    void setSuccJunction(const std::string &junctionId);
    /// add a succeeding lane
    void addSuccLane(bool yield, const std::string &laneId);
    /// closes the building
    void closeSuccLane();
    /// returns the name of the lane the succeeding lanes are added to
    std::string getSuccingLaneName() const;


    /// interface to use the junction control builder
    /// begins the building of a junction with the given id
    void openJunction(const std::string &id, const std::string &key, std::string type);
    /// adds an incoming lane to the junction;
    void addInLane(const std::string &id);
    /** adds a part of the right-of-way-logic to the junction;
        this method throws an exception when the key (the request) was
        already used in the currently build junction */
    //void setKey(const std::string &key);
    /// closes the building of a junction
    void closeJunction();

    /// interface for the generation of vehicle types
    /** adds a new vehicle type to the simulation; this method throws an
        exception when the id was already used for another vehicle type */
    void addVehicleType(const std::string &id, const float length,
			const float maxspeed, const float bmax,
			const float dmax, const float sigma);

    /// handling of routes
    /// begins the building of the route with the given id
    void openRoute(const std::string &id);
    /// adds the next route to the currently active route;
    void addRoutesEdge(const std::string &id);
    /// closes the building of the route
    void closeRoute();

    // ----- interfaces for the generation of vehicles
    /// adds a new vehicle to the simulation
    void addVehicle(const std::string &id, const std::string &vtypeid,
		    const std::string &routeid, const long depart);

    // ----- interfaces for the generation of detectors
    /// adds a new detector to the simulation
    void addDetector(MSDetector *detector);

    /// returns the used event control
    MSEventControl &getEventControl() const;

    /// returns the statistics about the build net
    std::string getStatistics() const;

    /// end of operations; builds the net
    MSNet *buildNet();
    /// end of operations; builds the vehicles
    MSEmitControl *buildVehicles();

    // returns the detectors
    MSNet::DetectorCont *getDetectors();
private:
    /** invalid copy constructor */
    NLContainer(const NLContainer &s);
    /** invalid assignment operator */
    NLContainer &operator=(const NLContainer &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLContainer.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

