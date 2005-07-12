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
// Revision 1.20  2005/07/12 12:14:38  dkrajzew
// edge-based mean data implemented; previous lane-based is now optional
//
// Revision 1.19  2005/05/04 07:55:27  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.18  2005/02/01 10:07:24  dkrajzew
// performance computation added
//
// Revision 1.17  2004/11/23 10:12:26  dkrajzew
// new detectors usage applied
//
// Revision 1.16  2004/08/02 11:57:12  dkrajzew
// using OutputDevices instead of ostreams
//
// Revision 1.15  2004/07/02 08:38:51  dkrajzew
// changes needed to implement the online-router (class derivation)
//
// Revision 1.14  2004/04/02 11:14:36  dkrajzew
// extended traffic lights are no longer template classes
//
// Revision 1.13  2004/01/26 06:49:06  dkrajzew
// work on detectors: e3-detectors loading and visualisation;
//  variable offsets and lengths for lsa-detectors;
//  coupling of detectors to tl-logics
//
// Revision 1.12  2004/01/12 14:59:51  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.11  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
// Revision 1.10  2003/07/07 08:13:15  dkrajzew
// first steps towards the usage of a real lane and junction geometry
//  implemented
//
// Revision 1.9  2003/06/05 11:39:31  dkrajzew
// class templates applied; documentation added
//
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
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/MSRouteLoader.h>
#include <netload/NLEdgeControlBuilder.h>
#include <netload/NLJunctionControlBuilder.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUISourceLane.h>
#include <guisim/GUIRouteHandler.h>
#include <guisim/GUIVehicle.h>
#include <guinetload/GUIEdgeControlBuilder.h>
#include <guinetload/GUIJunctionControlBuilder.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <sumo_only/SUMOFrame.h>
#include "GUIContainer.h"
#include "microsim/MSRouteLoaderControl.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIContainer::GUIContainer(NLEdgeControlBuilder &edgeBuilder,
                           NLJunctionControlBuilder &junctionBuilder)
    : NLContainer(edgeBuilder, junctionBuilder)
{
}


GUIContainer::~GUIContainer()
{
}


GUINet *
GUIContainer::buildGUINet(NLDetectorBuilder &db, const OptionsCont &options)
{
    closeJunctions(db);
    MSEdgeControl *edges = 0;
    MSJunctionControl *junctions = 0;
    MSEmitControl *emitters = 0;
    MSRouteLoaderControl *routeLoaders = 0;
    try {
        MSEdgeControl *edges = myEdgeControlBuilder.build();
        MSJunctionControl *junctions = myJunctionControlBuilder.build();
        MSRouteLoaderControl *routeLoaders = buildRouteLoaderControl(options);
        MSTLLogicControl *tlc = new MSTLLogicControl(getTLLogicVector());
        std::vector<OutputDevice*> streams = SUMOFrame::buildStreams(options);
        GUINet::initGUINet( "", edges, junctions, routeLoaders, tlc,
            !options.getBool("no-duration-log"),
            streams,
            options.getIntVector("dump-intervals"),
            options.getString("dump-basename"),
            options.getIntVector("lanedump-intervals"),
            options.getString("lanedump-basename"));
        return static_cast<GUINet*>(GUINet::getInstance());
    } catch (ProcessError &) {
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
//        MSNet::clear();
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
    static_cast<GUIEdgeControlBuilder&>(myEdgeControlBuilder).addSrcDestInfo(id, fromJ, toJ);
}


MSRouteLoader *
GUIContainer::buildRouteLoader(const std::string &file)
{
    return new MSRouteLoader(
        *(MSNet::getInstance()),
        new GUIRouteHandler(file, false));
}


void
GUIContainer::addJunctionShape(const Position2DVector &shape)
{
    static_cast<GUIJunctionControlBuilder&>(myJunctionControlBuilder).addJunctionShape(shape);
}


/**
void
GUIContainer::addLane(const string &id, const bool isDepartLane,
                      const float maxSpeed, const float length,
                      const float changeUrge)
{
    myID = id;
    myLaneIsDepart = isDepartLane;
    myCurrentMaxSpeed = maxSpeed;
    myCurrentLength = length;
    myCurrentChangeUrge = changeUrge;
}



void
GUIContainer::closeLane()
{
    MSLane *lane =
        static_cast<GUIEdgeControlBuilder&>(myEdgeControlBuilder).addLane(
            getNet(), myID, myCurrentMaxSpeed, myCurrentLength,
            myLaneIsDepart, myShape);
    // insert the lane into the lane-dictionary, checking
    if(!MSLane::dictionary(myID, lane)) {
        throw XMLIdAlreadyUsedException("Lanes", myID);
    }
}

**/

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:








