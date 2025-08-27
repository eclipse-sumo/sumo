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

#include <fxkeys.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNEFileSelector.h"
#include "GNEFileDialog.h"

#define FILELISTMASK  (ICONLIST_EXTENDEDSELECT|ICONLIST_SINGLESELECT|ICONLIST_BROWSESELECT|ICONLIST_MULTIPLESELECT)
#define FILESTYLEMASK (ICONLIST_DETAILED|ICONLIST_MINI_ICONS|ICONLIST_BIG_ICONS|ICONLIST_ROWS|ICONLIST_COLUMNS|ICONLIST_AUTOSIZE)

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Map
FXDEFMAP(GNEFileSelector) GNEFileSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_ACCEPT,          GNEFileSelector::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_FILEFILTER,      GNEFileSelector::onCmdFilter),
    FXMAPFUNC(SEL_DOUBLECLICKED,        FXFileSelector::ID_FILELIST,        GNEFileSelector::onCmdItemDblClicked),
    FXMAPFUNC(SEL_SELECTED,             FXFileSelector::ID_FILELIST,        GNEFileSelector::onCmdItemSelected),
    FXMAPFUNC(SEL_DESELECTED,           FXFileSelector::ID_FILELIST,        GNEFileSelector::onCmdItemDeselected),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   FXFileSelector::ID_FILELIST,        GNEFileSelector::onPopupMenu),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_DIRECTORY_UP,    GNEFileSelector::onCmdDirectoryUp),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_DIRECTORY_UP,    GNEFileSelector::onUpdDirectoryUp),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_DIRTREE,         GNEFileSelector::onCmdDirTree),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_HOME,            GNEFileSelector::onCmdHome),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_WORK,            GNEFileSelector::onCmdWork),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_VISIT,           GNEFileSelector::onCmdVisit),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_BOOKMARK,        GNEFileSelector::onCmdBookmark),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_NEW,             GNEFileSelector::onCmdNew),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_NEW,             GNEFileSelector::onUpdNew),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_DELETE,          GNEFileSelector::onCmdDelete),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_MOVE,            GNEFileSelector::onCmdMove),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_COPY,            GNEFileSelector::onCmdCopy),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_LINK,            GNEFileSelector::onCmdLink),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_COPY,            GNEFileSelector::onUpdSelected),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_MOVE,            GNEFileSelector::onUpdSelected),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_LINK,            GNEFileSelector::onUpdSelected),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_DELETE,          GNEFileSelector::onUpdSelected),
    FXMAPFUNCS(SEL_COMMAND,             FXFileSelector::ID_NORMAL_SIZE,     FXFileSelector::ID_GIANT_SIZE,  GNEFileSelector::onCmdImageSize),
    FXMAPFUNCS(SEL_UPDATE,              FXFileSelector::ID_NORMAL_SIZE,     FXFileSelector::ID_GIANT_SIZE,  GNEFileSelector::onUpdImageSize),
};

// Implementation
FXIMPLEMENT(GNEFileSelector, FXVerticalFrame, GNEFileSelectorMap, ARRAYNUMBER(GNEFileSelectorMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFileSelector::GNEFileSelector(GNEFileDialog* fileDialog, const std::vector<std::string>& extensions,
                                 const bool save, const bool multiElements):
    FXVerticalFrame(fileDialog->getContentFrame(), GUIDesignAuxiliarFrame),
    myFileDialog(fileDialog),
    myBookmarksRecentFiles(fileDialog->getApp(), TL("Visited Directories")) {
    // create horizontal frame for top buttons
    auto navigatorHorizontalFrame = new FXHorizontalFrame(this, GUIDesignDialogContentHorizontalFrame);
    // create two horizontal frame for file selector
    auto externFileboxframe = new FXHorizontalFrame(this, GUIDesignDialogContentFrame);
    auto fileboxframe = new FXHorizontalFrame(externFileboxframe, GUIDesignFileBoxFrame);
    // horizontal frame for filename
    auto filenameHorizontalFrame = new FXHorizontalFrame(this, GUIDesignDialogContentHorizontalFrame);
    // first create file selector
    myFileSelector = new FXFileList(fileboxframe, this, FXFileSelector::ID_FILELIST, ICONLIST_MINI_ICONS | ICONLIST_BROWSESELECT | ICONLIST_AUTOSIZE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // label for directory
    new FXLabel(navigatorHorizontalFrame, TL("Directory:"), nullptr, GUIDesignLabelFixed(100));
    // create directory box
    myDirBox = new FXDirBox(navigatorHorizontalFrame, this, FXFileSelector::ID_DIRTREE, DIRBOX_NO_OWN_ASSOC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_CENTER_Y, 0, 0, 0, 0, 1, 1, 1, 1);
    myDirBox->setNumVisible(5);
    myDirBox->setAssociations(myFileSelector->getAssociations());
    // create button for going up one directory
    new FXButton(navigatorHorizontalFrame,
                 (std::string("\t") + TL("Go up one directory") + std::string("\t") + TL("Move up to higher directory.")).c_str(),
                 GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_DIRUP_ICON),
                 this, FXFileSelector::ID_DIRECTORY_UP, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    // create button for go to home directory
    new FXButton(navigatorHorizontalFrame,
                 (std::string("\t") + TL("Go to home directory") + std::string("\t") + TL("Back to home directory.")).c_str(),
                 GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_GOTO_HOME),
                 this, FXFileSelector::ID_HOME, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    // create button for go to work directory
    new FXButton(navigatorHorizontalFrame,
                 (std::string("\t") + TL("Go to work directory") + std::string("\t") + TL("Back to working directory.")).c_str(),
                 GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_GOTO_WORK),
                 this, FXFileSelector::ID_WORK, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    // create button for bookmarks menu
    FXMenuButton* bookmenu = new FXMenuButton(navigatorHorizontalFrame,
            (std::string("\t") + TL("Bookmarks") + std::string("\t") + TL("Visit bookmarked directories.")).c_str(),
            GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_SET),
            myBookmarkMenuPane, MENUBUTTON_NOARROWS | MENUBUTTON_ATTACH_LEFT | MENUBUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    bookmenu->setTarget(this);
    bookmenu->setSelector(FXFileSelector::ID_BOOKMENU);
    // create button for creating a new directory
    new FXButton(navigatorHorizontalFrame,
                 (std::string("\t") + TL("Create new directory") + std::string("\t") + TL("Create new directory.")).c_str(),
                 GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FOLDER_NEW),
                 this, FXFileSelector::ID_NEW, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    // create button for show lists
    new FXButton(navigatorHorizontalFrame,
                 (std::string("\t") + TL("Show list") + std::string("\t") + TL("Display directory with small icons.")).c_str(),
                 GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_SHOW_SMALLICONS),
                 myFileSelector, FXFileList::ID_SHOW_MINI_ICONS, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    // create button for show icons
    new FXButton(navigatorHorizontalFrame,
                 (std::string("\t") + TL("Show icons") + std::string("\t") + TL("Display directory with big icons.")).c_str(),
                 GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_SHOW_BIGICONS),
                 myFileSelector, FXFileList::ID_SHOW_BIG_ICONS, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    // create button for show details
    new FXButton(navigatorHorizontalFrame,
                 (std::string("\t") + TL("Show details") + std::string("\t") + TL("Display detailed directory listing.")).c_str(),
                 GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_SHOW_DETAILS),
                 myFileSelector, FXFileList::ID_SHOW_DETAILS, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    // create button for toogle show/hide hidden files
    new FXToggleButton(navigatorHorizontalFrame,
                       (std::string("\t") + TL("Show hidden files") + std::string("\t") + TL("Show hidden files and directories.")).c_str(),
                       (std::string("\t") + TL("Hide Hidden Files") + std::string("\t") + TL("Hide hidden files and directories.")).c_str(),
                       GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_HIDDEN),
                       GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_SHOWN),
                       myFileSelector, FXFileList::ID_TOGGLE_HIDDEN, TOGGLEBUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    // create label for filename
    new FXLabel(filenameHorizontalFrame,
                TL("File Name:"),
                nullptr, GUIDesignLabelFixed(100));
    // create filename text field
    myFilenameTextField = new FXTextField(filenameHorizontalFrame, GUIDesignTextFieldNCol, this, FXFileSelector::ID_ACCEPT, GUIDesignTextFieldFileDialog);
    // create comboBox for file filter
    myFileFilterComboBox = new FXComboBox(filenameHorizontalFrame, GUIDesignComboBoxNCol, this, FXFileSelector::ID_FILEFILTER, GUIDesignComboBoxFileDialog);
    // create bookmarks menu pane
    myBookmarkMenuPane = new FXMenuPane(this, POPUP_SHRINKWRAP);
    new FXMenuCommand(myBookmarkMenuPane,
                      (TL("Set bookmark") + std::string("\t\t") + TL("Bookmark current directory.")).c_str(),
                      GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_SET),
                      this, FXFileSelector::ID_BOOKMARK);
    new FXMenuCommand(myBookmarkMenuPane,
                      (TL("Clear bookmarks") + std::string("\t\t") + TL("Clear bookmarks.")).c_str(),
                      GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_CLR),
                      &myBookmarksRecentFiles, FXRecentFiles::ID_CLEAR);
    FXMenuSeparator* sep1 = new FXMenuSeparator(myBookmarkMenuPane);
    sep1->setTarget(&myBookmarksRecentFiles);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_10);
    new FXFrame(navigatorHorizontalFrame, LAYOUT_FIX_WIDTH, 0, 0, 4, 1);
    myBookmarksRecentFiles.setTarget(this);
    myBookmarksRecentFiles.setSelector(FXFileSelector::ID_VISIT);
    // set shortcuts
    FXAccelTable* table = getShell()->getAccelTable();
    if (table) {
        table->addAccel(MKUINT(KEY_BackSpace, 0), this, FXSEL(SEL_COMMAND, FXFileSelector::ID_DIRECTORY_UP));
        table->addAccel(MKUINT(KEY_Delete, 0), this, FXSEL(SEL_COMMAND, FXFileSelector::ID_DELETE));
        table->addAccel(MKUINT(KEY_h, CONTROLMASK), this, FXSEL(SEL_COMMAND, FXFileSelector::ID_HOME));
        table->addAccel(MKUINT(KEY_w, CONTROLMASK), this, FXSEL(SEL_COMMAND, FXFileSelector::ID_WORK));
        table->addAccel(MKUINT(KEY_n, CONTROLMASK), this, FXSEL(SEL_COMMAND, FXFileSelector::ID_NEW));
        table->addAccel(MKUINT(KEY_a, CONTROLMASK), myFileSelector, FXSEL(SEL_COMMAND, FXFileList::ID_SELECT_ALL));
        table->addAccel(MKUINT(KEY_b, CONTROLMASK), myFileSelector, FXSEL(SEL_COMMAND, FXFileList::ID_SHOW_BIG_ICONS));
        table->addAccel(MKUINT(KEY_s, CONTROLMASK), myFileSelector, FXSEL(SEL_COMMAND, FXFileList::ID_SHOW_MINI_ICONS));
        table->addAccel(MKUINT(KEY_l, CONTROLMASK), myFileSelector, FXSEL(SEL_COMMAND, FXFileList::ID_SHOW_DETAILS));
    }
    // check if allow to create a new file, or select only existent files
    if (save) {
        mySelectmode = SelectMode::SAVE;
    } else {
        if (multiElements) {
            mySelectmode = SelectMode::MULTIPLE;
        } else {
            mySelectmode = SelectMode::EXISTING;
        }
    }
    switch (mySelectmode) {
        case SelectMode::EXISTING:
            myFileSelector->showOnlyDirectories(FALSE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
        case SelectMode::MULTIPLE:
        case SelectMode::MULTIPLE_ALL:
            myFileSelector->showOnlyDirectories(FALSE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_EXTENDEDSELECT);
            break;
        case SelectMode::DIRECTORY:
            myFileSelector->showOnlyDirectories(TRUE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
        default:
            myFileSelector->showOnlyDirectories(FALSE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
    }
    // set directory
    if (gCurrentFolder.length() != 0) {
        setDirectory(gCurrentFolder);
    } else {
        setDirectory(FXSystem::getCurrentDirectory());
    }
    // set extensions
    if (extensions.size() == 0) {
        throw ProcessError("At least one extension is needed");
    } else {
        for (const auto& extension : extensions) {
            myFileFilterComboBox->appendItem(extension.c_str());
        }
    }
    myFileFilterComboBox->setNumVisible(FXMIN(extensions.size(), 12));
    setCurrentPattern(0);
}


GNEFileSelector::~GNEFileSelector() {
    FXAccelTable* table = getShell()->getAccelTable();
    if (table) {
        table->removeAccel(MKUINT(KEY_BackSpace, 0));
        table->removeAccel(MKUINT(KEY_Delete, 0));
        table->removeAccel(MKUINT(KEY_h, CONTROLMASK));
        table->removeAccel(MKUINT(KEY_w, CONTROLMASK));
        table->removeAccel(MKUINT(KEY_n, CONTROLMASK));
        table->removeAccel(MKUINT(KEY_a, CONTROLMASK));
        table->removeAccel(MKUINT(KEY_b, CONTROLMASK));
        table->removeAccel(MKUINT(KEY_s, CONTROLMASK));
        table->removeAccel(MKUINT(KEY_l, CONTROLMASK));
    }
    delete myBookmarkMenuPane;
    myFileSelector = (FXFileList*) - 1L;
    myFilenameTextField = (FXTextField*) - 1L;
    myFileFilterComboBox = (FXComboBox*) - 1L;
    myBookmarkMenuPane = (FXMenuPane*) - 1L;
    myDirBox = (FXDirBox*) - 1L;
}


long
GNEFileSelector::onUpdNew(FXObject* sender, FXSelector, void*) {
    sender->handle(this, FXStat::isWritable(getDirectory()) ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}


std::vector<std::string>
GNEFileSelector::getSelectedFiles() const {
    std::vector<std::string> files;
    for (int i = 0; i < myFileSelector->getNumItems(); i++) {
        if (myFileSelector->isItemSelected(i) && (myFileSelector->getItemFilename(i) != "..") && (myFileSelector->getItemFilename(i) != ".")) {
            files.push_back(myFileSelector->getItemPathname(i).text());
        }
    }
    return files;
}


std::vector<std::string>
GNEFileSelector::getSelectedFilesOnly() const {
    std::vector<std::string> files;
    for (int i = 0; i < myFileSelector->getNumItems(); i++) {
        if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
            files.push_back(myFileSelector->getItemPathname(i).text());
        }
    }
    return files;
}


long
GNEFileSelector::onCmdCopy(FXObject*, FXSelector, void*) {
    const auto filenamelist = getSelectedFiles();
    FXString copymessage;
    for (FXint i = 0; !filenamelist[i].empty(); i++) {
        copymessage.format(TL("Copy file from location:\n\n%s\n\nto location: "), filenamelist.at(i));
        FXInputDialog inputdialog(this, TL("Copy File"), copymessage, NULL, INPUTDIALOG_STRING, 0, 0, 0, 0);
        inputdialog.setText(FXPath::absolute(FXPath::directory(filenamelist.at(i).c_str()), "CopyOf" + FXPath::name(filenamelist.at(i).c_str())));
        inputdialog.setNumColumns(60);
        if (inputdialog.execute()) {
            FXString newname = inputdialog.getText();
            if (!FXFile::copyFiles(filenamelist.at(i).c_str(), newname, FALSE)) {
                if (FXMessageBox::error(this, MBOX_YES_NO, TL("Error Copying File"), TL("Unable to copy file:\n\n%s  to:  %s\n\nContinue with operation?"), filenamelist.at(i).c_str(), newname.text()) == MBOX_CLICKED_NO) {
                    break;
                }
            }
        }
    }
    return 1;
}


long
GNEFileSelector::onCmdMove(FXObject*, FXSelector, void*) {
    const auto filenamelist = getSelectedFiles();
    FXString movemessage;
    for (FXint i = 0; !filenamelist[i].empty(); i++) {
        movemessage.format(TL("Move file from location:\n\n%s\n\nto location: "), filenamelist.at(i));
        FXInputDialog inputdialog(this, TL("Move File"), movemessage, NULL, INPUTDIALOG_STRING, 0, 0, 0, 0);
        inputdialog.setText(filenamelist.at(i).c_str());
        inputdialog.setNumColumns(60);
        if (inputdialog.execute()) {
            FXString newname = inputdialog.getText();
            if (!FXFile::moveFiles(filenamelist.at(i).c_str(), newname, FALSE)) {
                if (FXMessageBox::error(this, MBOX_YES_NO, TL("Error Moving File"), TL("Unable to move file:\n\n%s  to:  %s\n\nContinue with operation?"), filenamelist.at(i).c_str(), newname.text()) == MBOX_CLICKED_NO) {
                    break;
                }
            }
        }
    }
    return 1;
}


long
GNEFileSelector::onCmdLink(FXObject*, FXSelector, void*) {
    const auto filenamelist = getSelectedFiles();
    FXString linkmessage;
    for (FXint i = 0; !filenamelist[i].empty(); i++) {
        linkmessage.format(TL("Link file from location:\n\n%s\n\nto location: "), filenamelist.at(i));
        FXInputDialog inputdialog(this, TL("Link File"), linkmessage, NULL, INPUTDIALOG_STRING, 0, 0, 0, 0);
        inputdialog.setText(FXPath::absolute(FXPath::directory(filenamelist.at(i).c_str()), "LinkTo" + FXPath::name(filenamelist.at(i).c_str())));
        inputdialog.setNumColumns(60);
        if (inputdialog.execute()) {
            FXString newname = inputdialog.getText();
            if (!FXFile::symlink(filenamelist.at(i).c_str(), newname)) {
                if (FXMessageBox::error(this, MBOX_YES_NO, TL("Error Linking File"), TL("Unable to link file:\n\n%s  to:  %s\n\nContinue with operation?"), filenamelist.at(i).c_str(), newname.text()) == MBOX_CLICKED_NO) {
                    break;
                }
            }
        }
    }
    return 1;
}


long
GNEFileSelector::onCmdDelete(FXObject*, FXSelector, void*) {
    const auto filenamelist = getSelectedFiles();
    FXuint answer;
    for (FXint i = 0; !filenamelist[i].empty(); i++) {
        answer = FXMessageBox::warning(this, MBOX_YES_NO_CANCEL, TL("Deleting files"), TL("Are you sure you want to delete the file:\n\n%s"), filenamelist.at(i));
        if (answer == MBOX_CLICKED_CANCEL) {
            break;
        }
        if (answer == MBOX_CLICKED_NO) {
            continue;
        }
        if (!FXFile::removeFiles(filenamelist.at(i).c_str(), TRUE)) {
            if (FXMessageBox::error(this, MBOX_YES_NO, TL("Error Deleting File"), TL("Unable to delete file:\n\n%s\n\nContinue with operation?"), filenamelist.at(i)) == MBOX_CLICKED_NO) {
                break;
            }
        }
    }
    return 1;
}


long
GNEFileSelector::onUpdSelected(FXObject* sender, FXSelector, void*) {
    for (FXint i = 0; i < myFileSelector->getNumItems(); i++) {
        if (myFileSelector->isItemSelected(i)) {
            sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
            return 1;
        }
    }
    sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}


long
GNEFileSelector::onCmdImageSize(FXObject*, FXSelector sel, void*) {
    switch (FXSELID(sel)) {
        case FXFileSelector::ID_NORMAL_SIZE:
            setImageSize(32);
            break;
        case FXFileSelector::ID_MEDIUM_SIZE:
            setImageSize(48);
            break;
        case FXFileSelector::ID_GIANT_SIZE:
            setImageSize(64);
            break;
    }
    return 1;
}


long
GNEFileSelector::onUpdImageSize(FXObject* sender, FXSelector sel, void*) {
    FXbool check = FALSE;
    switch (FXSELID(sel)) {
        case FXFileSelector::ID_NORMAL_SIZE:
            check = (getImageSize() == 32);
            break;
        case FXFileSelector::ID_MEDIUM_SIZE:
            check = (getImageSize() == 48);
            break;
        case FXFileSelector::ID_GIANT_SIZE:
            check = (getImageSize() == 64);
            break;
    }
    sender->handle(this, check ? FXSEL(SEL_COMMAND, ID_CHECK) : FXSEL(SEL_COMMAND, ID_UNCHECK), NULL);
    return 1;
}


long
GNEFileSelector::onPopupMenu(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    if (event->moved) {
        return 1;
    }

    FXMenuPane filemenu(this);
    new FXMenuCommand(&filemenu, TL("Up one level"), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_DIRUP_ICON), this, FXFileSelector::ID_DIRECTORY_UP);
    new FXMenuCommand(&filemenu, TL("Home directory"), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_GOTO_HOME), this, FXFileSelector::ID_HOME);
    new FXMenuCommand(&filemenu, TL("Work directory"), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_GOTO_WORK), this, FXFileSelector::ID_WORK);
    new FXMenuCommand(&filemenu, TL("Select all"), NULL, myFileSelector, FXFileList::ID_SELECT_ALL);
    new FXMenuSeparator(&filemenu);

    FXMenuPane sortmenu(this);
    new FXMenuCascade(&filemenu, TL("Sort by"), NULL, &sortmenu);
    new FXMenuRadio(&sortmenu, TL("Name"), myFileSelector, FXFileList::ID_SORT_BY_NAME);
    new FXMenuRadio(&sortmenu, TL("Type"), myFileSelector, FXFileList::ID_SORT_BY_TYPE);
    new FXMenuRadio(&sortmenu, TL("Size"), myFileSelector, FXFileList::ID_SORT_BY_SIZE);
    new FXMenuRadio(&sortmenu, TL("Time"), myFileSelector, FXFileList::ID_SORT_BY_TIME);
    new FXMenuRadio(&sortmenu, TL("User"), myFileSelector, FXFileList::ID_SORT_BY_USER);
    new FXMenuRadio(&sortmenu, TL("Group"), myFileSelector, FXFileList::ID_SORT_BY_GROUP);
    new FXMenuSeparator(&sortmenu);
    new FXMenuCheck(&sortmenu, TL("Reverse"), myFileSelector, FXFileList::ID_SORT_REVERSE);
    new FXMenuCheck(&sortmenu, TL("Ignore case"), myFileSelector, FXFileList::ID_SORT_CASE);

    FXMenuPane viewmenu(this);
    new FXMenuCascade(&filemenu, TL("View"), NULL, &viewmenu);
    new FXMenuRadio(&viewmenu, TL("Small icons"), myFileSelector, FXFileList::ID_SHOW_MINI_ICONS);
    new FXMenuRadio(&viewmenu, TL("Big icons"), myFileSelector, FXFileList::ID_SHOW_BIG_ICONS);
    new FXMenuRadio(&viewmenu, TL("Details"), myFileSelector, FXFileList::ID_SHOW_DETAILS);
    new FXMenuSeparator(&viewmenu);
    new FXMenuRadio(&viewmenu, TL("Rows"), myFileSelector, FXFileList::ID_ARRANGE_BY_ROWS);
    new FXMenuRadio(&viewmenu, TL("Columns"), myFileSelector, FXFileList::ID_ARRANGE_BY_COLUMNS);
    new FXMenuSeparator(&viewmenu);
    new FXMenuCheck(&viewmenu, TL("Hidden files"), myFileSelector, FXFileList::ID_TOGGLE_HIDDEN);
    new FXMenuCheck(&viewmenu, TL("Preview images"), myFileSelector, FXFileList::ID_TOGGLE_IMAGES);
    new FXMenuSeparator(&viewmenu);
    new FXMenuRadio(&viewmenu, TL("Normal images"), this, FXFileSelector::ID_NORMAL_SIZE);
    new FXMenuRadio(&viewmenu, TL("Medium images"), this, FXFileSelector::ID_MEDIUM_SIZE);
    new FXMenuRadio(&viewmenu, TL("Giant images"), this, FXFileSelector::ID_GIANT_SIZE);

    FXMenuPane bookmenu(this);
    FXMenuCascade* bookcasc = new FXMenuCascade(&filemenu, TL("Bookmarks"), NULL, &bookmenu);
    bookcasc->setTarget(this);
    bookcasc->setSelector(FXFileSelector::ID_BOOKMENU);
    new FXMenuCommand(&bookmenu, TL("Set bookmark"), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_SET), this, FXFileSelector::ID_BOOKMARK);
    new FXMenuCommand(&bookmenu, TL("Clear bookmarks"), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_CLR), &myBookmarksRecentFiles, FXRecentFiles::ID_CLEAR);
    FXMenuSeparator* sep1 = new FXMenuSeparator(&bookmenu);
    sep1->setTarget(&myBookmarksRecentFiles);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_10);

    new FXMenuSeparator(&filemenu);
    new FXMenuCommand(&filemenu, TL("New directory..."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FOLDER_NEW), this, FXFileSelector::ID_NEW);
    new FXMenuCommand(&filemenu, TL("Copy..."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_COPY), this, FXFileSelector::ID_COPY);
    new FXMenuCommand(&filemenu, TL("Move..."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_MOVE), this, FXFileSelector::ID_MOVE);
    new FXMenuCommand(&filemenu, TL("Link..."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_LINK), this, FXFileSelector::ID_LINK);
    new FXMenuCommand(&filemenu, TL("Delete..."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_DELETE), this, ID_DELETE);

    filemenu.create();
    filemenu.popup(NULL, event->root_x, event->root_y);
    getApp()->runModalWhileShown(&filemenu);
    return 1;
}


long
GNEFileSelector::onCmdFilter(FXObject*, FXSelector, void* ptr) {
    FXString pat = FXFileSelector::patternFromText((FXchar*)ptr);
    myFileSelector->setPattern(pat);
    if (mySelectmode == SelectMode::SAVE) {
        FXString ext = FXFileSelector::extensionFromPattern(pat);
        if (!ext.empty()) {
            FXString name = FXPath::stripExtension(myFilenameTextField->getText());
            if (!name.empty()) {
                myFilenameTextField->setText(name + "." + ext);
            }
        }
    }
    return 1;
}


void
GNEFileSelector::setDirectory(const FXString& path) {
    FXString abspath = FXPath::absolute(path);
    FXTRACE((100, "path=%s abspath: %s\n", path.text(), abspath.text()));
    myFileSelector->setDirectory(abspath);
    myDirBox->setDirectory(abspath);
    if (mySelectmode != SelectMode::SAVE) {
        myFilenameTextField->setText(FXString::null);
    }
}


FXString
GNEFileSelector::getDirectory() const {
    return myFileSelector->getDirectory();
}


void
GNEFileSelector::setFilename(const FXString& path) {
    FXString fullname(FXPath::absolute(path));
    FXString name(FXPath::name(fullname));
    myFileSelector->setCurrentFile(fullname);
    myDirBox->setDirectory(myFileSelector->getDirectory());
    myFilenameTextField->setText(name);
}


std::string
GNEFileSelector::getFilename() const {
    FXint i;
    if (mySelectmode == SelectMode::MULTIPLE_ALL) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && myFileSelector->getItemFilename(i) != ".." && myFileSelector->getItemFilename(i) != ".") {
                return FXPath::absolute(myFileSelector->getDirectory(), myFileSelector->getItemFilename(i)).text();
            }
        }
    } else if (mySelectmode == SelectMode::MULTIPLE) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
                return FXPath::absolute(myFileSelector->getDirectory(), myFileSelector->getItemFilename(i)).text();
            }
        }
    } else if (!myFilenameTextField->getText().empty()) {
        //return FXPath::absolute(myFileSelector->getDirectory(),myFilenameTextField->getText());
        return FXPath::absolute(myFileSelector->getDirectory(), FXPath::expand(myFilenameTextField->getText())).text();    // FIXME don't always want to expand!
    }
    return "";
}


std::vector<std::string>
GNEFileSelector::getFilenames() const {
    if (mySelectmode == SelectMode::MULTIPLE_ALL) {
        return getSelectedFiles();
    } else {
        return getSelectedFilesOnly();
    }
}


void
GNEFileSelector::setPattern(const FXString& ptrn) {
    myFileFilterComboBox->setText(ptrn);
    myFileSelector->setPattern(ptrn);
}


FXString
GNEFileSelector::getPattern() const {
    return myFileSelector->getPattern();
}


void
GNEFileSelector::setCurrentPattern(FXint patno) {
    if (patno < 0 || patno >= myFileFilterComboBox->getNumItems()) {
        throw ProcessError("index out of range");
    }
    myFileFilterComboBox->setCurrentItem(patno);
    myFileSelector->setPattern(FXFileSelector::patternFromText(myFileFilterComboBox->getItemText(patno)));
}


FXint
GNEFileSelector::getCurrentPattern() const {
    return myFileFilterComboBox->getCurrentItem();
}


void
GNEFileSelector::setPatternText(FXint patno, const FXString& text) {
    if (patno < 0 || patno >= myFileFilterComboBox->getNumItems()) {
        throw ProcessError("index out of range");
    }
    myFileFilterComboBox->setItemText(patno, text);
    if (patno == myFileFilterComboBox->getCurrentItem()) {
        setPattern(FXFileSelector::patternFromText(text));
    }
}


FXString
GNEFileSelector::getPatternText(FXint patno) const {
    if (patno < 0 || patno >= myFileFilterComboBox->getNumItems()) {
        throw ProcessError("index out of range");
    }
    return myFileFilterComboBox->getItemText(patno);
}


FXint
GNEFileSelector::getNumPatterns() const {
    return myFileFilterComboBox->getNumItems();
}


void
GNEFileSelector::allowPatternEntry(FXbool allow) {
    myFileFilterComboBox->setComboStyle(allow ? COMBOBOX_NORMAL : COMBOBOX_STATIC);
}


FXbool
GNEFileSelector::allowPatternEntry() const {
    return (myFileFilterComboBox->getComboStyle() != COMBOBOX_STATIC);
}


void
GNEFileSelector::setItemSpace(FXint s) {
    myFileSelector->setItemSpace(s);
}


FXint
GNEFileSelector::getItemSpace() const {
    return myFileSelector->getItemSpace();
}


void
GNEFileSelector::setFileBoxStyle(FXuint style) {
    myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILESTYLEMASK) | (style & FILESTYLEMASK));
}


FXuint
GNEFileSelector::getFileBoxStyle() const {
    return myFileSelector->getListStyle()&FILESTYLEMASK;
}


void
GNEFileSelector::setMatchMode(FXuint mode) {
    myFileSelector->setMatchMode(mode);
}


FXuint
GNEFileSelector::getMatchMode() const {
    return myFileSelector->getMatchMode();
}


FXbool
GNEFileSelector::showHiddenFiles() const {
    return myFileSelector->showHiddenFiles();
}


void
GNEFileSelector::showHiddenFiles(FXbool showing) {
    myFileSelector->showHiddenFiles(showing);
}


