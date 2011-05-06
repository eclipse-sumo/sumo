/****************************************************************************/
/// @file    netgen_main.cpp
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
///
// Main for NETGEN
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <fstream>
#include <string>
#include <ctime>
#include <netbuild/NBNetBuilder.h>
#include <netgen/NGNet.h>
#include <netgen/NGRandomNetBuilder.h>
#include <netbuild/NBTypeCont.h>
#include <netwrite/NWFrame.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
checkOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
	bool ok = true;
    // check whether exactly one type of a network to build was wished
    int no = 0;
    if (oc.getBool("spider")) {
        no++;
    }
    if (oc.getBool("grid")) {
        no++;
    }
    if (oc.getBool("rand")) no++;
    if (no==0) {
        MsgHandler::getErrorInstance()->inform("You have to specify the type of network to generate.");
        ok = false;
    }
    if (no>1) {
        MsgHandler::getErrorInstance()->inform("You may specify only one type of network to generate at once.");
        ok = false;
    }
    // check whether the junction type to use is properly set
    if (oc.isSet("default-junction-type")) {
        std::string type = oc.getString("default-junction-type");
        if (type != toString(NODETYPE_TRAFFIC_LIGHT) &&
                type != toString(NODETYPE_PRIORITY_JUNCTION) &&
                type != toString(NODETYPE_RIGHT_BEFORE_LEFT)) {
            MsgHandler::getErrorInstance()->inform("Only the following junction types are known: " +
                    toString(NODETYPE_TRAFFIC_LIGHT) + ", " +
                    toString(NODETYPE_PRIORITY_JUNCTION) + ", " +
                    toString(NODETYPE_RIGHT_BEFORE_LEFT));
            ok = false;
        }
    }
    // check whether the output is valid and can be build
    if (!oc.isSet("output-file")) {
        MsgHandler::getErrorInstance()->inform("No output specified.");
        ok = false;
    }
    // check netwrite options
	ok &= NWFrame::checkOptions();
    return ok;
}


