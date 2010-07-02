/****************************************************************************/
/// @file    polyconvert_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// Main for POLYCONVERT
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
fillOptions() throw() {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>");

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
    oc.doRegister("xml", new Option_FileName());
    oc.addDescription("xml", "Input", "Reads pois from FILE assuming they're coded in XML");

    // osm import
    oc.doRegister("osm-files", new Option_FileName());
    oc.addSynonyme("osm-files", "osm");
    oc.addDescription("osm-files", "Input", "Reads pois from FILE+ assuming they're coded in OSM");
    oc.doRegister("osm.keep-full-type", new Option_Bool(false));
    oc.addDescription("osm.keep-full-type", "Input", "The type will be made of the key-value - pair.");

    // arcview import
    oc.doRegister("shapefile", new Option_FileName());
    oc.addSynonyme("shapefile", "shape-files");
    oc.addSynonyme("shapefile", "shape");
    oc.addDescription("shapefile", "Input", "Reads shapes from shape-files FILE+");
    oc.doRegister("shapefile.guess-projection", new Option_Bool(false));
    oc.addSynonyme("shapefile.guess-projection", "arcview.guess-projection");
    oc.addDescription("shapefile.guess-projection", "Input", "Guesses the shapefile's projection");
    oc.doRegister("shapefile.id-name", new Option_FileName());
    oc.addDescription("shapefile.id-name", "Input", "Defines where to find the id");

    // typemap reading
    oc.doRegister("typemap", new Option_FileName());
    oc.addDescription("typemap", "Input", "Reads types from FILE");


    // output
    oc.doRegister("output", 'o', new Option_FileName("polygons.xml"));
    oc.addDescription("output", "Output", "Write generated polygons/pois to FILE");


    // prunning options
    oc.doRegister("prune.on-net", new Option_Bool(false));
    oc.addDescription("prune.on-net", "Pruning", "Enables pruning on net boundaries");

    oc.doRegister("prune.on-net.offsets", new Option_String("0;0;0;0"));
    oc.addDescription("prune.on-net.offsets", "Pruning", "Uses STR as offset definition added to the net boundaries");

    oc.doRegister("prune.boundary", new Option_String());
    oc.addDescription("prune.boundary", "Pruning", "Uses STR as pruning boundary");

    oc.doRegister("prune.ignore", new Option_String());
    oc.addDescription("prune.ignore", "Pruning", "Items in STR will be kept though out of boundary");

    oc.doRegister("remove", new Option_String(""));
    oc.addDescription("remove", "Pruning", "Items with names in STR will be removed");


    oc.doRegister("x-offset-to-apply", new Option_Float(0));
    oc.addDescription("x-offset-to-apply", "Processing", "Adds FLOAT to net x-positions");

    oc.doRegister("y-offset-to-apply", new Option_Float(0));
    oc.addDescription("y-offset-to-apply", "Processing", "Adds FLOAT to net y-positions");


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
main(int argc, char **argv) {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.setApplicationDescription("Importer of polygons and POIs for the road traffic simulation SUMO.");
    oc.setApplicationName("polyconvert", "SUMO polyconvert Version " + (std::string)VERSION_STRING);
    int ret = 0;
    try {
        // initialise subsystems
        XMLSubSys::init(false);
        fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        MsgHandler::initOutputOptions();
        // build the projection
        Boundary origNetBoundary, pruningBoundary;
        Position2D netOffset;
        std::string proj;
        PCNetProjectionLoader::loadIfSet(oc, netOffset, origNetBoundary, pruningBoundary, proj);
        if (proj != "") {
            if (oc.isDefault("proj")) {
                oc.set("proj", proj);
            }
            if (oc.isDefault("x-offset-to-apply")) {
                oc.set("x-offset-to-apply", toString(netOffset.x()));
            }
            if (oc.isDefault("y-offset-to-apply")) {
                oc.set("y-offset-to-apply", toString(netOffset.y()));
            }
        }
#ifdef HAVE_PROJ
        unsigned numProjections = oc.getBool("proj.simple") + oc.getBool("proj.utm") + oc.getBool("proj.dhdn") + (oc.getString("proj").length() > 1);
        if ((oc.isSet("osm-files") || oc.isSet("dlr-navteq-poly-files") || oc.isSet("dlr-navteq-poi-files")) && numProjections == 0) {
            oc.set("proj.utm", true);
        }
        if ((oc.isSet("dlr-navteq-poly-files") || oc.isSet("dlr-navteq-poi-files")) && oc.isDefault("proj.shift")) {
            oc.set("proj.shift", std::string("5"));
        }
#endif
        if (!GeoConvHelper::init(oc)) {
            throw ProcessError("Could not build projection!");
        }

        // check whether the input shall be pruned
        bool prune = false;
        if (oc.getBool("prune.on-net")) {
            if (!oc.isSet("net")) {
                throw ProcessError("In order to prune the input on the net, you have to supply a network.");
            }
            bool ok = true;
            // !!! no proper error handling
            Boundary offsets = GeomConvHelper::parseBoundaryReporting(oc.getString("prune.on-net.offsets"), "--prune.on-net.offsets", 0, ok);
            pruningBoundary = Boundary(
                                  pruningBoundary.xmin()+offsets.xmin(),
                                  pruningBoundary.ymin()+offsets.ymin(),
                                  pruningBoundary.xmax()+offsets.xmax(),
                                  pruningBoundary.ymax()+offsets.ymax());
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
        PCTypeMap tm;
        if (oc.isSet("typemap")) {
            PCTypeDefHandler handler(oc, tm);
            if (!XMLSubSys::runParser(handler, oc.getString("typemap"))) {
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
        // check whether any errors occured
        if (!MsgHandler::getErrorInstance()->wasInformed()) {
            // no? ok, save
            toFill.save(oc.getString("output"));
        } else {
            throw ProcessError();
        }
    } catch (ProcessError &e) {
        if (std::string(e.what())!=std::string("Process Error") && std::string(e.what())!=std::string("")) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    SystemFrame::close();
    // report about ending
    if (ret==0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}



/****************************************************************************/

