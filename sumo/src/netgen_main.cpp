/****************************************************************************/
/// @file    netgen_main.cpp
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
///
// }
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
#include <fstream>
#include <string>
#include <ctime>
#include <netbuild/NBNetBuilder.h>
#include <netgen/NGNet.h>
#include <netgen/NGRandomNet.h>
#include <netbuild/NBTypeCont.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
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
bool
checkOptions(OptionsCont &oc)
{
    // check whether the output is valid and can be build
    if (!oc.isSet("output-file")) {
        MsgHandler::getErrorInstance()->inform("No output specified.");
        return false;
    }
    std::ofstream tst(oc.getString("output-file").c_str());
    if (!tst.good()) {
        MsgHandler::getErrorInstance()->inform("The output file '" + oc.getString("output-file") + "' can not be build.");
        return false;
    }
    // check whether exactly one type of a network to build was wished
    int no = 0;
    if (oc.getBool("spider-net")) {
        no++;
        if (oc.getInt("arms") < 3) {
            MsgHandler::getErrorInstance()->inform("Spider networks need at least 3 arms.");
            return false;
        }
        if (oc.getInt("circles") < 1) {
            MsgHandler::getErrorInstance()->inform("Spider networks need at least one circle.");
            return false;
        }
    }
    if (oc.getBool("grid-net")) no++;
    if (oc.getBool("random-net")) no++;
    if (no==0) {
        MsgHandler::getErrorInstance()->inform("You have to specify the type of network to generate.");
        return false;
    }
    if (no>1) {
        MsgHandler::getErrorInstance()->inform("You may specify only one type of network to generate at once.");
        return false;
    }
    // check whether the junction type to use is properly set
    if (!oc.isDefault("default-junction-type")) {
        string type = oc.getString("default-junction-type");
        if (type!="traffic_light"&&type!="priority"&&type!="actuated"&&type!="agentbased") {
            MsgHandler::getErrorInstance()->inform("Only the following junction types are known: traffic_light, priority, actuated, agentbased");
            return false;
        }
    }
    //
    return true;
}


