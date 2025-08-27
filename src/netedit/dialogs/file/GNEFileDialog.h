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
    /// @brief constructor
    GNEFileDialog(GNEApplicationWindow* applicationWindow, const std::string title, GUIIcon icon,
                  const std::vector<std::string>& extensions, const bool save, const bool multiElements);

    /// @brief destructor
    ~GNEFileDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// Change file name
    void setFilename(const FXString& path);

    /// Return file name, if any
    std::string getFilename() const;

    /// Return empty-string terminated list of selected file names, or NULL if none selected
    std::vector<std::string> getFilenames() const;

    /// Change file pattern
    void setPattern(const FXString& ptrn);

    /// Return file pattern
    FXString getPattern() const;

    /// @brief set current pattern
    void setCurrentPattern(FXint n);

    /// Return current pattern number
    FXint getCurrentPattern() const;

    /// Get pattern text for given pattern number
    FXString getPatternText(FXint patno) const;

    /// Change pattern text for pattern number
    void setPatternText(FXint patno, const FXString& text);

    /// Return number of patterns
    FXint getNumPatterns() const;

    /// Allow pattern entry
    void allowPatternEntry(FXbool allow);

    /// Return TRUE if pattern entry is allowed
    FXbool allowPatternEntry() const;

    /// Change directory
    void setDirectory(const FXString& path);

    /// Return directory
    FXString getDirectory() const;

    /// Set the inter-item spacing (in pixels)
    void setItemSpace(FXint s);

    /// Return the inter-item spacing (in pixels)
    FXint getItemSpace() const;

    /// Change wildcard matching mode
    void setMatchMode(FXuint mode);

    /// Return wildcard matching mode
    FXuint getMatchMode() const;

    /// Return TRUE if showing hidden files
    FXbool showHiddenFiles() const;

    /// Show or hide hidden files
    void showHiddenFiles(FXbool showing);

    /// Return TRUE if image preview on
    FXbool showImages() const;

    /// Show or hide preview images
    void showImages(FXbool showing);

    /// Return images preview size
    FXint getImageSize() const;

    /// Change images preview size
    void setImageSize(FXint size);

    /// Change File List style
    void setFileBoxStyle(FXuint style);

    /// Return File List style
    FXuint getFileBoxStyle() const;

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
