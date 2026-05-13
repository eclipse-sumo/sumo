/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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

GNENetDiffTool::GNENetDiffTool(GNEApplicationWindow* applicationWindow, const std::string& toolPath, FXMenuPane* menu) :
    GNEPythonTool(applicationWindow, toolPath, "", menu) {
    // fill options
    fillNetDiffOptions(myPythonToolsOptions);
    fillNetDiffOptions(myPythonToolsOptionsOriginal);
}


GNENetDiffTool::~GNENetDiffTool() {}


void
GNENetDiffTool::setCurrentValues() {
    myPythonToolsOptions.resetWritable();
    // obtain curren network folder
    const auto networkPath = OptionsCont::getOptions().getString("sumo-net-file");
    if (networkPath.empty()) {
        myPythonToolsOptions.set("outprefix", "");
    } else {
        myPythonToolsOptions.set("outprefix", FileHelpers::getFilePath(networkPath) + "diff");
    }
}


void
GNENetDiffTool::postProcessing() {
    // first check if there is a network
    if (myApplicationWindow->getViewNet()) {
        // get selector operator modul from selector frame
        auto selectorModul = myApplicationWindow->getViewNet()->getViewParent()->getSelectorFrame()->getSelectionOperationModul();
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
            loadShapes(myPythonToolsOptions.getString("outprefix") + ".changed.shape.add.xml");
        }
        if (myPythonToolsOptions.getBool("load-shapes-added")) {
            loadShapes(myPythonToolsOptions.getString("outprefix") + ".created.shape.add.xml");
        }
        if (myPythonToolsOptions.getBool("load-shapes-deleted")) {
            loadShapes(myPythonToolsOptions.getString("outprefix") + ".deleted.shape.add.xml");
        }
    }
}


std::string
GNENetDiffTool::getCommand() const {
    std::string arguments;
    // add arguments
    arguments += "\"" + myPythonToolsOptions.getString("original-net") + "\" ";
    arguments += "\"" + myPythonToolsOptions.getString("modified-net") + "\" ";
    arguments += "\"" + myPythonToolsOptions.getString("outprefix")    + "\" ";
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
    // add rest of arguments
    if (myPythonToolsOptions.getBool("load-shapes-modified")) {
        arguments += "--write-shapes ";
    }
    if (myPythonToolsOptions.getBool("write-selections")) {
        arguments += "--write-selections ";
    }
    if (myPythonToolsOptions.getBool("plain-geo")) {
        arguments += "--plain-geo ";
    }
    if (myPythonToolsOptions.getBool("verbose")) {
        arguments += "--verbose ";
    }
    if (myPythonToolsOptions.getBool("use-prefix")) {
        arguments += "--use-prefix ";
    }
    if (myPythonToolsOptions.getBool("direct")) {
        arguments += "--direct ";
    }
    if (myPythonToolsOptions.getBool("patch-on-import")) {
        arguments += "--patch-on-import ";
    }
    if (myPythonToolsOptions.getBool("remove-plain")) {
        arguments += "--remove-plain ";
    }
    if (myPythonToolsOptions.getString("copy").size() > 0) {
        arguments += "--copy " + myPythonToolsOptions.getString("copy") + " ";
    }
    if (myPythonToolsOptions.getString("path").size() > 0) {
        arguments += "--path " + myPythonToolsOptions.getString("path") + " ";
    }
    return getCommandPath() + " " + arguments;
}


void
GNENetDiffTool::fillNetDiffOptions(OptionsCont& options) {
    // build custom options for netdiff
    options.addOptionSubTopic("input");
    options.doRegister("original-net", new Option_Network(""));
    options.addDescription("original-net", "input", TL("Original network"));

    options.doRegister("modified-net", new Option_Network(""));
    options.addDescription("modified-net", "input", TL("Modified network"));

    options.addOptionSubTopic("output");
    options.doRegister("outprefix", new Option_FileName());
    options.addDescription("outprefix", "output", TL("Output prefix network"));

    options.doRegister("write-selections", new Option_Bool(false));
    options.addDescription("write-selections", "output", TL("Write selection files for created, deleted and changed elements"));

    options.doRegister("write-shapes", new Option_Bool(false));
    options.addDescription("write-shapes", "output", TL("Write shape files for created, deleted and changed elements"));

    options.doRegister("plain-geo", new Option_Bool(false));
    options.addDescription("plain-geo", "output", TL("Write geo coordinates instead of network coordinates"));

    options.addOptionSubTopic("Processing");
    options.doRegister("verbose", new Option_Bool(false));
    options.addDescription("verbose", "Processing", TL("Give more output"));

    options.doRegister("use-prefix", new Option_Bool(false));
    options.addDescription("use-prefix", "Processing", TL("Interpret source and dest as plain-xml prefix instead of network names"));

    options.doRegister("direct", new Option_Bool(false));
    options.addDescription("direct", "Processing", TL("Compare source and dest files directly"));

    options.doRegister("patch-on-import", new Option_Bool(false));
    options.addDescription("patch-on-import", "Processing", TL("Generate patch that can be applied during initial network import (exports additional connection elements)"));

    options.doRegister("remove-plain", new Option_Bool(false));
    options.addDescription("remove-plain", "Processing", TL("Avoid saving plain xml files of source and destination networks"));

    options.doRegister("copy", new Option_String());
    options.addDescription("copy", "Processing", TL("Comma-separated list of element names to copy (if they are unchanged)"));

    options.doRegister("path", new Option_String());
    options.addDescription("path", "Processing", TL("Path to binaries"));

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
    auto undoList = myApplicationWindow->getUndoList();
    // disable validation for additionals
    XMLSubSys::setValidation("never", "auto", "auto");
    // get (or create) bucket for this new file
    auto bucket = myApplicationWindow->getFileBucketHandler()->getBucket(FileBucket::Type::ADDITIONAL, file, true);
    // Create additional handler
    GNEGeneralHandler generalHandler(myApplicationWindow->getViewNet()->getNet(), bucket, myApplicationWindow->isUndoRedoAllowed());
    // begin undoList operation
    undoList->begin(Supermode::NETWORK, GUIIcon::SUPERMODENETWORK, TL("load shapes from '") + file + "'");
    // Run parser
    if (!generalHandler.parse()) {
        // write error
        WRITE_ERROR(TLF("Loading of shape file '%' failed", file));
    } else {
        // write info
        WRITE_MESSAGE(TLF("Loading of shape file '%' successfully", file));
    }
    // end undoList operation
    undoList->end();
    // restore validation for additionals
    XMLSubSys::setValidation("auto", "auto", "auto");
    // update view
    myApplicationWindow->getViewNet()->update();
}

/****************************************************************************/
