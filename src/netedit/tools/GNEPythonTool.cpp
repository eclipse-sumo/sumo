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

#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include <netedit/GNEApplicationWindow.h>
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
bool isToolFilename(const Option& option) {
    return option.isFileName() || option.isNetwork() || option.isAdditional()
           || option.isRoute() || option.isData() || option.isSumoConfig();
}


bool isSpecialToolPath(const std::string& path) {
    return path == "-" || path == "stdout" || path == "STDOUT" || path == "stderr"
           || path == "STDERR" || path == "nul" || path == "NUL" || path == "/dev/null";
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
            TemplateHandler::parseTemplate(myPythonToolsOptions, templateStr);
            // make a copy (needed for reset)
            TemplateHandler::parseTemplate(myPythonToolsOptionsOriginal, templateStr);
        } catch (ProcessError& e) {
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
    // add python script
    const char* pythonEnv = getenv("PYTHON");
    const std::string python = (pythonEnv == nullptr) ? "python" : pythonEnv;
    const char* sumoHomeEnv = getenv("SUMO_HOME");
    std::string sumoHome = "";
    if (sumoHomeEnv != nullptr && sumoHomeEnv != std::string("")) {
        sumoHome = std::string(sumoHomeEnv);
        // harmonise slash
        if (sumoHome.back() == '\\') {
            sumoHome.pop_back();
        }
        if (sumoHome.back() != '/') {
            sumoHome += "/";
        }
        // quote string to handle spaces but prevent double quotes
        if (sumoHome.front() != '"') {
            sumoHome = "\"" + sumoHome;
        }
        if (sumoHome.back() == '"') {
            sumoHome.pop_back();
        }
    }
    return python + " " + sumoHome + myToolPath + "\"";
}


std::string
GNEPythonTool::getCommandFilename(const std::string& filename, bool relativePaths) {
    std::string value = filename;
    if (relativePaths && FileHelpers::isAbsolute(filename) && !FileHelpers::isSocket(filename)) {
        std::error_code error;
        const auto directory = std::filesystem::current_path(error);
        if (!error) {
            const auto relative = std::filesystem::u8path(filename).lexically_relative(directory);
            if (!relative.empty() && relative != "." && std::find(relative.begin(), relative.end(), "..") == relative.end()) {
                value = relative.generic_u8string();
                // A relative filename must not become an option or a special output destination.
                if (value.front() == '-' || isSpecialToolPath(value)) {
                    value = "./" + value;
                }
            }
        }
    }
    return "\"" + StringUtils::escapeShell(value) + "\"";
}


std::string
GNEPythonTool::getCommand(bool relativePaths) const {
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
                const bool filename = isToolFilename(*option.second);
                const std::string listSeparator = option.second->getListSeparator();
                if (listSeparator != "") {
                    StringVector values;
                    if (filename && listSeparator == " ") {
                        std::istringstream input(option.second->getValueString());
                        std::string file;
                        while (input >> std::quoted(file)) {
                            values.push_back(file);
                        }
                    } else {
                        values = StringTokenizer(option.second->getValueString(), filename ? listSeparator : " ", true).getVector();
                    }
                    bool first = true;
                    for (const std::string& v : values) {
                        if (first) {
                            first = false;
                        } else {
                            arguments += listSeparator;
                        }
                        arguments += filename ? getCommandFilename(v, relativePaths) : "\"" + v + "\"";
                    }
                    arguments += " ";
                } else if (filename) {
                    arguments += getCommandFilename(option.second->getValueString(), relativePaths) + " ";
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
    if (handler.usesRelativePaths()) {
        for (const auto& option : myPythonToolsOptions) {
            Option& value = *option.second;
            if (!value.isWriteable() && isToolFilename(value)) {
                const auto& separator = value.getListSeparator();
                auto files = separator.empty() ? StringVector{value.getValueString()}
                             : StringTokenizer(value.getValueString(), ",", true).getVector();
                for (auto& fileName : files) {
                    if (!fileName.empty() && !isSpecialToolPath(fileName) && !StringUtils::startsWith(fileName, "${")) {
                        fileName = FileHelpers::checkForRelativity(fileName, file);
                    }
                }
                if (separator == " ") {
                    for (auto& fileName : files) {
                        std::ostringstream quoted;
                        quoted << std::quoted(fileName);
                        fileName = quoted.str();
                    }
                }
                const auto path = joinToString(files, separator);
                value.resetWritable();
                value.set(path, path, false);
            }
        }
    }
    // write info
    WRITE_MESSAGE(TLF("Loaded % configuration.", myPythonToolName));
    return true;
}


void
GNEPythonTool::saveConfiguration(const std::string& file) const {
    std::string command = getCommand(false) + " --relative-toolcfg -C \"" + file + "\" ";
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
