/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    afrotest_main.cpp
/// @author  Ruediger Ebendt
/// @date    01.06.2024
///
// Main for afrotest, essentially duarouter_main.cpp boiled down to what is needed
// @note Called with just the net as parameter, e.g. afrotest.exe --net-file=input_net.net.xml
/****************************************************************************/
#include <config.h>

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <string>
#include <limits.h>
#include <ctime>
#include <memory>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
#ifdef HAVE_FOX
#include <utils/foxtools/MsgHandlerSynchronized.h>
#endif
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/xml/XMLSubSys.h>
#include <router/ROFrame.h>
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <router/ROEdge.h>
#include <duarouter/RODUAEdgeBuilder.h>
#include <duarouter/RODUAFrame.h>

#include "AFROTest.h"

// ===========================================================================
// functions
// ===========================================================================
/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
/**
 * loads the net
 * The net is in this meaning made up by the net itself and the dynamic
 * weights which may be supplied in a separate file
 */
void
initNet(RONet& net, ROLoader& loader, OptionsCont& oc) {
    // load the net
    RODUAEdgeBuilder builder;
    ROEdge::setGlobalOptions(oc.getBool("weights.interpolate"));
    loader.loadNet(net, builder);
    // load the weights if desired/available
    if (oc.isSet("weight-files")) {
        loader.loadWeights(net, "weight-files", oc.getString("weight-attribute"), false, oc.getBool("weights.expand"));
    }
    if (oc.isSet("lane-weight-files")) {
        loader.loadWeights(net, "lane-weight-files", oc.getString("weight-attribute"), true, oc.getBool("weights.expand"));
    }
}

/**
 * @brief Test the routes with the arc flag router
 * @param[in] net The network
 * @param[in] oc The options container
 */
void
testRoutes(RONet& net, OptionsCont& oc) {
    auto ttFunction = gWeightsRandomFactor > 1 ? &ROEdge::getTravelTimeStaticRandomized : &ROEdge::getTravelTimeStatic;
    auto reversedTtFunction = gWeightsRandomFactor > 1 ? &FlippedEdge<ROEdge, RONode, ROVehicle>::getTravelTimeStaticRandomized : &FlippedEdge<ROEdge, RONode, ROVehicle>::getTravelTimeStatic;

    MsgHandler::getMessageInstance()->addRetriever(&(OutputDevice::getDevice("stdout")));

    ROVehicle defaultVehicle(SUMOVehicleParameter(), nullptr, net.getVehicleTypeSecure(DEFAULT_VTYPE_ID), &net);
    long long int startTime = SysUtils::getCurrentMillis();

    AFROTest aFROTest;
    WRITE_MESSAGE("weight period as string: " + oc.getString("weight-period") + ", as time: "
                  + oc.getString("weight-period") + ", SUMOTime_MAX: " + elapsedMs2string(SUMOTime_MAX));
    aFROTest.test(&defaultVehicle, oc.getBool("ignore-errors"), ttFunction, reversedTtFunction,
                  (oc.isSet("weight-files") ? string2time(oc.getString("weight-period")) : SUMOTime_MAX), nullptr, nullptr,
                  net.hasPermissions(), oc.isSet("restriction-params"));
    WRITE_MESSAGE("AFRO test program finished.");

    long long int timeSpent = SysUtils::getCurrentMillis() - startTime;
    WRITE_MESSAGE("AFRO test spent " + elapsedMs2string(timeSpent) + " in total (building + routing).");
}

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char** argv) {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.setApplicationDescription(TL("Test program for the arc flag router of the microscopic, multi-modal traffic simulation SUMO."));
    oc.setApplicationName("afrotest", "Eclipse SUMO afrotest Version " VERSION_STRING);
    int ret = 0;
    RONet* net = nullptr;
    try {
        XMLSubSys::init();
        RODUAFrame::fillOptions();
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions();
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        SystemFrame::checkOptions(oc);
        XMLSubSys::setValidation(oc.getString("xml-validation"), oc.getString("xml-validation.net"), oc.getString("xml-validation.routes"));
#ifdef HAVE_FOX
        if (oc.getInt("routing-threads") > 1) {
            // make the output aware of threading
            MsgHandler::setFactory(&MsgHandlerSynchronized::create);
        }
#endif
        MsgHandler::initOutputOptions();
        RandHelper::initRandGlobal();
        // load data
        ROLoader loader(oc, false, !oc.getBool("no-step-log"));
        net = new RONet();
        initNet(*net, loader, oc);
        // test routes
        try {
            testRoutes(*net, oc);
        } catch (XERCES_CPP_NAMESPACE::SAXParseException& e) {
            WRITE_ERROR(toString(e.getLineNumber()));
            ret = 1;
        } catch (XERCES_CPP_NAMESPACE::SAXException& e) {
            WRITE_ERROR(StringUtils::transcode(e.getMessage()));
            ret = 1;
        }
        if (MsgHandler::getErrorInstance()->wasInformed() || ret != 0) {
            throw ProcessError();
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
    delete net;
    SystemFrame::close();
    if (ret == 0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}


/****************************************************************************/
