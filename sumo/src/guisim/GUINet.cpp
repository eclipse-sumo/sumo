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
// Revision 1.24  2003/11/20 13:05:32  dkrajzew
// loading and using of predefined vehicle colors added
//
// Revision 1.23  2003/11/18 14:30:40  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.22  2003/11/12 14:01:54  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.21  2003/11/11 08:13:23  dkrajzew
// consequent usage of Position2D instead of two doubles
//
// Revision 1.20  2003/10/30 08:59:43  dkrajzew
// first implementation of aggregated views using E2-detectors
//
// Revision 1.19  2003/10/22 15:42:56  dkrajzew
// we have to distinct between two teleporter versions now
//
// Revision 1.18  2003/10/02 14:51:20  dkrajzew
// visualisation of E2-detectors implemented
//
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
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSTrafficLightLogic.h>
#include <gui/GUIGlObjectStorage.h>
#include <utils/gfx/RGBColor.h>
#include "GUINetWrapper.h"
#include <guisim/guilogging/GLObjectValuePassConnector.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUIEmitterWrapper.h>
#include <guisim/GUIVehicleTransfer.h>
#include <guisim/GUIDetectorWrapper.h>
#include <guisim/GUI_E2_ZS_Collector.h>
#include <guisim/GUI_E2_ZS_CollectorOverLanes.h>
#include <guisim/GUITrafficLightLogicWrapper.h>
#include <guisim/GUILaneStateReporter.h>
#include <microsim/MSLaneState.h>
#include <microsim/MSUpdateEachTimestepContainer.h>
#include "GUIVehicle.h"
#include "GUINet.h"
#include "GUIHelpingJunction.h"


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
                   MSRouteLoaderControl *rlc, MSTLLogicControl *tlc)
{
    MSNet::init(id, ec, jc, rlc, tlc);
    GUINet *net = static_cast<GUINet*>(MSNet::getInstance());
    // initialise edge storage for gui
    GUIEdge::fill(net->myEdgeWrapper);
    // initialise junction storage for gui
    GUIHelpingJunction::fill(net->myJunctionWrapper, net->_idStorage);
    // initialise detector storage for gui
    initDetectors();
    // initialise the tl-map
    initTLMap();
    // build the grid
    net->_grid.init();
    // get the boundery
    net->_boundery = net->_grid.getBoundery();
}


void
GUINet::initDetectors()
{
    GUINet *net = static_cast<GUINet*>(MSNet::getInstance());
	//
    MSDetectorSubSys::E2ZSDict::ValueVector loopVec2(
        MSDetectorSubSys::E2ZSDict::getInstance()->getStdVector() );
    net->myDetectorWrapper.reserve(loopVec2.size()+net->myDetectorWrapper.size());
    for(MSDetectorSubSys::E2ZSDict::ValueVector::iterator
        i2=loopVec2.begin(); i2!=loopVec2.end(); i2++) {

        const MSLane *lane = (*i2)->getLane();
        GUIEdge *edge =
            static_cast<GUIEdge*>(MSEdge::dictionary(lane->edge().id()));

        // build the wrapper
        if((*i2)->amVisible()) {
            GUIDetectorWrapper *wrapper =
                static_cast<GUI_E2_ZS_Collector*>(*i2)->buildDetectorWrapper(
                    net->_idStorage, edge->getLaneGeometry(lane));
            // add to list
            net->myDetectorWrapper.push_back(wrapper);
            // add to dictionary
            net->myDetectorDict[wrapper->microsimID()] = wrapper;
        }
    }

	//
    MSDetectorSubSys::E2ZSOLDict::ValueVector loopVec3(
        MSDetectorSubSys::E2ZSOLDict::getInstance()->getStdVector() );
    net->myDetectorWrapper.reserve(loopVec2.size()+net->myDetectorWrapper.size());
    for(MSDetectorSubSys::E2ZSOLDict::ValueVector::iterator
        i3=loopVec3.begin(); i3!=loopVec3.end(); i3++) {
        // build the wrapper
        GUIDetectorWrapper *wrapper =
            static_cast<GUI_E2_ZS_CollectorOverLanes*>(*i3)->buildDetectorWrapper(
                net->_idStorage);
        // add to list
        net->myDetectorWrapper.push_back(wrapper);
        // add to dictionary
        net->myDetectorDict[wrapper->microsimID()] = wrapper;
    }

	//
    MSDetectorSubSys::LoopDict::ValueVector loopVec(
        MSDetectorSubSys::LoopDict::getInstance()->getStdVector() );
    net->myDetectorWrapper.reserve(loopVec.size()+net->myDetectorWrapper.size());
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


void
GUINet::initTLMap()
{
    GUINet *net = static_cast<GUINet*>(MSNet::getInstance());
	//
    typedef std::vector<MSTrafficLightLogic*> LogicVector;
    // get the list of loaded tl-logics
    LogicVector tlls = MSTrafficLightLogic::getList();
    // allocate storage for the wrappers
    net->myTLLogicWrappers.reserve(tlls.size());
    // go through the logics
    for(LogicVector::iterator i=tlls.begin(); i!=tlls.end(); i++) {
        // get the logic
        MSTrafficLightLogic *tll = (*i);
        // build the wrapper
        GUITrafficLightLogicWrapper *tllw =
            new GUITrafficLightLogicWrapper(net->_idStorage, *tll);
        // get the links
        const MSTrafficLightLogic::LinkVectorVector &links =
            tll->getLinks();
        // build the association link->wrapper
        MSTrafficLightLogic::LinkVectorVector::const_iterator j;
        for(j=links.begin(); j!=links.end(); j++) {
            MSTrafficLightLogic::LinkVector::const_iterator j2;
            for(j2=(*j).begin(); j2!=(*j).end(); j2++) {
                net->myLinks2Logic[*j2] = tllw;
            }
        }
        // save the wrapper
    }
}


Position2D
GUINet::getJunctionPosition(const std::string &name) const
{
    MSJunction *junction = MSJunction::dictionary(name);
    return Position2D(junction->getPosition());
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
    return (*i).second->getPosition();
}


Position2D
GUINet::getEmitterPosition(const std::string &name) const
{
    std::map<std::string, GUIEmitterWrapper*>::const_iterator i=
        myEmitterDict.find(name);
    assert(i!=myEmitterDict.end());
    return (*i).second->getPosition();
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
    return edge->getBoundery();
}


MSVehicle *
GUINet::buildNewVehicle( std::string id, MSRoute* route,
                       MSNet::Time departTime,
                       const MSVehicleType* type,
                       int repNo, int repOffset, const RGBColor &col)
{
    size_t noIntervals = getNDumpIntervalls();
	myLoadedVehNo++;
    GUIVehicle * veh = new GUIVehicle(_idStorage,
        id, route, departTime,
        type, noIntervals, repNo, repOffset, col);
    return veh;
/*    return buildNewGUIVehicle(id, route, departTime, type, repNo,
        repOffset, RGBColor(-1, -1, -1));*/
}

/*
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
*/

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


GUIGlObjectStorage &
GUINet::getIDStorage()
{
    return _idStorage;
}


unsigned int
GUINet::getLinkTLID(MSLink *link) const
{
    std::map<MSLink*, GUITrafficLightLogicWrapper*>::const_iterator i =
        myLinks2Logic.find(link);
    assert(i!=myLinks2Logic.end());
    return (*i).second->getGlID();
}


void
GUINet::guiSimulationStep()
{
    MSUpdateEachTimestepContainer<MSUpdateEachTimestep<GUILaneStateReporter> >::getInstance()->updateAll();
    MSUpdateEachTimestepContainer<MSUpdateEachTimestep<GLObjectValuePassConnector<double> > >::getInstance()->updateAll();
    MSUpdateEachTimestepContainer<MSUpdateEachTimestep<GLObjectValuePassConnector<SimplePhaseDef> > >::getInstance()->updateAll();
    myAggBounderyStorage.initStep();
}


GUILaneStateBounderiesStorage &
GUINet::getAggregatedValueBoundery()
{
    return myAggBounderyStorage;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUINet.icc"
//#endif

// Local Variables:
// mode:C++
// End:


