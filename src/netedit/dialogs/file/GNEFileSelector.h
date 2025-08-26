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

#include <utils/foxtools/fxheader.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFileSelector : public FXPacker {
    /// @brief FOX declaration
    FXDECLARE(GNEFileSelector)

public:
    /// Constructor
    GNEFileSelector(FXComposite* p, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    /// Destructor
    virtual ~GNEFileSelector();

    /// Return a pointer to the "Accept" button
    FXButton* acceptButton() const {
        return accept;
    }

    /// Return a pointer to the "Cancel" button
    FXButton* cancelButton() const {
        return cancel;
    }

    /// Change file name
    void setFilename(const FXString& path);

    /// Return file name, if any
    FXString getFilename() const;

    /**
    * Return array of strings containing the selected file names, terminated
    * by an empty string.  This string array must be freed using delete [].
    * If no files were selected, a NULL is returned.
    */
    FXString* getFilenames() const;

    /// Change file pattern
    void setPattern(const FXString& ptrn);

    /// Return file pattern
    FXString getPattern() const;

    /**
    * Change the list of file patterns shown in the file dialog.
    * Each pattern comprises an optional name, followed by a pattern in
    * parentheses.  The patterns are separated by newlines.
    * For example,
    *
    *  "*\n*.cpp,*.cc\n*.hpp,*.hh,*.h"
    *
    * and
    *
    *  "All Files (*)\nC++ Sources (*.cpp,*.cc)\nC++ Headers (*.hpp,*.hh,*.h)"
    *
    * will set the same three patterns, but the former shows no pattern names.
    */
    void setPatternList(const FXString& patterns);

    /// Return list of patterns
    FXString getPatternList() const;

    /**
    * After setting the list of patterns, this call will
    * initially select pattern n as the active one.
    */
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

    /**
    * Given filename pattern of the form "GIF Format (*.gif)",
    * returns the pattern only, i.e. "*.gif" in this case.
    * If the parentheses are not found then returns the entire
    * input pattern.
    */
    static FXString patternFromText(const FXString& pattern);

    /**
    * Given a pattern of the form "*.gif,*.GIF", return
    * the first extension of the pattern, i.e. "gif" in this
    * example. Returns empty string if it doesn't work out.
    */
    static FXString extensionFromPattern(const FXString& pattern);

    /// Change directory
    void setDirectory(const FXString& path);

    /// Return directory
    FXString getDirectory() const;

    /// Set the inter-item spacing (in pixels)
    void setItemSpace(FXint s);

    /// Return the inter-item spacing (in pixels)
    FXint getItemSpace() const;

    /// Change file list style
    void setFileBoxStyle(FXuint style);

    /// Return file list style
    FXuint getFileBoxStyle() const;

    /// Change file selection mode
    void setSelectMode(FXuint mode);

    /// Return file selection mode
    FXuint getSelectMode() const {
        return selectmode;
    }

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

    /// Show readonly button
    void showReadOnly(FXbool show);

    /// Return TRUE if readonly is shown
    FXbool shownReadOnly() const;

    /// Set initial state of readonly button
    void setReadOnly(FXbool state);

    /// Get readonly state
    FXbool getReadOnly() const;

    /// Allow or disallow navigation
    void allowNavigation(FXbool flag) {
        navigable = flag;
    }

    /// Is navigation allowed?
    FXbool allowNavigation() const {
        return navigable;
    }

    long onCmdAccept(FXObject*, FXSelector, void*);
    long onCmdFilter(FXObject*, FXSelector, void*);
    long onCmdItemDblClicked(FXObject*, FXSelector, void*);
    long onCmdItemSelected(FXObject*, FXSelector, void*);
    long onCmdItemDeselected(FXObject*, FXSelector, void*);
    long onCmdDirectoryUp(FXObject*, FXSelector, void*);
    long onUpdDirectoryUp(FXObject*, FXSelector, void*);
    long onCmdDirTree(FXObject*, FXSelector, void*);
    long onCmdHome(FXObject*, FXSelector, void*);
    long onCmdWork(FXObject*, FXSelector, void*);
    long onCmdBookmark(FXObject*, FXSelector, void*);
    long onCmdVisit(FXObject*, FXSelector, void*);
    long onCmdNew(FXObject*, FXSelector, void*);
    long onUpdNew(FXObject*, FXSelector, void*);
    long onCmdMove(FXObject*, FXSelector, void*);
    long onCmdCopy(FXObject*, FXSelector, void*);
    long onCmdLink(FXObject*, FXSelector, void*);
    long onCmdDelete(FXObject*, FXSelector, void*);
    long onUpdSelected(FXObject*, FXSelector, void*);
    long onPopupMenu(FXObject*, FXSelector, void*);
    long onCmdImageSize(FXObject*, FXSelector, void*);
    long onUpdImageSize(FXObject*, FXSelector, void*);
    long onUpdNavigable(FXObject*, FXSelector, void*);

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEFileSelector)

    FXFileList*        myFileSelector;           // File list widget
    FXTextField*       filename;          // File name entry field
    FXComboBox*        filefilter;        // Combobox for pattern list
    FXMenuPane*        bookmarkmenu;      // Menu for bookmarks
    FXHorizontalFrame* navbuttons;        // Navigation buttons
    FXHorizontalFrame* fileboxframe;      // Frame around file list
    FXMatrix*          entryblock;        // Entry block
    FXCheckButton*     readonly;          // Open file as read only
    FXDirBox*          dirbox;            // Directory hierarchy list
    FXButton*          accept;            // Accept button
    FXButton*          cancel;            // Cancel button
    FXIcon*            updiricon;         // Up directory icon
    FXIcon*            listicon;          // List mode icon
    FXIcon*            detailicon;        // Detail mode icon
    FXIcon*            iconsicon;         // Icon mode icon
    FXIcon*            homeicon;          // Go home icon
    FXIcon*            workicon;          // Go home icon
    FXIcon*            shownicon;         // Files shown icon
    FXIcon*            hiddenicon;        // Files hidden icon
    FXIcon*            markicon;          // Book mark icon
    FXIcon*            clearicon;         // Book clear icon
    FXIcon*            newicon;           // New directory icon
    FXIcon*            deleteicon;        // Delete file icon
    FXIcon*            moveicon;          // Rename file icon
    FXIcon*            copyicon;          // Copy file icon
    FXIcon*            linkicon;          // Link file icon
    FXRecentFiles      bookmarks;         // Bookmarked places
    FXuint             selectmode;        // Select mode
    FXbool             navigable;         // May navigate

    FXString* getSelectedFiles() const;

    FXString* getSelectedFilesOnly() const;

private:
    /// @brief disable copy constructor
    GNEFileSelector(const GNEFileSelector&) = delete;

    /// @brief disable assignment operator
    GNEFileSelector& operator=(const GNEFileSelector&) = delete;
};

/****************************************************************************/
