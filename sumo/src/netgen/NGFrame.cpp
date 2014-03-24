/****************************************************************************/
/// @file    NGFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    06.05.2011
/// @version $Id$
///
// Sets and checks options for netgen
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2011-2014 DLR (http://www.dlr.de/) and contributors
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
#include "NGFrame.h"
#include <netbuild/NBNetBuilder.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/ToString.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
NGFrame::fillOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
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


    //  register spider-net options
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


    //  register random-net options
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

    oc.doRegister("rand.min-angle", new Option_Float(DEG2RAD(45.0)));
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
}


bool
NGFrame::checkOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    bool ok = true;
    // check whether exactly one type of a network to build was wished
    int no = 0;
    if (oc.getBool("spider")) {
        no++;
    }
    if (oc.getBool("grid")) {
        no++;
    }
    if (oc.getBool("rand")) {
        no++;
    }
    if (no == 0) {
        WRITE_ERROR("You have to specify the type of network to generate.");
        ok = false;
    }
    if (no > 1) {
        WRITE_ERROR("You may specify only one type of network to generate at once.");
        ok = false;
    }
    // check whether the junction type to use is properly set
    if (oc.isSet("default-junction-type")) {
        std::string type = oc.getString("default-junction-type");
        if (type != toString(NODETYPE_TRAFFIC_LIGHT) &&
                type != toString(NODETYPE_TRAFFIC_LIGHT_NOJUNCTION) &&
                type != toString(NODETYPE_PRIORITY) &&
                type != toString(NODETYPE_PRIORITY_STOP) &&
                type != toString(NODETYPE_ALLWAY_STOP) &&
                type != toString(NODETYPE_RIGHT_BEFORE_LEFT)) {
            WRITE_ERROR("Only the following junction types are known: " +
                        toString(NODETYPE_TRAFFIC_LIGHT) + ", " +
                        toString(NODETYPE_TRAFFIC_LIGHT_NOJUNCTION) + ", " +
                        toString(NODETYPE_PRIORITY) + ", " +
                        toString(NODETYPE_PRIORITY_STOP) + ", " +
                        toString(NODETYPE_ALLWAY_STOP) + ", " +
                        toString(NODETYPE_RIGHT_BEFORE_LEFT));
            ok = false;
        }
    }
    return ok;
}


/****************************************************************************/

