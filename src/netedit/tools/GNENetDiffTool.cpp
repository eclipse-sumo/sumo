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
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SysUtils.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/handlers/TemplateHandler.h>
#include <utils/options/OptionsLoader.h>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include "GNENetDiffTool.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNENetDiffTool::GNENetDiffTool(GNEApplicationWindow* GNEApp, const std::string &pythonPath, FXMenuPane* menu) :
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


std::string
GNENetDiffTool::getCommand() const {
    // add python script
    const char* pythonEnv = getenv("PYTHON");
    const std::string python = (pythonEnv == nullptr)? "python" : pythonEnv;
    const char* sumoHomeEnv = getenv("SUMO_HOME");
    const std::string sumoHome = (sumoHomeEnv == nullptr)? "" : sumoHomeEnv + std::string("/");
    // get command
    std::string command = python + " " + sumoHome + myPythonPath;
    // declare arguments
    std::string arguments;
    // add arguments
    arguments += (myPythonToolsOptions.getString("original-net") + " ");
    arguments += (myPythonToolsOptions.getString("modified-net") + " ");
    arguments += myPythonToolsOptions.getString("outprefix") + " ";
    // check if save shapes
    if (myPythonToolsOptions.getBool("load-shapes-modified") ||
        myPythonToolsOptions.getBool("load-shapes-added") ||
        myPythonToolsOptions.getBool("load-shapes-added")) {
        arguments += "--write-shapes";
    }

    return command + " " + arguments;
}


void
GNENetDiffTool::fillNetDiffOptions(OptionsCont &options) {
    // build custom options for netdiff
    options.addOptionSubTopic("Input");
    options.doRegister("original-net", new Option_Network());
    options.addDescription("original-net", "Input", TL("Original network"));

    options.doRegister("modified-net", new Option_Network());
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

/****************************************************************************/
