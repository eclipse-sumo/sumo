//---------------------------------------------------------------------------//
//                        GUINetBuilder.cpp -
//  Builds the gui-network
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
// Revision 1.24  2005/02/17 10:33:29  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.23  2005/01/27 14:19:35  dkrajzew
// ability to load from a string added
//
// Revision 1.22  2004/12/16 12:23:37  dkrajzew
// first steps towards a better parametrisation of traffic lights
//
// Revision 1.21  2004/11/25 16:26:46  dkrajzew
// consolidated and debugged some detectors and their usage
//
// Revision 1.20  2004/11/23 10:12:27  dkrajzew
// new detectors usage applied
//
// Revision 1.19  2004/08/02 11:56:10  dkrajzew
// "time-to-teleport" option added
//
// Revision 1.18  2004/07/02 08:38:51  dkrajzew
// changes needed to implement the online-router (class derivation)
//
// Revision 1.17  2004/04/02 11:14:36  dkrajzew
// extended traffic lights are no longer template classes
//
// Revision 1.16  2004/03/19 12:56:48  dkrajzew
// porting to FOX
//
// Revision 1.15  2004/01/26 06:49:06  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics
//
// Revision 1.14  2004/01/12 14:44:30  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.13  2003/12/11 06:18:35  dkrajzew
// network loading and initialisation improved
//
// Revision 1.12  2003/12/04 13:25:52  dkrajzew
// handling of internal links added; documentation added;
//  some dead code removed
//
// Revision 1.11  2003/10/22 15:41:28  dkrajzew
// we have to distinct between two teleporter versions now
//
// Revision 1.10  2003/09/05 14:56:11  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.9  2003/08/18 12:35:09  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.8  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
// Revision 1.7  2003/07/07 08:13:15  dkrajzew
// first steps towards the usage of a real lane and junction geometry
//  implemented
//
// Revision 1.6  2003/06/18 11:08:05  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.5  2003/03/20 16:16:31  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.4  2003/03/12 16:53:41  dkrajzew
// first extensions for geometry handling
//
// Revision 1.3  2003/02/07 10:38:19  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <parsers/SAXParser.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <utils/options/OptionsCont.h>
#include <guisim/GUINet.h>
#include <netload/NLNetHandler.h>
#include <netload/NLLoadFilter.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/XMLHelpers.h>
#include <microsim/MSGlobals.h>
#include "GUINetHandler.h"
#include "GUIEdgeControlBuilder.h"
#include "GUIJunctionControlBuilder.h"
#include "GUIContainer.h"
#include "GUIDetectorBuilder.h"
#include "GUITriggerBuilder.h"
#include "GUINetBuilder.h"
#include <guisim/GUIVehicleControl.h>
#include <xercesc/framework/MemBufInputSource.hpp>


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif



static const char*  gMemBufId = "internal";


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUINetBuilder::GUINetBuilder(const OptionsCont &oc,
                             NLEdgeControlBuilder &eb,
                             NLJunctionControlBuilder &jb,
                             bool allowAggregatedViews)
    : NLNetBuilder(oc, eb, jb),
    myAgregatedViewsAllowed(allowAggregatedViews)
{
}


GUINetBuilder::~GUINetBuilder()
{
}


MSNet *
GUINetBuilder::buildNetworkFromDescription(MSVehicleControl *vc,
                                           const std::string &description)
{
    // preinit network
    GUINet::preInitGUINet(m_pOptions.getInt("b"), vc);

    // we need a specialised detector and trigger builders
    GUIDetectorBuilder db;
    GUITriggerBuilder tb;
    // initialise loading buffer ...
    GUIContainer *container = new GUIContainer(
        myEdgeBuilder, myJunctionBuilder);
    // get the matching handler
    GUINetHandler handler("", *container, db, tb,
        m_pOptions.getFloat("actuated-tl.detector-pos"),
        m_pOptions.getFloat("agent-tl.detector-len"),
        m_pOptions.getInt("agent-tl.learn-horizon"),
        m_pOptions.getInt("agent-tl.decision-horizon"),
        m_pOptions.getFloat("agent-tl.min-diff"),
        m_pOptions.getInt("agent-tl.tcycle"),

        m_pOptions.getFloat("actuated-tl.max-gap"),
        m_pOptions.getFloat("actuated-tl.passing-time"),
        m_pOptions.getFloat("actuated-tl.detector-gap"));
    handler.setWanted(LOADFILTER_NET);
    // ... and the parser
    GUINet *net = 0;
    SAX2XMLReader* parser = XMLHelpers::getSAXReader(handler);
    MemBufInputSource* memBufIS =
        new MemBufInputSource((const XMLByte*)description.c_str(),
            description.length(), gMemBufId, false);
    parser->parse(*memBufIS);
    if(!MsgHandler::getErrorInstance()->wasInformed()) {
        net = container->buildGUINet(db, m_pOptions);
        if(net!=0) {
            net->closeBuilding(*this);
        }
    }
    delete parser;
    delete container;
    return net;
}


MSNet *
GUINetBuilder::buildNet(MSVehicleControl *vc)
{
    // preinit network
    GUINet::preInitGUINet(m_pOptions.getInt("b"), vc);

    // we need a specialised detector and trigger builders
    GUIDetectorBuilder db;
    GUITriggerBuilder tb;
    // initialise loading buffer ...
    GUIContainer *container = new GUIContainer(
        myEdgeBuilder, myJunctionBuilder);
    // get the matching handler
    GUINetHandler handler("", *container, db, tb,
        m_pOptions.getFloat("actuated-tl.detector-pos"),
        m_pOptions.getFloat("agent-tl.detector-len"),
        m_pOptions.getInt("agent-tl.learn-horizon"),
        m_pOptions.getInt("agent-tl.decision-horizon"),
        m_pOptions.getFloat("agent-tl.min-diff"),
        m_pOptions.getInt("agent-tl.tcycle"),

        m_pOptions.getFloat("actuated-tl.max-gap"),
        m_pOptions.getFloat("actuated-tl.passing-time"),
        m_pOptions.getFloat("actuated-tl.detector-gap"));
    // ... and the parser
    SAX2XMLReader* parser = XMLHelpers::getSAXReader(handler);
    GUINet *net = 0;
    bool ok = load(LOADFILTER_ALL, m_pOptions.getString("n"),
        handler, *parser);
    if(!MsgHandler::getErrorInstance()->wasInformed()) {
        net = container->buildGUINet(db, m_pOptions);
    }
    if(net==0) {
        ok = false;
    }
    // load the junctions
    if(m_pOptions.isSet("r")&&ok&&m_pOptions.getInt("route-steps")<=0) {
        ok = load(LOADFILTER_DYNAMIC, m_pOptions.getString("r"),
            handler, *parser);
    }
    // load additional net elements (sources, detectors, ...)
    if(m_pOptions.isSet("a")&&ok) {
        ok = load(LOADFILTER_NETADD, m_pOptions.getString("a"),
            handler, *parser);
    }
    // close building
    if(ok) {
        net->closeBuilding(*this);
    }
    subreport("Loading done.", "Loading failed.");
    delete parser;
    delete container;
    return net;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


