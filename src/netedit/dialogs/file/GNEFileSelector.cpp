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

#include <fxkeys.h>
#include <netedit/dialogs/basic/GNEErrorBasicDialog.h>
#include <netedit/dialogs/basic/GNEQuestionBasicDialog.h>
#include <netedit/GNEApplicationWindow.h>
#include <utils/common/StringTokenizer.h>
#include <utils/foxtools/MFXTextFieldIcon.h>
#include <utils/foxtools/MFXToggleButtonTooltip.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIIOGlobals.h>

#include "GNEFilePathDialog.h"
#include "GNEFileSelector.h"

#define FILELISTMASK  (ICONLIST_EXTENDEDSELECT|ICONLIST_SINGLESELECT|ICONLIST_BROWSESELECT|ICONLIST_MULTIPLESELECT)
#define FILESTYLEMASK (ICONLIST_DETAILED|ICONLIST_MINI_ICONS|ICONLIST_BIG_ICONS|ICONLIST_ROWS|ICONLIST_COLUMNS|ICONLIST_AUTOSIZE)

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Map
FXDEFMAP(GNEFileSelector) GNEFileSelectorMap[] = {
    // interaction
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_ACCEPT,          GNEFileSelector::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_FILEFILTER,      GNEFileSelector::onCmdFilter),
    FXMAPFUNC(SEL_DOUBLECLICKED,        FXFileSelector::ID_FILELIST,        GNEFileSelector::onCmdItemDoubleClicked),
    FXMAPFUNC(SEL_SELECTED,             FXFileSelector::ID_FILELIST,        GNEFileSelector::onCmdItemSelected),
    FXMAPFUNC(SEL_DESELECTED,           FXFileSelector::ID_FILELIST,        GNEFileSelector::onCmdItemDeselected),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   FXFileSelector::ID_FILELIST,        GNEFileSelector::onPopupMenu),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_DIRECTORY_UP,    GNEFileSelector::onCmdDirectoryUp),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_DIRECTORY_UP,    GNEFileSelector::onUpdDirectoryUp),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_DIRTREE,         GNEFileSelector::onCmdDirTree),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_HOME,            GNEFileSelector::onCmdHomeFolder),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_WORK,            GNEFileSelector::onCmdWorkFolder),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_VISIT,           GNEFileSelector::onCmdVisit),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_BOOKMARK,        GNEFileSelector::onCmdBookmark),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_NEW,             GNEFileSelector::onCmdNewFolder),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_NEW,             GNEFileSelector::onUpdNewFolder),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_DELETE,          GNEFileSelector::onCmdDelete),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_MOVE,            GNEFileSelector::onCmdMove),
    FXMAPFUNC(SEL_COMMAND,              FXFileSelector::ID_COPY,            GNEFileSelector::onCmdCopy),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_COPY,            GNEFileSelector::onUpdSelected),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_MOVE,            GNEFileSelector::onUpdSelected),
    FXMAPFUNC(SEL_UPDATE,               FXFileSelector::ID_DELETE,          GNEFileSelector::onUpdSelected),
    FXMAPFUNCS(SEL_COMMAND,             FXFileSelector::ID_NORMAL_SIZE,     FXFileSelector::ID_GIANT_SIZE,  GNEFileSelector::onCmdImageSize),
    FXMAPFUNCS(SEL_UPDATE,              FXFileSelector::ID_NORMAL_SIZE,     FXFileSelector::ID_GIANT_SIZE,  GNEFileSelector::onUpdImageSize),
    FXMAPFUNC(SEL_KEYPRESS,             0,                                  GNEFileSelector::onKeyPress),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_BUTTON_CONFIG,              GNEFileSelector::onCmdConfigFolder),
};

