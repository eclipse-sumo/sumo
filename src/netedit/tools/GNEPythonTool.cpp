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
/// @file    GNEPythonTool.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Python tools used in netedit
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SysUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/handlers/TemplateHandler.h>
#include <utils/options/OptionsLoader.h>
#include <xercesc/parsers/SAXParser.hpp>

#include "GNEPythonTool.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNEPythonTool::GNEPythonTool(GNEApplicationWindow* GNEApp, const std::string& pythonPath,
                             const std::string& templateStr, FXMenuPane* menu) :
    myGNEApp(GNEApp),
    myPythonPath(pythonPath),
    myPythonToolName(FileHelpers::getFileFromPath(pythonPath, true)) {
    // build menu command
    myMenuCommand = GUIDesigns::buildFXMenuCommandShortcut(menu, myPythonToolName, "", TL("Execute python tool '") + myPythonToolName + "'.",
                    GUIIconSubSys::getIcon(GUIIcon::TOOL_PYTHON), GNEApp, MID_GNE_OPENPYTHONTOOLDIALOG);
    // parse tool options
    if (templateStr.size() > 0) {
        try {
            TemplateHandler::parseTemplate(myPythonToolsOptions, templateStr);
            // make a copy (needed for reset)
            TemplateHandler::parseTemplate(myPythonToolsOptionsOriginal, templateStr);
        } catch (ProcessError& e) {
            WRITE_ERROR("Error parsing template of tool: " + myPythonToolName + " (" + e.what() + ")");
        }
    }
}


GNEPythonTool::~GNEPythonTool() {}


GNEApplicationWindow*
GNEPythonTool::getGNEApp() const {
    return myGNEApp;
}


const std::string&
GNEPythonTool::getToolName() const {
    return myPythonToolName;
}


OptionsCont&
GNEPythonTool::getToolsOptions() {
    return myPythonToolsOptions;
}


FXMenuCommand*
GNEPythonTool::getMenuCommand() const {
    return myMenuCommand;
}


void
GNEPythonTool::setCurrentValues() {
    // nothing to do here, use in children
}


void
GNEPythonTool::postProcessing() {
    // nothing to do here, use in children
}


std::string
GNEPythonTool::getCommand() const {
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
    for (const auto& option : myPythonToolsOptions) {
        // only add modified values
        if (!option.second->isDefault()) {
            // for boolean values avoid use "true"
            if (option.second->isBool()) {
                arguments += ("--" + option.first + " ");
            } else { 
                if (!option.second->isPositional()) {
                    arguments += ("--" + option.first + " ");
                }
                const std::string sep = option.second->getListSeparator();
                if (sep != "") {
                    StringTokenizer st(option.second->getValueString(), " ", true);
                    bool first = true;
                    for (const std::string& v : st.getVector()) {
                        if (first) {
                            first = false;
                        } else {
                            arguments += sep;
                        }
                        arguments += ("\"" + v + "\"");
                    }
                    arguments += " ";
                } else {
                    arguments += ("\"" + option.second->getValueString() + "\" ");
                }
            }
        }
    }
    return command + " " + arguments;
}


const std::string
GNEPythonTool::getDefaultValue(const std::string& name) const {
    const auto value = myPythonToolsOptionsOriginal.getValueString(name);
    // filter "none" values
    if (value == "none") {
        return "";
    } else {
        return value;
    }
}


bool
GNEPythonTool::loadConfiguration(const std::string& file) {
    // make all options writables
    myPythonToolsOptions.resetWritable();
    // build parser
    XERCES_CPP_NAMESPACE::SAXParser parser;
    parser.setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Never);
    parser.setDisableDefaultEntityResolution(true);
    // start the parsing
    OptionsLoader handler(myPythonToolsOptions);
    try {
        parser.setDocumentHandler(&handler);
        parser.setErrorHandler(&handler);
        parser.parse(StringUtils::transcodeToLocal(file).c_str());
        if (handler.errorOccurred()) {
            WRITE_ERROR(TL("Could not load netedit configuration '") + file + "'.");
            return false;
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        WRITE_ERROR(TL("Could not load netedit configuration '") + file + "':\n " + StringUtils::transcode(e.getMessage()));
        return false;
    }
    // write info
    WRITE_MESSAGE(TLF("Loaded % configuration.", myPythonToolName));
    return true;
}


void
GNEPythonTool::saveConfiguration(const std::string& file) const {
    // add python script
    const char* pythonEnv = getenv("PYTHON");
    const std::string python = (pythonEnv == nullptr) ? "python" : pythonEnv;
    const char* sumoHomeEnv = getenv("SUMO_HOME");
    const std::string sumoHome = (sumoHomeEnv == nullptr) ? "" : sumoHomeEnv + std::string("/");
    // get command
    std::string command = python + " " + sumoHome + myPythonPath + " -C " + file + " ";
    // add arguments
    for (const auto& option : myPythonToolsOptions) {
        // only write modified values
        if (!option.second->isDefault()) {
            command += ("--" + option.first + " \"" + option.second->getValueString() + "\" ");
        }
    }
    // start in background
#ifndef WIN32
    command = command + " &";
#else
    // see "help start" for the parameters
    command = "start /B \"\" " + command;
#endif
    // write info
    WRITE_MESSAGE(TLF("Saved % configuration.", myPythonToolName));
    // yay! fun with dangerous commands... Never use this over the internet
    SysUtils::runHiddenCommand(command);
}

/****************************************************************************/
