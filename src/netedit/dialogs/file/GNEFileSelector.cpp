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
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNEFileSelector.h"

#define FILELISTMASK  (ICONLIST_EXTENDEDSELECT|ICONLIST_SINGLESELECT|ICONLIST_BROWSESELECT|ICONLIST_MULTIPLESELECT)
#define FILESTYLEMASK (ICONLIST_DETAILED|ICONLIST_MINI_ICONS|ICONLIST_BIG_ICONS|ICONLIST_ROWS|ICONLIST_COLUMNS|ICONLIST_AUTOSIZE)

// Default pattern
static const FXchar allfiles[] = "All Files (*)";

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
FXIMPLEMENT(GNEFileSelector, FXPacker, GNEFileSelectorMap, ARRAYNUMBER(GNEFileSelectorMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFileSelector::GNEFileSelector(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXPacker(p, opts, x, y, w, h), myBookmarsRecentFiles(p->getApp(), "Visited Directories") {
    FXAccelTable* table = getShell()->getAccelTable();
    target = tgt;
    message = sel;
    auto navbuttons = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X, 0, 0, 0, 0, DEFAULT_SPACING, DEFAULT_SPACING, DEFAULT_SPACING, DEFAULT_SPACING, 0, 0);
    auto entryblock = new FXMatrix(this, 3, MATRIX_BY_COLUMNS | LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X);
    new FXLabel(entryblock, TL("File Name:"), NULL, JUSTIFY_LEFT | LAYOUT_CENTER_Y);
    myFilenameTextField = new FXTextField(entryblock, 25, this, FXFileSelector::ID_ACCEPT, TEXTFIELD_ENTER_ONLY | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | FRAME_SUNKEN | FRAME_THICK);
    new FXButton(entryblock, TL("OK"), NULL, this, FXFileSelector::ID_ACCEPT, BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_FILL_X, 0, 0, 0, 0, 20, 20);
    accept = new FXButton(navbuttons, FXString::null, NULL, NULL, 0, LAYOUT_FIX_X | LAYOUT_FIX_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT, 0, 0, 0, 0, 0, 0, 0, 0);
    new FXLabel(entryblock, TL("File Filter:"), NULL, JUSTIFY_LEFT | LAYOUT_CENTER_Y);
    FXHorizontalFrame* filterframe = new FXHorizontalFrame(entryblock, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    myFileFilterComboBox = new FXComboBox(filterframe, 10, this, FXFileSelector::ID_FILEFILTER, COMBOBOX_STATIC | LAYOUT_FILL_X | FRAME_SUNKEN | FRAME_THICK);
    myFileFilterComboBox->setNumVisible(4);
    cancel = new FXButton(entryblock, TL("Cancel"), NULL, NULL, 0, BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_FILL_X, 0, 0, 0, 0, 20, 20);
    auto fileboxframe = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_SUNKEN | FRAME_THICK, 0, 0, 0, 0, 0, 0, 0, 0);
    myFileSelector = new FXFileList(fileboxframe, this, FXFileSelector::ID_FILELIST, ICONLIST_MINI_ICONS | ICONLIST_BROWSESELECT | ICONLIST_AUTOSIZE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXLabel(navbuttons, TL("Directory:"), NULL, LAYOUT_CENTER_Y);
    myDirBox = new FXDirBox(navbuttons, this, FXFileSelector::ID_DIRTREE, DIRBOX_NO_OWN_ASSOC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_CENTER_Y, 0, 0, 0, 0, 1, 1, 1, 1);
    myDirBox->setNumVisible(5);
    myDirBox->setAssociations(myFileSelector->getAssociations());
    myBookmarkMenuPane = new FXMenuPane(this, POPUP_SHRINKWRAP);
    new FXMenuCommand(myBookmarkMenuPane, TL("Set bookmark\t\tBookmark current directory."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_SET), this, FXFileSelector::ID_BOOKMARK);
    new FXMenuCommand(myBookmarkMenuPane, TL("Clear myBookmarsRecentFiles\t\tClear myBookmarsRecentFiles."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_CLR), &myBookmarsRecentFiles, FXRecentFiles::ID_CLEAR);
    FXMenuSeparator* sep1 = new FXMenuSeparator(myBookmarkMenuPane);
    sep1->setTarget(&myBookmarsRecentFiles);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(myBookmarkMenuPane, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_10);
    new FXFrame(navbuttons, LAYOUT_FIX_WIDTH, 0, 0, 4, 1);
    new FXButton(navbuttons, TL("\tGo up one directory\tMove up to higher directory."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_DIRUP_ICON), this, FXFileSelector::ID_DIRECTORY_UP, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXButton(navbuttons, TL("\tGo to home directory\tBack to home directory."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_GOTO_HOME), this, FXFileSelector::ID_HOME, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXButton(navbuttons, TL("\tGo to work directory\tBack to working directory."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_GOTO_WORK), this, FXFileSelector::ID_WORK, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    FXMenuButton* bookmenu = new FXMenuButton(navbuttons, TL("\tBookmarks\tVisit bookmarked directories."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_SET), myBookmarkMenuPane, MENUBUTTON_NOARROWS | MENUBUTTON_ATTACH_LEFT | MENUBUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    bookmenu->setTarget(this);
    bookmenu->setSelector(FXFileSelector::ID_BOOKMENU);
    new FXButton(navbuttons, TL("\tCreate new directory\tCreate new directory."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FOLDER_NEW), this, FXFileSelector::ID_NEW, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXButton(navbuttons, TL("\tShow list\tDisplay directory with small icons."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_SHOW_SMALLICONS), myFileSelector, FXFileList::ID_SHOW_MINI_ICONS, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXButton(navbuttons, TL("\tShow icons\tDisplay directory with big icons."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_SHOW_BIGICONS), myFileSelector, FXFileList::ID_SHOW_BIG_ICONS, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXButton(navbuttons, TL("\tShow details\tDisplay detailed directory listing."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_SHOW_DETAILS), myFileSelector, FXFileList::ID_SHOW_DETAILS, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXToggleButton(navbuttons, TL("\tShow hidden files\tShow hidden files and directories."), TL("\tHide Hidden Files\tHide hidden files and directories."), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_HIDDEN), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_FILE_SHOWN), myFileSelector, FXFileList::ID_TOGGLE_HIDDEN, TOGGLEBUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    myBookmarsRecentFiles.setTarget(this);
    myBookmarsRecentFiles.setSelector(FXFileSelector::ID_VISIT);
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
    setSelectMode(SELECTFILE_ANY);    // For backward compatibility, this HAS to be the default!
    setPatternList(allfiles);
    setDirectory(FXSystem::getCurrentDirectory());
    myFileSelector->setFocus();
    accept->hide();
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
    accept = (FXButton*) - 1L;
    cancel = (FXButton*) - 1L;
}


long
GNEFileSelector::onUpdNew(FXObject* sender, FXSelector, void*) {
    sender->handle(this, FXStat::isWritable(getDirectory()) ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}


FXString*
GNEFileSelector::getSelectedFiles() const {
    FXString* files = NULL;
    FXint i, n;
    for (i = n = 0; i < myFileSelector->getNumItems(); i++) {
        if (myFileSelector->isItemSelected(i) && myFileSelector->getItemFilename(i) != ".." && myFileSelector->getItemFilename(i) != ".") {
            n++;
        }
    }
    if (n) {
        files = new FXString [n + 1];
        for (i = n = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && myFileSelector->getItemFilename(i) != ".." && myFileSelector->getItemFilename(i) != ".") {
                files[n++] = myFileSelector->getItemPathname(i);
            }
        }
        files[n] = FXString::null;
    }
    return files;
}


FXString*
GNEFileSelector::getSelectedFilesOnly() const {
    FXString* files = NULL;
    FXint i, n;
    for (i = n = 0; i < myFileSelector->getNumItems(); i++) {
        if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
            n++;
        }
    }
    if (n) {
        files = new FXString [n + 1];
        for (i = n = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
                files[n++] = myFileSelector->getItemPathname(i);
            }
        }
        files[n] = FXString::null;
    }
    return files;
}


long
GNEFileSelector::onCmdCopy(FXObject*, FXSelector, void*) {
    FXString* filenamelist = getSelectedFiles();
    FXString copymessage;
    if (filenamelist) {
        for (FXint i = 0; !filenamelist[i].empty(); i++) {
            copymessage.format(TL("Copy file from location:\n\n%s\n\nto location: "), filenamelist[i].text());
            FXInputDialog inputdialog(this, TL("Copy File"), copymessage, NULL, INPUTDIALOG_STRING, 0, 0, 0, 0);
            inputdialog.setText(FXPath::absolute(FXPath::directory(filenamelist[i]), "CopyOf" + FXPath::name(filenamelist[i])));
            inputdialog.setNumColumns(60);
            if (inputdialog.execute()) {
                FXString newname = inputdialog.getText();
                if (!FXFile::copyFiles(filenamelist[i], newname, FALSE)) {
                    if (FXMessageBox::error(this, MBOX_YES_NO, TL("Error Copying File"), TL("Unable to copy file:\n\n%s  to:  %s\n\nContinue with operation?"), filenamelist[i].text(), newname.text()) == MBOX_CLICKED_NO) {
                        break;
                    }
                }
            }
        }
        delete [] filenamelist;
    }
    return 1;
}


long
GNEFileSelector::onCmdMove(FXObject*, FXSelector, void*) {
    FXString* filenamelist = getSelectedFiles();
    FXString movemessage;
    if (filenamelist) {
        for (FXint i = 0; !filenamelist[i].empty(); i++) {
            movemessage.format(TL("Move file from location:\n\n%s\n\nto location: "), filenamelist[i].text());
            FXInputDialog inputdialog(this, TL("Move File"), movemessage, NULL, INPUTDIALOG_STRING, 0, 0, 0, 0);
            inputdialog.setText(filenamelist[i]);
            inputdialog.setNumColumns(60);
            if (inputdialog.execute()) {
                FXString newname = inputdialog.getText();
                if (!FXFile::moveFiles(filenamelist[i], newname, FALSE)) {
                    if (FXMessageBox::error(this, MBOX_YES_NO, TL("Error Moving File"), TL("Unable to move file:\n\n%s  to:  %s\n\nContinue with operation?"), filenamelist[i].text(), newname.text()) == MBOX_CLICKED_NO) {
                        break;
                    }
                }
            }
        }
        delete [] filenamelist;
    }
    return 1;
}


long
GNEFileSelector::onCmdLink(FXObject*, FXSelector, void*) {
    FXString* filenamelist = getSelectedFiles();
    FXString linkmessage;
    if (filenamelist) {
        for (FXint i = 0; !filenamelist[i].empty(); i++) {
            linkmessage.format(TL("Link file from location:\n\n%s\n\nto location: "), filenamelist[i].text());
            FXInputDialog inputdialog(this, TL("Link File"), linkmessage, NULL, INPUTDIALOG_STRING, 0, 0, 0, 0);
            inputdialog.setText(FXPath::absolute(FXPath::directory(filenamelist[i]), "LinkTo" + FXPath::name(filenamelist[i])));
            inputdialog.setNumColumns(60);
            if (inputdialog.execute()) {
                FXString newname = inputdialog.getText();
                if (!FXFile::symlink(filenamelist[i], newname)) {
                    if (FXMessageBox::error(this, MBOX_YES_NO, TL("Error Linking File"), TL("Unable to link file:\n\n%s  to:  %s\n\nContinue with operation?"), filenamelist[i].text(), newname.text()) == MBOX_CLICKED_NO) {
                        break;
                    }
                }
            }
        }
        delete [] filenamelist;
    }
    return 1;
}


long
GNEFileSelector::onCmdDelete(FXObject*, FXSelector, void*) {
    FXString* filenamelist = getSelectedFiles();
    FXuint answer;
    if (filenamelist) {
        for (FXint i = 0; !filenamelist[i].empty(); i++) {
            answer = FXMessageBox::warning(this, MBOX_YES_NO_CANCEL, TL("Deleting files"), TL("Are you sure you want to delete the file:\n\n%s"), filenamelist[i].text());
            if (answer == MBOX_CLICKED_CANCEL) {
                break;
            }
            if (answer == MBOX_CLICKED_NO) {
                continue;
            }
            if (!FXFile::removeFiles(filenamelist[i], TRUE)) {
                if (FXMessageBox::error(this, MBOX_YES_NO, TL("Error Deleting File"), TL("Unable to delete file:\n\n%s\n\nContinue with operation?"), filenamelist[i].text()) == MBOX_CLICKED_NO) {
                    break;
                }
            }
        }
        delete [] filenamelist;
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
    new FXMenuCommand(&bookmenu, TL("Clear myBookmarsRecentFiles"), GUIIconSubSys::getIcon(GUIIcon::FILEDIALOG_BOOK_CLR), &myBookmarsRecentFiles, FXRecentFiles::ID_CLEAR);
    FXMenuSeparator* sep1 = new FXMenuSeparator(&bookmenu);
    sep1->setTarget(&myBookmarsRecentFiles);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &myBookmarsRecentFiles, FXRecentFiles::ID_FILE_10);

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
    if (mySelectmode == SELECTFILE_ANY) {
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
    if (mySelectmode != SELECTFILE_ANY) {
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


FXString
GNEFileSelector::getFilename() const {
    FXint i;
    if (mySelectmode == SELECTFILE_MULTIPLE_ALL) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && myFileSelector->getItemFilename(i) != ".." && myFileSelector->getItemFilename(i) != ".") {
                return FXPath::absolute(myFileSelector->getDirectory(), myFileSelector->getItemFilename(i));
            }
        }
    } else if (mySelectmode == SELECTFILE_MULTIPLE) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
                return FXPath::absolute(myFileSelector->getDirectory(), myFileSelector->getItemFilename(i));
            }
        }
    } else {
        if (!myFilenameTextField->getText().empty()) {
            //return FXPath::absolute(myFileSelector->getDirectory(),myFilenameTextField->getText());
            return FXPath::absolute(myFileSelector->getDirectory(), FXPath::expand(myFilenameTextField->getText()));    // FIXME don't always want to expand!
        }
    }
    return FXString::null;
}


FXString*
GNEFileSelector::getFilenames() const {
    FXString* files;
    if (mySelectmode == SELECTFILE_MULTIPLE_ALL) {
        files = getSelectedFiles();
    } else {
        files = getSelectedFilesOnly();
    }
    return files;
}


void
GNEFileSelector::setPatternList(const FXString& patterns) {
    FXint count;
    myFileFilterComboBox->clearItems();
    count = myFileFilterComboBox->fillItems(patterns);
    if (count == 0) {
        myFileFilterComboBox->appendItem(allfiles);
    }
    myFileFilterComboBox->setNumVisible(FXMIN(count, 12));
    setCurrentPattern(0);
}


FXString
GNEFileSelector::getPatternList() const {
    FXString pat;
    for (FXint i = 0; i < myFileFilterComboBox->getNumItems(); i++) {
        if (!pat.empty()) {
            pat += '\n';
        }
        pat += myFileFilterComboBox->getItemText(i);
    }
    return pat;
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
        throw ProcessError("index out of range.\n");
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
        throw ProcessError("index out of range.");
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
GNEFileSelector::setSelectMode(FXuint mode) {
    switch (mode) {
        case SELECTFILE_EXISTING:
            myFileSelector->showOnlyDirectories(FALSE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
        case SELECTFILE_MULTIPLE:
        case SELECTFILE_MULTIPLE_ALL:
            myFileSelector->showOnlyDirectories(FALSE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_EXTENDEDSELECT);
            break;
        case SELECTFILE_DIRECTORY:
            myFileSelector->showOnlyDirectories(TRUE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
        default:
            myFileSelector->showOnlyDirectories(FALSE);
            myFileSelector->setListStyle((myFileSelector->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
    }
    mySelectmode = mode;
}


FXuint
GNEFileSelector::getSelectMode() const {
    return mySelectmode;
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
    if (mySelectmode == SELECTFILE_MULTIPLE) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + myFileSelector->getItemFilename(i) + "\"";
            }
        }
        myFilenameTextField->setText(text);
    } else if (mySelectmode == SELECTFILE_MULTIPLE_ALL) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && myFileSelector->getItemFilename(i) != ".." && myFileSelector->getItemFilename(i) != ".") {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + myFileSelector->getItemFilename(i) + "\"";
            }
        }
        myFilenameTextField->setText(text);
    } else if (mySelectmode == SELECTFILE_DIRECTORY) {
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
    if (mySelectmode == SELECTFILE_MULTIPLE) {
        for (i = 0; i < myFileSelector->getNumItems(); i++) {
            if (myFileSelector->isItemSelected(i) && !myFileSelector->isItemDirectory(i)) {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + myFileSelector->getItemFilename(i) + "\"";
            }
        }
        myFilenameTextField->setText(text);
    } else if (mySelectmode == SELECTFILE_MULTIPLE_ALL) {
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
GNEFileSelector::onCmdItemDblClicked(FXObject*, FXSelector, void* ptr) {
    FXSelector sel = accept->getSelector();
    FXObject* tgt = accept->getTarget();
    FXint index = (FXint)(FXival)ptr;
    if (0 <= index) {

        // If directory, open the directory
        if (myFileSelector->isItemShare(index) || myFileSelector->isItemDirectory(index)) {
            setDirectory(myFileSelector->getItemPathname(index));
            return 1;
        }

        // Only return if we wanted a file
        if (mySelectmode != SELECTFILE_DIRECTORY) {
            if (tgt) {
                tgt->handle(accept, FXSEL(SEL_COMMAND, sel), (void*)(FXuval)1);
            }
        }
    }
    return 1;
}


long
GNEFileSelector::onCmdAccept(FXObject*, FXSelector, void*) {
    FXSelector sel = accept->getSelector();
    FXObject* tgt = accept->getTarget();

    // Get (first) myFilenameTextField or directory
    FXString path = getFilename();

    // Only do something if a selection was made
    if (!path.empty()) {

        // Is directory?
        if (FXStat::isDirectory(path)) {

            // In directory mode:- we got our answer!
            if (mySelectmode == SELECTFILE_DIRECTORY || mySelectmode == SELECTFILE_MULTIPLE_ALL) {
                if (tgt) {
                    tgt->handle(accept, FXSEL(SEL_COMMAND, sel), (void*)(FXuval)1);
                }
                return 1;
            }

            // Hop over to that directory
            myDirBox->setDirectory(path);
            myFileSelector->setDirectory(path);
            myFilenameTextField->setText(FXString::null);
            return 1;
        }

        // Get directory part of path
        FXString dir = FXPath::directory(path);

        // In file mode, directory part of path should exist
        if (FXStat::isDirectory(dir)) {

            // In any mode, existing directory part is good enough
            if (mySelectmode == SELECTFILE_ANY) {
                if (tgt) {
                    tgt->handle(accept, FXSEL(SEL_COMMAND, sel), (void*)(FXuval)1);
                }
                return 1;
            }

            // Otherwise, the whole myFilenameTextField must exist and be a file
            if (FXStat::exists(path)) {
                if (tgt) {
                    tgt->handle(accept, FXSEL(SEL_COMMAND, sel), (void*)(FXuval)1);
                }
                return 1;
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
        myFilenameTextField->setText(path);
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
    myBookmarsRecentFiles.appendFile(getDirectory());
    return 1;
}


long
GNEFileSelector::onCmdDirTree(FXObject*, FXSelector, void* ptr) {
    myFileSelector->setDirectory((FXchar*)ptr);
    if (mySelectmode == SELECTFILE_DIRECTORY) {
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
