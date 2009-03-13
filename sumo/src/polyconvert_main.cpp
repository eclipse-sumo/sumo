/****************************************************************************/
/// @file    polyconvert_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// Main for POLYCONVERT
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
#include <utils/importio/LineReader.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/geom/Boundary.h>
#include <polyconvert/PCLoaderVisum.h>
#include <polyconvert/PCLoaderElmar.h>
#include <polyconvert/PCLoaderXML.h>
#include <polyconvert/PCLoaderOSM.h>
#include <polyconvert/PCLoaderArcView.h>
#include <polyconvert/PCTypeMap.h>
#include <polyconvert/PCTypeDefHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/RandHelper.h>
#include <utils/geom/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


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
    oc.addOptionSubTopic("Projection");
    oc.addOptionSubTopic("Prunning");
    oc.addOptionSubTopic("Building Defaults");
    oc.addOptionSubTopic("Report");


    // register options
    // add i/o options
    // original network
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Loads SUMO-network FILE as reference to offset and projection");

    // elmar import
    oc.doRegister("elmar", new Option_FileName());
    oc.addDescription("elmar", "Input", "Reads polygons from FILE assuming they're coded in Elmar-format");
    oc.doRegister("elmar-poi-files", new Option_FileName());
    oc.addDescription("elmar-poi-files", "Input", "Reads pois from FILE+ assuming they're coded in Elmar-format");

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
    oc.doRegister("shape-file", new Option_FileName());
    oc.addSynonyme("shape-file", "shape");
    oc.addDescription("shape-file", "Input", "Reads shapes from shape-files FILE+");
    oc.doRegister("arcview.guess-projection", new Option_Bool(false));
    oc.addDescription("arcview.guess-projection", "Input", "Guesses the shapefile's projection");
    oc.doRegister("shape-file.id-name", new Option_FileName());
    oc.addDescription("shape-file.id-name", "Input", "Defines where to find the id");

    // typemap reading
    oc.doRegister("typemap", new Option_FileName());
    oc.addDescription("typemap", "Input", "Reads types from FILE");


    // output
    oc.doRegister("output", 'o', new Option_FileName("polygons.xml"));
    oc.addDescription("output", "Output", "Write generated polygons/pois to FILE");


    // projection options
    oc.doRegister("use-projection", new Option_Bool(false));
    oc.addDescription("use-projection", "Projection", "Enables reprojection from geo to cartesian");

    oc.doRegister("proj.simple", new Option_Bool(false));
    oc.addDescription("proj.simple", "Projection", "Uses a simple method for projection");

    oc.doRegister("proj", new Option_String());
    oc.addDescription("proj", "Projection", "Uses STR as proj.4 definition for projection");

    oc.doRegister("proj.inverse", new Option_Bool(false));
    oc.addDescription("proj.inverse", "Projection", "Inverses projection");


    // prunning options
    oc.addOptionSubTopic("Prunning");

    oc.doRegister("prune.on-net", new Option_Bool(false));
    oc.addDescription("prune.on-net", "Prunning", "Enables prunning on net boundaries");

    oc.doRegister("prune.on-net.offsets", new Option_String("0;0;0;0"));
    oc.addDescription("prune.on-net.offsets", "Prunning", "Uses STR as offset definition added to the net boundaries");

    oc.doRegister("prune.boundary", new Option_String());
    oc.addDescription("prune.boundary", "Prunning", "Uses STR as prunning boundary");

    oc.doRegister("prune.ignore", new Option_String());
    oc.addDescription("prune.ignore", "Prunning", "Items in STR will be kept though out of boundary");

    oc.doRegister("remove", new Option_String(""));
    oc.addDescription("remove", "Prunning", "Items with names in STR will be removed");


    // building defaults options
    oc.doRegister("color", new Option_String("0.2,0.5,1."));
    oc.addDescription("color", "Building Defaults", "Sets STR as default color");

    oc.doRegister("prefix", new Option_String(""));
    oc.addDescription("prefix", "Building Defaults", "Sets STR as default prefix");

    oc.doRegister("type", new Option_String("unknown"));
    oc.addDescription("type", "Building Defaults", "Sets STR as default type");

    oc.doRegister("layer", new Option_Integer(-1));
    oc.addDescription("layer", "Building Defaults", "Sets INT as default layer");


    // register report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.addDescription("verbose", "Report", "Switches to verbose output");

    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.addDescription("suppress-warnings", "Report", "Disables output of warnings");

    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.addDescription("print-options", "Report", "Prints option values before processing");

    oc.doRegister("help", '?', new Option_Bool(false));
    oc.addDescription("help", "Report", "Prints this screen");

    oc.doRegister("log-file", 'l', new Option_FileName());
    oc.addDescription("log-file", "Report", "Writes all messages to FILE");


    // random initialisation (not used!!!)
    RandHelper::insertRandOptions();
}


