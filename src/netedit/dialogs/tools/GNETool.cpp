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
/// @file    GNETool.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Python tools used in netedit
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/FileHelpers.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/handlers/TemplateHandler.h>
#include <utils/common/MsgHandler.h>

#include "GNETool.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNETool::GNETool(GNEApplicationWindow* GNEApp, const std::string &pythonPath, 
        const std::string &templateStr, FXMenuPane* menu) :
    myGNEApp(GNEApp),
    myPythonPath(pythonPath),
    myToolName(FileHelpers::getFileFromPath(pythonPath, true)) {
    // build menu command
    myMenuCommand = GUIDesigns::buildFXMenuCommandShortcut(menu, myToolName, "", TL("Execute python tool '") + myToolName + "'.",
        GUIIconSubSys::getIcon(GUIIcon::TOOL_PYTHON), GNEApp, MID_GNE_OPENTOOLDIALOG);
    // parse tool options
    if (templateStr.size() > 0) {
        try {
            TemplateHandler::parseTemplate(myToolsOptions, templateStr);
        } catch (ProcessError&) {
            WRITE_ERROR("Error parsing template of tool: " + myToolName);
        }
    }

}


GNETool::~GNETool() {}


GNEApplicationWindow*
GNETool::getGNEApp() const {
    return myGNEApp;
}


const std::string&
GNETool::getToolName() const {
    return myToolName;
}


OptionsCont&
GNETool::getToolsOptions() {
    return myToolsOptions;
}


FXMenuCommand*
GNETool::getMenuCommand() const {
    return myMenuCommand;
}


std::string
GNETool::getCommand() const {
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
    for (const auto &option : myToolsOptions) {
        arguments += ("--" + option.first + " \"" + option.second->getValueString() + "\" ");
    }
    return command + " " + arguments;
}

/****************************************************************************/
