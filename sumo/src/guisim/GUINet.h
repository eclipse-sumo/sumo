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
// GUIGrid now handles the set of things to draw in another manner than GUIEdgeGrid did; Further things to draw implemented
//
// Revision 1.11  2003/06/05 06:29:50  dkrajzew
// first tries to build under linux: warnings removed; moc-files included Makefiles added
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
// centering of the network debugged; additional parameter of maximum display size added
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
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utility>
#include <microsim/MSNet.h>
#include <utils/geom/Boundery.h>
#include <utils/geom/Position2D.h>
#include <gui/GUIGlObjectStorage.h>
#include "GUIGrid.h"


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
class GUIEmitterWrapper;
class GUINetWrapper;
class GUITrafficLightLogicWrapper;
class RGBColor;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
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
    const Boundery &getBoundery() const;

    /// preinitialises the network (before the network is loaded
    static void preInitGUINet( MSNet::Time startTimeStep,
        TimeVector dumpMeanDataIntervalls, std::string baseNameDumpFiles);

    /// initialises the network (after the loading)
    static void initGUINet( std::string id, MSEdgeControl* ec, MSJunctionControl* jc,
        /*DetectorCont* detectors,*/ MSRouteLoaderControl *rlc, MSTLLogicControl *tlc);

    /// returns the position of a junction (!!! shouldn't it be a const&?)
    Position2D getJunctionPosition(const std::string &name) const;

    /// returns the position of a vehicle (!!! shouldn't it be a const&?)
    Position2D getVehiclePosition(const std::string &name,
        bool useCenter=true) const;

    /// returns the position of a detector (!!! shouldn't it be a const&?)
    Position2D getDetectorPosition(const std::string &name) const;

    /// returns the position of a emitter (!!! shouldn't it be a const&?)
    Position2D getEmitterPosition(const std::string &name) const;

    /// returns the information whether the vehicle still exists
    bool vehicleExists(const std::string &name) const;

    /// returns the boundery of an edge (!!! shouldn't it be a const&?)
    Boundery getEdgeBoundery(const std::string &name) const;

    /// builds a new GUIVehicle
    MSVehicle *buildNewVehicle( std::string id, MSRoute* route,
        MSNet::Time departTime, const MSVehicleType* type,
        int repNo, int repOffset);

    MSVehicle *buildNewGUIVehicle( std::string id, MSRoute* route,
        MSNet::Time departTime, const MSVehicleType* type,
        int repNo, int repOffset, const RGBColor &color);

    size_t getDetectorWrapperNo() const;

    GUINetWrapper *getWrapper() const;

    GUIGlObjectStorage &getIDStorage();

    unsigned int getLinkTLID(MSLink *link) const;


    friend class GUIViewTraffic; // !!!
    friend class GUIViewAggregatedLanes; // !!!
    friend class GUISUMOAbstractView; // !!!
    friend class GUIGrid;

private:
    /// Initialises the detector wrappers
    static void initDetectors();

    /// Initialises the tl-logic map and wrappers
    static void initTLMap();

protected:
    /// default constructor
    GUINet();

    /** A grid laid over the network to allow the drawing of visible items only */
    GUIGrid _grid;

    /// the networks boundery
    Boundery _boundery;

    /** @brief A container for numerical ids of objects
        in order to make them grippable by openGL */
    GUIGlObjectStorage _idStorage;

    /// The wrapper for the network
    GUINetWrapper *myWrapper;

    /// Wrapped MS-edges
    std::vector<GUIEdge*> myEdgeWrapper;

    /// Wrapped MS-junctions
    std::vector<GUIJunctionWrapper*> myJunctionWrapper;

    /// Wrapped MS-detectors
    std::vector<GUIDetectorWrapper*> myDetectorWrapper;

    /// Wraped MS-emitters
    std::vector<GUIEmitterWrapper*> myEmitter;

    /// Wrapped TL-Logics
    std::vector<MSTrafficLightLogic*> myTLLogicWrappers;

    /// A detector dictionary
    std::map<std::string, GUIDetectorWrapper*> myDetectorDict;

    /// An emitter dictionary
    std::map<std::string, GUIEmitterWrapper*> myEmitterDict;

    /// A link2tl-logic map
    std::map<MSLink*, GUITrafficLightLogicWrapper*> myLinks2Logic;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUINet.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

