/****************************************************************************/
/// @file    NIOptionsIO.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Sets and checks options for netimport
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
NIOptionsIO::fillOptions()
{
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>");
    oc.addCallExample("-n ./nodes.xml -e ./edges.xml -v -t ./owntypes.xml");


    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Projection");
    oc.addOptionSubTopic("TLS Building");
    oc.addOptionSubTopic("Ramp Guessing");
    oc.addOptionSubTopic("Edge Removal");
    oc.addOptionSubTopic("Unregulated Nodes");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Building Defaults");
    oc.addOptionSubTopic("Report");

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

    oc.doRegister("arcview", new Option_FileName());
    oc.addDescription("arcview", "Input", "Read ARCVIEW-net from files starting with 'FILE'");

    oc.doRegister("elmar", new Option_FileName());
    oc.addDescription("elmar", "Input", "Read splitted Elmar-network from path 'FILE'");

    oc.doRegister("elmar2", new Option_FileName());
    oc.addDescription("elmar2", "Input", "Read unsplitted Elmar-network from path 'FILE'");

    oc.doRegister("tiger", new Option_FileName());
    oc.addDescription("tiger", "Input", "Read Tiger-network from path 'FILE'");

    oc.doRegister("osm-files", new Option_FileName());
    oc.addSynonyme("osm-files", "osm");
    oc.addDescription("osm-files", "Input", "Read OSM-network from path 'FILE(s)'");

    oc.doRegister("visum-file", new Option_FileName());
    oc.addSynonyme("visum-file", "visum");
    oc.addDescription("visum-file", "Input", "Read VISUM-net from FILE");

    oc.doRegister("vissim-file", new Option_FileName());
    oc.addSynonyme("vissim-file", "vissim");
    oc.addDescription("vissim-file", "Input", "Read VISSIM-net from FILE");

    oc.doRegister("robocup-net", new Option_FileName());
    oc.addDescription("robocup-net", "Input", "Read RoboCup-net from DIR");


    // register processing options
    oc.doRegister("dismiss-loading-errors", new Option_Bool(false)); // !!! describe, document
    oc.addDescription("dismiss-loading-errors", "Processing", "Continue on broken input");

    oc.doRegister("capacity-norm", new Option_Float((SUMOReal) 1800));
    oc.addDescription("capacity-norm", "Processing", "The factor for flow to no. lanes conv");

    oc.doRegister("speed-in-kmh", new Option_Bool(false));
    oc.addDescription("speed-in-kmh", "Processing", "vmax is parsed as given in km/h (some)");


    oc.doRegister("arcview.street-id", new Option_String());
    oc.addDescription("arcview.street-id", "Processing", "Read edge ids from column STR (ArcView)");

    oc.doRegister("arcview.from-id", new Option_String());
    oc.addDescription("arcview.from-id", "Processing", "Read from-node ids from column STR (ArcView)");

    oc.doRegister("arcview.to-id", new Option_String());
    oc.addDescription("arcview.to-id", "Processing", "Read to-node ids from column STR (ArcView)");

    oc.doRegister("arcview.type-id", new Option_String());
    oc.addDescription("arcview.type-id", "Processing", "Read type ids from column STR (ArcView)");

    oc.doRegister("arcview.use-defaults-on-failure", new Option_Bool(false));
    oc.addDescription("arcview.use-defaults-on-failure", "Processing", "Uses edge type defaults on problems (ArcView)");

    oc.doRegister("arcview.all-bidi", new Option_Bool(false));
    oc.addDescription("arcview.all-bidi", "Processing", "Insert edges in both directions (ArcView)");

    oc.doRegister("arcview.utm", new Option_Integer(32));
    oc.addDescription("arcview.utm", "Processing", "Use INT as UTM zone (ArcView)");

    oc.doRegister("arcview.guess-projection", new Option_Bool(false));
    oc.addDescription("arcview.guess-projection", "Processing", "Guess the proper projection (ArcView)");

    // register further vissim-options
    oc.doRegister("vissim.offset", new Option_Float(5.0f));
    oc.addDescription("vissim.offset", "Processing", "Structure join offset (VISSIM)");

    oc.doRegister("vissim.default-speed", new Option_Float(50.0f));
    oc.addDescription("vissim.default-speed", "Processing", "Use FLOAT as default speed (VISSIM)");

    oc.doRegister("vissim.speed-norm", new Option_Float(1.0f));
    oc.addDescription("vissim.speed-norm", "Processing", "Factor for edge velocity (VISSIM)");

    oc.doRegister("vissim.report-unset-speeds", new Option_Bool(false));
    oc.addDescription("vissim.report-unset-speeds", "Processing", "Writes lanes without an explicite speed set.");

    // register further visum-options
    oc.doRegister("visum.use-net-prio", new Option_Bool(false));
    oc.addDescription("visum.use-net-prio", "Processing", "Uses priorities from types");

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
NIOptionsIO::checkOptions()
{
    return true;
}



/****************************************************************************/

