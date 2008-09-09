/****************************************************************************/
/// @file    polyconvert_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// Main for POLYCONVERT
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <polyconvert/PCVisum.h>
#include <polyconvert/PCVisumPoints.h>
#include <polyconvert/PCElmar.h>
#include <polyconvert/PCElmarPoints.h>
#include <polyconvert/PCXMLPoints.h>
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
fillOptions()
{
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

    oc.doRegister("elmar-points", new Option_FileName());
    oc.addDescription("elmar-points", "Input", "Reads pois from FILE assuming they're coded in Elmar-format");

    // visum import
    oc.doRegister("visum-file", new Option_FileName());
    oc.addSynonyme("visum-file", "visum");
    oc.addDescription("visum-file", "Input", "Reads polygons from FILE assuming it's a Visum-net");

    oc.doRegister("visum-points", new Option_FileName());
    oc.addDescription("visum-points", "Input", "Reads pois from FILE assuming it's a Visum-net");

    // xml import
    oc.doRegister("xml-points", new Option_FileName());
    oc.addDescription("xml-points", "Input", "Reads pois from FILE assuming they're coded in XML");

    // typemap reading
    oc.doRegister("typemap", new Option_FileName());
    oc.addDescription("typemap", "Input", "Reads types from FILE");


    // output
    oc.doRegister("output", 'o', new Option_FileName("polygons.xml"));
    oc.addDescription("output", "Output", "Write generated polygons/pois to FILE");


    // projection options
    oc.addOptionSubTopic("Projection");

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
    oc.addOptionSubTopic("Building Defaults");

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
getNamedNetworkBoundary(const std::string &file, const std::string &name)
{
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
getNetworkOrigBoundary(const std::string &file)
{
    try {
        return getNamedNetworkBoundary(file, "orig-boundary");
    } catch (ProcessError &) {}
    throw ProcessError("Could not find the original boundary in net.");
}


Boundary
getNetworkConvBoundary(const std::string &file)
{
    try {
        return getNamedNetworkBoundary(file, "conv-boundary");
    } catch (ProcessError &) {}
    throw ProcessError("Could not find the converted boundary in net.");
}


Position2D
getNetworkOffset(const std::string &file)
{
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
getOrigProj(const std::string &file)
{
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
main(int argc, char **argv)
{
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
        XMLSubSys::init();
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
        // elmar's polygons
        if (oc.isSet("elmar")) {
            PCElmar pce(toFill, tm);
            pce.load(oc);
        }
        // elmar's points
        if (oc.isSet("elmar-points")) {
            PCElmarPoints pce(toFill, tm);
            pce.load(oc);
        }
        // visum
        if (oc.isSet("visum-file")) {
            PCVisum pcv(toFill, tm);
            pcv.load(oc);
        }
        // visum-points
        if (oc.isSet("visum-points")) {
            PCVisumPoints pcv(toFill, tm);
            pcv.load(oc);
        }
        // xml-points
        if (oc.isSet("xml-points")) {
            PCXMLPoints pcv(toFill, tm, oc);
            pcv.load(oc);
        }

        // check whether any errors occured
        if (!MsgHandler::getErrorInstance()->wasInformed()) {
            // no? ok, save
            toFill.save(oc.getString("output"), oc.getInt("layer"));
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

