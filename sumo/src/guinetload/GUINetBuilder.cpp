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
#include <utils/common/SErrorHandler.h>
#include <utils/options/OptionsCont.h>
#include <guisim/GUINet.h>
#include <netload/NLNetHandler.h>
#include <netload/NLLoadFilter.h>
#include "GUINetHandler.h"
#include "GUIEdgeControlBuilder.h"
#include "GUIContainer.h"
#include "GUINetBuilder.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUINetBuilder::GUINetBuilder(const OptionsCont &oc)
    : NLNetBuilder(oc)
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
GUINetBuilder::buildGUINet()
{
    GUIContainer *container = new GUIContainer(new GUIEdgeControlBuilder());
    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(
        XMLString::transcode("http://xml.org/sax/features/validation"),
        false);
    GUINet *net = 0;
    // get the matching handler
    NLNetHandler *handler =
        new GUINetHandler(m_pOptions.getBool("v"), m_pOptions.getBool("w"),
            "", *container);
    bool ok = load(handler, *parser);
    subreport("Loading done.", "Loading failed.");
    if(!SErrorHandler::errorOccured()) {
        net = container->buildGUINet(
            m_pOptions.getUIntVector("dump-intervals"),
            m_pOptions.getString("dump-basename"),
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