/** build and retrieve the options (settings) */
void
fillOptions(OptionsCont &oc)
{
    // give some application descriptions
    oc.setApplicationDescription("Road network generator for the microscopic road traffic simulation SUMO.");
#ifdef WIN32
    oc.setApplicationName("netgen.exe", "SUMO netgen Version " + (string)VERSION_STRING);
#else
    oc.setApplicationName("sumo-netgen", "SUMO netgen Version " + (string)VERSION_STRING);
#endif
    oc.addCallExample("-c <CONFIGURATION>");
    oc.addCallExample("--grid-net [grid-network options] -o <OUTPUTFILE>");
    oc.addCallExample("--spider-net [spider-network opts] -o <OUTPUTFILE>");
    oc.addCallExample("--random-net [random-network opts] -o <OUTPUTFILE>");

    oc.setAdditionalHelpMessage(" Either \"--grid-net\", \"--spider-net\" or \"--random-net\" must be \n  supplied. In dependance to these switches other options are used.");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Grid Network");
    oc.addOptionSubTopic("Spider Network");
    oc.addOptionSubTopic("Random Network");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Projection"); // !!! in netgen?
    oc.addOptionSubTopic("TLS Building");
    oc.addOptionSubTopic("Ramp Guessing");
    oc.addOptionSubTopic("Edge Removal");
    oc.addOptionSubTopic("Unregulated Nodes");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Building Defaults");
    oc.addOptionSubTopic("Report");


    // register the options
    // register grid-net options
    oc.doRegister("grid-net", 'g', new Option_Bool(false));
    oc.addSynonyme("grid-net", "grid");
    oc.addDescription("grid-net", "Grid Network", "Forces NETGEN to build a grid-like network");

    oc.doRegister("grid-number", new Option_Integer(5));
    oc.addSynonyme("grid-number", "number");
    oc.addDescription("grid-number", "Grid Network", "The number of junctions in both dirs");

    oc.doRegister("grid-length", new Option_Float(100));
    oc.addSynonyme("grid-length", "length");
    oc.addDescription("grid-length", "Grid Network", "The length of streets in both dirs");

    oc.doRegister("grid-x-number", new Option_Integer(5));
    oc.addSynonyme("grid-x-number", "x-no");
    oc.addDescription("grid-x-number", "Grid Network", "The number of junctions in x-dir; Overrides --grid-number");

    oc.doRegister("grid-y-number", new Option_Integer(5));
    oc.addSynonyme("grid-y-number", "y-no");
    oc.addDescription("grid-y-number", "Grid Network", "The number of junctions in y-dir; Overrides --grid-number");

    oc.doRegister("grid-x-length", new Option_Float(100));
    oc.addSynonyme("grid-x-length", "x-length");
    oc.addDescription("grid-x-length", "Grid Network", "The length of horizontal streets; Overrides --grid-length");

    oc.doRegister("grid-y-length", new Option_Float(100));
    oc.addSynonyme("grid-y-length", "y-length");
    oc.addDescription("grid-y-length", "Grid Network", "The length of vertical streets; Overrides --grid-length");


    // register spider-net options
    oc.doRegister("spider-net", 's', new Option_Bool(false));
    oc.addSynonyme("spider-net", "spider");
    oc.addDescription("spider-net", "Spider Network", "Forces NETGEN to build a spider-net-like network");

    oc.doRegister("spider-arm-number", new Option_Integer(13));
    oc.addSynonyme("spider-arm-number", "arms");
    oc.addDescription("spider-arm-number", "Spider Network", "The number of axes within the net");

    oc.doRegister("spider-circle-number", new Option_Integer(20));
    oc.addSynonyme("spider-circle-number", "circles");
    oc.addDescription("spider-circle-number", "Spider Network", "The number of circles of the net");

    oc.doRegister("spider-space-rad", new Option_Float(100));
    oc.addSynonyme("spider-space-rad", "radius");
    oc.addDescription("spider-space-rad", "Spider Network", "The distances between the circles");

    oc.doRegister("spider-omit-center", new Option_Bool(false));
    oc.addSynonyme("spider-omit-center", "nocenter");
    oc.addDescription("spider-omit-center", "Spider Network", "Omit the central node of the network");


    // register random-net options
    oc.doRegister("random-net", 'r', new Option_Bool(false));
    oc.addSynonyme("random-net", "random");
    oc.addDescription("random-net", "Random Network", "Forces NETGEN to build a random network");

    oc.doRegister("rand-iterations", new Option_Integer(2000));
    oc.addSynonyme("rand-iterations", "iterations");
    oc.addDescription("rand-iterations", "Random Network", "Describes how many times an edge shall be added to the net");

    oc.doRegister("rand-max-distance", new Option_Float(250));
    oc.addSynonyme("rand-max-distance", "max-dist");
    oc.addDescription("rand-max-distance", "Random Network", "");

    oc.doRegister("rand-min-distance", new Option_Float(100));
    oc.addSynonyme("rand-min-distance", "min-dist");
    oc.addDescription("rand-min-distance", "Random Network", "");

    oc.doRegister("rand-min-angle", new Option_Float((SUMOReal)(45.0/180.0*PI)));
    oc.addSynonyme("rand-min-angle", "min-angle");
    oc.addDescription("rand-min-angle", "Random Network", "");

    oc.doRegister("rand-num-tries", new Option_Integer(50));
    oc.addSynonyme("rand-num-tries", "num-tries");
    oc.addDescription("rand-num-tries", "Random Network", "");

    oc.doRegister("rand-connectivity", new Option_Float((SUMOReal) 0.95));
    oc.addSynonyme("rand-connectivity", "connectivity");
    oc.addDescription("rand-connectivity", "Random Network", "");

    oc.doRegister("rand-neighbor-dist1", new Option_Float(0));
    oc.addSynonyme("rand-neighbor-dist1", "dist1");
    oc.addDescription("rand-neighbor-dist1", "Random Network", "");

    oc.doRegister("rand-neighbor-dist2", new Option_Float(0));
    oc.addSynonyme("rand-neighbor-dist2", "dist2");
    oc.addDescription("rand-neighbor-dist2", "Random Network", "");

    oc.doRegister("rand-neighbor-dist3", new Option_Float(10));
    oc.addSynonyme("rand-neighbor-dist3", "dist3");
    oc.addDescription("rand-neighbor-dist3", "Random Network", "");

    oc.doRegister("rand-neighbor-dist4", new Option_Float(10));
    oc.addSynonyme("rand-neighbor-dist4", "dist4");
    oc.addDescription("rand-neighbor-dist4", "Random Network", "");

    oc.doRegister("rand-neighbor-dist5", new Option_Float(2));
    oc.addSynonyme("rand-neighbor-dist5", "dist5");
    oc.addDescription("rand-neighbor-dist5", "Random Network", "");

    oc.doRegister("rand-neighbor-dist6", new Option_Float(1));
    oc.addSynonyme("rand-neighbor-dist6", "dist6");
    oc.addDescription("rand-neighbor-dist6", "Random Network", "");


    // add netbuilding options
    NBNetBuilder::insertNetBuildOptions(oc);
    // register building options
    oc.doRegister("default-junction-type", 'j', new Option_String("priority"));
    oc.addSynonyme("default-junction-type", "junctions");
    oc.addDescription("default-junction-type", "Building Defaults", "[traffic_light|priority|actuated|agentbased] Determines the type of the build junctions");


    // add rand options
    RandHelper::insertRandOptions(oc);
}


