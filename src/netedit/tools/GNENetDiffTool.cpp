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
/// @file    GNENetDiffTool.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2023
///
// Netdiff tool used in netedit
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/GNEGeneralHandler.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>

#include "GNENetDiffTool.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNENetDiffTool::GNENetDiffTool(GNEApplicationWindow* GNEApp, const std::string& pythonPath, FXMenuPane* menu) :
    GNEPythonTool(GNEApp, pythonPath, "", menu) {
    // fill options
    fillNetDiffOptions(myPythonToolsOptions);
    fillNetDiffOptions(myPythonToolsOptionsOriginal);
}


GNENetDiffTool::~GNENetDiffTool() {}


void
GNENetDiffTool::setCurrentValues() {
    myPythonToolsOptions.resetWritable();
    // obtain curren network folder
    const auto networkPath = OptionsCont::getOptions().getString("net-file");
    if (networkPath.empty()) {
        myPythonToolsOptions.set("outprefix", "");
    } else {
        myPythonToolsOptions.set("outprefix", FileHelpers::getFilePath(networkPath) + "diff");
    }
}


void
GNENetDiffTool::postProcessing() {
    // first check if there is a network
    if (myGNEApp->getViewNet()) {
        // get selector operator modul from selector frame
        auto selectorModul = myGNEApp->getViewNet()->getViewParent()->getSelectorFrame()->getSelectionOperationModul();
        // select elements
        if (myPythonToolsOptions.getBool("select-modified")) {
            selectorModul->loadFromFile(myPythonToolsOptions.getString("outprefix") + ".changed.sel.txt");
        }
        if (myPythonToolsOptions.getBool("select-added")) {
            selectorModul->loadFromFile(myPythonToolsOptions.getString("outprefix") + ".created.sel.txt");
        }
        if (myPythonToolsOptions.getBool("select-deleted")) {
            selectorModul->loadFromFile(myPythonToolsOptions.getString("outprefix") + ".deleted.sel.txt");
        }
        // load shapes
        if (myPythonToolsOptions.getBool("load-shapes-modified")) {
            loadShapes(myPythonToolsOptions.getString("outprefix") + ".changed.shape.xml");
        }
        if (myPythonToolsOptions.getBool("load-shapes-added")) {
            loadShapes(myPythonToolsOptions.getString("outprefix") + ".created.shape.xml");
        }
        if (myPythonToolsOptions.getBool("load-shapes-deleted")) {
            loadShapes(myPythonToolsOptions.getString("outprefix") + ".deleted.shape.xml");
        }
    }
}


std::string
GNENetDiffTool::getCommand() const {
    // add python script
    const char* pythonEnv = getenv("PYTHON");
    const std::string python = (pythonEnv == nullptr) ? "python" : pythonEnv;
    const char* sumoHomeEnv = getenv("SUMO_HOME");
    const std::string sumoHome = (sumoHomeEnv == nullptr) ? "" : sumoHomeEnv + std::string("/");
    // get command
    std::string command = python + " " + sumoHome + myPythonPath;
    // declare arguments
    std::string arguments;
    // add arguments
    arguments += (myPythonToolsOptions.getString("original-net") + " ");
    arguments += (myPythonToolsOptions.getString("modified-net") + " ");
    arguments += myPythonToolsOptions.getString("outprefix") + " ";
    // check if save selection
    if (myPythonToolsOptions.getBool("select-modified") ||
            myPythonToolsOptions.getBool("select-added") ||
            myPythonToolsOptions.getBool("select-deleted")) {
        arguments += "--write-selections ";
    }
    // check if save shapes
    if (myPythonToolsOptions.getBool("load-shapes-modified") ||
            myPythonToolsOptions.getBool("load-shapes-added") ||
            myPythonToolsOptions.getBool("load-shapes-deleted")) {
        arguments += "--write-shapes ";
    }
    return command + " " + arguments;
}


void
GNENetDiffTool::fillNetDiffOptions(OptionsCont& options) {
    // build custom options for netdiff
    options.addOptionSubTopic("Input");
    options.doRegister("original-net", new Option_Network(""));
    options.addDescription("original-net", "Input", TL("Original network"));

    options.doRegister("modified-net", new Option_Network(""));
    options.addDescription("modified-net", "Input", TL("Modified network"));

    options.doRegister("outprefix", new Option_FileName());
    options.addDescription("outprefix", "Input", TL("Output prefix network"));

    options.addOptionSubTopic("Select");
    options.doRegister("select-modified", new Option_Bool(false));
    options.addDescription("select-modified", "Select", TL("Select modified elements"));

    options.doRegister("select-added", new Option_Bool(false));
    options.addDescription("select-added", "Select", TL("Select added elements"));

    options.doRegister("select-deleted", new Option_Bool(false));
    options.addDescription("select-deleted", "Select", TL("Select deleted elements"));

    options.addOptionSubTopic("Load");
    options.doRegister("load-shapes-modified", new Option_Bool(false));
    options.addDescription("load-shapes-modified", "Load", TL("Load shapes for elements"));

    options.doRegister("load-shapes-added", new Option_Bool(false));
    options.addDescription("load-shapes-added", "Load", TL("Load shapes for added"));

    options.doRegister("load-shapes-deleted", new Option_Bool(false));
    options.addDescription("load-shapes-deleted", "Load", TL("Load shapes for deleted elements"));
}


void
GNENetDiffTool::loadShapes(const std::string& file) {
    // get undo list
    auto undoList = myGNEApp->getUndoList();
    // disable validation for additionals
    XMLSubSys::setValidation("never", "auto", "auto");
    // Create additional handler
    GNEGeneralHandler generalHandler(myGNEApp->getViewNet()->getNet(), file, true, true);
    // begin undoList operation
    undoList->begin(Supermode::NETWORK, GUIIcon::SUPERMODENETWORK, TL("load shapes from '") + file + "'");
    // Run parser
    if (!generalHandler.parse()) {
        // write error
        WRITE_ERROR(TL("Loading of shape file failed: ") + file);
    } else {
        // write info
        WRITE_MESSAGE(TL("Loading of shape file sucessfully: ") + file);
    }
    // end undoList operation
    undoList->end();
    // restore validation for additionals
    XMLSubSys::setValidation("auto", "auto", "auto");
    // update view
    myGNEApp->getViewNet()->update();
}

/****************************************************************************/