/** build and retrieve the options (settings) */
void
fillOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>");
    oc.addCallExample("--grid [grid-network options] -o <OUTPUTFILE>");
    oc.addCallExample("--spider [spider-network options] -o <OUTPUTFILE>");
    oc.addCallExample("--rand [random-network options] -o <OUTPUTFILE>");

    oc.setAdditionalHelpMessage(" Either \"--grid\", \"--spider\" or \"--rand\" must be supplied.\n  In dependance to these switches other options are used.");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc);
    oc.addOptionSubTopic("Grid Network");
    oc.addOptionSubTopic("Spider Network");
    oc.addOptionSubTopic("Random Network");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("TLS Building");
    oc.addOptionSubTopic("Ramp Guessing");
    oc.addOptionSubTopic("Edge Removal");
    oc.addOptionSubTopic("Unregulated Nodes");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Building Defaults");
    SystemFrame::addReportOptions(oc);


    // register the options
    //  register grid-net options
    oc.doRegister("grid", 'g', new Option_Bool(false));
    oc.addSynonyme("grid", "grid-net", true);
    oc.addDescription("grid", "Grid Network", "Forces NETGEN to build a grid-like network");

    oc.doRegister("grid.number", new Option_Integer(5));
    oc.addSynonyme("grid.number", "grid-number", true);
    oc.addSynonyme("grid.number", "number");
    oc.addDescription("grid.number", "Grid Network", "The number of junctions in both dirs");

    oc.doRegister("grid.length", new Option_Float(100));
    oc.addSynonyme("grid.length", "grid-length", true);
    oc.addSynonyme("grid.length", "length");
    oc.addDescription("grid.length", "Grid Network", "The length of streets in both dirs");

    oc.doRegister("grid.x-number", new Option_Integer(5));
    oc.addSynonyme("grid.x-number", "grid-x-number", true);
    oc.addSynonyme("grid.x-number", "x-no");
    oc.addDescription("grid.x-number", "Grid Network", "The number of junctions in x-dir; Overrides --grid-number");

    oc.doRegister("grid.y-number", new Option_Integer(5));
    oc.addSynonyme("grid.y-number", "grid-y-number", true);
    oc.addSynonyme("grid.y-number", "y-no");
    oc.addDescription("grid.y-number", "Grid Network", "The number of junctions in y-dir; Overrides --grid-number");

    oc.doRegister("grid.x-length", new Option_Float(100));
    oc.addSynonyme("grid.x-length", "grid-x-length", true);
    oc.addSynonyme("grid.x-length", "x-length");
    oc.addDescription("grid.x-length", "Grid Network", "The length of horizontal streets; Overrides --grid-length");

    oc.doRegister("grid.y-length", new Option_Float(100));
    oc.addSynonyme("grid.y-length", "grid-y-length", true);
    oc.addSynonyme("grid.y-length", "y-length");
    oc.addDescription("grid.y-length", "Grid Network", "The length of vertical streets; Overrides --grid-length");

    oc.doRegister("grid.attach-length", new Option_Float(0));
    oc.addSynonyme("grid.attach-length", "attach-length", true);
    oc.addDescription("grid.attach-length", "Grid Network", "The length of streets attached at the boundary; 0 means no streets are attached");


    // register spider-net options
    oc.doRegister("spider", 's', new Option_Bool(false));
    oc.addSynonyme("spider", "spider-net", true);
    oc.addDescription("spider", "Spider Network", "Forces NETGEN to build a spider-net-like network");

    oc.doRegister("spider.arm-number", new Option_Integer(13));
    oc.addSynonyme("spider.arm-number", "spider-arm-number", true);
    oc.addSynonyme("spider.arm-number", "arms");
    oc.addDescription("spider.arm-number", "Spider Network", "The number of axes within the net");

    oc.doRegister("spider.circle-number", new Option_Integer(20));
    oc.addSynonyme("spider.circle-number", "spider-circle-number", true);
    oc.addSynonyme("spider.circle-number", "circles");
    oc.addDescription("spider.circle-number", "Spider Network", "The number of circles of the net");

    oc.doRegister("spider.space-radius", new Option_Float(100));
    oc.addSynonyme("spider.space-radius", "spider-space-rad", true);
    oc.addSynonyme("spider.space-radius", "radius");
    oc.addDescription("spider.space-radius", "Spider Network", "The distances between the circles");

    oc.doRegister("spider.omit-center", new Option_Bool(false));
    oc.addSynonyme("spider.omit-center", "spider-omit-center", true);
    oc.addSynonyme("spider.omit-center", "nocenter");
    oc.addDescription("spider.omit-center", "Spider Network", "Omit the central node of the network");


    // register random-net options
    oc.doRegister("rand", 'r', new Option_Bool(false));
    oc.addSynonyme("rand", "random-net", true);
    oc.addDescription("rand", "Random Network", "Forces NETGEN to build a random network");

    oc.doRegister("rand.iterations", new Option_Integer(2000));
    oc.addSynonyme("rand.iterations", "rand-iterations", true);
    oc.addSynonyme("rand.iterations", "iterations");
    oc.addDescription("rand.iterations", "Random Network", "Describes how many times an edge shall be added to the net");

    oc.doRegister("rand.bidi-probability", new Option_Float(1));
    oc.addSynonyme("rand.bidi-probability", "rand-bidi-probability", true);
    oc.addSynonyme("rand.bidi-probability", "bidi");
    oc.addDescription("rand.bidi-probability", "Random Network", "Defines the probability to build a reverse edge");

    oc.doRegister("rand.max-distance", new Option_Float(250));
    oc.addSynonyme("rand.max-distance", "rand-max-distance", true);
    oc.addSynonyme("rand.max-distance", "max-dist");
    oc.addDescription("rand.max-distance", "Random Network", "");

    oc.doRegister("rand.min-distance", new Option_Float(100));
    oc.addSynonyme("rand.min-distance", "rand-min-distance", true);
    oc.addSynonyme("rand.min-distance", "min-dist");
    oc.addDescription("rand.min-distance", "Random Network", "");

    oc.doRegister("rand.min-angle", new Option_Float((SUMOReal)(45.0/180.0*PI)));
    oc.addSynonyme("rand.min-angle", "rand-min-anglee", true);
    oc.addSynonyme("rand.min-angle", "min-angle");
    oc.addDescription("rand.min-angle", "Random Network", "");

    oc.doRegister("rand.num-tries", new Option_Integer(50));
    oc.addSynonyme("rand.num-tries", "rand-num-tries", true);
    oc.addSynonyme("rand.num-tries", "num-tries");
    oc.addDescription("rand.num-tries", "Random Network", "");

    oc.doRegister("rand.connectivity", new Option_Float((SUMOReal) 0.95));
    oc.addSynonyme("rand.connectivity", "rand-connectivity", true);
    oc.addSynonyme("rand.connectivity", "connectivity");
    oc.addDescription("rand.connectivity", "Random Network", "");

    oc.doRegister("rand.neighbor-dist1", new Option_Float(0));
    oc.addSynonyme("rand.neighbor-dist1", "rand-neighbor-dist1", true);
    oc.addSynonyme("rand.neighbor-dist1", "dist1");
    oc.addDescription("rand.neighbor-dist1", "Random Network", "");

    oc.doRegister("rand.neighbor-dist2", new Option_Float(0));
    oc.addSynonyme("rand.neighbor-dist2", "rand-neighbor-dist2", true);
    oc.addSynonyme("rand.neighbor-dist2", "dist2");
    oc.addDescription("rand.neighbor-dist2", "Random Network", "");

    oc.doRegister("rand.neighbor-dist3", new Option_Float(10));
    oc.addSynonyme("rand.neighbor-dist3", "rand-neighbor-dist3", true);
    oc.addSynonyme("rand.neighbor-dist3", "dist3");
    oc.addDescription("rand.neighbor-dist3", "Random Network", "");

    oc.doRegister("rand.neighbor-dist4", new Option_Float(10));
    oc.addSynonyme("rand.neighbor-dist4", "rand-neighbor-dist4", true);
    oc.addSynonyme("rand.neighbor-dist4", "dist4");
    oc.addDescription("rand.neighbor-dist4", "Random Network", "");

    oc.doRegister("rand.neighbor-dist5", new Option_Float(2));
    oc.addSynonyme("rand.neighbor-dist5", "rand-neighbor-dist5", true);
    oc.addSynonyme("rand.neighbor-dist5", "dist5");
    oc.addDescription("rand.neighbor-dist5", "Random Network", "");

    oc.doRegister("rand.neighbor-dist6", new Option_Float(1));
    oc.addSynonyme("rand.neighbor-dist6", "rand-neighbor-dist6", true);
    oc.addSynonyme("rand.neighbor-dist6", "dist6");
    oc.addDescription("rand.neighbor-dist6", "Random Network", "");


    // add netbuilding options
    NBNetBuilder::insertNetBuildOptions(oc);
    // add netwriting options
	NWFrame::fillOptions();
    // register building options
    oc.doRegister("default-junction-type", 'j', new Option_String());
    oc.addSynonyme("default-junction-type", "junctions");
    oc.addDescription("default-junction-type", "Building Defaults", "[traffic_light|priority|right_before_left] Determines the type of the build junctions");


    // add rand options
    RandHelper::insertRandOptions();
}


