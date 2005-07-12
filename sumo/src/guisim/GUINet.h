#ifndef GUINet_h
#define GUINet_h
//---------------------------------------------------------------------------//
//                        GUINet.h -
//  A MSNet extended by some values for usage within the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
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
// Revision 1.32  2005/07/12 12:18:47  dkrajzew
// edge-based mean data implemented; previous lane-based is now optional
//
// Revision 1.31  2005/05/04 08:02:54  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.30  2005/02/01 10:08:23  dkrajzew
// performance computation added; got rid of MSNet::Time
//
// Revision 1.29  2004/12/16 12:20:09  dkrajzew
// debugging
//
// Revision 1.28  2004/11/25 16:26:47  dkrajzew
// consolidated and debugged some detectors and their usage
//
// Revision 1.27  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.26  2004/08/02 11:58:00  dkrajzew
// using OutputDevices instead of ostreams
//
// Revision 1.25  2004/07/02 08:57:29  dkrajzew
// using global object selection; detector handling (handling of additional elements) revisited
//
// Revision 1.24  2004/04/02 11:17:07  dkrajzew
// simulation-wide output files are now handled by MSNet directly
//
// Revision 1.23  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.22  2003/12/11 06:24:55  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.21  2003/11/20 13:05:32  dkrajzew
// loading and using of predefined vehicle colors added
//
// Revision 1.20  2003/11/17 07:15:27  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.19  2003/11/12 14:01:54  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.18  2003/10/22 15:42:56  dkrajzew
// we have to distinct between two teleporter versions now
//
// Revision 1.17  2003/10/22 07:07:06  dkrajzew
// patching of lane states on force vehicle removal added
//
// Revision 1.16  2003/09/05 15:01:24  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.15  2003/08/04 11:35:52  dkrajzew
// only GUIVehicles need a color definition; process of building cars changed
//
// Revision 1.14  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.13  2003/07/22 14:59:27  dkrajzew
// changes due to new detector handling
//
// Revision 1.12  2003/07/16 15:24:55  dkrajzew
// GUIGrid now handles the set of things to draw in another manner than
//  GUIEdgeGrid did; Further things to draw implemented
//
// Revision 1.11  2003/06/05 06:29:50  dkrajzew
// first tries to build under linux: warnings removed;
//  moc-files included Makefiles added
//
// Revision 1.10  2003/05/28 07:52:31  dkrajzew
// new usage of MSEventControl adapted
//
// Revision 1.9  2003/05/21 15:15:41  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.8  2003/05/20 09:26:57  dkrajzew
// data retrieval for new views added
//
// Revision 1.7  2003/04/16 09:50:06  dkrajzew
// centering of the network debugged; additional parameter
//  of maximum display size added
//
// Revision 1.6  2003/04/15 09:09:14  dkrajzew
// documentation added
//
// Revision 1.5  2003/03/20 16:19:28  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.4  2003/03/12 16:52:06  dkrajzew
// centering of objects debuggt
//
// Revision 1.3  2003/02/07 10:39:17  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utility>
#include <microsim/MSNet.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/gui/windows/GUIGrid.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdgeControl;
class MSJunctionControl;
class MSEmitControl;
class MSEventControl;
class MSRouteLoaderControl;
class MSTLLogicControl;
class MSTrafficLightLogic;
class MSLink;
class GUIJunctionWrapper;
class GUIDetectorWrapper;
class GUITriggerWrapper;
class GUINetWrapper;
class GUITrafficLightLogicWrapper;
class RGBColor;
class GUIEdge;
class OutputDevice;
class GUIVehicle;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUINet
 * This gui version of the network allows the retrieval of some more
 * information than the normal network version does. Due to this, not only
 * these retrival, but also some further initialisation methods must have
 * been implemented. Nonethenless, this class has almost the same functions
 * as the MSNet-class.
 * Some microsimulation items are wrapped in certain classes to allow their
 * drawing and their enumerated access. This enumeration is realised by
 * inserting the wrapped items into vectors and is needed to fasten the
 * network's drawing as only visible items are being drawn.
 */
