//---------------------------------------------------------------------------//
//                        GUIContainer.cpp -
//  A loading container derived from NLContainer with additional values
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
// Revision 1.8  2003/05/28 07:54:54  dkrajzew
// new usage of MSEventControl adapted
//
// Revision 1.7  2003/05/21 15:15:40  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.6  2003/04/04 15:17:15  roessel
// Added #include "microsim/MSRouteLoaderControl.h"
//
// Revision 1.5  2003/03/20 16:16:31  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.4  2003/03/12 16:53:40  dkrajzew
// first extensions for geometry handling
//
// Revision 1.3  2003/02/07 10:38:17  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <microsim/MSEdgeControl.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSJunctionLogic.h>
#include <netload/NLEdgeControlBuilder.h>
#include <netload/NLJunctionControlBuilder.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUISourceLane.h>
#include <guisim/GUIVehicle.h>
#include <guinetload/GUIEdgeControlBuilder.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include "GUIContainer.h"
#include "microsim/MSRouteLoaderControl.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIContainer::GUIContainer(NLEdgeControlBuilder * const edgeBuilder)
    : NLContainer(edgeBuilder)
{
}


GUIContainer::~GUIContainer()
{
}


GUINet *
GUIContainer::buildGUINet(MSNet::TimeVector dumpMeanDataIntervalls,
                          std::string baseNameDumpFiles,
                          const OptionsCont &options)
{
    MSEdgeControl *edges = 0;
    MSJunctionControl *junctions = 0;
    MSEmitControl *emitters = 0;
    MSRouteLoaderControl *routeLoaders = 0;
    try {
        MSEdgeControl *edges = m_pECB->build();
        MSJunctionControl *junctions = m_pJCB->build();
        MSRouteLoaderControl *routeLoaders = buildRouteLoaderControl(options);
        GUINet::initGUINet( m_Id, edges, junctions, m_pDetectors, routeLoaders );
        return static_cast<GUINet*>(GUINet::getInstance());
    } catch (ProcessError &e) {
        delete edges;
        delete junctions;
        delete emitters;
        delete routeLoaders;
        MSEdge::clear();
        MSEdgeControl::clear();
        MSEmitControl::clear();
        MSEventControl::clear();
        MSJunction::clear();
        MSJunctionControl::clear();
        MSJunctionLogic::clear();
        MSLane::clear();
        MSNet::clear();
        MSVehicle::clear();
        MSVehicleType::clear();
        MSRoute::clear();
        return 0;
    }
}


void
GUIContainer::addSrcDestInfo(const std::string &id, const std::string &from,
                             const std::string &to)
{
    // retrieve the junctions
    MSJunction *fromJ = MSJunction::dictionary(from);
    MSJunction *toJ = MSJunction::dictionary(to);
    if(fromJ==0) {
        throw XMLIdNotKnownException("junction", from);
    }
    if(toJ==0) {
        throw XMLIdNotKnownException("junction", to);
    }
    // set the values
    static_cast<GUIEdgeControlBuilder*>(m_pECB)->addSrcDestInfo(id, fromJ, toJ);
}


void
GUIContainer::addLane(const string &id, const bool isDepartLane,
                      const float maxSpeed, const float length,
                      const float changeUrge)
{
    MSLane *lane =
        m_pECB->addLane(getNet(), id, maxSpeed, length, isDepartLane);
    if(!MSLane::dictionary(id, lane))
        throw XMLIdAlreadyUsedException("Lanes", id);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIContainer.icc"
//#endif

// Local Variables:
// mode:C++
// End:








