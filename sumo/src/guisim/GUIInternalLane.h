#ifndef GUIInternalLane_h
#define GUIInternalLane_h
//---------------------------------------------------------------------------//
//                        GUIInternalLane.h -
//  A MSLane extended by some values needed by the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 04.09.2003
//  copyright            : (C) 2003 by DLR http://ivf.dlr.de/
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
// Revision 1.2  2003/11/20 14:40:26  dkrajzew
// push() debugged; dead code removed
//
// Revision 1.1  2003/09/05 15:02:47  dkrajzew
// first steps for reading of internal lanes
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utility>
#include <microsim/MSInternalLane.h>
#include <microsim/MSEdge.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/qutils/NewQMutex.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class MSNet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * An extended MSLane. A mechanism to avoid concurrent
 * visualisation and simulation what may cause problems when vehicles
 * disappear is implemented using a mutex
 */
class GUIInternalLane :
    public MSInternalLane {
public:
    /// constructor
    GUIInternalLane( MSNet &net, std::string id, double maxSpeed,
        double length, MSEdge* egde, const Position2DVector &shape );

    /// destructor
    ~GUIInternalLane();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void moveNonCritical();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void moveCritical();

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

    GUILaneWrapper *buildLaneWrapper(GUIGlObjectStorage &idStorage,
        bool allowAggregation);

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
    NewQMutex _lock;

    /// The shape of the lane
    Position2DVector myShape;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIInternalLane.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

