/****************************************************************************/
/// @file    NIFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Sets and checks options for netimport
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
#include <iostream>
#include <fstream>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include <netbuild/NBNetBuilder.h>
#include <netwrite/NWFrame.h>
#include <utils/common/SystemFrame.h>
#include "NIImporter_DlrNavteq.h"
#include "NIFrame.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
NIFrame::fillOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    // register input formats
    oc.doRegister("sumo-net-file", 's', new Option_FileName());
    oc.addSynonyme("sumo-net-file", "sumo-net", true);
    oc.addDescription("sumo-net-file", "Input", "Read SUMO-net from FILE");
    oc.addXMLDefault("sumo-net-file", "net");

    oc.doRegister("node-files", 'n', new Option_FileName());
    oc.addSynonyme("node-files", "xml-node-files", true);
    oc.addSynonyme("node-files", "xml-nodes", true);
    oc.addSynonyme("node-files", "nodes");
    oc.addDescription("node-files", "Input", "Read XML-node defs from FILE");

    oc.doRegister("edge-files", 'e', new Option_FileName());
    oc.addSynonyme("edge-files", "xml-edge-files", true);
    oc.addSynonyme("edge-files", "xml-edges", true);
    oc.addSynonyme("edge-files", "edges");
    oc.addDescription("edge-files", "Input", "Read XML-edge defs from FILE");

    oc.doRegister("connection-files", 'x', new Option_FileName());
    oc.addSynonyme("connection-files", "xml-connection-files", true);
    oc.addSynonyme("connection-files", "xml-connections", true);
    oc.addSynonyme("connection-files", "connections");
    oc.addDescription("connection-files", "Input", "Read XML-connection defs from FILE");

    oc.doRegister("tllogic-files", 'i', new Option_FileName());
    oc.addDescription("tllogic-files", "Input", "Read XML-traffic light defs from FILE");

    oc.doRegister("type-files", 't', new Option_FileName());
    oc.addSynonyme("type-files", "xml-type-files", true);
    oc.addSynonyme("type-files", "xml-types", true);
    oc.addSynonyme("type-files", "types");
    oc.addDescription("type-files", "Input", "Read XML-type defs from FILE");

    oc.doRegister("shapefile-prefix", new Option_FileName());
    oc.addSynonyme("shapefile-prefix", "shapefile");
    oc.addSynonyme("shapefile-prefix", "arcview", true);
    oc.addSynonyme("shapefile-prefix", "tiger", true);
    oc.addDescription("shapefile-prefix", "Input", "Read shapefiles (ArcView, Tiger, ...) from files starting with 'FILE'");

    oc.doRegister("dlr-navteq-prefix", new Option_FileName());
    oc.addSynonyme("dlr-navteq-prefix", "dlr-navteq");
    oc.addSynonyme("dlr-navteq-prefix", "elmar2", true);
    oc.addDescription("dlr-navteq-prefix", "Input", "Read converted Navteq GDF data (unsplitted Elmar-network) from path 'FILE'");

    oc.doRegister("osm-files", new Option_FileName());
    oc.addSynonyme("osm-files", "osm");
    oc.addDescription("osm-files", "Input", "Read OSM-network from path 'FILE(s)'");

    oc.doRegister("opendrive-files", new Option_FileName());
    oc.addSynonyme("opendrive-files", "opendrive");
    oc.addDescription("opendrive-files", "Input", "Read OpenDRIVE-network from FILE");

    oc.doRegister("visum-file", new Option_FileName());
    oc.addSynonyme("visum-file", "visum");
    oc.addDescription("visum-file", "Input", "Read VISUM-net from FILE");

    oc.doRegister("vissim-file", new Option_FileName());
    oc.addSynonyme("vissim-file", "vissim");
    oc.addDescription("vissim-file", "Input", "Read VISSIM-net from FILE");

    oc.doRegister("robocup-dir", new Option_FileName());
    oc.addSynonyme("robocup-dir", "robocup-net", true);
    oc.addSynonyme("robocup-dir", "robocup");
    oc.addDescription("robocup-dir", "Input", "Read RoboCup-net from DIR");

    oc.doRegister("matsim-files", new Option_FileName());
    oc.addSynonyme("matsim-files", "matsim");
    oc.addDescription("matsim-files", "Input", "Read MATsim-net from FILE");

    oc.doRegister("itsumo-files", new Option_FileName());
    oc.addSynonyme("itsumo-files", "itsumo");
    oc.addDescription("itsumo-files", "Input", "Read ITSUMO-net from FILE");

    oc.doRegister("heightmap.shapefiles", new Option_FileName());
    oc.addDescription("heightmap.shapefiles", "Input", "Read heightmap from ArcGIS shapefile");

    oc.doRegister("heightmap.geotiff", new Option_FileName());
    oc.addDescription("heightmap.geotiff", "Input", "Read heightmap from GeoTIFF");

    // register basic processing options
    oc.doRegister("ignore-errors", new Option_Bool(false));
    oc.addSynonyme("ignore-errors", "dismiss-loading-errors", true);
    oc.addDescription("ignore-errors", "Processing", "Continue on broken input");

    oc.doRegister("ignore-errors.connections", new Option_Bool(false));
    oc.addDescription("ignore-errors.connections", "Processing", "Continue on invalid connections");

    oc.doRegister("show-errors.connections-first-try", new Option_Bool(false));
    oc.addDescription("show-errors.connections-first-try", "Processing", "Show errors in connections at parsing");

    oc.doRegister("ignore-errors.edge-type", new Option_Bool(false));
    oc.addDescription("ignore-errors.edge-type", "Processing", "Continue on unknown edge types");

    oc.doRegister("lanes-from-capacity.norm", new Option_Float((SUMOReal) 1800));
    oc.addSynonyme("lanes-from-capacity.norm", "capacity-norm");
    oc.addDescription("lanes-from-capacity.norm", "Processing", "The factor for flow to no. lanes conversion");

    oc.doRegister("speed-in-kmh", new Option_Bool(false));
    oc.addDescription("speed-in-kmh", "Processing", "vmax is parsed as given in km/h (some)");

    oc.doRegister("construction-date", new Option_String());
    oc.addDescription("construction-date", "Processing", "Use YYYY-MM-DD date to determine the readiness of features under construction");


    // register xml options
    oc.doRegister("plain.extend-edge-shape", new Option_Bool(false));
    oc.addSynonyme("plain.extend-edge-shape", "xml.keep-shape", true);
    oc.addDescription("plain.extend-edge-shape", "Processing", "If edge shapes do not end at the node positions, extend them");


    // register matsim options
    oc.doRegister("matsim.keep-length", new Option_Bool(false));
    oc.addDescription("matsim.keep-length", "Processing", "The edge lengths given in the MATSIM-file will be kept");

    oc.doRegister("matsim.lanes-from-capacity", new Option_Bool(false));
    oc.addDescription("matsim.lanes-from-capacity", "Processing", "The lane number will be computed from the capacity");


    // register shapefile options
    oc.doRegister("shapefile.street-id", new Option_String());
    oc.addSynonyme("shapefile.street-id", "arcview.street-id", true);
    oc.addDescription("shapefile.street-id", "Processing", "Read edge ids from column STR");

    oc.doRegister("shapefile.from-id", new Option_String());
    oc.addSynonyme("shapefile.from-id", "arcview.from-id", true);
    oc.addDescription("shapefile.from-id", "Processing", "Read from-node ids from column STR");

    oc.doRegister("shapefile.to-id", new Option_String());
    oc.addSynonyme("shapefile.to-id", "arcview.to-id", true);
    oc.addDescription("shapefile.to-id", "Processing", "Read to-node ids from column STR");

    oc.doRegister("shapefile.type-id", new Option_String());
    oc.addSynonyme("shapefile.type-id", "arcview.type-id", true);
    oc.addDescription("shapefile.type-id", "Processing", "Read type ids from column STR");

    oc.doRegister("shapefile.use-defaults-on-failure", new Option_Bool(false));
    oc.addSynonyme("shapefile.use-defaults-on-failure", "arcview.use-defaults-on-failure", true);
    oc.addDescription("shapefile.use-defaults-on-failure", "Processing", "Uses edge type defaults on problems");

    oc.doRegister("shapefile.all-bidirectional", new Option_Bool(false));
    oc.addSynonyme("shapefile.all-bidirectional", "shapefile.all-bidi");
    oc.addSynonyme("shapefile.all-bidirectional", "arcview.all-bidi", true);
    oc.addDescription("shapefile.all-bidirectional", "Processing", "Insert edges in both directions");

    oc.doRegister("shapefile.guess-projection", new Option_Bool(false));
    oc.addSynonyme("shapefile.guess-projection", "arcview.guess-projection", true);
    oc.addDescription("shapefile.guess-projection", "Processing", "Guess the proper projection");


    // register vissim options
    oc.doRegister("vissim.join-distance", new Option_Float(5.0f));
    oc.addSynonyme("vissim.join-distance", "vissim.offset", true);
    oc.addDescription("vissim.join-distance", "Processing", "Structure join offset");

    oc.doRegister("vissim.default-speed", new Option_Float(50.0f));
    oc.addDescription("vissim.default-speed", "Processing", "Use FLOAT as default speed");

    oc.doRegister("vissim.speed-norm", new Option_Float(1.0f));
    oc.addDescription("vissim.speed-norm", "Processing", "Factor for edge velocity");

    oc.doRegister("vissim.report-unset-speeds", new Option_Bool(false));
    oc.addDescription("vissim.report-unset-speeds", "Processing", "Writes lanes without an explicit speed set");


    // register visum options
    oc.doRegister("visum.use-type-priority", new Option_Bool(false));
    oc.addDescription("visum.use-type-priority", "Processing", "Uses priorities from types");

    oc.doRegister("visum.use-type-laneno", new Option_Bool(false));
    oc.addDescription("visum.use-type-laneno", "Processing", "Uses lane numbers from types");

    oc.doRegister("visum.use-type-speed", new Option_Bool(false));
    oc.addDescription("visum.use-type-speed", "Processing", "Uses speeds from types");

    oc.doRegister("visum.connector-speeds", new Option_Float(100.));
    oc.addDescription("visum.connector-speeds", "Processing", "Sets connector speed");

    oc.doRegister("visum.connectors-lane-number", new Option_Integer(3));
    oc.addSynonyme("visum.connectors-lane-number", "visum.connector-laneno", true);
    oc.addDescription("visum.connectors-lane-number", "Processing", "Sets connector lane number");

    oc.doRegister("visum.no-connectors", new Option_Bool(false));
    oc.addDescription("visum.no-connectors", "Processing", "Excludes connectors");

    oc.doRegister("visum.recompute-lane-number", new Option_Bool(false));
    oc.addSynonyme("visum.recompute-lane-number", "visum.recompute-laneno", true);
    oc.addDescription("visum.recompute-lane-number", "Processing", "Computes the number of lanes from the edges' capacities");

    oc.doRegister("visum.verbose-warnings", new Option_Bool(false));
    oc.addDescription("visum.verbose-warnings", "Processing", "Prints all warnings, some of which are due to VISUM misbehaviour");


    // register osm options
    oc.doRegister("osm.skip-duplicates-check", new Option_Bool(false));
    oc.addDescription("osm.skip-duplicates-check", "Processing", "Skips the check for duplicate nodes and edges");

    oc.doRegister("osm.elevation", new Option_Bool(false));
    oc.addDescription("osm.elevation", "Processing", "Imports elevation data");

    oc.doRegister("osm.layer-elevation", new Option_Float(0));
    oc.addDescription("osm.layer-elevation", "Processing", "Reconstruct (relative) elevation based on layer data. Each layer is raised by FLOAT m");

    oc.doRegister("osm.layer-elevation.max-grade", new Option_Float(10));
    oc.addDescription("osm.layer-elevation.max-grade", "Processing", "Maximum grade threshold in % at 50km/h when reconstrucing elevation based on layer data. The value is scaled according to road speed.");

    oc.doRegister("osm.oneway-spread-right", new Option_Bool(false));
    oc.addDescription("osm.oneway-spread-right", "Processing", "Whether one-way roads should be spread to the side instead of centered");

    // register opendrive options
    oc.doRegister("opendrive.import-all-lanes", new Option_Bool(false));
    oc.addDescription("opendrive.import-all-lanes", "Processing", "Imports all lane types");
    oc.doRegister("opendrive.ignore-widths", new Option_Bool(false));
    oc.addDescription("opendrive.ignore-widths", "Processing", "Whether lane widths shall be ignored.");

    // register some additional options
    oc.doRegister("tls.discard-loaded", new Option_Bool(false));
    oc.addDescription("tls.discard-loaded", "TLS Building", "Does not instatiate traffic lights loaded from other formats than XML");

    oc.doRegister("tls.discard-simple", new Option_Bool(false));
    oc.addDescription("tls.discard-simple", "TLS Building", "Does not instatiate traffic lights at geometry-like nodes loaded from other formats than XML");
}