class GUINet
    : public MSNet
{
public:
    /// destructor
    ~GUINet();

    /// returns the bounder of the network
    const Boundary &getBoundary() const;

    /// preinitialises the network (before the network is loaded
    static void preInitGUINet( SUMOTime startTimeStep,
        MSVehicleControl *vc);

    /// initialises the network (after the loading)
    static void initGUINet( std::string id, MSEdgeControl* ec, MSJunctionControl* jc,
        MSRouteLoaderControl *rlc, MSTLLogicControl *tlc,
        bool logExecutionTime,
        const std::vector<OutputDevice*> &streams,
        TimeVector dumpMeanDataIntervalls, std::string baseNameDumpFiles,
        TimeVector laneDumpMeanDataIntervalls, std::string baseNameLaneDumpFiles);

    /// returns the position of a junction
    Position2D getJunctionPosition(const std::string &name) const;

    /// returns the position of a vehicle
    Position2D getVehiclePosition(const std::string &name,
        bool useCenter=true) const;

    /// returns the information whether the vehicle still exists
    bool vehicleExists(const std::string &name) const;

    /// returns the boundary of an edge
    Boundary getEdgeBoundary(const std::string &name) const;

    /// Some further steps needed for gui processing
    void guiSimulationStep();

    //{@ functions for performace measurements
    /// Returns the duration of the last step (sim+visualisation+idle) (in ms)
    int getWholeDuration() const;

    /// Returns the duration of the last step's simulation part (in ms)
    int getSimDuration() const;

    /// Returns the simulation speed as a factor to real time
    double getRTFactor() const;

    /// Returns the update per seconds rate
    double getUPS() const;

    /// Returns the simulation speed as a factor to real time
    double getMeanRTFactor(int duration) const;

    /// Returns the update per seconds rate
    double getMeanUPS() const;

    // Returns the duration of the last step's visualisation part (in ms)
    //int getVisDuration() const;

    /// Returns the duration of the last step's idle part (in ms)
    int getIdleDuration() const;

    /// Sets the duration of the last step's simulation part
    void setSimDuration(int val);

    // Sets the duration of the last step's visualisation part
    //void setVisDuration(int val);

    /// Sets the duration of the last step's idle part
    void setIdleDuration(int val);

    //}

    size_t getDetectorWrapperNo() const;

    GUINetWrapper *getWrapper() const;

    unsigned int getLinkTLID(MSLink *link) const;

    std::vector<size_t> getJunctionIDs() const; // !!! should not be done herein

    std::vector<size_t> getAdditionalIDs() const;


    friend class GUIViewTraffic; // !!!
    friend class GUIViewAggregatedLanes; // !!!
    friend class GUISUMOAbstractView; // !!!
    friend class GUIGridBuilder;

    virtual void closeBuilding(const NLNetBuilder &nb);
	bool hasPosition(GUIVehicle *v) const;
// !!! 4 UniDortmund #ifdef NETWORKING_BLA
	void networking(SUMOTime startTimeStep, SUMOTime currentStep);
// !!! 4 UniDortmund #endif

private:
    /// Initialises the detector wrappers
    void initDetectors();

    /// Initialises the tl-logic map and wrappers
    void initTLMap();

protected:
    /// default constructor
    GUINet();

    /** A grid laid over the network to allow the drawing of visible items only */
    GUIGrid _grid;

    /// the networks boundary
    Boundary _boundary;

    /// The wrapper for the network
    GUINetWrapper *myWrapper;

    /// Wrapped MS-edges
    std::vector<GUIEdge*> myEdgeWrapper;

    /// Wrapped MS-junctions
    std::vector<GUIJunctionWrapper*> myJunctionWrapper;

    /// Wrapped TL-Logics
    std::vector<MSTrafficLightLogic*> myTLLogicWrappers;

    /// A detector dictionary
    std::map<std::string, GUIDetectorWrapper*> myDetectorDict;

    /// A link2tl-logic map
    std::map<MSLink*, GUITrafficLightLogicWrapper*> myLinks2Logic;

    /// The step durations (simulation, /*visualisation, */idle)
    int myLastSimDuration, /*myLastVisDuration, */myLastIdleDuration;

    long myLastVehicleMovementCount, myOverallSimDuration, myOverallVehicleCount;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

