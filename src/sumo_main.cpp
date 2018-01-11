/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    sumo_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Axel Wegener
/// @author  Thimor Bohn
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Main for SUMO
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

#include <ctime>
#include <string>
#include <iostream>
#include <netload/NLBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <traci-server/TraCIServer.h>


// ===========================================================================
// functions
// ===========================================================================
/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char** argv) {
    OptionsCont& oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("A microscopic road traffic simulation.");
    oc.setApplicationName("sumo", "SUMO Version " VERSION_STRING);
    int ret = 0;
    try {
        // initialise subsystems
        XMLSubSys::init();
        OptionsIO::setArgs(argc, argv);
        // load the net
        MSNet::SimulationState state = MSNet::SIMSTATE_LOADING;
        while (state == MSNet::SIMSTATE_LOADING) {
            MSNet* net = NLBuilder::init();
            if (net != nullptr) {
                state = net->simulate(string2time(oc.getString("begin")), string2time(oc.getString("end")));
                delete net;
            } else {
                break;
            }
        }
    } catch (const ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (const std::exception& e) {
        if (std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    TraCIServer::close();
    SystemFrame::close();
    return ret;
}


/****************************************************************************/
