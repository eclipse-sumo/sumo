/****************************************************************************/
/// @file    NWFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Sets and checks options for netwrite
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
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/SysUtils.h>
#include <utils/iodevices/OutputDevice.h>
#include <netbuild/NBNetBuilder.h>
#include "NWFrame.h"
#include "NWWriter_SUMO.h"
#include "NWWriter_Amitran.h"
#include "NWWriter_MATSim.h"
#include "NWWriter_XML.h"
#include "NWWriter_OpenDrive.h"
#include "NWWriter_DlrNavteq.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
const std::string NWFrame::MAJOR_VERSION = "0.27";


// ===========================================================================
// method definitions
// ===========================================================================
void
NWFrame::fillOptions(bool forNetgen) {
    OptionsCont& oc = OptionsCont::getOptions();
    // register options
    oc.doRegister("output-file", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "sumo-output");
    oc.addSynonyme("output-file", "output");
    oc.addDescription("output-file", "Output", "The generated net will be written to FILE");

    oc.doRegister("plain-output-prefix", new Option_FileName());
    oc.addSynonyme("plain-output-prefix", "plain-output");
    oc.addSynonyme("plain-output-prefix", "plain");
    oc.addDescription("plain-output-prefix", "Output", "Prefix of files to write plain xml nodes, edges and connections to");

    oc.doRegister("junctions.join-output", new Option_FileName());
    oc.addDescription("junctions.join-output", "Output",
                      "Writes information about joined junctions to FILE (can be loaded as additional node-file to reproduce joins");

#ifdef HAVE_PROJ
    if (!forNetgen) {
        oc.doRegister("proj.plain-geo", new Option_Bool(false));
        oc.addDescription("proj.plain-geo", "Projection", "Write geo coordinates in plain-xml");
    }
#endif // HAVE_PROJ

    oc.doRegister("amitran-output", new Option_FileName());
    oc.addDescription("amitran-output", "Output", "The generated net will be written to FILE using Amitran format");

    oc.doRegister("matsim-output", new Option_FileName());
    oc.addDescription("matsim-output", "Output", "The generated net will be written to FILE using MATsim format");

    oc.doRegister("opendrive-output", new Option_FileName());
    oc.addDescription("opendrive-output", "Output", "The generated net will be written to FILE using OpenDRIVE format");

    oc.doRegister("dlr-navteq-output", new Option_FileName());
    oc.addDescription("dlr-navteq-output", "Output", "The generated net will be written to dlr-navteq files with the given PREFIX");

    oc.doRegister("dlr-navteq.precision", new Option_Integer(2));
    oc.addDescription("dlr-navteq.precision", "Output", "The network coordinates are written with the specified level of output precision");

    oc.doRegister("output.street-names", new Option_Bool(false));
    oc.addDescription("output.street-names", "Output", "Street names will be included in the output (if available)");

    oc.doRegister("output.original-names", new Option_Bool(false));
    oc.addDescription("output.original-names", "Output", "Writes original names, if given, as parameter");

    oc.doRegister("street-sign-output", new Option_FileName());
    oc.addDescription("street-sign-output", "Output", "Writes street signs as POIs to FILE");

    // register opendrive options
    oc.doRegister("opendrive-output.straight-threshold", new Option_Float(0.00000001)); // matching the angular output precision in NWWriter_OpenDrive
    oc.addDescription("opendrive-output.straight-threshold", "Output", "Builds parameterized curves whenever the angular change  between straight segments exceeds FLOAT degrees");
}


bool
NWFrame::checkOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    bool ok = true;
    // check whether the output is valid and can be build
    if (!oc.isSet("output-file")
            && !oc.isSet("plain-output-prefix")
            && !oc.isSet("amitran-output")
            && !oc.isSet("matsim-output")
            && !oc.isSet("opendrive-output")
            && !oc.isSet("dlr-navteq-output")) {
        std::string net = "net.net.xml";
        if (oc.isSet("configuration-file")) {
            net = FileHelpers::getConfigurationRelative(oc.getString("configuration-file"), net);
        }
        oc.setDefault("output-file", net);
    }
    // some outputs need internal lanes
    if (oc.isSet("opendrive-output") && oc.getBool("no-internal-links")) {
        WRITE_ERROR("OpenDRIVE export needs internal links computation.");
        ok = false;
    }
    if (oc.isSet("opendrive-output") && oc.isDefault("rectangular-lane-cut")) {
        oc.set("rectangular-lane-cut", "true");
    }
    if (oc.isSet("opendrive-output") && !oc.getBool("rectangular-lane-cut")) {
        WRITE_WARNING("OpenDRIVE cannot represent oblique lane cuts and should use option 'rectangular-lane-cut'.");
    }
    if (oc.isSet("dlr-navteq-output") && oc.isDefault("numerical-ids")) {
        oc.set("numerical-ids", "true");
    }
    return ok;
}


void
NWFrame::writeNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    long before = SysUtils::getCurrentMillis();
    PROGRESS_BEGIN_MESSAGE("Writing network");
    NWWriter_SUMO::writeNetwork(oc, nb);
    NWWriter_Amitran::writeNetwork(oc, nb);
    NWWriter_MATSim::writeNetwork(oc, nb);
    NWWriter_OpenDrive::writeNetwork(oc, nb);
    NWWriter_DlrNavteq::writeNetwork(oc, nb);
    NWWriter_XML::writeNetwork(oc, nb);
    PROGRESS_TIME_MESSAGE(before);
}


void
NWFrame::writePositionLong(const Position& pos, OutputDevice& dev) {
    dev.writeAttr(SUMO_ATTR_X, pos.x());
    dev.writeAttr(SUMO_ATTR_Y, pos.y());
    if (pos.z() != 0) {
        dev.writeAttr(SUMO_ATTR_Z, pos.z());
    }
}

/****************************************************************************/

