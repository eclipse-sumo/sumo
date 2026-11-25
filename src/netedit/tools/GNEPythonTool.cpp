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

#include <filesystem>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <netedit/GNEApplicationWindow.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileBucket.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SysUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/handlers/TemplateHandler.h>
#include <utils/options/OptionsLoader.h>
#include <xercesc/parsers/SAXParser.hpp>

#include "GNEPythonTool.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

namespace {

namespace fs = std::filesystem;

std::string
pathString(const fs::path& path) {
    const auto result = path.generic_u8string();
    return std::string(result.begin(), result.end());
}


std::string
currentWorkingDirectory() {
    std::error_code error;
    return pathString(fs::current_path(error));
}


bool
canWrite(const fs::path& path, bool directory) {
#ifdef WIN32
    const HANDLE output = CreateFileW(path.c_str(), directory ? FILE_ADD_FILE : FILE_WRITE_DATA,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                                      OPEN_EXISTING, directory ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL, nullptr);
    if (output == INVALID_HANDLE_VALUE) {
        return false;
    }
    CloseHandle(output);
    return true;
#else
    return access(path.c_str(), directory ? W_OK | X_OK : W_OK) == 0;
#endif
}


const char*
pythonCommand() {
    const char* pythonEnv = getenv("PYTHON");
    return pythonEnv == nullptr ? "python" : pythonEnv;
}

}


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
    return std::string(pythonCommand()) + " " + sumoHome + myToolPath + "\"";
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


std::string
GNEPythonTool::getRunCommand() const {
    const auto command = getCommand();
    const auto directory = getWorkingDirectory();
    if (directory == currentWorkingDirectory()) {
        return command;
    }
    // Pass the directory as hex so the legacy shell launcher does not expand path characters.
    const char* digits = "0123456789abcdef";
    std::string encoded;
    encoded.reserve(directory.size() * 2);
    for (const unsigned char c : directory) {
        encoded += digits[c >> 4];
        encoded += digits[c & 15];
    }
    const std::string python = pythonCommand();
    return python + " -c \"import os,runpy,sys; "
           "directory=bytes.fromhex(sys.argv[1]).decode('utf-8'); sys.argv=sys.argv[2:]; "
           "sys.argv[0]=os.path.abspath(sys.argv[0]); "
           "sys.path[:1]=sys.path[:1] if sys.flags.isolated or getattr(sys.flags,'safe_path',False) "
           "else [os.path.dirname(sys.argv[0])]; "
           "os.chdir(directory); runpy.run_path(sys.argv[0],run_name='__main__')\" "
           + encoded + command.substr(python.size());
}


std::string
GNEPythonTool::getWorkingDirectory() const {
    if (!myWorkingDirectory.empty()) {
        std::error_code error;
        const auto directory = fs::absolute(fs::u8path(myWorkingDirectory), error);
        return error ? myWorkingDirectory : pathString(directory);
    }
    const auto network = myApplicationWindow != nullptr && myApplicationWindow->getViewNet() != nullptr
                         ? myApplicationWindow->getFileBucketHandler()->getDefaultFilename(FileBucket::Type::NETWORK)
                         : std::string();
    return getDefaultWorkingDirectory(network);
}


void
GNEPythonTool::setWorkingDirectory(const std::string& directory) {
    myWorkingDirectory = directory;
}


std::string
GNEPythonTool::getDefaultWorkingDirectory(const std::string& network) {
    const auto current = currentWorkingDirectory();
    if (!network.empty() && !isWritableDirectory(current)) {
        const auto file = fs::u8path(current) / fs::u8path(network);
        const auto directory = pathString(file.parent_path());
        if (isWritableDirectory(directory)) {
            return directory;
        }
    }
    return current;
}


bool
GNEPythonTool::isWritableDirectory(const std::string& directory) {
    const auto file = fs::u8path(directory);
    std::error_code error;
    return fs::is_directory(file, error) && canWrite(file, true);
}


bool
GNEPythonTool::isWritableOutput(const std::string& name, const Option& option,
                               const std::string& value, const std::string& directory) const {
    if (option.getSubTopic() != "output" || option.isBool() || option.isInteger() || option.isFloat()
            || value.empty()) {
        return true;
    }
    // These output options describe the content instead of naming a destination.
    if ((myPythonToolName == "plotFlows" && name == "extension")
            || (myPythonToolName == "osmTaxiStop" && name == "type")) {
        return true;
    }
    // macrOutput writes plots into a directory. An existing file selects its parent.
    if (myPythonToolName == "macrOutput" && name == "output") {
        const auto output = fs::u8path(directory) / fs::u8path(value);
        std::error_code error;
        const auto target = fs::is_regular_file(output, error) ? output.parent_path() : output;
        return isWritableDirectory(pathString(target));
    }
    const bool outputDirectory = name == "output-dir"
                                 || (myPythonToolName == "aggregateAndSplitMatrices" && name == "dir");
    const bool createsDirectory = myPythonToolName == "createScreenshotSequence";
    const bool prefix = name == "outprefix" || name == "prefix" || StringUtils::endsWith(name, "-prefix")
                        || (myPythonToolName == "vissim_parseRoutes" && name == "output")
                        || (myPythonToolName == "duaIterate_analysis" && (name == "output" || name == "teleports" || name == "mpl"))
                        || (myPythonToolName == "plotFlows" && name == "csv-output");
    const auto separator = option.getListSeparator();
    const auto paths = separator.empty() ? std::vector<std::string>{value} : StringTokenizer(value, separator).getVector();
    auto base = fs::u8path(directory);
    if ((myPythonToolName == "osmGet" && name == "prefix")
            || (myPythonToolName == "tileGet" && (name == "prefix" || name == "decals-file"))) {
        base /= fs::u8path(myPythonToolsOptions.getValueString("output-dir"));
    }
    for (const auto& path : paths) {
        if (myPythonToolName == "duaIterate_analysis" && name == "mpl" && path == "SHOW") {
            continue;
        }
        const auto file = base / fs::u8path(path);
        if (outputDirectory) {
            if (isWritableDirectory(pathString(file))) {
                continue;
            }
            std::error_code error;
            const auto status = fs::status(file, error);
            if (!createsDirectory || status.type() != fs::file_type::not_found
                    || !isWritableDirectory(pathString(file.parent_path()))) {
                return false;
            }
        } else if (prefix) {
            if (!isWritableDirectory(pathString(file.parent_path()))) {
                return false;
            }
        } else {
            std::error_code error;
            const auto status = fs::status(file, error);
            if (fs::exists(status)) {
                if (!fs::is_regular_file(status) || !canWrite(file, false)) {
                    return false;
                }
            } else if (status.type() != fs::file_type::not_found || !isWritableDirectory(pathString(file.parent_path()))) {
                return false;
            }
        }
    }
    return true;
}


std::string
GNEPythonTool::resolveOutput(const std::string& path) const {
    return pathString(fs::u8path(getWorkingDirectory()) / fs::u8path(path));
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
