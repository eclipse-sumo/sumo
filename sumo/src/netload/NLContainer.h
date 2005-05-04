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
// Revision 1.26  2005/05/04 07:55:29  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.25  2004/07/02 09:37:31  dkrajzew
// work on class derivation (for online-routing mainly)
//
// Revision 1.23  2004/04/02 11:23:51  dkrajzew
// extended traffic lights are now no longer templates; MSNet now handles all simulation-wide output
//
// Revision 1.22  2004/02/18 05:32:51  dkrajzew
// missing pass of lane continuation to detector builder added
//
// Revision 1.21  2004/01/26 07:07:36  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics; different detector visualistaion in dependence to his controller
//
// Revision 1.20  2004/01/12 15:12:05  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.19  2004/01/12 14:36:21  dkrajzew
// removed some dead code; documentation added
//
// Revision 1.18  2003/12/04 13:18:23  dkrajzew
// handling of internal links added
//
// Revision 1.17  2003/11/18 14:23:57  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.16  2003/09/05 15:20:19  dkrajzew
// loading of internal links added
//
// Revision 1.15  2003/08/04 11:40:21  dkrajzew
// false inclusion hierarchy patched; missing inclusions added
//
// Revision 1.14  2003/07/30 09:25:17  dkrajzew
// loading of directions and priorities of links implemented
//
// Revision 1.13  2003/07/21 18:07:44  roessel
// Adaptions due to new MSInductLoop.
//
// Revision 1.12  2003/07/16 15:35:30  dkrajzew
// unneeded pointer to obsolete MSModel removed
//
// Revision 1.11  2003/07/07 08:35:10  dkrajzew
// changes due to loading of geometry applied from the gui-version (no major drawbacks in loading speed)
//
// Revision 1.10  2003/06/18 11:18:05  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.8  2003/06/06 10:40:18  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.7  2003/06/05 11:52:26  dkrajzew
// class templates applied; documentation added
//
// Revision 1.6  2003/03/19 08:07:33  dkrajzew
// bugs and syntactical faults arised during compilation on Linux removed
//
// Revision 1.5  2003/03/03 15:06:32  dkrajzew
// new import format applied; new detectors applied
//
// Revision 1.4  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.3  2002/10/21 09:52:57  dkrajzew
// support for route multireferencing added
//
// Revision 1.2  2002/10/17 10:34:48  dkrajzew
// possibility of retrival of the preinitialised net during loading implemented for trigger handling
//
// Revision 1.1  2002/10/16 15:36:47  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>
#include <microsim/MSEmitControl.h>
#include <microsim/MSLink.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NLEdgeControlBuilder;
class NLJunctionControlBuilder;
class NLDetectorBuilder;
class NLSucceedingLaneBuilder;
class MSEdge;
class MSNet;
class MSRouteCont;
class MSVehicleType;
class MSVehicle;
class MSJunctionLogic;
class MSRouteLoaderControl;
class OptionsCont;
class MSTrafficLightLogic;
class MSInductLoop;
class MSExtendedTrafficLightLogic;
class MSRouteLoader;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
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
public:
    /// Definitions of a string vector
    typedef std::vector<std::string> StringVector;

    /// Definition of a map from string -> stringvector
    typedef std::map<std::string, StringVector> SSVMap;