Boundary
getNamedNetworkBoundary(const std::string &file, const std::string &name) throw(ProcessError) {
    LineReader lr(file);
    if (!lr.good()) {
        throw ProcessError("Could not open net '" + file + "'.");
    }
    while (lr.hasMore()) {
        string line = lr.readLine();
        if (line.find("<" + name + ">")!=string::npos) {
            size_t beg = line.find('>');
            size_t end = line.find('<', beg);
            string my = line.substr(beg+1, end-beg-1);
            return GeomConvHelper::parseBoundary(my);
        }
    }
    throw ProcessError();
}


Boundary
getNetworkOrigBoundary(const std::string &file) throw(ProcessError) {
    try {
        return getNamedNetworkBoundary(file, "orig-boundary");
    } catch (ProcessError &) {}
    throw ProcessError("Could not find the original boundary in net.");
}


Boundary
getNetworkConvBoundary(const std::string &file) throw(ProcessError) {
    try {
        return getNamedNetworkBoundary(file, "conv-boundary");
    } catch (ProcessError &) {}
    throw ProcessError("Could not find the converted boundary in net.");
}


Position2D
getNetworkOffset(const std::string &file) throw(ProcessError) {
    LineReader lr(file);
    if (!lr.good()) {
        throw ProcessError("Could not open net '" + file + "'.");
    }
    while (lr.hasMore()) {
        string line = lr.readLine();
        if (line.find("<net-offset>")!=string::npos) {
            size_t beg = line.find('>');
            size_t end = line.find('<', beg);
            string my = line.substr(beg+1, end-beg-1);
            return GeomConvHelper::parseShape(my)[0];
        }
    }
    throw ProcessError("Could not find projection description in net.");
}


std::string
getOrigProj(const std::string &file) throw(ProcessError) {
    LineReader lr(file);
    if (!lr.good()) {
        throw ProcessError("Could not open net '" + file + "'.");
    }
    while (lr.hasMore()) {
        string line = lr.readLine();
        if (line.find("<orig-proj>")!=string::npos) {
            size_t beg = line.find('>');
            size_t end = line.find('<', beg);
            return line.substr(beg+1, end-beg-1);
        }
    }
    throw ProcessError("Could not find projection description in net.");
}


int
main(int argc, char **argv) {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.setApplicationDescription("Importer of polygons and POIs for the road traffic simulation SUMO.");
#ifdef WIN32
    oc.setApplicationName("polyconvert.exe", "SUMO polyconvert Version " + (string)VERSION_STRING);
#else
    oc.setApplicationName("sumo-polyconvert", "SUMO polyconvert Version " + (string)VERSION_STRING);
#endif
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
        RandHelper::initRandGlobal();
        // build the projection
        Boundary origNetBoundary;
        Position2D netOffset;
        string proj;
        if (!oc.getBool("use-projection")) {
            GeoConvHelper::init("!", Position2D());
        } else if (oc.getBool("proj.simple")) {
            GeoConvHelper::init("-", Position2D());
        } else {
            if (oc.isSet("net")) {
                origNetBoundary = getNetworkOrigBoundary(oc.getString("net"));
                netOffset = getNetworkOffset(oc.getString("net"));
                proj = getOrigProj(oc.getString("net"));
            }
            if (oc.isSet("proj")) {
                proj = oc.getString("proj");
            }
            if (!GeoConvHelper::init(proj, netOffset, oc.getBool("proj.inverse"))) {
                throw ProcessError("Could not build projection!");
            }
        }

        // check whether the input shall be prunned
        bool prune = false;
        Boundary prunningBoundary;
        if (oc.getBool("prune.on-net")) {
            if (!oc.isSet("net")) {
                throw ProcessError("In order to prune the input on the net, you have to supply a network.");
            }
            prunningBoundary = getNetworkConvBoundary(oc.getString("net"));
            Boundary offsets = GeomConvHelper::parseBoundary(oc.getString("prune.on-net.offsets"));
            prunningBoundary = Boundary(
                                   prunningBoundary.xmin()+offsets.xmin(),
                                   prunningBoundary.ymin()+offsets.ymin(),
                                   prunningBoundary.xmax()+offsets.xmax(),
                                   prunningBoundary.ymax()+offsets.ymax());
            prune = true;
        }
        if (oc.isSet("prune.boundary")) {
            prunningBoundary = GeomConvHelper::parseBoundary(oc.getString("prune.boundary"));
            prune = true;
        }

        PCPolyContainer toFill(prune, prunningBoundary, oc.getStringVector("remove"));

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
        PCLoaderElmar::loadIfSet(oc, toFill, tm); // Elmar-files
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
        if (string(e.what())!=string("Process Error") && string(e.what())!=string("")) {
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
        cout << "Success." << endl;
    }
    return ret;
}



/****************************************************************************/

