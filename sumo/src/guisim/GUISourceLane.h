#ifndef GUISourceLane_h
#define GUISourceLane_h
//---------------------------------------------------------------------------//
//                        GUISourceLane.h -
//  A MSLane extended by some values needed by the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 25 Nov 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.8  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.7  2004/07/02 08:52:49  dkrajzew
// numerical id added (for online-routing)
//
// Revision 1.6  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.5  2003/09/05 14:59:54  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.4  2003/07/16 15:24:55  dkrajzew
// GUIGrid now handles the set of things to draw in another manner than GUIEdgeGrid did; Further things to draw implemented
//
// Revision 1.3  2003/07/07 08:14:48  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.2  2003/04/15 09:09:14  dkrajzew
// documentation added
//
// Revision 1.1  2003/02/07 10:39:17  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utility>
#include <microsim/MSSourceLane.h>
#include <microsim/MSEdge.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/foxtools/FXMutex.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class MSNet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUISourceLane
 * An extended MSSourceLane. A mechanism to avoid concurrent
 * visualisation and simulation what may cause problems when vehicles
 * disappear is implemented using a mutex
 */
class GUISourceLane :
    public MSSourceLane {
public:
    /// constructor
    GUISourceLane( MSNet &net, std::string id, double maxSpeed,
        double length, MSEdge* edge, size_t numericalID,
        const Position2DVector &shape );

    /// destructor
    ~GUISourceLane();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
//    void moveNonCriticalSingle();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
//    void moveCriticalSingle();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
//    void moveNonCriticalMulti();


    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
//    void moveCriticalMulti();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void moveNonCritical(/*
        const MSEdge::LaneCont::const_iterator &firstNeighLane,
        const MSEdge::LaneCont::const_iterator &lastNeighLane */);

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void moveCritical(/*
        const MSEdge::LaneCont::const_iterator &firstNeighLane,
        const MSEdge::LaneCont::const_iterator &lastNeighLane */);

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void setCritical();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool emit( MSVehicle& newVeh );

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool isEmissionSuccess( MSVehicle* aVehicle );

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void integrateNewVehicle();

    /// allows the processing of vehicles for threads
    void releaseVehicles();

    /// returns the vehicles closing their processing for other threads
    const VehCont &getVehiclesSecure();

    GUILaneWrapper *buildLaneWrapper(GUIGlObjectStorage &idStorage);

    friend class GUILaneChanger;

    friend class GUILaneWrapper;

protected:
    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool push( MSVehicle* veh );

    /// moves myTmpVehicles int myVehicles after a lane change procedure
    void swapAfterLaneChange();

private:
    /// The mutex used to avoid concurrent updates of the vehicle buffer
    FXEX::FXMutex _lock;

    /// The shape of the lane
    Position2DVector myShape;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUISourceLane.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

