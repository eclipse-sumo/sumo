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
#include "GUIContainer.h"


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
    MSEdgeControl *edges = m_pECB->build();
    MSJunctionControl *junctions = m_pJCB->build();
    MSEmitControl *emitters = new MSEmitControl("");
    MSRouteLoaderControl *routeLoaders = buildRouteLoaderControl(options);
    GUINet::initGUINet( m_Id, edges, junctions, emitters,
        m_EventControl, m_pDetectors, routeLoaders);
    return static_cast<GUINet*>(GUINet::getInstance());
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








