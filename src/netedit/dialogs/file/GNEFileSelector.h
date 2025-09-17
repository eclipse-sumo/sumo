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
/// @file    GNEFileSelector.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// widget used for file selection
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <utils/foxtools/fxheader.h>

#include "GNEFileDialog.h"

// ===========================================================================
// class declaration
// ===========================================================================

class MFXComboBoxIcon;
class MFXStaticToolTip;
class MFXTextFieldIcon;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFileSelector : public FXVerticalFrame {
    /// @brief FOX declaration
    FXDECLARE(GNEFileSelector)

public:
    /// @brief Constructor
    GNEFileSelector(GNEFileDialog* fileDialog, const std::vector<std::string>& extensions,
                    GNEFileDialog::OpenMode openMode, GNEFileDialog::ConfigType configType);

    /// @brief Destructor
    virtual ~GNEFileSelector();

    /// @brief set path (either file or directory, used for testing)
    void setPath(const std::string& path);

    /// @brief set file filter (used for testing)
    void setFilter(const int index);

    /// @brief Change file name
    void setFilename(const FXString& path);

    /// @brief Return file name, if any
    std::string getFilename() const;

    /// @brief get file names
    std::vector<std::string> getFilenames() const;

    /// @brief Return directory
    std::string getDirectory() const;

    /// @brief get file extension
    const std::vector<std::string>& getFileExtension() const;

    /// @brief Change directory
    void setDirectory(const FXString& path);

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

    /// @brief get file name entry field
    MFXTextFieldIcon* getFilenameTextField() const;

    /// @name list of FOX handlers
    /// @{

    /// @brief Handler for accepting a command.
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief Handler for filtering items.
    long onCmdFilter(FXObject*, FXSelector, void*);

    /// @brief Handler for double-clicking an item.
    long onCmdItemDoubleClicked(FXObject* obj, FXSelector sel, void* ptr);

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
    long onCmdHomeFolder(FXObject*, FXSelector, void*);

    /// @brief Handler for navigating to the work directory.
    long onCmdWorkFolder(FXObject*, FXSelector, void*);

    /// @brief Handler for navigating to the sumo/netedit config directory.
    long onCmdConfigFolder(FXObject*, FXSelector, void*);

    /// @brief Handler for opening myBookmarksRecentFiles.
    long onCmdBookmark(FXObject*, FXSelector, void*);

    /// @brief Handler for visiting a bookmarked location.
    long onCmdVisit(FXObject*, FXSelector, void*);

    /// @brief Handler for creating a new item or directory.
    long onCmdNewFolder(FXObject*, FXSelector, void*);

    /// @brief Update handler for enabling/disabling new command.
    long onUpdNewFolder(FXObject*, FXSelector, void*);

    /// @brief Handler for moving an item.
    long onCmdMove(FXObject*, FXSelector, void*);

    /// @brief Handler for copying an item.
    long onCmdCopy(FXObject*, FXSelector, void*);

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

    /// @brief called when user press a key
    long onKeyPress(FXObject* obj, FXSelector sel, void* ptr);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEFileSelector)

    /// @brief Pointer to parent file dialog
    GNEFileDialog* myFileDialog = nullptr;

    /// @brief open mode
    const GNEFileDialog::OpenMode myOpenMode = GNEFileDialog::OpenMode::SAVE;

    /// @brief config type
    const GNEFileDialog::ConfigType myConfigType = GNEFileDialog::ConfigType::NETEDIT;

    /// @brief File list widget
    FXFileList* myFileSelector = nullptr;

    /// @brief File name entry field
    MFXTextFieldIcon* myFilenameTextField = nullptr;

    /// @brief Combobox for pattern list
    MFXComboBoxIcon* myFileFilterComboBox = nullptr;

    /// @brief Menu for myBookmarksRecentFiles
    FXMenuPane* myBookmarkMenuPane = nullptr;

    /// @brief Directory hierarchy list
    FXDirBox* myDirBox = nullptr;

    /// @brief Bookmarked places
    FXRecentFiles myBookmarksRecentFiles = nullptr;

    /// @brief extensions
    std::vector<std::vector<std::string> > myExtensions;

private:
    /// @brief get selected files
    std::vector<std::string> getSelectedFiles() const;

    /// @brief get selected files that are not directories
    std::vector<std::string> getSelectedFilesOnly() const;

    /// @brief build bookmarks menu
    void buildBookmarkMenuPane(FXHorizontalFrame* navigatorHorizontalFrame);

    /// @brief build buttons
    void buildButtons(FXHorizontalFrame* navigatorHorizontalFrame, MFXStaticToolTip* staticTooltipMenu);

    /// @brief build shortcuts
    void buildShortcuts();

    /// @brief parse extensions from pattern text
    void parseExtensions(const std::vector<std::string>& extensions);

    /// @brief disable copy constructor
    GNEFileSelector(const GNEFileSelector&) = delete;

    /// @brief disable assignment operator
    GNEFileSelector& operator=(const GNEFileSelector&) = delete;
};

/****************************************************************************/