TNGNet *
buildNetwork(NBNetBuilder &nb)
{
    TNGNet *net = new TNGNet(nb);
    OptionsCont &oc = OptionsSubSys::getOptions();
    // spider-net
    if (oc.getBool("spider-net")) {
        net->CreateSpiderWeb(
            oc.getInt("arms"),
            oc.getInt("circles"),
            oc.getFloat("radius"),
            !oc.getBool("nocenter"));
        return net;
    }
    // grid-net
    if (oc.getBool("grid-net")) {
        int xNo = oc.getInt("x-no");
        int yNo = oc.getInt("y-no");
        SUMOReal xLength = oc.getFloat("x-length");
        SUMOReal yLength = oc.getFloat("y-length");
        if (oc.isDefault("x-no")&&!oc.isDefault("number")) {
            xNo = oc.getInt("number");
        }
        if (oc.isDefault("y-no")&&!oc.isDefault("number")) {
            yNo = oc.getInt("number");
        }
        if (oc.isDefault("x-length")&&!oc.isDefault("length")) {
            xLength = oc.getFloat("length");
        }
        if (oc.isDefault("y-length")&&!oc.isDefault("length")) {
            yLength = oc.getFloat("length");
        }
        net->CreateChequerBoard(xNo, yNo, xLength, yLength);
        return net;
    }
    // random net
    TNGRandomNet RandomNet(net);
    RandomNet.SetMaxDistance(oc.getFloat("rand-max-distance"));
    RandomNet.SetMinDistance(oc.getFloat("rand-min-distance"));
    RandomNet.SetMinLinkAngle(oc.getFloat("min-angle"));
    RandomNet.SetNumTries(oc.getInt("num-tries"));
    RandomNet.SetConnectivity(oc.getFloat("connectivity"));
    RandomNet.NeighbourDistribution.Add(1, oc.getFloat("dist1"));
    RandomNet.NeighbourDistribution.Add(2, oc.getFloat("dist2"));
    RandomNet.NeighbourDistribution.Add(3, oc.getFloat("dist3"));
    RandomNet.NeighbourDistribution.Add(4, oc.getFloat("dist4"));
    RandomNet.NeighbourDistribution.Add(5, oc.getFloat("dist5"));
    RandomNet.NeighbourDistribution.Add(6, oc.getFloat("dist6"));
    RandomNet.CreateNet(oc.getInt("iterations"));
    return net;
}



int
main(int argc, char **argv)
{
    int ret = 0;
    try {
        // initialise the application system (messaging, xml, options)
        int init_ret = SystemFrame::init(false, argc, argv, fillOptions);
        if (init_ret<0) {
            OptionsSubSys::getOptions().printHelp(cout, init_ret == -2, init_ret == -4);
            SystemFrame::close();
            return 0;
        } else if (init_ret!=0||!checkOptions(OptionsSubSys::getOptions())) {
            throw ProcessError();
        }
        // initialise the (default) types
        GeoConvHelper::init("!", Position2D());
        OptionsCont &oc = OptionsSubSys::getOptions();
        NBNetBuilder nb;
        nb.getTypeCont().setDefaults(oc.getInt("L"), oc.getFloat("S"), oc.getInt("P"));
        // build the netgen-network description
        TNGNet *net = buildNetwork(nb);
        // ... and we have to do this...
        oc.resetWritable();
        // transfer to the netbuilding structures
        nb.preCheckOptions(oc);
        net->toNB();
        delete net;
        nb.buildLoaded();
        GeoConvHelper::close();
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
    if (ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}



/****************************************************************************/

