/****************************************************************************/
/// @file    NILoader.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Perfoms network import
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOSAXReader.h>
#include <utils/xml/XMLSubSys.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBHeightMapper.h>
#include <netbuild/NBNetBuilder.h>
#include <netimport/NIXMLEdgesHandler.h>
#include <netimport/NIXMLNodesHandler.h>
#include <netimport/NIXMLTrafficLightsHandler.h>
#include <netimport/NIXMLTypesHandler.h>
#include <netimport/NIXMLConnectionsHandler.h>
#include <netimport/NIImporter_DlrNavteq.h>
#include <netimport/NIImporter_VISUM.h>
#include <netimport/vissim/NIImporter_Vissim.h>
#include <netimport/NIImporter_ArcView.h>
#include <netimport/NIImporter_SUMO.h>
#include <netimport/NIImporter_RobocupRescue.h>
#include <netimport/NIImporter_OpenStreetMap.h>
#include <netimport/NIImporter_OpenDrive.h>
#include <netimport/NIImporter_MATSim.h>
#include <netimport/NIImporter_ITSUMO.h>
#include "typemap.h"
#include "NILoader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NILoader::NILoader(NBNetBuilder& nb)
    : myNetBuilder(nb) {}


NILoader::~NILoader() {}


void
NILoader::load(OptionsCont& oc) {
    // load types first
    NIXMLTypesHandler* handler =
        new NIXMLTypesHandler(myNetBuilder.getTypeCont());
    if (!oc.isSet("type-files")) {
        std::vector<std::string> files;
        if (oc.isSet("osm-files")) {
            files.push_back(osmTypemap);
        }
        if (oc.isSet("opendrive-files")) {
            files.push_back(opendriveTypemap);
        }
        loadXMLType(handler, files, "types", true);
    } else {
        loadXMLType(handler, oc.getStringVector("type-files"), "types");
    }
    // try to load height data so it is ready for use by other importers
    NBHeightMapper::loadIfSet(oc);
    // try to load using different methods
    NIImporter_SUMO::loadNetwork(oc, myNetBuilder);
    NIImporter_RobocupRescue::loadNetwork(oc, myNetBuilder);
    NIImporter_OpenStreetMap::loadNetwork(oc, myNetBuilder);
    NIImporter_VISUM::loadNetwork(oc, myNetBuilder);
    NIImporter_ArcView::loadNetwork(oc, myNetBuilder);
    NIImporter_Vissim::loadNetwork(oc, myNetBuilder);
    NIImporter_DlrNavteq::loadNetwork(oc, myNetBuilder);
    NIImporter_OpenDrive::loadNetwork(oc, myNetBuilder);
    NIImporter_MATSim::loadNetwork(oc, myNetBuilder);
    NIImporter_ITSUMO::loadNetwork(oc, myNetBuilder);
    if (oc.getBool("tls.discard-loaded") || oc.getBool("tls.discard-simple")) {
        myNetBuilder.getNodeCont().discardTrafficLights(myNetBuilder.getTLLogicCont(), oc.getBool("tls.discard-simple"),
                oc.getBool("tls.guess-signals"));
        int removed = myNetBuilder.getTLLogicCont().getNumExtracted();
        if (removed > 0) {
            WRITE_MESSAGE(" Removed " + toString(removed) + " traffic lights before loading plain-XML");
        }
    }
    loadXML(oc);
    // check the loaded structures
    if (myNetBuilder.getNodeCont().size() == 0) {
        throw ProcessError("No nodes loaded.");
    }
    if (myNetBuilder.getEdgeCont().size() == 0) {
        throw ProcessError("No edges loaded.");
    }
    // report loaded structures
    WRITE_MESSAGE(" Import done:");
    if (myNetBuilder.getDistrictCont().size() > 0) {
        WRITE_MESSAGE("   " + toString(myNetBuilder.getDistrictCont().size()) + " districts loaded.");
    }
    WRITE_MESSAGE("   " + toString(myNetBuilder.getNodeCont().size()) + " nodes loaded.");
    if (myNetBuilder.getTypeCont().size() > 0) {
        WRITE_MESSAGE("   " + toString(myNetBuilder.getTypeCont().size()) + " types loaded.");
    }
    WRITE_MESSAGE("   " + toString(myNetBuilder.getEdgeCont().size()) + " edges loaded.");
    if (myNetBuilder.getEdgeCont().getNoEdgeSplits() > 0) {
        WRITE_MESSAGE("The split of edges was performed " + toString(myNetBuilder.getEdgeCont().getNoEdgeSplits()) + " times.");
    }
    if (GeoConvHelper::getProcessing().usingGeoProjection()) {
        WRITE_MESSAGE("Proj projection parameters used: '" + GeoConvHelper::getProcessing().getProjString() + "'.");
    }
}


