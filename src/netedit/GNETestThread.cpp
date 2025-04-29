/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNETestThread.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Thread used for testing netedit
/****************************************************************************/

#include <netbuild/NBFrame.h>
#include <netbuild/NBNetBuilder.h>
#include <netimport/NIFrame.h>
#include <netimport/NILoader.h>
#include <netwrite/NWFrame.h>
#include <utils/common/MsgRetrievingFunction.h>
#include <utils/common/SystemFrame.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/xml/XMLSubSys.h>

#include "GNEApplicationWindow.h"
#include "GNEEvent_NetworkLoaded.h"
#include "GNETestThread.h"
#include "GNENet.h"

// ===========================================================================
// member method definitions
// ===========================================================================
GNETestThread::GNETestThread(GNEApplicationWindow* applicationWindow, MFXSynchQue<GUIEvent*>& eq, FXEX::MFXThreadEvent& ev) :
    MFXSingleEventThread(applicationWindow->getApp(), applicationWindow),
    myApplicationWindow(applicationWindow),
    myEventQueue(eq),
    myEventThrow(ev) {
}


GNETestThread::~GNETestThread() {}


FXint
GNETestThread::run() {
    auto& neteditOptions = OptionsCont::getOptions();
    // flag for check if input is valid
    bool validInput = false;
    // declare network
    GNENet* net = nullptr;
    // declare loaded file
    std::string loadedFile;
    // check conditions
    if (neteditOptions.getBool("new")) {
        // create new network
        validInput = true;
    } else if (neteditOptions.getString("osm-files").size() > 0) {
        // load an osm file
        validInput = true;
    } else if (neteditOptions.getString("net-file").size() > 0) {
        // load a network file
        validInput = true;
        loadedFile = neteditOptions.getString("net-file");
    } else if (neteditOptions.getString("sumocfg-file").size() > 0) {
        // set sumo config as loaded file
        loadedFile = neteditOptions.getString("sumocfg-file");
        // declare parser for sumo config file
        GNEApplicationWindowHelper::GNESumoConfigHandler confighandler(myApplicationWindow->getSumoOptions(), loadedFile);
        // if there is an error loading sumo config, stop
        if (confighandler.loadSumoConfig()) {
            validInput = true;
        } else {
            WRITE_ERRORF(TL("Loading of sumo config file '%' failed."), loadedFile);
            submitEndAndCleanup(net, loadedFile);
            return 0;
        }
    } else if (neteditOptions.getString("configuration-file").size() > 0) {
        // set netedit config as loaded file
        loadedFile = neteditOptions.getString("configuration-file");
        // declare parser for netedit config file
        GNEApplicationWindowHelper::GNENeteditConfigHandler confighandler(loadedFile);
        // if there is an error loading sumo config, stop
        if (confighandler.loadNeteditConfig()) {
            validInput = true;
        } else {
            WRITE_ERRORF(TL("Loading of netedit config file '%' failed."), loadedFile);
            submitEndAndCleanup(net, loadedFile);
            return 0;
        }
    }
    // check input
    if (!validInput) {
        WRITE_ERROR(TL("Invalid input network option. Load with either sumo/netedit/netconvert config or with -new option"));
        submitEndAndCleanup(net, loadedFile);
        return 0;
    }
    // update aggregate warnings
    if (neteditOptions.isDefault("aggregate-warnings")) {
        neteditOptions.setDefault("aggregate-warnings", "5");
    }
    // init output options
    MsgHandler::initOutputOptions();
    // if there is an error checking options, stop
    if (!(NIFrame::checkOptions(neteditOptions) && NBFrame::checkOptions(neteditOptions) &&
            NWFrame::checkOptions(neteditOptions) && SystemFrame::checkOptions(neteditOptions))) {
        // options are not valid
        WRITE_ERROR(TL("Invalid Options. Nothing loaded"));
        submitEndAndCleanup(net, loadedFile);
        return 0;
    }
    // clear message instances
    MsgHandler::getErrorInstance()->clear();
    MsgHandler::getWarningInstance()->clear();
    MsgHandler::getMessageInstance()->clear();
    // init global random seed
    RandHelper::initRandGlobal();
    // check if geo projection can be initialized
    if (!GeoConvHelper::init(neteditOptions)) {
        WRITE_ERROR(TL("Could not build projection!"));
        submitEndAndCleanup(net, loadedFile);
        return 0;
    }
    // set validation
    XMLSubSys::setValidation(neteditOptions.getString("xml-validation"), neteditOptions.getString("xml-validation.net"), neteditOptions.getString("xml-validation.routes"));
    // check if Debug has to be enabled
    MsgHandler::enableDebugMessages(neteditOptions.getBool("gui-testing-debug"));
    // check if GL Debug has to be enabled
    MsgHandler::enableDebugGLMessages(neteditOptions.getBool("gui-testing-debug-gl"));
    // create netBuilder (will be destroyed in GNENet destructor)
    NBNetBuilder* netBuilder = new NBNetBuilder();
    // apply netedit options in netBuilder. In this options we have all information for building network
    netBuilder->applyOptions(neteditOptions);
    // check if create a new net
    if (neteditOptions.getBool("new")) {
        // create new network
        net = new GNENet(netBuilder, myApplicationWindow->getTagPropertiesDatabase());
    } else {
        // declare net loader
        NILoader nl(*netBuilder);
        try {
            // try to load network using netedit options
            nl.load(neteditOptions);
            if (true) {    // CHECK
                // make coordinate conversion usable before first netBuilder->compute()
                GeoConvHelper::computeFinal();
            } else {
                WRITE_MESSAGE(TL("Performing initial computation ..."));
                // perform one-time processing (i.e. edge removal)
                netBuilder->compute(neteditOptions);
                // @todo remove one-time processing options!
            }
            // check if ignore errors
            if (neteditOptions.getBool("ignore-errors")) {
                MsgHandler::getErrorInstance()->clear();
            }
            // check whether any errors occurred
            if (MsgHandler::getErrorInstance()->wasInformed()) {
                throw ProcessError();
            } else {
                // now create net with al information loaded in net builder
                net = new GNENet(netBuilder, myApplicationWindow->getTagPropertiesDatabase());
                // check if change traffic direction
                if (neteditOptions.getBool("lefthand")) {
                    // force initial geometry computation without volatile options because the net will look strange otherwise
                    net->computeAndUpdate(neteditOptions, false);
                }
                // check if add prefixes
                if (neteditOptions.getString("prefix").size() > 0) {
                    // change prefixes in attributeCarriers
                    net->getAttributeCarriers()->addPrefixToEdges(neteditOptions.getString("prefix"));
                    net->getAttributeCarriers()->addPrefixToJunctions(neteditOptions.getString("prefix"));
                    // change prefix in containers
                    net->getNetBuilder()->getNodeCont().addPrefix(neteditOptions.getString("prefix"));
                    net->getNetBuilder()->getEdgeCont().addPrefix(neteditOptions.getString("prefix"));
                }
            }
        } catch (ProcessError& e) {
            if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
                WRITE_ERROR(e.what());
            }
            WRITE_ERROR(TL("Failed to build network."));
            // check if delete network
            if (net != nullptr) {
                delete net;
                net = nullptr;
            } else {
                // GNENet not created, then delete netBuilder
                delete netBuilder;
            }
        } catch (std::exception& e) {
            WRITE_ERROR(e.what());
            // check if delete network
            if (net != nullptr) {
                delete net;
                net = nullptr;
            } else {
                // GNENet not created, then delete netBuilder
                delete netBuilder;
            }
        }
    }
    // only a single setting file is supported
    submitEndAndCleanup(net, loadedFile, neteditOptions.getString("gui-settings-file"), neteditOptions.getBool("registry-viewport"));
    return 0;
}



void
GNETestThread::submitEndAndCleanup(GNENet* net, const std::string& loadedFile, const std::string& guiSettingsFile, const bool viewportFromRegistry) {
    // inform parent about the process
    myEventQueue.push_back(new GNEEvent_NetworkLoaded(net, loadedFile, guiSettingsFile, viewportFromRegistry));
    myEventThrow.signal();
}


void
GNETestThread::startTest() {
    // start thread
    start();
}

/****************************************************************************/
