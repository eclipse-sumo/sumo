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
/// @file    GNEPythonTool.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Python tools used in netedit
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEExternalRunner.h>
#include <netedit/dialogs/basic/GNEErrorBasicDialog.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SysUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/handlers/TemplateHandler.h>
#include <utils/options/OptionsLoader.h>
#include <xercesc/parsers/SAXParser.hpp>

#include "GNEPythonTool.h"

namespace {

std::string
pythonCommand() {
    const char* configured = getenv("PYTHON");
    const std::string python = configured == nullptr || *configured == '\0' ? "python" : configured;
    return python.find(' ') != std::string::npos && FileHelpers::isReadable(python) ? "\"" + python + "\"" : python;
}


std::string
toolFileArgument(const std::string& path) {
    const char* home = getenv("SUMO_HOME");
    std::string directory = home == nullptr ? "" : home;
    if (directory.size() >= 2 && directory.front() == '"' && directory.back() == '"') {
        directory = directory.substr(1, directory.size() - 2);
    }
    if (!directory.empty()) {
        if (directory.back() == '\\') {
            directory.back() = '/';
        } else if (directory.back() != '/') {
            directory += '/';
        }
    }
    return "\"" + directory + path + "\"";
}

}

// ============================================-===============================
// member method definitions
// ===========================================================================

GNEPythonTool::GNEPythonTool(GNEApplicationWindow* applicationWindow, const std::string& toolPath,
                             const std::string& templateStr, FXMenuPane* menu) :
    myApplicationWindow(applicationWindow),
    myToolPath(toolPath),
    myPythonToolName(FileHelpers::getFileFromPath(toolPath, true)) {
    // build menu command
    myMenuCommand = GUIDesigns::buildFXMenuCommandShortcut(menu, myPythonToolName, "", TL("Execute python tool '") + myPythonToolName + "'.",
                    GUIIconSubSys::getIcon(GUIIcon::TOOL_PYTHON), applicationWindow, MID_GNE_OPENPYTHONTOOLDIALOG);
    // parse tool options
    if (templateStr.size() > 0) {
        try {
            parseToolTemplate(templateStr);
        } catch (ProcessError& e) {
            myPythonToolsOptions.clear();
            myPythonToolsOptionsOriginal.clear();
            WRITE_ERROR("Error parsing template of tool: " + myPythonToolName + " (" + e.what() + ")");
        }
    }
}


GNEPythonTool::~GNEPythonTool() {}


const std::string&
GNEPythonTool::getToolName() const {
    return myPythonToolName;
}


OptionsCont&
GNEPythonTool::getToolsOptions() {
    return myPythonToolsOptions;
}


void
GNEPythonTool::parseToolTemplate(const std::string& templateStr) {
    myPythonToolsOptions.clear();
    myPythonToolsOptionsOriginal.clear();
    TemplateHandler::parseTemplate(myPythonToolsOptions, templateStr);
    TemplateHandler::parseTemplate(myPythonToolsOptionsOriginal, templateStr);
    myTemplateLoaded = true;
}


bool
GNEPythonTool::ensureTemplate() {
    if (myTemplateLoaded || myPythonToolsOptionsOriginal.begin() != myPythonToolsOptionsOriginal.end()) {
        return true;
    }
    const std::string command = pythonCommand() + " " + toolFileArgument("tools/neteditToolTemplate.py")
                                + " " + toolFileArgument(myToolPath);
    std::string output;
    if (GNEExternalRunner::captureOutput(command, output)) {
        try {
            parseToolTemplate(output);
            return true;
        } catch (const ProcessError& e) {
            myPythonToolsOptions.clear();
            myPythonToolsOptionsOriginal.clear();
            output = e.what();
        }
    }
    GNEErrorBasicDialog(myApplicationWindow, TL("Could not load Python tool options"),
                        TLF("Could not load options for '%'.", myPythonToolName), output);
    return false;
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
GNEPythonTool::getCommandPath() const {
    return pythonCommand() + " " + toolFileArgument(myToolPath);
}


std::string
GNEPythonTool::getCommand() const {
    std::string arguments;
    // add arguments
    for (const auto& option : myPythonToolsOptions) {
        // only add modified values
        if (!option.second->isDefault()) {
            // for boolean values avoid use "true"
            if (option.second->isBool()) {
                if (option.second->getBool()) {
                    arguments += ("--" + option.first + " ");
                } else {
                    arguments += ("--no-" + option.first + " ");
                }
            } else {
                if (!option.second->isPositional()) {
                    arguments += ("--" + option.first + " ");
                }
                const std::string listSeparator = option.second->getListSeparator();
                if (listSeparator != "") {
                    StringTokenizer st(option.second->getValueString(), " ", true);
                    bool first = true;
                    for (const std::string& v : st.getVector()) {
                        if (first) {
                            first = false;
                        } else {
                            arguments += listSeparator;
                        }
                        arguments += ("\"" + v + "\"");
                    }
                    arguments += " ";
                } else {
                    arguments += ("\"" + StringUtils::escapeShell(option.second->getValueString()) + "\" ");
                }
            }
        }
    }
    return getCommandPath() + " " + arguments;
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
    // make all options writable
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
        parser.parse(XMLSubSys::transcodeToLocal(file).c_str());
        if (handler.errorOccurred()) {
            WRITE_ERROR(TL("Could not load tool configuration '") + file + "'.");
            return false;
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        WRITE_ERROR(TL("Could not load tool configuration '") + file + "':\n " + XMLSubSys::transcode(e.getMessage()));
        return false;
    }
    // write info
    WRITE_MESSAGE(TLF("Loaded % configuration.", myPythonToolName));
    return true;
}


void
GNEPythonTool::saveConfiguration(const std::string& file) const {
    std::string command = getCommand() + " -C \"" + file + "\" ";
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