/* -------------------------------------------------------------------------
 * file loading methods
 * ----------------------------------------------------------------------- */
void
NILoader::loadXML(OptionsCont& oc) {
    // load nodes
    loadXMLType(new NIXMLNodesHandler(myNetBuilder.getNodeCont(),
                                      myNetBuilder.getTLLogicCont(), oc),
                oc.getStringVector("node-files"), "nodes");
    // load the edges
    loadXMLType(new NIXMLEdgesHandler(myNetBuilder.getNodeCont(),
                                      myNetBuilder.getEdgeCont(),
                                      myNetBuilder.getTypeCont(),
                                      myNetBuilder.getDistrictCont(),
                                      myNetBuilder.getTLLogicCont(),
                                      oc),
                oc.getStringVector("edge-files"), "edges");
    if (!deprecatedVehicleClassesSeen.empty()) {
        WRITE_WARNING("Deprecated vehicle class(es) '" + toString(deprecatedVehicleClassesSeen) + "' in input edge files.");
    }
    // load the connections
    loadXMLType(new NIXMLConnectionsHandler(myNetBuilder.getEdgeCont(), myNetBuilder.getNodeCont(), myNetBuilder.getTLLogicCont()),
                oc.getStringVector("connection-files"), "connections");
    // load traffic lights (needs to come last, references loaded edges and connections)
    loadXMLType(new NIXMLTrafficLightsHandler(
                    myNetBuilder.getTLLogicCont(), myNetBuilder.getEdgeCont()),
                oc.getStringVector("tllogic-files"), "traffic lights");
}


void
NILoader::loadXMLType(SUMOSAXHandler* handler, const std::vector<std::string>& files,
                      const std::string& type, const bool stringParse) {
    // build parser
    std::string exceptMsg = "";
    // start the parsing
    try {
        for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
            if (stringParse) {
                handler->setFileName("built in type map");
                SUMOSAXReader* reader = XMLSubSys::getSAXReader(*handler);
                reader->parseString(*file);
                delete reader;
                continue;
            }
            if (!FileHelpers::isReadable(*file)) {
                WRITE_ERROR("Could not open " + type + "-file '" + *file + "'.");
                exceptMsg = "Process Error";
                continue;
            }
            PROGRESS_BEGIN_MESSAGE("Parsing " + type + " from '" + *file + "'");
            XMLSubSys::runParser(*handler, *file);
            PROGRESS_DONE_MESSAGE();
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch) {
        exceptMsg = TplConvert::_2str(toCatch.getMessage())
                    + "\n  The " + type  + " could not be loaded from '" + handler->getFileName() + "'.";
    } catch (const ProcessError& toCatch) {
        exceptMsg = std::string(toCatch.what()) + "\n  The " + type  + " could not be loaded from '" + handler->getFileName() + "'.";
    } catch (...) {
        exceptMsg = "The " + type  + " could not be loaded from '" + handler->getFileName() + "'.";
    }
    delete handler;
    if (exceptMsg != "") {
        throw ProcessError(exceptMsg);
    }
}

/****************************************************************************/
