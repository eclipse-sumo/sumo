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
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.6  2003/06/18 11:08:05  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
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
#include <sax2/XMLReaderFactory.hpp>
#include <sax2/DefaultHandler.hpp>
#include <utils/options/OptionsCont.h>
#include <guisim/GUINet.h>
#include <guisim/GUIVehicleTransfer.h>
#include <netload/NLNetHandler.h>
#include <netload/NLLoadFilter.h>
#include <utils/common/MsgHandler.h>
#include "GUINetHandler.h"
#include "GUIEdgeControlBuilder.h"
#include "GUIJunctionControlBuilder.h"
#include "GUIContainer.h"
#include "GUINetBuilder.h"


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUINetBuilder::GUINetBuilder(const OptionsCont &oc)
    : NLNetBuilder(oc, new GUIVehicleTransfer())
{
    GUINet::preInitGUINet(
        oc.getInt("b"),
        oc.getUIntVector("dump-intervals"),
        oc.getString("dump-basename"));
}


GUINetBuilder::~GUINetBuilder()
{
}


GUINet *
GUINetBuilder::buildGUINet(bool allowAggregation)
{
    GUIContainer *container = new GUIContainer(
        new GUIEdgeControlBuilder(allowAggregation),
        new GUIJunctionControlBuilder());
    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(
        XMLString::transcode("http://xml.org/sax/features/validation"),
        false);
    GUINet *net = 0;
    // get the matching handler
    NLNetHandler *handler =
        new GUINetHandler("", *container);
    bool ok = load(handler, *parser);
    subreport("Loading done.", "Loading failed.");
    if(!MsgHandler::getErrorInstance()->wasInformed()) {
        net = container->buildGUINet(/*
            m_pOptions.getUIntVector("dump-intervals"),
            m_pOptions.getString("dump-basename"),*/
            m_pOptions);
    }
    delete parser;
    if(ok) {
        report(*container);
    }
    delete handler;
    delete container;
    return net;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUINetBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:


