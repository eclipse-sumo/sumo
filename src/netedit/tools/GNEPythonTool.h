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
/// @file    GNEPythonTool.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Python tools used in netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <utils/options/OptionsCont.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEPythonTool {

public:
    /// @brief Constructor
    GNEPythonTool(GNEApplicationWindow* applicationWindow, const std::string& toolPath,
                  const std::string& templateStr, FXMenuPane* menu);

    /// @brief destructor
    virtual ~GNEPythonTool();

    /// @brief get tool name
    const std::string& getToolName() const;

    /// @brief get tools options
    OptionsCont& getToolsOptions();

    /// @brief get menu command
    FXMenuCommand* getMenuCommand() const;

    /// @brief set current values (used for set values like current folder and similar)
    virtual void setCurrentValues();

    /// @brief execute post processing
    virtual void postProcessing();

    /// @brief get command (python + script + arguments)
    virtual std::string getCommand() const;

    /// @brief get the command that runs the tool in its selected directory
    std::string getRunCommand() const;

    /// @brief use the explicit working directory, or a writable default
    std::string getWorkingDirectory() const;

    /// @brief remember a working directory for this tool; empty selects the default
    void setWorkingDirectory(const std::string& directory);

    /// @brief choose a writable tool directory using the current network filename
    static std::string getDefaultWorkingDirectory(const std::string& network);

    /// @brief check whether a directory can accommodate new output files
    static bool isWritableDirectory(const std::string& directory);

    /// @brief validate the destinations of a declared output file option
    bool isWritableOutput(const std::string& name, const Option& option,
                                 const std::string& value, const std::string& directory) const;

    /// @brief resolve a relative tool output where the child writes it
    std::string resolveOutput(const std::string& path) const;

    /// @brief get default value of the given parameter
    const std::string getDefaultValue(const std::string& name) const;

    /// @brief load configuration
    bool loadConfiguration(const std::string& file);

    /// @brief save configuration
    void saveConfiguration(const std::string& file) const;

protected:
    /// @brief application window
    GNEApplicationWindow* myApplicationWindow = nullptr;

    /// @brief menu command associated with this tool
    FXMenuCommand* myMenuCommand = nullptr;

    /// @brief tools options
    OptionsCont myPythonToolsOptions;

    /// @brief original tools options
    OptionsCont myPythonToolsOptionsOriginal;

    /// @brief python tool path relative to SUMO_HOME
    const std::string myToolPath;

    /// @brief tool name
    const std::string myPythonToolName;

    /// @brief get command (python + script)
    std::string getCommandPath() const;

private:
    /// @brief editable per-tool directory override
    std::string myWorkingDirectory;

    /// @brief Invalidated copy constructor.
    GNEPythonTool(const GNEPythonTool&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPythonTool& operator=(const GNEPythonTool&) = delete;
};
