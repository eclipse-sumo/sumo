/****************************************************************************/
/// @file    polyconvert_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Michael Behrisch
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// Main for POLYCONVERT
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <string>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/importio/LineReader.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/geom/Boundary.h>
#include <polyconvert/PCLoaderVisum.h>
#include <polyconvert/PCLoaderDlrNavteq.h>
#include <polyconvert/PCLoaderXML.h>
#include <polyconvert/PCLoaderOSM.h>
#include <polyconvert/PCLoaderArcView.h>
#include <polyconvert/PCTypeMap.h>
#include <polyconvert/PCTypeDefHandler.h>
#include <polyconvert/PCNetProjectionLoader.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
fillOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>", "run with configuration options set in file");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    GeoConvHelper::addProjectionOptions(oc);
    oc.addOptionSubTopic("Pruning");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Building Defaults");
    SystemFrame::addReportOptions(oc); // fill this subtopic, too


    // register options
    // add i/o options
    // original network
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Loads SUMO-network FILE as reference to offset and projection");

    // dlrnavteq import
    oc.doRegister("dlr-navteq-poly-files", new Option_FileName());
    oc.addDescription("dlr-navteq-poly-files", "Input", "Reads polygons from FILE assuming they're coded in DLR-Navteq (Elmar)-format");
    oc.doRegister("dlr-navteq-poi-files", new Option_FileName());
    oc.addDescription("dlr-navteq-poi-files", "Input", "Reads pois from FILE+ assuming they're coded in DLR-Navteq (Elmar)-format");

    // visum import
    oc.doRegister("visum-files", new Option_FileName());
    oc.addSynonyme("visum-files", "visum");
    oc.addDescription("visum-files", "Input", "Reads polygons from FILE assuming it's a Visum-net");

    // xml import
    oc.doRegister("xml-files", new Option_FileName());
    oc.addSynonyme("xml-files", "xml");
    oc.addDescription("xml-files", "Input", "Reads pois and shapes from FILE assuming they're coded in XML");

    // osm import
    oc.doRegister("osm-files", new Option_FileName());
    oc.addSynonyme("osm-files", "osm");
    oc.addDescription("osm-files", "Input", "Reads pois from FILE+ assuming they're coded in OSM");
    oc.doRegister("osm.keep-full-type", new Option_Bool(false));
    oc.addDescription("osm.keep-full-type", "Input", "The type will be made of the key-value - pair.");
    oc.doRegister("osm.use-name", new Option_Bool(false));
    oc.addDescription("osm.use-name", "Input", "The id will be set from the given 'name' attribute.");

    // arcview import
    oc.doRegister("shapefile-prefixes", new Option_FileName());
    oc.addSynonyme("shapefile-prefixes", "shapefile-prefix");
    oc.addSynonyme("shapefile-prefixes", "shapefile");
    oc.addSynonyme("shapefile-prefixes", "shape-files", true);
    oc.addDescription("shapefile-prefixes", "Input", "Reads shapes from shapefiles FILE+");

    oc.doRegister("shapefile.guess-projection", new Option_Bool(false));
    oc.addSynonyme("shapefile.guess-projection", "arcview.guess-projection", true);
    oc.addDescription("shapefile.guess-projection", "Input", "Guesses the shapefile's projection");

    oc.doRegister("shapefile.id-column", new Option_String());
    oc.addSynonyme("shapefile.id-column", "shapefile.id-name", true);
    oc.addSynonyme("shapefile.id-column", "shape-files.id-name", true);
    oc.addDescription("shapefile.id-column", "Input", "Defines in which column the id can be found");

    oc.doRegister("shapefile.use-running-id", new Option_Bool());
    oc.addDescription("shapefile.use-running-id", "Input", "A running number will be used as id.");

    // typemap reading
    oc.doRegister("type-file", new Option_FileName());
    oc.addSynonyme("type-file", "typemap", true);
    oc.addDescription("type-file", "Input", "Reads types from FILE");


    // output
    oc.doRegister("output-file", 'o', new Option_FileName("polygons.xml"));
    oc.addSynonyme("output-file", "output");
    oc.addDescription("output-file", "Output", "Write generated polygons/pois to FILE");


    // prunning options
    oc.doRegister("prune.in-net", new Option_Bool(false));
    oc.addSynonyme("prune.in-net", "prune.on-net", true);
    oc.addDescription("prune.in-net", "Pruning", "Enables pruning on net boundaries");

    oc.doRegister("prune.in-net.offsets", new Option_String("0,0,0,0"));
    oc.addSynonyme("prune.in-net.offsets", "prune.on-net.offsets", true);
    oc.addDescription("prune.in-net.offsets", "Pruning", "Uses STR as offset definition added to the net boundaries");

    oc.doRegister("prune.boundary", new Option_String());
    oc.addDescription("prune.boundary", "Pruning", "Uses STR as pruning boundary");

    oc.doRegister("prune.keep-list", new Option_String());
    oc.addSynonyme("prune.keep-list", "prune.keep");
    oc.addSynonyme("prune.keep-list", "prune.ignore", true);
    oc.addDescription("prune.keep-list", "Pruning", "Items in STR will be kept though out of boundary");

    oc.doRegister("prune.explicit", new Option_String(""));
    oc.addSynonyme("prune.explicit", "remove");
    oc.addDescription("prune.explicit", "Pruning", "Items with names in STR will be removed");


    oc.doRegister("offset.x", new Option_Float(0));
    oc.addSynonyme("offset.x", "x-offset-to-apply", true);
    oc.addDescription("offset.x", "Processing", "Adds FLOAT to net x-positions");

    oc.doRegister("offset.y", new Option_Float(0));
    oc.addSynonyme("offset.y", "y-offset-to-apply", true);
    oc.addDescription("offset.y", "Processing", "Adds FLOAT to net y-positions");

    oc.doRegister("all-attributes", new Option_Bool(false));
    oc.addDescription("all-attributes", "Processing", "Imports all attributes as key/value pairs");

    oc.doRegister("ignore-errors", new Option_Bool(false));
    oc.addDescription("ignore-errors", "Processing", "Continue on broken input");


    // building defaults options
    oc.doRegister("color", new Option_String("0.2,0.5,1."));
    oc.addDescription("color", "Building Defaults", "Sets STR as default color");

    oc.doRegister("prefix", new Option_String(""));
    oc.addDescription("prefix", "Building Defaults", "Sets STR as default prefix");

    oc.doRegister("type", new Option_String("unknown"));
    oc.addDescription("type", "Building Defaults", "Sets STR as default type");

    oc.doRegister("layer", new Option_Integer(-1));
    oc.addDescription("layer", "Building Defaults", "Sets INT as default layer");

    oc.doRegister("discard", new Option_Bool(false));
    oc.addDescription("discard", "Building Defaults", "Sets default action to discard");
}


int
main(int argc, char** argv) {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.setApplicationDescription("Importer of polygons and POIs for the road traffic simulation SUMO.");
    oc.setApplicationName("polyconvert", "SUMO polyconvert Version " + (std::string)VERSION_STRING);
    int ret = 0;
    try {
        // initialise subsystems
        XMLSubSys::init();
        fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        XMLSubSys::setValidation(oc.getBool("xml-validation"));
        MsgHandler::initOutputOptions();
        // build the projection
        int shift = 0;
        if ((oc.isSet("dlr-navteq-poly-files") || oc.isSet("dlr-navteq-poi-files")) && oc.isDefault("proj.scale")) {
            shift = 5;
        }
        if (!oc.isSet("net")) {
            // from the given options
#ifdef HAVE_PROJ
            unsigned numProjections = oc.getBool("simple-projection") + oc.getBool("proj.utm") + oc.getBool("proj.dhdn") + (oc.getString("proj").length() > 1);
            if ((oc.isSet("osm-files") || oc.isSet("dlr-navteq-poly-files") || oc.isSet("dlr-navteq-poi-files")) && numProjections == 0) {
                oc.set("proj.utm", "true");
            }
            oc.set("proj.scale", toString(shift));
#endif
            if (!GeoConvHelper::init(oc)) {
                throw ProcessError("Could not build projection!");
            }
        } else {
            // from the supplied network
            // @todo warn about given options being ignored
            PCNetProjectionLoader::load(oc.getString("net"), shift);
        }
        Boundary pruningBoundary = GeoConvHelper::getFinal().getConvBoundary();
        // check whether the input shall be pruned
        bool prune = false;
        if (oc.getBool("prune.in-net")) {
            if (!oc.isSet("net")) {
                throw ProcessError("In order to prune the input on the net, you have to supply a network.");
            }
            bool ok = true;
            // !!! no proper error handling
            Boundary offsets = GeomConvHelper::parseBoundaryReporting(oc.getString("prune.in-net.offsets"), "--prune.on-net.offsets", 0, ok);
            pruningBoundary = Boundary(
                                  pruningBoundary.xmin() + offsets.xmin(),
                                  pruningBoundary.ymin() + offsets.ymin(),
                                  pruningBoundary.xmax() + offsets.xmax(),
                                  pruningBoundary.ymax() + offsets.ymax());
            prune = true;
        }
        if (oc.isSet("prune.boundary")) {
            bool ok = true;
            // !!! no proper error handling
            pruningBoundary = GeomConvHelper::parseBoundaryReporting(oc.getString("prune.boundary"), "--prune.boundary", 0, ok);
            prune = true;
        }

        PCPolyContainer toFill(prune, pruningBoundary, oc.getStringVector("remove"));

        // read in the type defaults
        PCTypeMap tm(oc);
        if (oc.isSet("type-file")) {
            PCTypeDefHandler handler(oc, tm);
            if (!XMLSubSys::runParser(handler, oc.getString("type-file"))) {
                // something failed
                throw ProcessError();
            }
        }

        // read in the data
        PCLoaderXML::loadIfSet(oc, toFill, tm); // SUMO-XML
        PCLoaderOSM::loadIfSet(oc, toFill, tm); // OSM-XML
        PCLoaderDlrNavteq::loadIfSet(oc, toFill, tm); // Elmar-files
        PCLoaderVisum::loadIfSet(oc, toFill, tm); // VISUM
        PCLoaderArcView::loadIfSet(oc, toFill, tm); // shape-files
        // error processing
        if (MsgHandler::getErrorInstance()->wasInformed() && oc.getBool("ignore-errors")) {
            MsgHandler::getErrorInstance()->clear();
        }
        if (!MsgHandler::getErrorInstance()->wasInformed()) {
            // no? ok, save
            toFill.save(oc.getString("output-file"));
        } else {
            throw ProcessError();
        }
    } catch (const ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (const std::exception& e) {
        if (std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    SystemFrame::close();
    // report about ending
    if (ret == 0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}



/****************************************************************************/