public:
    /// standard constructor
    NLContainer(NLEdgeControlBuilder &edgeBuilder,
        NLJunctionControlBuilder &junctionBuilder);

    /// standard destructor
    virtual ~NLContainer();

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

    /// sets the number of nodes available within the file
    void setEdgeNumber(size_t noEdges);

    /// sets the number of edges available within the file
    void setNodeNumber(size_t noNodes);

    /// preallocates the space for the vehicles
    void preallocateVehicles();

    /// inserts a new junction type key
    void addKey(std::string key);


    // interface to use the edge control builder
    /** adds an edge (allocates it); this method may throw an exception if
        an edge with the given id already exists */
    void addEdge(const std::string &id);

    /// rechooses a previously allocated edge
    void chooseEdge(const std::string &id, const std::string &func);

    /** adds a lane to the last chosen edge; this method may throw exceptions
        if a) no edge was chosen b) a lane with the given id already existed
        and c) another lane was already set as the depart-lane */
    virtual void addLane(const std::string &id, const bool isDepartLane,
         const float maxSpeed, const float length,
         const float changeUrge);

    /// closes the lane currently build
    virtual void closeLane();

    /// closes the addition of an edges lanes
    void closeLanes();

    /// add a Polygon
    void addPoly(const std::string &name, const std::string &type, const std::string &color);

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

    /// clears the inclanes-list
    void initIncomingLanes();


    // interfaces for the building of succeeding lanes
    /** opens the computation of a container holding the succeding lanes of a
        lane */
    void openSuccLane(const std::string &laneId);

	/// sets the shape of the current lane
    void addLaneShape(const Position2DVector &shape);

    /// sets the succeeding junction
    //void setSuccJunction(const std::string &junctionId);

    /// add a succeeding lane
    void addSuccLane(bool yield, const std::string &laneId,
        const std::string &viaID,
        MSLink::LinkDirection dir, MSLink::LinkState state,
        bool internalEnd,
        const std::string &tlid="", size_t linkNo=0);

    /// closes the building
    void closeSuccLane();

    /// returns the name of the lane the succeeding lanes are added to
    std::string getSuccingLaneName() const;


    // interface to use the junction control builder
    /** begins the building of a junction with the given id */
    void openJunction(const std::string &id, const std::string &key,
        std::string type, double x, double y);

    /// adds an incoming lane to the junction;
    void addIncomingLane(const std::string &id);

    /// adds an internal lane to the junction;
    void addInternalLane(const std::string &id);

    /// closes the building of a junction
    void closeJunction();

    /// Returns the list of the current incoming lanes
    const std::vector<MSLane*> &getIncomingLanes() const;


    /// Returns preceding lane information
    const SSVMap &getLaneConts() const;


    /// Adds a build traffic light logic
    void addTLLogic(MSTrafficLightLogic *logic);

    /// Retrieves a traffic light logic
    MSTrafficLightLogic *getTLLogic(const std::string &id) const;


    void addJunctionInitInfo(MSExtendedTrafficLightLogic *key,
        const std::vector<MSLane*> &lv, double det_offset);

    /// end of operations; builds the net
    MSNet *buildMSNet(NLDetectorBuilder &db,
        const OptionsCont &options);

    /// returns the preallocated (preinitialised) net
    MSNet &getNet();

protected:
    /// builds the route loader control
    MSRouteLoaderControl *buildRouteLoaderControl(const OptionsCont &oc);

    void closeJunctions(NLDetectorBuilder &db);

    std::vector<MSTrafficLightLogic*> getTLLogicVector() const;

    virtual MSRouteLoader *buildRouteLoader(const std::string &file);

private:
    /** invalid copy constructor */
    NLContainer(const NLContainer &s);

    /** invalid assignment operator */
    NLContainer &operator=(const NLContainer &s);

protected:
    /** pointer to the NLEdgeControlBuilder (storage for edges during
        building) */
    NLEdgeControlBuilder      &myEdgeControlBuilder;

    /** pointer to the NLJunctionControlBuilder
         (storage for junctions during building) */
    NLJunctionControlBuilder  &myJunctionControlBuilder;

    /** pointer to the NLSucceedingLaneBuilder
         (storage for building succeeding lanes) */
    NLSucceedingLaneBuilder   *m_pSLB;

    /** definition of a container for junction logic keys
        the value is not really needed, we use a map to avoid double
        loading */
    typedef std::map<std::string, int> LogicKeyCont;

    /// the list of junction keys used
    LogicKeyCont            m_LogicKeys;

    /// The list of traffic light logics build
    std::map<std::string, MSTrafficLightLogic*> myLogics;

    /* @brief Backward edge continuation map
       The key is the name of an edge, the value is the list of edges
       that do approach the key-edge */
    SSVMap myContinuations;


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

    std::string myCurrentID;

    /// The shape of the current lane
    Position2DVector myShape;


    typedef std::vector<MSLane*> LaneVector;
    typedef std::pair<LaneVector, double> TLInitInfo;
    typedef std::map<MSExtendedTrafficLightLogic*, TLInitInfo> TLLogicInitInfoMap;
    TLLogicInitInfoMap myJunctions2PostLoadInit;

    // { Information about a lane
    /// The id of the current lane
    std::string myID;

    /// The information whether the current lane is a depart lane
    bool myLaneIsDepart;

    /// The maximum speed allowed on the current lane
    float myCurrentMaxSpeed;

    /// The length of the current lane
    float myCurrentLength;

    /// The changeUrge.Information of the current lane
    float myCurrentChangeUrge;
    // }

private:
    /// definition of a map to store junction logics into
    typedef std::map<std::string, MSJunctionLogic*> LogicCont;

    /// the map to store junction logics into
    static LogicCont _logics;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
