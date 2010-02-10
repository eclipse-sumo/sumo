/****************************************************************************/
/// @file    NIOptionsIO.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Sets and checks options for netimport
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

#include <string>
#include <iostream>
#include <fstream>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NIOptionsIO.h"
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/common/SystemFrame.h>
#include <utils/geom/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
NIOptionsIO::fillOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>");
    oc.addCallExample("-n ./nodes.xml -e ./edges.xml -v -t ./owntypes.xml");


    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    GeoConvHelper::addProjectionOptions(oc);
    oc.addOptionSubTopic("TLS Building");
    oc.addOptionSubTopic("Ramp Guessing");
    oc.addOptionSubTopic("Edge Removal");
    oc.addOptionSubTopic("Unregulated Nodes");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Building Defaults");
    SystemFrame::addReportOptions(oc); // fill this subtopic, too

    // register options
    // register I/O options
    oc.doRegister("sumo-net", 's', new Option_FileName());
    oc.addDescription("sumo-net", "Input", "Read SUMO-net from FILE");

    oc.doRegister("xml-node-files", 'n', new Option_FileName());
    oc.addSynonyme("xml-node-files", "xml-nodes");
    oc.addDescription("xml-node-files", "Input", "Read XML-node defs from FILE");

    oc.doRegister("xml-edge-files", 'e', new Option_FileName());
    oc.addSynonyme("xml-edge-files", "xml-edges");
    oc.addDescription("xml-edge-files", "Input", "Read XML-edge defs from FILE");

    oc.doRegister("xml-connection-files", 'x', new Option_FileName());
    oc.addSynonyme("xml-connection-files", "xml-connections");
    oc.addDescription("xml-connection-files", "Input", "Read XML-connection defs from FILE");

    oc.doRegister("xml-type-files", 't', new Option_FileName());
    oc.addSynonyme("xml-type-files", "xml-types");
    oc.addDescription("xml-type-files", "Input", "Read XML-type defs from FILE");

    oc.doRegister("shapefile", new Option_FileName());
    oc.addSynonyme("shapefile", "arcview");
    oc.addSynonyme("shapefile", "tiger");
    oc.addDescription("shapefile", "Input", "Read shapefiles (ArcView, Tiger, ...) from files starting with 'FILE'");

    oc.doRegister("dlr-navteq", new Option_FileName());
    oc.addSynonyme("dlr-navteq", "elmar2");
    oc.addDescription("dlr-navteq", "Input", "Read converted Navteq GDF data (unsplitted Elmar-network) from path 'FILE'");

    oc.doRegister("osm-files", new Option_FileName());
    oc.addSynonyme("osm-files", "osm");
    oc.addDescription("osm-files", "Input", "Read OSM-network from path 'FILE(s)'");

    oc.doRegister("opendrive", new Option_FileName());
    oc.addDescription("opendrive", "Input", "Read openDrive-network from FILE");

    oc.doRegister("visum-file", new Option_FileName());
    oc.addSynonyme("visum-file", "visum");
    oc.addDescription("visum-file", "Input", "Read VISUM-net from FILE");

    oc.doRegister("vissim-file", new Option_FileName());
    oc.addSynonyme("vissim-file", "vissim");
    oc.addDescription("vissim-file", "Input", "Read VISSIM-net from FILE");

    oc.doRegister("robocup-net", new Option_FileName());
    oc.addSynonyme("robocup-net", "robocup");
    oc.addDescription("robocup-net", "Input", "Read RoboCup-net from DIR");


    // register processing options
    oc.doRegister("dismiss-loading-errors", new Option_Bool(false)); // !!! describe, document
    oc.addDescription("dismiss-loading-errors", "Processing", "Continue on broken input");

    oc.doRegister("capacity-norm", new Option_Float((SUMOReal) 1800));
    oc.addDescription("capacity-norm", "Processing", "The factor for flow to no. lanes conv");

    oc.doRegister("speed-in-kmh", new Option_Bool(false));
    oc.addDescription("speed-in-kmh", "Processing", "vmax is parsed as given in km/h (some)");


    oc.doRegister("xml.keep-shape", new Option_Bool(false));
    oc.addDescription("xml.keep-shape", "Processing", "No node positions are added to the edge shape");


    oc.doRegister("shapefile.street-id", new Option_String());
    oc.addSynonyme("shapefile.street-id", "arcview.street-id");
    oc.addDescription("shapefile.street-id", "Processing", "Read edge ids from column STR");

    oc.doRegister("shapefile.from-id", new Option_String());
    oc.addSynonyme("shapefile.from-id", "arcview.from-id");
    oc.addDescription("shapefile.from-id", "Processing", "Read from-node ids from column STR");

    oc.doRegister("shapefile.to-id", new Option_String());
    oc.addSynonyme("shapefile.to-id", "arcview.to-id");
    oc.addDescription("shapefile.to-id", "Processing", "Read to-node ids from column STR");

    oc.doRegister("shapefile.type-id", new Option_String());
    oc.addSynonyme("shapefile.type-id", "arcview.type-id");
    oc.addDescription("shapefile.type-id", "Processing", "Read type ids from column STR");

    oc.doRegister("shapefile.use-defaults-on-failure", new Option_Bool(false));
    oc.addSynonyme("shapefile.use-defaults-on-failure", "arcview.use-defaults-on-failure");
    oc.addDescription("shapefile.use-defaults-on-failure", "Processing", "Uses edge type defaults on problems");

    oc.doRegister("shapefile.all-bidi", new Option_Bool(false));
    oc.addSynonyme("shapefile.all-bidi", "arcview.all-bidi");
    oc.addDescription("shapefile.all-bidi", "Processing", "Insert edges in both directions");

    oc.doRegister("shapefile.utm", new Option_Integer(32));
    oc.addSynonyme("shapefile.utm", "arcview.utm");
    oc.addDescription("shapefile.utm", "Processing", "Use INT as UTM zone");

    oc.doRegister("shapefile.guess-projection", new Option_Bool(false));
    oc.addSynonyme("shapefile.guess-projection", "arcview.guess-projection");
    oc.addDescription("shapefile.guess-projection", "Processing", "Guess the proper projection");

    // register further vissim-options
    oc.doRegister("vissim.offset", new Option_Float(5.0f));
    oc.addDescription("vissim.offset", "Processing", "Structure join offset");

    oc.doRegister("vissim.default-speed", new Option_Float(50.0f));
    oc.addDescription("vissim.default-speed", "Processing", "Use FLOAT as default speed");

    oc.doRegister("vissim.speed-norm", new Option_Float(1.0f));
    oc.addDescription("vissim.speed-norm", "Processing", "Factor for edge velocity");

    oc.doRegister("vissim.report-unset-speeds", new Option_Bool(false));
    oc.addDescription("vissim.report-unset-speeds", "Processing", "Writes lanes without an explicite speed set.");

    // register further visum-options
    oc.doRegister("visum.use-type-priority", new Option_Bool(false));
    oc.addDescription("visum.use-type-priority", "Processing", "Uses priorities from types");

    oc.doRegister("visum.use-type-laneno", new Option_Bool(false));
    oc.addDescription("visum.use-type-laneno", "Processing", "Uses lane numbers from types");

    oc.doRegister("visum.use-type-speed", new Option_Bool(false));
    oc.addDescription("visum.use-type-speed", "Processing", "Uses speeds from types");

    oc.doRegister("visum.connector-speeds", new Option_Float(100.));
    oc.addDescription("visum.connector-speeds", "Processing", "Sets connector speed");

    oc.doRegister("visum.connector-laneno", new Option_Integer(3));
    oc.addDescription("visum.connector-laneno", "Processing", "Sets connector lane number");

    oc.doRegister("visum.no-connectors", new Option_Bool(false));
    oc.addDescription("visum.no-connectors", "Processing", "Excludes connectors");

    oc.doRegister("visum.recompute-laneno", new Option_Bool(false));
    oc.addDescription("visum.recompute-laneno", "Processing", "Excludes connectors");


    // add netbuilding options
    NBNetBuilder::insertNetBuildOptions(oc);

    // add rand options
    RandHelper::insertRandOptions();
}


bool
NIOptionsIO::checkOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    bool ok = oc.checkDependingSuboptions("shapefile", "shapefile.");
    ok &= oc.checkDependingSuboptions("visum-file", "visum.");
    ok &= oc.checkDependingSuboptions("vissim-file", "vissim.");
#ifdef HAVE_PROJ
    unsigned numProjections = oc.getBool("proj.simple") + oc.getBool("proj.utm") + oc.getBool("proj.dhdn") + (oc.getString("proj").length() > 1);
    if ((oc.isSet("osm-files") || oc.isSet("dlr-navteq")) && numProjections == 0) {
        oc.set("proj.utm", true);
    }
    if (oc.isSet("dlr-navteq") && oc.isDefault("proj.shift")) {
        oc.set("proj.shift", std::string("5"));
    }
#endif
    return ok;
}



/****************************************************************************/

