/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEFileDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Dialog used for opening/saving files
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFileSelector;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFileDialog : public GNEDialog {

public:
    /// @brief file open mode
    enum class OpenMode {
        SAVE,           // A single file, existing or not (to save to)
        LOAD_SINGLE,    // An existing file (to load)
        LOAD_MULTIPLE,  // Multiple existing files
        LOAD_DIRECTORY  // Existing directory, including '.' or '..'
    };

    /// @brief constructor
    GNEFileDialog(GNEApplicationWindow* applicationWindow, const std::string title, GUIIcon icon,
                  const std::vector<std::string>& extensions, GNEFileDialog::OpenMode openMode);

    /// @brief destructor
    ~GNEFileDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @brief Change file name
    void setFilename(const FXString& path);

    /// @brief Return file name, if any
    std::string getFilename() const;

    /// @brief Return empty-string terminated list of selected file names, or NULL if none selected
    std::vector<std::string> getFilenames() const;

    /// @brief Return directory
    std::string getDirectory() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief called when accept or yes button is pressed (can be reimplemented in children)
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief the file selector widget
    GNEFileSelector* myFileSelector;

private:
    /// @brief invalidate copy constructor
    GNEFileDialog(const GNEFileDialog&) = delete;

    /// @brief invalidate assignment operator
    GNEFileDialog& operator=(const GNEFileDialog&) = delete;
};