NGNet *
buildNetwork(NBNetBuilder &nb) {
    OptionsCont &oc = OptionsCont::getOptions();
    // spider-net
    if (oc.getBool("spider")) {
        // check values
        bool hadError = false;
        if (oc.getInt("spider.arm-number") < 3) {
            MsgHandler::getErrorInstance()->inform("Spider networks need at least 3 arms.");
            hadError = true;
        }
        if (oc.getInt("spider.circle-number") < 1) {
            MsgHandler::getErrorInstance()->inform("Spider networks need at least one circle.");
            hadError = true;
        }
        if (oc.getFloat("spider.space-radius") < 10) {
            MsgHandler::getErrorInstance()->inform("The radius of spider networks must be at least 10m.");
            hadError = true;
        }
        if (hadError) {
            throw ProcessError();
        }
        // build if everything's ok
        NGNet *net = new NGNet(nb);
        net->createSpiderWeb(oc.getInt("spider.arm-number"), oc.getInt("spider.circle-number"),
            oc.getFloat("spider.space-radius"), !oc.getBool("spider.omit-center"));
        return net;
    }
    // grid-net
    if (oc.getBool("grid")) {
        // get options
        int xNo = oc.getInt("grid.x-number");
        int yNo = oc.getInt("grid.y-number");
        SUMOReal xLength = oc.getFloat("grid.x-length");
        SUMOReal yLength = oc.getFloat("grid.y-length");
        SUMOReal attachLength = oc.getFloat("grid.attach-length");
        if (oc.isDefault("grid.x-number")&&!oc.isDefault("grid.number")) {
            xNo = oc.getInt("grid.number");
        }
        if (oc.isDefault("grid.y-number")&&!oc.isDefault("grid.number")) {
            yNo = oc.getInt("grid.number");
        }
        if (oc.isDefault("grid.x-length")&&!oc.isDefault("grid.length")) {
            xLength = oc.getFloat("grid.length");
        }
        if (oc.isDefault("grid.y-length")&&!oc.isDefault("grid.length")) {
            yLength = oc.getFloat("grid.length");
        }
        // check values
        bool hadError = false;
        if (xNo<2 || yNo<2) {
            MsgHandler::getErrorInstance()->inform("The number of nodes must be at least 2 in both directions.");
            hadError = true;
        }
        if (xLength<10. || yLength<10.) {
            MsgHandler::getErrorInstance()->inform("The distance between nodes must be at least 10m in both directions.");
            hadError = true;
        }
        if (attachLength != 0.0 && attachLength<10.) {
            MsgHandler::getErrorInstance()->inform("The length of attached streets must be at least 10m.");
            hadError = true;
        }
        if (hadError) {
            throw ProcessError();
        }
        // build if everything's ok
        NGNet *net = new NGNet(nb);
        net->createChequerBoard(xNo, yNo, xLength, yLength, attachLength);
        return net;
    }
    // random net
    TNeighbourDistribution neighborDist;
    neighborDist.add(1, oc.getFloat("rand.neighbor-dist1"));
    neighborDist.add(2, oc.getFloat("rand.neighbor-dist2"));
    neighborDist.add(3, oc.getFloat("rand.neighbor-dist3"));
    neighborDist.add(4, oc.getFloat("rand.neighbor-dist4"));
    neighborDist.add(5, oc.getFloat("rand.neighbor-dist5"));
    neighborDist.add(6, oc.getFloat("rand.neighbor-dist6"));
    NGNet *net = new NGNet(nb);
    NGRandomNetBuilder randomNet(*net,
                                 oc.getFloat("rand.min-angle"),
                                 oc.getFloat("rand.min-distance"),
                                 oc.getFloat("rand.max-distance"),
                                 oc.getFloat("rand.connectivity"),
                                 oc.getInt("rand.num-tries"),
                                 neighborDist);
    randomNet.createNet(oc.getInt("rand.iterations"));
    return net;
}



int
main(int argc, char **argv) {
    OptionsCont &oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Road network generator for the microscopic road traffic simulation SUMO.");
    oc.setApplicationName("netgen", "SUMO netgen Version " + (std::string)VERSION_STRING);
    int ret = 0;
    try {
        // initialise the application system (messaging, xml, options)
        XMLSubSys::init(false);
        fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        MsgHandler::initOutputOptions();
        if (!checkOptions()) throw ProcessError();
        RandHelper::initRandGlobal();
        NBNetBuilder nb;
        nb.applyOptions(oc);
        // build the netgen-network description
        NGNet *net = buildNetwork(nb);
        // ... and we have to do this...
        oc.resetWritable();
        // transfer to the netbuilding structures
        net->toNB();
        delete net;
        // report generated structures
        WRITE_MESSAGE(" Generation done;");
        WRITE_MESSAGE("   " + toString<int>(nb.getNodeCont().size()) + " nodes generated.");
        WRITE_MESSAGE("   " + toString<int>(nb.getEdgeCont().size()) + " edges generated.");
        nb.buildLoaded(oc);
		NWFrame::writeNetwork(oc, nb);
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
    OutputDevice::closeAll();
    SystemFrame::close();
    if (ret==0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}



/****************************************************************************/