FXbool
GNEFileSelector::showImages() const {
    return myFileSelector->showImages();
}


void
GNEFileSelector::showImages(FXbool showing) {
    myFileSelector->showImages(showing);
}


FXint
GNEFileSelector::getImageSize() const {
    return myFileSelector->getImageSize();
}


void
GNEFileSelector::setImageSize(FXint size) {
    myFileSelector->setImageSize(size);
}


long
GNEFileSelector::onCmdItemSelected(FXObject*, FXSelector, void* ptr) {
    FXint index = (FXint)(FXival)ptr;
    FXint i;
    FXString text, file;
    if (mySelectmode == SelectMode::MULTIPLE) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + myFileSelector->getItemFilename(i) + "\"";
            }
        }
        myFilenameTextField->setText(text);
    } else if (mySelectmode == SelectMode::MULTIPLE_ALL) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && myFileSelector->getItemFilename(i) != ".." && myFileSelector->getItemFilename(i) != ".") {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + myFileSelector->getItemFilename(i) + "\"";
            }
        }
        myFilenameTextField->setText(text);
    } else if (mySelectmode == SelectMode::DIRECTORY) {
        if (myFileSelector->isItemDirectory(index)) {
            text = myFileSelector->getItemFilename(index);
            myFilenameTextField->setText(text);
        }
    } else {
        if (!myFileSelector->isItemDirectory(index)) {
            text = myFileSelector->getItemFilename(index);
            myFilenameTextField->setText(text);
        }
    }
    return 1;
}


long
GNEFileSelector::onCmdItemDeselected(FXObject*, FXSelector, void*) {
    FXint i;
    FXString text, file;
    if (mySelectmode == SelectMode::MULTIPLE) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + myFileSelector->getItemFilename(i) + "\"";
            }
        }
        myFilenameTextField->setText(text);
    } else if (mySelectmode == SelectMode::MULTIPLE_ALL) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && myFileSelector->getItemFilename(i) != ".." && myFileSelector->getItemFilename(i) != ".") {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + myFileSelector->getItemFilename(i) + "\"";
            }
        }
        myFilenameTextField->setText(text);
    }
    return 1;
}


long
GNEFileSelector::onCmdItemDblClicked(FXObject* obj, FXSelector sel, void* ptr) {
    FXint index = (FXint)(FXival)ptr;
    if (0 <= index) {
        // If directory, open the directory
        if (myFileSelector->isItemShare(index) || myFileSelector->isItemDirectory(index)) {
            setDirectory(myFileSelector->getItemPathname(index));
            return 1;
        }
        // Only return if we wanted a file
        if (mySelectmode != SelectMode::DIRECTORY) {
            return myFileDialog->onCmdAccept(obj, sel, ptr);
        }
    }
    return 1;
}


long
GNEFileSelector::onCmdAccept(FXObject* obj, FXSelector sel, void* ptr) {
    // Get (first) myFilenameTextField or directory
    std::string path = getFilename();
    // Only do something if a selection was made
    if (!path.empty()) {
        // Is directory?
        if (FXStat::isDirectory(path.c_str())) {
            // In directory mode:- we got our answer!
            if (mySelectmode == SelectMode::DIRECTORY || mySelectmode == SelectMode::MULTIPLE_ALL) {
                return myFileDialog->onCmdAccept(obj, sel, ptr);
            }
            // Hop over to that directory
            myDirBox->setDirectory(path.c_str());
            myFileSelector->setDirectory(path.c_str());
            myFilenameTextField->setText(FXString::null);
            return 1;
        }
        // Get directory part of path
        FXString dir = FXPath::directory(path.c_str());
        // In file mode, directory part of path should exist
        if (FXStat::isDirectory(dir)) {
            // In any mode, existing directory part is good enough
            if (mySelectmode == SelectMode::SAVE) {
                return myFileDialog->onCmdAccept(obj, sel, ptr);
            }
            // Otherwise, the whole myFilenameTextField must exist and be a file
            if (FXStat::exists(path.c_str())) {
                return myFileDialog->onCmdAccept(obj, sel, ptr);

            }
        }
        // Go up to the lowest directory which still exists
        while (!FXPath::isTopDirectory(dir) && !FXStat::isDirectory(dir)) {
            dir = FXPath::upLevel(dir);
        }
        // Switch as far as we could go
        myDirBox->setDirectory(dir);
        myFileSelector->setDirectory(dir);
        // Put the tail end back for further editing
        FXASSERT(dir.length() <= path.length());
        if (ISPATHSEP(path[dir.length()])) {
            path.erase(0, dir.length() + 1);
        } else {
            path.erase(0, dir.length());
        }
        // Replace text box with new stuff
        myFilenameTextField->setText(path.c_str());
        myFilenameTextField->selectAll();
    }
    // Beep
    getApp()->beep();
    return 1;
}


long
GNEFileSelector::onCmdDirectoryUp(FXObject*, FXSelector, void*) {
    setDirectory(FXPath::upLevel(myFileSelector->getDirectory()));
    return 1;
}


long
GNEFileSelector::onUpdDirectoryUp(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, !FXPath::isTopDirectory(getDirectory()) ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
}


long
GNEFileSelector::onCmdHome(FXObject*, FXSelector, void*) {
    setDirectory(FXSystem::getHomeDirectory());
    return 1;
}


long
GNEFileSelector::onCmdWork(FXObject*, FXSelector, void*) {
    setDirectory(FXSystem::getCurrentDirectory());
    return 1;
}


long
GNEFileSelector::onCmdVisit(FXObject*, FXSelector, void* ptr) {
    setDirectory((FXchar*)ptr);
    return 1;
}


long
GNEFileSelector::onCmdBookmark(FXObject*, FXSelector, void*) {
    myBookmarksRecentFiles.appendFile(getDirectory());
    return 1;
}


long
GNEFileSelector::onCmdDirTree(FXObject*, FXSelector, void* ptr) {
    myFileSelector->setDirectory((FXchar*)ptr);
    if (mySelectmode == SelectMode::DIRECTORY) {
        myFilenameTextField->setText(FXString::null);
    }
    return 1;
}


long
GNEFileSelector::onCmdNew(FXObject*, FXSelector, void*) {
    FXString dir = myFileSelector->getDirectory();
    FXString name = "DirectoryName";
    FXGIFIcon newdirectoryicon(getApp(), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FOLDER_BIG));
    if (FXInputDialog::getString(name, this, TL("Create New Directory"), TL("Create new directory with name: "), &newdirectoryicon)) {
        FXString dirname = FXPath::absolute(dir, name);
        if (FXStat::exists(dirname)) {
            FXMessageBox::error(this, MBOX_OK, TL("Already Exists"), TL("File or directory %s already exists.\n"), dirname.text());
            return 1;
        }
        if (!FXDir::create(dirname)) {
            FXMessageBox::error(this, MBOX_OK, TL("Cannot Create"), TL("Cannot create directory %s.\n"), dirname.text());
            return 1;
        }
        setDirectory(dirname);
    }
    return 1;
}

/*******************************************************************************/
