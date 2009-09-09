/****************************************************************************/
/// @file    NILoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Perfoms network import
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/importio/LineReader.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <netimport/NIXMLEdgesHandler.h>
#include <netimport/NIXMLNodesHandler.h>
#include <netimport/NIXMLTypesHandler.h>
#include <netimport/NIXMLConnectionsHandler.h>
#include <netimport/NIElmar2NodesHandler.h>
#include <netimport/NIElmar2EdgesHandler.h>
#include <netimport/NIImporter_VISUM.h>
#include <netimport/vissim/NIImporter_Vissim.h>
#include <netimport/NIImporter_ArcView.h>
#include <netimport/NIImporter_SUMO.h>
#include <netimport/NIImporter_RobocupRescue.h>
#include <netimport/NIImporter_OpenStreetMap.h>
#include <utils/xml/XMLSubSys.h>
#include "NILoader.h"
#include <utils/common/TplConvert.h>
#include <utils/geom/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NILoader::NILoader(NBNetBuilder &nb) throw()
        : myNetBuilder(nb) {}


NILoader::~NILoader() throw() {}


void
NILoader::load(OptionsCont &oc) {
    // build the projection
    if (!GeoConvHelper::init(oc)) {
        throw ProcessError("Could not build projection!");
    }
    // load types first
    NIXMLTypesHandler *handler =
        new NIXMLTypesHandler(myNetBuilder.getTypeCont());
    loadXMLType(handler, oc.getStringVector("xml-type-files"), "types");
    // try to load using different methods
    NIImporter_SUMO::loadNetwork(oc, myNetBuilder);
    NIImporter_RobocupRescue::loadNetwork(oc, myNetBuilder);
    NIImporter_OpenStreetMap::loadNetwork(oc, myNetBuilder);
    NIImporter_VISUM::loadNetwork(oc, myNetBuilder);
    NIImporter_ArcView::loadNetwork(oc, myNetBuilder);
    NIImporter_Vissim::loadNetwork(oc, myNetBuilder);
    loadDlrNavteq(oc);
    loadXML(oc);
    // check the loaded structures
    if (myNetBuilder.getNodeCont().size()==0) {
        throw ProcessError("No nodes loaded.");
    }
    if (myNetBuilder.getEdgeCont().size()==0) {
        throw ProcessError("No edges loaded.");
    }
    // report loaded structures
    WRITE_MESSAGE(" Import done:");
    if (myNetBuilder.getDistrictCont().size()>0) {
        WRITE_MESSAGE("   " + toString(myNetBuilder.getDistrictCont().size()) + " districts loaded.");
    }
    WRITE_MESSAGE("   " + toString(myNetBuilder.getNodeCont().size()) + " nodes loaded.");
    if (myNetBuilder.getTypeCont().size()>0) {
        WRITE_MESSAGE("   " + toString(myNetBuilder.getTypeCont().size()) + " types loaded.");
    }
    WRITE_MESSAGE("   " + toString(myNetBuilder.getEdgeCont().size()) + " edges loaded.");
    if (myNetBuilder.getEdgeCont().getNoEdgeSplits()>0) {
        WRITE_MESSAGE("The split of edges was performed "+ toString(myNetBuilder.getEdgeCont().getNoEdgeSplits()) + " times.");
    }
    if (GeoConvHelper::usingGeoProjection()) {
        WRITE_MESSAGE("Proj projection parameters used: '" + GeoConvHelper::getProjString() + "'.");
    }
}


/* -------------------------------------------------------------------------
 * file loading methods
 * ----------------------------------------------------------------------- */
void
NILoader::loadXML(OptionsCont &oc) {
    // load nodes
    loadXMLType(new NIXMLNodesHandler(myNetBuilder.getNodeCont(),
                                      myNetBuilder.getTLLogicCont(), oc),
                oc.getStringVector("xml-node-files"), "nodes");
    // load the edges
    loadXMLType(new NIXMLEdgesHandler(myNetBuilder.getNodeCont(),
                                      myNetBuilder.getEdgeCont(),
                                      myNetBuilder.getTypeCont(),
                                      myNetBuilder.getDistrictCont(), oc),
                oc.getStringVector("xml-edge-files"), "edges");
    // load the connections
    loadXMLType(new NIXMLConnectionsHandler(myNetBuilder.getEdgeCont()),
                oc.getStringVector("xml-connection-files"), "connections");
}


/** loads a single user-specified file */
void
NILoader::loadXMLType(SUMOSAXHandler *handler, const std::vector<std::string> &files,
                      const std::string &type) {
    // build parser
    SAX2XMLReader* parser = XMLSubSys::getSAXReader(*handler);
    std::string exceptMsg = "";
    // start the parsing
    try {
        for (std::vector<std::string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
            if (!FileHelpers::exists(*file)) {
                MsgHandler::getErrorInstance()->inform("Could not open " + type + "-file '" + *file + "'.");
                exceptMsg = "Process Error";
                continue;
            }
            handler->setFileName(*file);
            MsgHandler::getMessageInstance()->beginProcessMsg("Parsing " + type + " from '" + *file + "'...");
            parser->parse(file->c_str());
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
    } catch (const XMLException& toCatch) {
        exceptMsg = TplConvert<XMLCh>::_2str(toCatch.getMessage())
                    + "\n  The " + type  + " could not be loaded from '" + handler->getFileName() + "'.";
    } catch (const ProcessError& toCatch) {
        exceptMsg = std::string(toCatch.what()) + "\n  The " + type  + " could not be loaded from '" + handler->getFileName() + "'.";
    } catch (...) {
        exceptMsg = "The " + type  + " could not be loaded from '" + handler->getFileName() + "'.";
    }
    delete parser;
    delete handler;
    if (exceptMsg != "") {
        throw ProcessError(exceptMsg);
    }
}


bool
NILoader::useLineReader(LineReader &lr, const std::string &file,
                        LineHandler &lh) {
    // check opening
    if (!lr.setFile(file)) {
        MsgHandler::getErrorInstance()->inform("The file '" + file + "' could not be opened.");
        return false;
    }
    lr.readAll(lh);
    return true;
}


void
NILoader::loadDlrNavteq(OptionsCont &oc) {
    if (!oc.isSet("dlr-navteq")) {
        return;
    }
    LineReader lr;
    // load nodes
    std::map<std::string, Position2DVector> myGeoms;
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading nodes...");
    std::string file = oc.getString("dlr-navteq") + "_nodes_unsplitted.txt";
    NIElmar2NodesHandler handler1(myNetBuilder.getNodeCont(), file, myGeoms);
    if (!useLineReader(lr, file, handler1)) {
        throw ProcessError();
    }
    MsgHandler::getMessageInstance()->endProcessMsg("done.");

    // load edges
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading edges...");
    file = oc.getString("dlr-navteq") + "_links_unsplitted.txt";
    // parse the file
    NIElmar2EdgesHandler handler2(myNetBuilder.getNodeCont(),
                                  myNetBuilder.getEdgeCont(), file, myGeoms);
    if (!useLineReader(lr, file, handler2)) {
        throw ProcessError();
    }
    myNetBuilder.getEdgeCont().recheckLaneSpread();
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
}


/****************************************************************************/