// Implementation
FXIMPLEMENT(GNEFileSelector, FXVerticalFrame, GNEFileSelectorMap, ARRAYNUMBER(GNEFileSelectorMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFileSelector::GNEFileSelector(GNEFileDialog* fileDialog, const std::vector<std::string>& extensions,
                                 GNEFileDialog::OpenMode openMode, GNEFileDialog::ConfigType configType):
    FXVerticalFrame(fileDialog->getContentFrame(), GUIDesignAuxiliarFrame),
    myFileDialog(fileDialog),
    myOpenMode(openMode),
    myConfigType(configType),
    myBookmarksRecentFiles(fileDialog->getApplicationWindow()->getApp(), TL("Visited Directories")) {
    // get static tooltip
    const auto tooltipMenu = fileDialog->getApplicationWindow()->getStaticTooltipMenu();
    // create horizontal frame for top buttons
    auto navigatorHorizontalFrame = new FXHorizontalFrame(this, GUIDesignDialogContentHorizontalFrame);
    // create two horizontal frame for file selector
    auto externFileboxframe = new FXHorizontalFrame(this, GUIDesignDialogContentFrame);
    auto fileboxframe = new FXHorizontalFrame(externFileboxframe, GUIDesignFileBoxFrame);
    // horizontal frame for filename
    auto filenameHorizontalFrame = new FXHorizontalFrame(this, GUIDesignDialogContentHorizontalFrame);
    // first create file selector
    myFileSelector = new FXFileList(fileboxframe, this, FXFileSelector::ID_FILELIST, GUIDesignFileList);
    // label for directory
    new FXLabel(navigatorHorizontalFrame, TL("Directory:"), nullptr, GUIDesignLabelFixed(100));
    // create directory box
    myDirBox = new FXDirBox(navigatorHorizontalFrame, this, FXFileSelector::ID_DIRTREE, GUIDesignDirBox);
    myDirBox->setNumVisible(5);
    myDirBox->setAssociations(myFileSelector->getAssociations());
    // build bookmark menu pane
    buildBookmarkMenuPane(navigatorHorizontalFrame);
    // build buttons
    buildButtons(navigatorHorizontalFrame, tooltipMenu);
    // create label for filename
    new FXLabel(filenameHorizontalFrame,
                TL("File Name:"),
                nullptr, GUIDesignLabelFixed(100));
    // create filename text field and set focus
    myFilenameTextField = new MFXTextFieldIcon(filenameHorizontalFrame, tooltipMenu, GUIIcon::EMPTY,
            this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFileDialog);
    // create comboBox for file filter
    myFileFilterComboBox = new MFXComboBoxIcon(filenameHorizontalFrame, tooltipMenu, false, GUIDesignComboBoxVisibleItems,
            this, FXFileSelector::ID_FILEFILTER, GUIDesignComboBoxFileDialog);
    // build shortcuts
    buildShortcuts();
    // continue depending of open mode
    switch (myOpenMode) {
        case GNEFileDialog::OpenMode::SAVE:
            myFileSelector->showOnlyDirectories(FALSE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
        case GNEFileDialog::OpenMode::LOAD_SINGLE:
            myFileSelector->showOnlyDirectories(FALSE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
        case GNEFileDialog::OpenMode::LOAD_MULTIPLE:
            myFileSelector->showOnlyDirectories(FALSE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_EXTENDEDSELECT);
            break;
        case GNEFileDialog::OpenMode::LOAD_DIRECTORY:
            myFileSelector->showOnlyDirectories(TRUE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
        default:
            throw ProcessError("Invalid open mode");
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
            myFileFilterComboBox->appendIconItem(extension.c_str());
        }
    }
    myFileFilterComboBox->setNumVisible(FXMIN((int)extensions.size(), 12));
    // parse extensions
    parseExtensions(extensions);
    // apply first filter
    myFileSelector->setPattern(FXFileSelector::patternFromText(extensions.front().c_str()));
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
}


void
GNEFileSelector::setPath(const std::string& path) {
    myFilenameTextField->setText(path.c_str(), TRUE);
}


void
GNEFileSelector::setFilter(const int index) {
    myFileFilterComboBox->setCurrentItem(index, TRUE);
}


long
GNEFileSelector::onUpdNewFolder(FXObject* sender, FXSelector, void*) {
    // check if directory is writable
    const bool writable = FXStat::isWritable(myFileSelector->getDirectory());
    return sender->handle(this, writable ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
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
    const auto filenameList = getSelectedFiles();
    // first check if we have files to copy
    if (filenameList.size() > 0) {
        // get only first filename
        const std::string originFilePath = filenameList.front();
        // create default destiny filename
        std::string destinyFilename = FXPath::absolute(FXPath::directory(originFilePath.c_str()), "CopyOf" + FXPath::name(originFilePath.c_str())).text();
        // create file path dialog
        const auto filePathDialog = new GNEFilePathDialog(myFileDialog->getApplicationWindow(), TL("Copy File"), TL("Select destination file"), destinyFilename);
        // continue depending of filePathDialog results
        if (filePathDialog->getResult() == GNEDialog::Result::ACCEPT) {
            // get destiny filename from dialog
            destinyFilename = filePathDialog->getFilePath();
            // check if we selected the same file
            if (FXFile::identical(originFilePath.c_str(), destinyFilename.c_str())) {
                // open error dialog
                GNEErrorBasicDialog(myFileDialog->getApplicationWindow(), TL("Error copying file"),
                                    TLF("Unable to copy file:\n%\n", destinyFilename),
                                    TL("The source and destination files are the same"));
            } else {
                // check if file exist
                if (FXStat::exists(destinyFilename.c_str())) {
                    // open question dialog
                    const auto overwriteDialog = GNEQuestionBasicDialog(myFileDialog->getApplicationWindow(), GNEDialog::Buttons::YES_NO,
                                                 TL("Overwrite file"), TLF("The destination file:\n%\n", destinyFilename),
                                                 TL("already exist. Overwrite?"));
                    // check if abort
                    if (overwriteDialog.getResult() != GNEDialog::Result::ACCEPT) {
                        return 1;
                    }
                }
                // try to copy overwritten
                if (!FXFile::copyFiles(originFilePath.c_str(), destinyFilename.c_str(), TRUE)) {
                    // open error dialog
                    GNEErrorBasicDialog(myFileDialog->getApplicationWindow(), TL("Error copying file"),
                                        TLF("Unable to copy file:\n%\n", destinyFilename),
                                        TL("Check destination file permissions"));
                }
            }
        }
    }
    return 1;
}


long
GNEFileSelector::onCmdMove(FXObject*, FXSelector, void*) {
    const auto filenameList = getSelectedFiles();
    // first check if we have files to move
    if (filenameList.size() > 0) {
        // get only first filename
        const std::string originFilePath = filenameList.front();
        // create file path dialog
        const auto filePathDialog = new GNEFilePathDialog(myFileDialog->getApplicationWindow(), TL("Move File"), TL("Select destination file"), originFilePath);
        // continue depending of filePathDialog results
        if (filePathDialog->getResult() == GNEDialog::Result::ACCEPT) {
            // get destiny filename from dialog
            const std::string destinyFilename = filePathDialog->getFilePath();
            // check if we selected the same file
            if (FXFile::identical(originFilePath.c_str(), destinyFilename.c_str())) {
                // open error dialog
                GNEErrorBasicDialog(myFileDialog->getApplicationWindow(), TL("Error moving file"),
                                    TLF("Unable to move file:\n%\n", destinyFilename),
                                    TL("The source and destination files are the same"));
            } else {
                // check if file exist
                if (FXStat::exists(destinyFilename.c_str())) {
                    // open question dialog
                    const auto overwriteDialog = GNEQuestionBasicDialog(myFileDialog->getApplicationWindow(), GNEDialog::Buttons::YES_NO,
                                                 TL("Overwrite file"), TLF("The destination file:\n%\n", destinyFilename),
                                                 TL("already exist. Overwrite?"));
                    // check if abort
                    if (overwriteDialog.getResult() != GNEDialog::Result::ACCEPT) {
                        return 1;
                    }
                }
                // try to move overwritten
                if (!FXFile::moveFiles(originFilePath.c_str(), destinyFilename.c_str(), TRUE)) {
                    // open error dialog
                    GNEErrorBasicDialog(myFileDialog->getApplicationWindow(), TL("Error moving file"),
                                        TLF("Unable to move file:\n%\n", destinyFilename),
                                        TL("Check destination file permissions"));
                }
            }
        }
    }
    return 1;
}


long
GNEFileSelector::onCmdDelete(FXObject*, FXSelector, void*) {
    const auto filenameList = getSelectedFiles();
    // first check if we have files to link
    if (filenameList.size() > 0) {
        // get only first filename
        const std::string fileToDelete = filenameList.front();
        // open question dialog
        const auto askDialog = GNEQuestionBasicDialog(myFileDialog->getApplicationWindow(), GNEDialog::Buttons::YES_NO,
                               TL("Deleting file"), TL("Are you sure you want to delete the file:"),
                               fileToDelete);
        // check if continue
        if (askDialog.getResult() == GNEDialog::Result::ACCEPT) {
            // try to remove it
            if (!FXFile::removeFiles(fileToDelete.c_str(), TRUE)) {
                // open error dialog
                GNEErrorBasicDialog(myFileDialog->getApplicationWindow(), TL("Error deleting file"),
                                    TLF("Unable to delete file:\n%\n", fileToDelete),
                                    TL("Check file permissions"));
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
GNEFileSelector::onKeyPress(FXObject* obj, FXSelector sel, void* ptr) {
    // if ESC key is pressed, close dialog aborting
    FXEvent* event = (FXEvent*)ptr;
    if (event->code == KEY_Return) {
        return onCmdAccept(obj, sel, ptr);
    } else {
        return FXVerticalFrame::onKeyPress(obj, sel, ptr);
    }
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
    auto configDirectory = new FXMenuCommand(&filemenu, TL("Config directory"), nullptr, this, MID_GNE_BUTTON_CONFIG);
    // disable if configuration file is empty
    if (OptionsCont::getOptions().getString("configuration-file").empty()) {
        configDirectory->disable();
    }
    // set icon and tip depending of config type
    if (myConfigType == GNEFileDialog::ConfigType::NETEDIT) {
        configDirectory->setIcon(GUIIconSubSys::getIcon(GUIIcon::NETEDIT_MINI));
    } else {
        configDirectory->setIcon(GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI));
    }
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
    // disabled linker because it doesn't work
    new FXMenuCommand(&filemenu, TL("Delete..."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_DELETE), this, FXFileSelector::ID_DELETE);

    filemenu.create();
    filemenu.popup(NULL, event->root_x, event->root_y);
    getApp()->runModalWhileShown(&filemenu);
    return 1;
}


long
GNEFileSelector::onCmdFilter(FXObject*, FXSelector, void* ptr) {
    const FXString pat = FXFileSelector::patternFromText((FXchar*)ptr);
    myFileSelector->setPattern(pat);
    if (myOpenMode == GNEFileDialog::OpenMode::SAVE) {
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
    const FXString abspath = FXPath::absolute(path);
    myFileSelector->setDirectory(abspath);
    myDirBox->setDirectory(abspath);
    if (myOpenMode != GNEFileDialog::OpenMode::SAVE) {
        myFilenameTextField->setText(FXString::null);
    }
}


void
GNEFileSelector::setFilename(const FXString& path) {
    const FXString fullname(FXPath::absolute(path));
    FXString name(FXPath::name(fullname));
    myFileSelector->setCurrentFile(fullname);
    myDirBox->setDirectory(myFileSelector->getDirectory());
    myFilenameTextField->setText(name);
}


std::string
GNEFileSelector::getFilename() const {
    if (myOpenMode == GNEFileDialog::OpenMode::LOAD_MULTIPLE) {
        for (FXint i = 0; i < myFileSelector->getNumItems(); i++) {
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
    return getSelectedFilesOnly();
}


std::string
GNEFileSelector::getDirectory() const {
    return myFileSelector->getDirectory().text();
}


const std::vector<std::string>&
GNEFileSelector::getFileExtension() const {
    return myExtensions.at(myFileFilterComboBox->getCurrentItem());
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


MFXTextFieldIcon*
GNEFileSelector::getFilenameTextField() const {
    return myFilenameTextField;
}


long
GNEFileSelector::onCmdItemSelected(FXObject*, FXSelector, void* ptr) {
    const FXint index = (FXint)(FXival)ptr;
    FXString text, file;
    if (myOpenMode == GNEFileDialog::OpenMode::LOAD_MULTIPLE) {
        for (FXint i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + myFileSelector->getItemFilename(i) + "\"";
            }
        }
        myFilenameTextField->setText(text);
    } else if (myOpenMode == GNEFileDialog::OpenMode::LOAD_DIRECTORY) {
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
    FXString text, file;
    if (myOpenMode == GNEFileDialog::OpenMode::LOAD_MULTIPLE) {
        for (FXint i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
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
GNEFileSelector::onCmdItemDoubleClicked(FXObject* obj, FXSelector sel, void* ptr) {
    const FXint index = (FXint)(FXival)ptr;
    if (0 <= index) {
        // If directory, open the directory
        if (myFileSelector->isItemShare(index) || myFileSelector->isItemDirectory(index)) {
            setDirectory(myFileSelector->getItemPathname(index));
            return 1;
        }
        // Only return if we wanted a file
        if (myOpenMode != GNEFileDialog::OpenMode::LOAD_DIRECTORY) {
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
    if (path.size() > 0) {
        // Is directory?
        if (FXStat::isDirectory(path.c_str())) {
            // In directory mode:- we got our answer!
            if (myOpenMode == GNEFileDialog::OpenMode::LOAD_DIRECTORY) {
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
            if (myOpenMode == GNEFileDialog::OpenMode::SAVE) {
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
        FXASSERT(dir.length() <= (int)path.size());
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
    // check if this is a top directory
    const bool topDirectory = FXPath::isTopDirectory(myFileSelector->getDirectory());
    return sender->handle(this, topDirectory ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
}


long
GNEFileSelector::onCmdHomeFolder(FXObject*, FXSelector, void*) {
    setDirectory(FXSystem::getHomeDirectory());
    return 1;
}


long
GNEFileSelector::onCmdWorkFolder(FXObject*, FXSelector, void*) {
    setDirectory(FXSystem::getCurrentDirectory());
    return 1;
}


long
GNEFileSelector::onCmdConfigFolder(FXObject*, FXSelector, void*) {
    // get config file folder
    const auto configFileFolder = FXPath::directory(OptionsCont::getOptions().getString("configuration-file").c_str());
    setDirectory(configFileFolder);
    return 1;
}


long
GNEFileSelector::onCmdVisit(FXObject*, FXSelector, void* ptr) {
    setDirectory((FXchar*)ptr);
    return 1;
}


long
GNEFileSelector::onCmdBookmark(FXObject*, FXSelector, void*) {
    myBookmarksRecentFiles.appendFile(myFileSelector->getDirectory());
    return 1;
}


long
GNEFileSelector::onCmdDirTree(FXObject*, FXSelector, void* ptr) {
    myFileSelector->setDirectory((FXchar*)ptr);
    if (myOpenMode == GNEFileDialog::OpenMode::LOAD_DIRECTORY) {
        myFilenameTextField->setText(FXString::null);
    }
    return 1;
}


long
GNEFileSelector::onCmdNewFolder(FXObject*, FXSelector, void*) {
    // create file path dialog
    const auto filePathDialog = new GNEFilePathDialog(myFileDialog->getApplicationWindow(), TL("Create New Directory"), TL("Create new directory with name:"), "DirectoryName");
    // continue depending of filePathDialog results
    if (filePathDialog->getResult() == GNEDialog::Result::ACCEPT) {
        const FXString dirname = FXPath::absolute(myFileSelector->getDirectory(), filePathDialog->getFilePath().c_str());
        // check if exist
        if (FXStat::exists(dirname)) {
            // open error dialog
            GNEErrorBasicDialog(myFileDialog->getApplicationWindow(), TL("Directory already Exists"),
                                TLF("The new directory:\n%", dirname.text()),
                                TL("already exists"));
            return 1;
        }
        // try to create it
        if (!FXDir::create(dirname)) {
            // open error dialog
            GNEErrorBasicDialog(myFileDialog->getApplicationWindow(), TL("Cannot create directory"),
                                TLF("Cannot create directory:\n%", dirname.text()),
                                TL("Check folder permissions"));
            return 1;
        }
        // set as current directory
        setDirectory(dirname);
    }
    return 1;
}


void
GNEFileSelector::buildBookmarkMenuPane(FXHorizontalFrame* navigatorHorizontalFrame) {
    // create bookmarks menu pane
    myBookmarkMenuPane = new FXMenuPane(this, POPUP_SHRINKWRAP);
    new FXMenuCommand(myBookmarkMenuPane, (TL("Set bookmark") + std::string("\t\t") + TL("Bookmark current directory.")).c_str(),
                      GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_SET), this, FXFileSelector::ID_BOOKMARK);
    new FXMenuCommand(myBookmarkMenuPane, (TL("Clear bookmarks") + std::string("\t\t") + TL("Clear bookmarks.")).c_str(),
                      GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_CLR), &myBookmarksRecentFiles, FXRecentFiles::ID_CLEAR);
    FXMenuSeparator* separator = new FXMenuSeparator(myBookmarkMenuPane);
    separator->setTarget(&myBookmarksRecentFiles);
    separator->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, nullptr, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, nullptr, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, nullptr, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, nullptr, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, nullptr, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, nullptr, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, nullptr, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, nullptr, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, nullptr, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, nullptr, &myBookmarksRecentFiles, FXRecentFiles::ID_FILE_10);
    new FXFrame(navigatorHorizontalFrame, LAYOUT_FIX_WIDTH, 0, 0, 4, 1);
    myBookmarksRecentFiles.setTarget(this);
    myBookmarksRecentFiles.setSelector(FXFileSelector::ID_VISIT);
}


void
GNEFileSelector::buildButtons(FXHorizontalFrame* navigatorHorizontalFrame, MFXStaticToolTip* staticTooltipMenu) {
    // create button for going up one directory
    auto goUpButton = new MFXButtonTooltip(navigatorHorizontalFrame, staticTooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_DIRUP_ICON),
                                           this, FXFileSelector::ID_DIRECTORY_UP, GUIDesignButtonIconFileDialog);
    goUpButton->setTipText(TL("Go up one directory"));
    // create button for go to home directory
    auto goHomeButton = new MFXButtonTooltip(navigatorHorizontalFrame, staticTooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_GOTO_HOME),
            this, FXFileSelector::ID_HOME, GUIDesignButtonIconFileDialog);
    goHomeButton->setTipText(TL("Go to home directory"));
    // create button for go to work directory
    auto goWorkDirectory = new MFXButtonTooltip(navigatorHorizontalFrame, staticTooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_GOTO_WORK),
            this, FXFileSelector::ID_WORK, GUIDesignButtonIconFileDialog);
    goWorkDirectory->setTipText(TL("Go to work directory"));
    // create button for go to work directory
    auto goConfigDirectory = new MFXButtonTooltip(navigatorHorizontalFrame, staticTooltipMenu, "", nullptr,
            this, MID_GNE_BUTTON_CONFIG, GUIDesignButtonIconFileDialog);
    // disable if configuration file is empty
    if (OptionsCont::getOptions().getString("configuration-file").empty()) {
        goConfigDirectory->disable();
    }
    // set icon and tip depending of config type
    if (myConfigType == GNEFileDialog::ConfigType::NETEDIT) {
        goConfigDirectory->setIcon(GUIIconSubSys::getIcon(GUIIcon::NETEDIT_MINI));
        goConfigDirectory->setTipText(TL("Go to netedit config directory"));
    } else {
        goConfigDirectory->setIcon(GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI));
        goConfigDirectory->setTipText(TL("Go to sumo config directory"));
    }
    // create button for bookmarks menu
    auto bookmenuTooltip = new MFXMenuButtonTooltip(navigatorHorizontalFrame, staticTooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_SET),
            myBookmarkMenuPane, this, GUIDesignButtonIconFileDialog);
    bookmenuTooltip->setTipText(TL("Bookmarks"));
    bookmenuTooltip->setSelector(FXFileSelector::ID_BOOKMENU);
    // create button for creating a new directory
    auto newDirectoryButton = new MFXButtonTooltip(navigatorHorizontalFrame, staticTooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FOLDER_NEW),
            this, FXFileSelector::ID_NEW, GUIDesignButtonIconFileDialog);
    newDirectoryButton->setTipText(TL("Create new directory"));
    // create button for show lists
    auto showListButton = new MFXButtonTooltip(navigatorHorizontalFrame, staticTooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_SHOW_SMALLICONS),
            myFileSelector, FXFileList::ID_SHOW_MINI_ICONS, GUIDesignButtonIconFileDialog);
    showListButton->setTipText(TL("Display directory with small icons"));
    // create button for show icons
    auto showIconsButton = new MFXButtonTooltip(navigatorHorizontalFrame, staticTooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_SHOW_BIGICONS),
            myFileSelector, FXFileList::ID_SHOW_BIG_ICONS, GUIDesignButtonIconFileDialog);
    showIconsButton->setTipText(TL("Display directory with big icons"));
    // create button for show details
    auto showDetailsButton = new MFXButtonTooltip(navigatorHorizontalFrame, staticTooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_SHOW_DETAILS),
            myFileSelector, FXFileList::ID_SHOW_DETAILS, GUIDesignButtonIconFileDialog);
    showDetailsButton->setTipText(TL("Display detailed directory listing"));
    // create button for toogle show/hide hidden files
    auto showHiddeToogleButton = new MFXToggleButtonTooltip(navigatorHorizontalFrame, staticTooltipMenu, "", "",
            GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_HIDDEN),
            GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_SHOWN),
            myFileSelector, FXFileList::ID_TOGGLE_HIDDEN, GUIDesignButtonIconFileDialog);
    showHiddeToogleButton->setTipText(TL("Toggle show hidden files and directories"));
}


void
GNEFileSelector::buildShortcuts() {
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
}


void
GNEFileSelector::parseExtensions(const std::vector<std::string>& extensions) {
    // convert extensions in FXString
    for (const auto& extension : extensions) {
        // first get all characters within () excluding spaces
        const std::string cleanExtension = FXFileSelector::patternFromText(extension.c_str()).text();
        // declare subextensions
        std::vector<std::string> subExtensions;
        // now subdivide
        if (cleanExtension != "*") {
            // split extensions
            const auto subExtensionsStr = StringTokenizer(cleanExtension, ",").getVector();
            for (const auto& subExtensionStr : subExtensionsStr) {
                // ignore first *
                subExtensions.push_back(subExtensionStr.substr(1));
            }
        }
        myExtensions.push_back(subExtensions);
    }
}

/*******************************************************************************/