bool
NIFrame::checkOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    bool ok = oc.checkDependingSuboptions("shapefile", "shapefile.");
    ok &= oc.checkDependingSuboptions("visum-file", "visum.");
    ok &= oc.checkDependingSuboptions("vissim-file", "vissim.");
#ifdef HAVE_PROJ
    int numProjections = oc.getBool("simple-projection") + oc.getBool("proj.utm") + oc.getBool("proj.dhdn") + (oc.getString("proj").length() > 1);
    if ((oc.isSet("osm-files") || oc.isSet("dlr-navteq-prefix") || oc.isSet("shapefile-prefix")) && numProjections == 0) {
        if (oc.isDefault("proj")) {
            oc.set("proj.utm", "true");
        }
    }
    if (oc.isSet("dlr-navteq-prefix") && oc.isDefault("proj.scale")) {
        oc.set("proj.scale", toString(NIImporter_DlrNavteq::GEO_SCALE));
    }
#else
    if ((oc.isSet("osm-files") || oc.isSet("dlr-navteq-prefix") || oc.isSet("shapefile-prefix")) && !oc.getBool("simple-projection")) {
        WRITE_ERROR("Cannot import network data without PROJ-Library. Please install packages proj before building sumo");
        ok = false;
    }
#endif
    if (oc.isSet("sumo-net-file")) {
        if (oc.isWriteable("no-turnarounds")) {
            // changed default since turnarounds are loaded from the net file.
            oc.set("no-turnarounds", "true");
        }
        if (oc.isWriteable("offset.disable-normalization")) {
            // changed default since we wish to preserve the network as far as possible
            oc.set("offset.disable-normalization", "true");
        }
    }
    if (!oc.isSet("type-files")) {
        const char* sumoPath = std::getenv("SUMO_HOME");
        if (sumoPath == 0) {
            WRITE_WARNING("Environment variable SUMO_HOME is not set, using built in type maps.");
        } else {
            const std::string path = sumoPath + std::string("/data/typemap/");
            if (oc.isSet("osm-files")) {
                oc.setDefault("type-files", path + "osmNetconvert.typ.xml");
            }
            if (oc.isSet("opendrive-files")) {
                oc.setDefault("type-files", path + "opendriveNetconvert.typ.xml");
            }
        }
    }
    if (oc.isSet("opendrive-files")) {
        if (oc.isDefault("tls.left-green.time")) {
            // legacy behavior. see #2114
            oc.set("tls.left-green.time", "0");
        }
        if (oc.isDefault("rectangular-lane-cut")) {
            // a better interpretation of imported geometries
            oc.set("rectangular-lane-cut", "true");
        }
    }
    return ok;
}



/****************************************************************************/

