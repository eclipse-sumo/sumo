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
    GNEPythonTool(GNEApplicationWindow* GNEApp, const std::string& pythonPath,
                  const std::string& templateStr, FXMenuPane* menu);

    /// @brief destructor
    virtual ~GNEPythonTool();

    /// @brief get to GNEApplicationWindow
    GNEApplicationWindow* getGNEApp() const;

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

    /// @brief get default value of the given parameter
    const std::string getDefaultValue(const std::string& name) const;

    /// @brief load configuration
    bool loadConfiguration(const std::string& file);

    /// @brief save configuration
    void saveConfiguration(const std::string& file) const;

protected:
    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEApp;

    /// @brief menu command associated with this tool
    FXMenuCommand* myMenuCommand;

    /// @brief tools options
    OptionsCont myPythonToolsOptions;

    /// @brief original tools options
    OptionsCont myPythonToolsOptionsOriginal;

    /// @brief python path
    const std::string myPythonPath;

    /// @brief tool name
    const std::string myPythonToolName;

private:
    /// @brief Invalidated copy constructor.
    GNEPythonTool(const GNEPythonTool&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPythonTool& operator=(const GNEPythonTool&) = delete;
};

