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
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEmitControl.h>
#include <gui/GUIGlObjectStorage.h>
#include <guisim/GUIEdge.h>
#include "GUIEdgeGrid.h"
#include "GUIVehicle.h"
#include "GUINet.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUINet::GUINet()
    : MSNet(), _edgeGrid(10, 10)
{
}


GUINet::~GUINet()
{
    _idStorage.clear();
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
                   MSEventControl* evc,
                   DetectorCont* detectors,
                   MSRouteLoaderControl *rlc)
{
    MSNet::init(id, ec, jc, evc, detectors, rlc);
    GUINet *net = static_cast<GUINet*>(MSNet::getInstance());
    net->_edgeGrid.init();
    net->_boundery = net->_edgeGrid.getBoundery();
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
                       int repNo, int repOffset,
                       float *defColor)
{
    size_t noIntervals = getNDumpIntervalls();
/*    if(withGUI()) {
        noIntervals++;
    }*/
	myLoadedVehNo++;
    GUIVehicle * veh = new GUIVehicle(_idStorage,
        id, route, departTime,
        type, noIntervals, repNo, repOffset, defColor);
    return veh;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUINet.icc"
//#endif

// Local Variables:
// mode:C++
// End:


