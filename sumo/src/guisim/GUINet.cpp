//---------------------------------------------------------------------------//
//                        GUINet.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.17  2003/09/22 12:38:51  dkrajzew
// detectors need const Lanes
//
// Revision 1.16  2003/09/05 15:01:24  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.15  2003/08/04 11:35:51  dkrajzew
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
// Revision 1.6  2003/04/14 08:27:17  dkrajzew
// new globject concept implemented
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
#include <utility>
#include <microsim/MSNet.h>
#include <microsim/MSJunction.h>
#include <microsim/MSInductLoop.h>
#include <microsim/MSDetectorSubSys.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEmitControl.h>
#include <gui/GUIGlObjectStorage.h>
#include <utils/gfx/RGBColor.h>
#include "GUINetWrapper.h"
#include <guisim/GUIEdge.h>
#include <guisim/GUIEmitterWrapper.h>
#include <guisim/GUIDetectorWrapper.h>
//#include "GUIEdgeGrid.h"
#include "GUIVehicle.h"
#include "GUINet.h"
#include "GUIHelpingJunction.h"
//#include "GUIHelpingDetector.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUINet::GUINet()
    : MSNet(), _grid(*this, 10, 10),
    myWrapper(new GUINetWrapper(_idStorage, *this))
{
}


GUINet::~GUINet()
{
    _idStorage.clear();
    delete myWrapper;
}


const Boundery &
GUINet::getBoundery() const
{
    return _boundery;
}


void
GUINet::preInitGUINet( MSNet::Time startTimeStep,
                      TimeVector dumpMeanDataIntervalls,
                      std::string baseNameDumpFiles )
{
    myInstance = new GUINet();
    myInstance->myStep = startTimeStep;
    initMeanData(dumpMeanDataIntervalls, baseNameDumpFiles/*, true*/);
	myInstance->myEmitter = new MSEmitControl("");
}


void
GUINet::initGUINet( std::string id, MSEdgeControl* ec, MSJunctionControl* jc,
                   /*DetectorCont* detectors, */MSRouteLoaderControl *rlc,
                   MSTLLogicControl *tlc)
{
    MSNet::init(id, ec, jc,/*detectors, */rlc, tlc);
    GUINet *net = static_cast<GUINet*>(MSNet::getInstance());
    // initialise edge storage for gui
    GUIEdge::fill(net->myEdgeWrapper);
    // initialise junction storage for gui
    GUIHelpingJunction::fill(net->myJunctionWrapper, net->_idStorage);
    // initialise detector storage for gui
    initDetectors();
    // initialise the lane states for lane wrappers when wished
    // build the grid
//    net->_edgeGrid.init();
    net->_grid.init();
    // get the boundery
    net->_boundery = net->_grid.getBoundery();
}


void
GUINet::initDetectors()
{
    GUINet *net = static_cast<GUINet*>(MSNet::getInstance());
    MSDetectorSubSys::LoopDict::ValueVector loopVec(
        MSDetectorSubSys::LoopDict::getInstance()->getStdVector() );
    size_t size = loopVec.size();
    net->myDetectorWrapper.reserve(size);
    for(MSDetectorSubSys::LoopDict::ValueVector::iterator
        i=loopVec.begin(); i!=loopVec.end(); i++) {

        const MSLane *lane = (*i)->getLane();
        GUIEdge *edge =
            static_cast<GUIEdge*>(MSEdge::dictionary(lane->edge().id()));

        // build the wrapper
        GUIDetectorWrapper *wrapper =
            (*i)->buildDetectorWrapper(
                net->_idStorage, edge->getLaneGeometry(lane));
        // add to list
        net->myDetectorWrapper.push_back(wrapper);
        // add to dictionary
        net->myDetectorDict[wrapper->microsimID()] = wrapper;
    }

}


Position2D
GUINet::getJunctionPosition(const std::string &name) const
{
    MSJunction *junction = MSJunction::dictionary(name);
    return Position2D(junction->getXCoordinate(), junction->getYCoordinate());
}


Position2D
GUINet::getVehiclePosition(const std::string &name, bool useCenter) const
{
    MSVehicle *vehicle = MSVehicle::dictionary(name);
    if(vehicle==0) {
        // !!!
    }
    const GUIEdge * const edge =
        static_cast<const GUIEdge * const>(vehicle->getEdge());
    double pos = vehicle->pos();
    if(useCenter) {
        pos -= (vehicle->length() / 2.0);
    }
    return edge->getLanePosition(vehicle->getLane(), pos);
}


Position2D
GUINet::getDetectorPosition(const std::string &name) const
{
    std::map<std::string, GUIDetectorWrapper*>::const_iterator i=
        myDetectorDict.find(name);
    assert(i!=myDetectorDict.end());
    GUIDetectorWrapper *tmp = (*i).second;
    return Position2D(
        (*i).second->getXCoordinate(),
        (*i).second->getYCoordinate());
}


Position2D
GUINet::getEmitterPosition(const std::string &name) const
{
    std::map<std::string, GUIEmitterWrapper*>::const_iterator i=
        myEmitterDict.find(name);
    assert(i!=myEmitterDict.end());
    return Position2D(
        (*i).second->getXCoordinate(),
        (*i).second->getYCoordinate());
}


bool
GUINet::vehicleExists(const std::string &name) const
{
    return MSVehicle::dictionary(name)!=0;
}


Boundery
GUINet::getEdgeBoundery(const std::string &name) const
{
    GUIEdge *edge = static_cast<GUIEdge*>(MSEdge::dictionary(name));
    return Boundery(
        edge->fromXPos(), edge->fromYPos(), edge->toXPos(), edge->toYPos());
}


MSVehicle *
GUINet::buildNewVehicle( std::string id, MSRoute* route,
                       MSNet::Time departTime,
                       const MSVehicleType* type,
                       int repNo, int repOffset)
{
    return buildNewGUIVehicle(id, route, departTime, type, repNo,
        repOffset, RGBColor(-1, -1, -1));
}


MSVehicle *
GUINet::buildNewGUIVehicle( std::string id, MSRoute* route,
                       MSNet::Time departTime,
                       const MSVehicleType* type,
                       int repNo, int repOffset,
                       const RGBColor &color)
{
    size_t noIntervals = getNDumpIntervalls();
	myLoadedVehNo++;
    GUIVehicle * veh = new GUIVehicle(_idStorage,
        id, route, departTime,
        type, noIntervals, repNo, repOffset, color);
    return veh;
}

size_t
GUINet::getDetectorWrapperNo() const
{
    // !!! maybe this should return all the values for lanes, junction, detectors etc.
    return myDetectorWrapper.size();
}


GUINetWrapper *
GUINet::getWrapper() const
{
    return myWrapper;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUINet.icc"
//#endif

// Local Variables:
// mode:C++
// End:


