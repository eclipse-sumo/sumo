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
/// @file    GNEFileSelector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// widget used for file selection
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <utils/foxtools/fxheader.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFileSelector : public FXPacker {
    /// @brief FOX declaration
    FXDECLARE(GNEFileSelector)

public:
    /// @brief file selection modes
    enum class SelectMode {
        SAVE,           // A single file, existing or not (to save to)
        EXISTING,       // An existing file (to load)
        MULTIPLE,       // Multiple existing files
        MULTIPLE_ALL,   // Multiple existing files or directories, but not '.' and '..'
        DIRECTORY       // Existing directory, including '.' or '..'
    };

    /// @brief Constructor
    GNEFileSelector(FXComposite* p, const std::vector<std::string>& extensions,
                    const bool save, const bool multiElements);

    /// @brief Destructor
    virtual ~GNEFileSelector();

    /// @brief Change file name
    void setFilename(const FXString& path);

    /// @brief Return file name, if any
    std::string getFilename() const;

    /// @brief get file names
    std::vector<std::string> getFilenames() const;

    /// Change file pattern
    void setPattern(const FXString& ptrn);

    /// Return file pattern
    FXString getPattern() const;

    /// @brief set current pattern
    void setCurrentPattern(FXint n);

    /// @brief Return current pattern number
    FXint getCurrentPattern() const;

    /// @brief Get pattern text for given pattern number
    FXString getPatternText(FXint patno) const;

    /// @brief Change pattern text for pattern number
    void setPatternText(FXint patno, const FXString& text);

    /// @brief Return number of patterns
    FXint getNumPatterns() const;

    /// @brief Allow pattern entry
    void allowPatternEntry(FXbool allow);

    /// @brief Return TRUE if pattern entry is allowed
    FXbool allowPatternEntry() const;

    /// @brief Change directory
    void setDirectory(const FXString& path);

    /// @brief Return directory
    FXString getDirectory() const;

    /// @brief Set the inter-item spacing (in pixels)
    void setItemSpace(FXint s);

    /// @brief Return the inter-item spacing (in pixels)
    FXint getItemSpace() const;

    /// @brief Change file list style
    void setFileBoxStyle(FXuint style);

    /// @brief Return file list style
    FXuint getFileBoxStyle() const;

    ///@brief  Change wildcard matching mode
    void setMatchMode(FXuint mode);

    /// @brief Return wildcard matching mode
    FXuint getMatchMode() const;

    /// @brief Return TRUE if showing hidden files
    FXbool showHiddenFiles() const;

    /// @brief Show or hide hidden files
    void showHiddenFiles(FXbool showing);

    /// @brief Return TRUE if image preview on
    FXbool showImages() const;

    /// @brief Show or hide preview images
    void showImages(FXbool showing);

    /// @brief Return images preview size
    FXint getImageSize() const;

    /// @brief Change images preview size
    void setImageSize(FXint size);

    /// @name list of FOX handlers
    /// @{

    /// @brief Handler for accepting a command.
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief Handler for filtering items.
    long onCmdFilter(FXObject*, FXSelector, void*);

    /// @brief Handler for double-clicking an item.
    long onCmdItemDblClicked(FXObject*, FXSelector, void*);

    /// @brief Handler for selecting an item.
    long onCmdItemSelected(FXObject*, FXSelector, void*);

    /// @brief Handler for deselecting an item.
    long onCmdItemDeselected(FXObject*, FXSelector, void*);

    /// @brief Handler for moving up one directory.
    long onCmdDirectoryUp(FXObject*, FXSelector, void*);

    /// @brief Update handler for enabling/disabling directory up command.
    long onUpdDirectoryUp(FXObject*, FXSelector, void*);

    /// @brief Handler for directory tree navigation.
    long onCmdDirTree(FXObject*, FXSelector, void*);

    /// @brief Handler for navigating to the home directory.
    long onCmdHome(FXObject*, FXSelector, void*);

    /// @brief Handler for navigating to the work directory.
    long onCmdWork(FXObject*, FXSelector, void*);

    /// @brief Handler for opening myBookmarsRecentFiles.
    long onCmdBookmark(FXObject*, FXSelector, void*);

    /// @brief Handler for visiting a bookmarked location.
    long onCmdVisit(FXObject*, FXSelector, void*);

    /// @brief Handler for creating a new item or directory.
    long onCmdNew(FXObject*, FXSelector, void*);

    /// @brief Update handler for enabling/disabling new command.
    long onUpdNew(FXObject*, FXSelector, void*);

    /// @brief Handler for moving an item.
    long onCmdMove(FXObject*, FXSelector, void*);

    /// @brief Handler for copying an item.
    long onCmdCopy(FXObject*, FXSelector, void*);

    /// @brief Handler for creating a link to an item.
    long onCmdLink(FXObject*, FXSelector, void*);

    /// @brief Handler for deleting an item.
    long onCmdDelete(FXObject*, FXSelector, void*);

    /// @brief Update handler for enabling/disabling commands based on selection.
    long onUpdSelected(FXObject*, FXSelector, void*);

    /// @brief Handler for showing the popup menu.
    long onPopupMenu(FXObject*, FXSelector, void*);

    /// @brief Handler for showing or calculating image size.
    long onCmdImageSize(FXObject*, FXSelector, void*);

    /// @brief Update handler for enabling/disabling image size command.
    long onUpdImageSize(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEFileSelector)

    /// @brief File list widget
    FXFileList* myFileSelector = nullptr;

    /// @brief File name entry field
    FXTextField* myFilenameTextField = nullptr;

    /// @brief Combobox for pattern list
    FXComboBox* myFileFilterComboBox = nullptr;

    /// @brief Menu for myBookmarsRecentFiles
    FXMenuPane* myBookmarkMenuPane = nullptr;

    /// @brief Directory hierarchy list
    FXDirBox* myDirBox = nullptr;

    /// @brief Accept button (temporal)
    FXButton* accept = nullptr;

    /// @brief Cancel button (temporal)
    FXButton* cancel = nullptr;

    /// @brief Bookmarked places
    FXRecentFiles myBookmarsRecentFiles = nullptr;

    /// @brief Select mode
    SelectMode mySelectmode = SelectMode::SAVE;

    /// @brief get selected files
    std::vector<std::string> getSelectedFiles() const;

    /// @brief get selected files that are not directories
    std::vector<std::string> getSelectedFilesOnly() const;

private:
    /// @brief disable copy constructor
    GNEFileSelector(const GNEFileSelector&) = delete;

    /// @brief disable assignment operator
    GNEFileSelector& operator=(const GNEFileSelector&) = delete;
};

/****************************************************************************/
