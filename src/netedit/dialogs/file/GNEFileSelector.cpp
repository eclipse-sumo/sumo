/********************************************************************************
*                                                                               *
*                  F i l e   S e l e c t i o n   W i d g e t                    *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2006 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: FXFileSelector.cpp,v 1.196 2006/01/23 15:51:05 fox Exp $                 *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxkeys.h"
#include "FXHash.h"
#include "FXThread.h"
#include "FXStream.h"
#include "FXObjectList.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXPath.h"
#include "FXSystem.h"
#include "FXStat.h"
#include "FXFile.h"
#include "FXDir.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXAccelTable.h"
#include "FXApp.h"
#include "FXFont.h"
#include "FXGIFIcon.h"
#include "FXRecentFiles.h"
#include "FXFrame.h"
#include "FXLabel.h"
#include "FXTextField.h"
#include "FXButton.h"
#include "FXToggleButton.h"
#include "FXCheckButton.h"
#include "FXMenuButton.h"
#include "FXPacker.h"
#include "FXHorizontalFrame.h"
#include "FXVerticalFrame.h"
#include "FXMatrix.h"
#include "FXShell.h"
#include "FXPopup.h"
#include "FXMenuPane.h"
#include "FXScrollBar.h"
#include "FXScrollArea.h"
#include "FXList.h"
#include "FXTreeList.h"
#include "FXComboBox.h"
#include "FXTreeListBox.h"
#include "FXDirBox.h"
#include "FXHeader.h"
#include "FXIconList.h"
#include "FXFileList.h"
#include "FXFileSelector.h"
#include "FXMenuCaption.h"
#include "FXMenuCommand.h"
#include "FXMenuCascade.h"
#include "FXMenuRadio.h"
#include "FXMenuCheck.h"
#include "FXMenuSeparator.h"
#include "FXTopWindow.h"
#include "FXDialogBox.h"
#include "FXInputDialog.h"
#include "FXSeparator.h"
#include "FXMessageBox.h"
#include "icons.h"

/*
  Notes:
  - Getting a file name according to what we want:

    - Any filename for saving (but with existing dir part)
    - An existing file for loading
    - An existing directory
    - Multiple filenames.

  - Get network drives to work.

  - Change filter specification; below sets two filters:

      "Source Files (*.cpp,*.cc,*.C)\nHeader files (*.h,*.H)"

    Instead of ',' you should also be able to use '|' in the above.

  - Multi-file mode needs to allow for manual entry in the text field.

  - Got nifty handling when entering path in text field:

      1) If its a directory you typed, switch to the directory.

      2) If the directory part of the file name exists:
         if SELECTFILE_ANY mode, then we're done.
         if SELECTFILE_EXISTING mode AND file exists, we're done.
         if SELECTFILE_MULTIPLE mode AND all files exist, we're done.

      3) Else use the fragment of the directory which still exists, and
         switch to that directory; leave the incorrect tail-end in the
         text field to be edited further

  - In directory mode, only way to return is by accept.

  - Switching directories zaps text field value, but not in SELECTFILE_ANY
    mode, because when saving a file you may want to give the same name
    even if directory changes.

  - When changing filter, maybe update the extension (if not more than
    one extension given).

  - Perhaps ".." should be excluded from SELECTFILE_MULTIPLE_ALL selections.
  - Drag corner would be nice.
  - When copying, moving, deleting, linking multiple files, build the list
    of selected files first, to take care of FXFileList possibly updating
    before operation is finished.
*/


#define FILELISTMASK  (ICONLIST_EXTENDEDSELECT|ICONLIST_SINGLESELECT|ICONLIST_BROWSESELECT|ICONLIST_MULTIPLESELECT)
#define FILESTYLEMASK (ICONLIST_DETAILED|ICONLIST_MINI_ICONS|ICONLIST_BIG_ICONS|ICONLIST_ROWS|ICONLIST_COLUMNS|ICONLIST_AUTOSIZE)


using namespace FX;

/*******************************************************************************/

namespace FX {


// Map
FXDEFMAP(FXFileSelector) FXFileSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_ACCEPT, FXFileSelector::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_FILEFILTER, FXFileSelector::onCmdFilter),
    FXMAPFUNC(SEL_DOUBLECLICKED, FXFileSelector::ID_FILELIST, FXFileSelector::onCmdItemDblClicked),
    FXMAPFUNC(SEL_SELECTED, FXFileSelector::ID_FILELIST, FXFileSelector::onCmdItemSelected),
    FXMAPFUNC(SEL_DESELECTED, FXFileSelector::ID_FILELIST, FXFileSelector::onCmdItemDeselected),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE, FXFileSelector::ID_FILELIST, FXFileSelector::onPopupMenu),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_DIRECTORY_UP, FXFileSelector::onCmdDirectoryUp),
    FXMAPFUNC(SEL_UPDATE, FXFileSelector::ID_DIRECTORY_UP, FXFileSelector::onUpdDirectoryUp),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_DIRTREE, FXFileSelector::onCmdDirTree),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_HOME, FXFileSelector::onCmdHome),
    FXMAPFUNC(SEL_UPDATE, FXFileSelector::ID_HOME, FXFileSelector::onUpdNavigable),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_WORK, FXFileSelector::onCmdWork),
    FXMAPFUNC(SEL_UPDATE, FXFileSelector::ID_WORK, FXFileSelector::onUpdNavigable),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_VISIT, FXFileSelector::onCmdVisit),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_BOOKMARK, FXFileSelector::onCmdBookmark),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_NEW, FXFileSelector::onCmdNew),
    FXMAPFUNC(SEL_UPDATE, FXFileSelector::ID_NEW, FXFileSelector::onUpdNew),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_DELETE, FXFileSelector::onCmdDelete),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_MOVE, FXFileSelector::onCmdMove),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_COPY, FXFileSelector::onCmdCopy),
    FXMAPFUNC(SEL_COMMAND, FXFileSelector::ID_LINK, FXFileSelector::onCmdLink),
    FXMAPFUNC(SEL_UPDATE, FXFileSelector::ID_COPY, FXFileSelector::onUpdSelected),
    FXMAPFUNC(SEL_UPDATE, FXFileSelector::ID_MOVE, FXFileSelector::onUpdSelected),
    FXMAPFUNC(SEL_UPDATE, FXFileSelector::ID_LINK, FXFileSelector::onUpdSelected),
    FXMAPFUNC(SEL_UPDATE, FXFileSelector::ID_DELETE, FXFileSelector::onUpdSelected),
    FXMAPFUNC(SEL_UPDATE, FXFileSelector::ID_BOOKMENU, FXFileSelector::onUpdNavigable),
    FXMAPFUNCS(SEL_COMMAND, FXFileSelector::ID_NORMAL_SIZE, FXFileSelector::ID_GIANT_SIZE, FXFileSelector::onCmdImageSize),
    FXMAPFUNCS(SEL_UPDATE, FXFileSelector::ID_NORMAL_SIZE, FXFileSelector::ID_GIANT_SIZE, FXFileSelector::onUpdImageSize),
};


// Implementation
FXIMPLEMENT(FXFileSelector, FXPacker, FXFileSelectorMap, ARRAYNUMBER(FXFileSelectorMap))


// Default pattern
static const FXchar allfiles[] = "All Files (*)";


/*******************************************************************************/

// Separator item
FXFileSelector::FXFileSelector(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h): FXPacker(p, opts, x, y, w, h), bookmarks(p->getApp(), "Visited Directories") {
    FXAccelTable* table = getShell()->getAccelTable();
    target = tgt;
    message = sel;
    navbuttons = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X, 0, 0, 0, 0, DEFAULT_SPACING, DEFAULT_SPACING, DEFAULT_SPACING, DEFAULT_SPACING, 0, 0);
    entryblock = new FXMatrix(this, 3, MATRIX_BY_COLUMNS | LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X);
    new FXLabel(entryblock, tr("&File Name:"), NULL, JUSTIFY_LEFT | LAYOUT_CENTER_Y);
    filename = new FXTextField(entryblock, 25, this, ID_ACCEPT, TEXTFIELD_ENTER_ONLY | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | FRAME_SUNKEN | FRAME_THICK);
    new FXButton(entryblock, tr("&OK"), NULL, this, ID_ACCEPT, BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_FILL_X, 0, 0, 0, 0, 20, 20);
    accept = new FXButton(navbuttons, FXString::null, NULL, NULL, 0, LAYOUT_FIX_X | LAYOUT_FIX_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT, 0, 0, 0, 0, 0, 0, 0, 0);
    new FXLabel(entryblock, tr("File F&ilter:"), NULL, JUSTIFY_LEFT | LAYOUT_CENTER_Y);
    FXHorizontalFrame* filterframe = new FXHorizontalFrame(entryblock, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    filefilter = new FXComboBox(filterframe, 10, this, ID_FILEFILTER, COMBOBOX_STATIC | LAYOUT_FILL_X | FRAME_SUNKEN | FRAME_THICK);
    filefilter->setNumVisible(4);
    readonly = new FXCheckButton(filterframe, tr("Read Only"), NULL, 0, ICON_BEFORE_TEXT | JUSTIFY_LEFT | LAYOUT_CENTER_Y);
    cancel = new FXButton(entryblock, tr("&Cancel"), NULL, NULL, 0, BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_FILL_X, 0, 0, 0, 0, 20, 20);
    fileboxframe = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_SUNKEN | FRAME_THICK, 0, 0, 0, 0, 0, 0, 0, 0);
    filebox = new FXFileList(fileboxframe, this, ID_FILELIST, ICONLIST_MINI_ICONS | ICONLIST_BROWSESELECT | ICONLIST_AUTOSIZE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXLabel(navbuttons, tr("Directory:"), NULL, LAYOUT_CENTER_Y);
    updiricon = new FXGIFIcon(getApp(), dirupicon);
    listicon = new FXGIFIcon(getApp(), showsmallicons);
    iconsicon = new FXGIFIcon(getApp(), showbigicons);
    detailicon = new FXGIFIcon(getApp(), showdetails);
    homeicon = new FXGIFIcon(getApp(), gotohome);
    workicon = new FXGIFIcon(getApp(), gotowork);
    shownicon = new FXGIFIcon(getApp(), fileshown);
    hiddenicon = new FXGIFIcon(getApp(), filehidden);
    markicon = new FXGIFIcon(getApp(), bookset);
    clearicon = new FXGIFIcon(getApp(), bookclr);
    newicon = new FXGIFIcon(getApp(), foldernew);
    deleteicon = new FXGIFIcon(getApp(), filedelete);
    moveicon = new FXGIFIcon(getApp(), filemove);
    copyicon = new FXGIFIcon(getApp(), filecopy);
    linkicon = new FXGIFIcon(getApp(), filelink);
    dirbox = new FXDirBox(navbuttons, this, ID_DIRTREE, DIRBOX_NO_OWN_ASSOC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_CENTER_Y, 0, 0, 0, 0, 1, 1, 1, 1);
    dirbox->setNumVisible(5);
    dirbox->setAssociations(filebox->getAssociations());
    bookmarkmenu = new FXMenuPane(this, POPUP_SHRINKWRAP);
    new FXMenuCommand(bookmarkmenu, tr("&Set bookmark\t\tBookmark current directory."), markicon, this, ID_BOOKMARK);
    new FXMenuCommand(bookmarkmenu, tr("&Clear bookmarks\t\tClear bookmarks."), clearicon, &bookmarks, FXRecentFiles::ID_CLEAR);
    FXMenuSeparator* sep1 = new FXMenuSeparator(bookmarkmenu);
    sep1->setTarget(&bookmarks);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(bookmarkmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(bookmarkmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(bookmarkmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(bookmarkmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(bookmarkmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(bookmarkmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(bookmarkmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(bookmarkmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(bookmarkmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(bookmarkmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_10);
    new FXFrame(navbuttons, LAYOUT_FIX_WIDTH, 0, 0, 4, 1);
    new FXButton(navbuttons, tr("\tGo up one directory\tMove up to higher directory."), updiricon, this, ID_DIRECTORY_UP, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXButton(navbuttons, tr("\tGo to home directory\tBack to home directory."), homeicon, this, ID_HOME, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXButton(navbuttons, tr("\tGo to work directory\tBack to working directory."), workicon, this, ID_WORK, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    FXMenuButton* bookmenu = new FXMenuButton(navbuttons, tr("\tBookmarks\tVisit bookmarked directories."), markicon, bookmarkmenu, MENUBUTTON_NOARROWS | MENUBUTTON_ATTACH_LEFT | MENUBUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    bookmenu->setTarget(this);
    bookmenu->setSelector(ID_BOOKMENU);
    new FXButton(navbuttons, tr("\tCreate new directory\tCreate new directory."), newicon, this, ID_NEW, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXButton(navbuttons, tr("\tShow list\tDisplay directory with small icons."), listicon, filebox, FXFileList::ID_SHOW_MINI_ICONS, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXButton(navbuttons, tr("\tShow icons\tDisplay directory with big icons."), iconsicon, filebox, FXFileList::ID_SHOW_BIG_ICONS, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXButton(navbuttons, tr("\tShow details\tDisplay detailed directory listing."), detailicon, filebox, FXFileList::ID_SHOW_DETAILS, BUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    new FXToggleButton(navbuttons, tr("\tShow hidden files\tShow hidden files and directories."), tr("\tHide Hidden Files\tHide hidden files and directories."), hiddenicon, shownicon, filebox, FXFileList::ID_TOGGLE_HIDDEN, TOGGLEBUTTON_TOOLBAR | FRAME_RAISED, 0, 0, 0, 0, 3, 3, 3, 3);
    bookmarks.setTarget(this);
    bookmarks.setSelector(ID_VISIT);
    readonly->hide();
    if (table) {
        table->addAccel(MKUINT(KEY_BackSpace, 0), this, FXSEL(SEL_COMMAND, ID_DIRECTORY_UP));
        table->addAccel(MKUINT(KEY_Delete, 0), this, FXSEL(SEL_COMMAND, ID_DELETE));
        table->addAccel(MKUINT(KEY_h, CONTROLMASK), this, FXSEL(SEL_COMMAND, ID_HOME));
        table->addAccel(MKUINT(KEY_w, CONTROLMASK), this, FXSEL(SEL_COMMAND, ID_WORK));
        table->addAccel(MKUINT(KEY_n, CONTROLMASK), this, FXSEL(SEL_COMMAND, ID_NEW));
        table->addAccel(MKUINT(KEY_a, CONTROLMASK), filebox, FXSEL(SEL_COMMAND, FXFileList::ID_SELECT_ALL));
        table->addAccel(MKUINT(KEY_b, CONTROLMASK), filebox, FXSEL(SEL_COMMAND, FXFileList::ID_SHOW_BIG_ICONS));
        table->addAccel(MKUINT(KEY_s, CONTROLMASK), filebox, FXSEL(SEL_COMMAND, FXFileList::ID_SHOW_MINI_ICONS));
        table->addAccel(MKUINT(KEY_l, CONTROLMASK), filebox, FXSEL(SEL_COMMAND, FXFileList::ID_SHOW_DETAILS));
    }
    setSelectMode(SELECTFILE_ANY);    // For backward compatibility, this HAS to be the default!
    setPatternList(allfiles);
    setDirectory(FXSystem::getCurrentDirectory());
    filebox->setFocus();
    accept->hide();
    navigable = TRUE;
}


// Change in items which are selected
long FXFileSelector::onCmdItemSelected(FXObject*, FXSelector, void* ptr) {
    FXint index = (FXint)(FXival)ptr;
    FXint i;
    FXString text, file;
    if (selectmode == SELECTFILE_MULTIPLE) {
        for (i = 0; i < filebox->getNumItems(); i++) {
            if (filebox->isItemSelected(i) && !filebox->isItemDirectory(i)) {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + filebox->getItemFilename(i) + "\"";
            }
        }
        filename->setText(text);
    } else if (selectmode == SELECTFILE_MULTIPLE_ALL) {
        for (i = 0; i < filebox->getNumItems(); i++) {
            if (filebox->isItemSelected(i) && filebox->getItemFilename(i) != ".." && filebox->getItemFilename(i) != ".") {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + filebox->getItemFilename(i) + "\"";
            }
        }
        filename->setText(text);
    } else if (selectmode == SELECTFILE_DIRECTORY) {
        if (filebox->isItemDirectory(index)) {
            text = filebox->getItemFilename(index);
            filename->setText(text);
        }
    } else {
        if (!filebox->isItemDirectory(index)) {
            text = filebox->getItemFilename(index);
            filename->setText(text);
        }
    }
    return 1;
}


// Change in items which are selected
long FXFileSelector::onCmdItemDeselected(FXObject*, FXSelector, void*) {
    FXint i;
    FXString text, file;
    if (selectmode == SELECTFILE_MULTIPLE) {
        for (i = 0; i < filebox->getNumItems(); i++) {
            if (filebox->isItemSelected(i) && !filebox->isItemDirectory(i)) {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + filebox->getItemFilename(i) + "\"";
            }
        }
        filename->setText(text);
    } else if (selectmode == SELECTFILE_MULTIPLE_ALL) {
        for (i = 0; i < filebox->getNumItems(); i++) {
            if (filebox->isItemSelected(i) && filebox->getItemFilename(i) != ".." && filebox->getItemFilename(i) != ".") {
                if (!text.empty()) {
                    text += ' ';
                }
                text += "\"" + filebox->getItemFilename(i) + "\"";
            }
        }
        filename->setText(text);
    }
    return 1;
}


// Double-clicked item in file list
long FXFileSelector::onCmdItemDblClicked(FXObject*, FXSelector, void* ptr) {
    FXSelector sel = accept->getSelector();
    FXObject* tgt = accept->getTarget();
    FXint index = (FXint)(FXival)ptr;
    if (0 <= index) {

        // If directory, open the directory
        if (filebox->isItemShare(index) || filebox->isItemDirectory(index)) {
            if (allowNavigation()) {
                setDirectory(filebox->getItemPathname(index));
            }
            return 1;
        }

        // Only return if we wanted a file
        if (selectmode != SELECTFILE_DIRECTORY) {
            if (tgt) {
                tgt->handle(accept, FXSEL(SEL_COMMAND, sel), (void*)(FXuval)1);
            }
        }
    }
    return 1;
}


// Hit the accept button or enter in text field
long FXFileSelector::onCmdAccept(FXObject*, FXSelector, void*) {
    FXSelector sel = accept->getSelector();
    FXObject* tgt = accept->getTarget();

    // Get (first) filename or directory
    FXString path = getFilename();

    // Only do something if a selection was made
    if (!path.empty()) {

        // Is directory?
        if (FXStat::isDirectory(path)) {

            // In directory mode:- we got our answer!
            if (selectmode == SELECTFILE_DIRECTORY || selectmode == SELECTFILE_MULTIPLE_ALL) {
                if (tgt) {
                    tgt->handle(accept, FXSEL(SEL_COMMAND, sel), (void*)(FXuval)1);
                }
                return 1;
            }

            // No navigation allowed
            if (!allowNavigation()) {
                filename->setText(FXPath::relative(getDirectory(), path));
                filename->selectAll();
                getApp()->beep();
                return 1;
            }

            // Hop over to that directory
            dirbox->setDirectory(path);
            filebox->setDirectory(path);
            filename->setText(FXString::null);
            return 1;
        }

        // Get directory part of path
        FXString dir = FXPath::directory(path);

        // In file mode, directory part of path should exist
        if (FXStat::isDirectory(dir)) {

            // In any mode, existing directory part is good enough
            if (selectmode == SELECTFILE_ANY) {
                if (tgt) {
                    tgt->handle(accept, FXSEL(SEL_COMMAND, sel), (void*)(FXuval)1);
                }
                return 1;
            }

            // Otherwise, the whole filename must exist and be a file
            if (FXStat::exists(path)) {
                if (tgt) {
                    tgt->handle(accept, FXSEL(SEL_COMMAND, sel), (void*)(FXuval)1);
                }
                return 1;
            }
        }

        // No navigation allowed
        if (!allowNavigation()) {
            filename->setText(FXPath::relative(getDirectory(), path));
            filename->selectAll();
            getApp()->beep();
            return 1;
        }

        // Go up to the lowest directory which still exists
        while (!FXPath::isTopDirectory(dir) && !FXStat::isDirectory(dir)) {
            dir = FXPath::upLevel(dir);
        }

        // Switch as far as we could go
        dirbox->setDirectory(dir);
        filebox->setDirectory(dir);

        // Put the tail end back for further editing
        FXASSERT(dir.length() <= path.length());
        if (ISPATHSEP(path[dir.length()])) {
            path.erase(0, dir.length() + 1);
        } else {
            path.erase(0, dir.length());
        }

        // Replace text box with new stuff
        filename->setText(path);
        filename->selectAll();
    }

    // Beep
    getApp()->beep();
    return 1;
}


// User clicked up directory button
long FXFileSelector::onCmdDirectoryUp(FXObject*, FXSelector, void*) {
    if (allowNavigation()) {
        setDirectory(FXPath::upLevel(filebox->getDirectory()));
    }
    return 1;
}


// Can we still go up
long FXFileSelector::onUpdDirectoryUp(FXObject* sender, FXSelector, void*) {
    sender->handle(this, allowNavigation() && !FXPath::isTopDirectory(getDirectory()) ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}


// Can we navigate
long FXFileSelector::onUpdNavigable(FXObject* sender, FXSelector, void*) {
    sender->handle(this, allowNavigation() ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}


// Back to home directory
long FXFileSelector::onCmdHome(FXObject*, FXSelector, void*) {
    if (allowNavigation()) {
        setDirectory(FXSystem::getHomeDirectory());
    }
    return 1;
}


// Back to current working directory
long FXFileSelector::onCmdWork(FXObject*, FXSelector, void*) {
    if (allowNavigation()) {
        setDirectory(FXSystem::getCurrentDirectory());
    }
    return 1;
}


// Move to recent directory
long FXFileSelector::onCmdVisit(FXObject*, FXSelector, void* ptr) {
    if (allowNavigation()) {
        setDirectory((FXchar*)ptr);
    }
    return 1;
}


// Bookmark this directory
long FXFileSelector::onCmdBookmark(FXObject*, FXSelector, void*) {
    bookmarks.appendFile(getDirectory());
    return 1;
}


// Switched directories using directory tree
long FXFileSelector::onCmdDirTree(FXObject*, FXSelector, void* ptr) {
    if (allowNavigation()) {
        filebox->setDirectory((FXchar*)ptr);
        if (selectmode == SELECTFILE_DIRECTORY) {
            filename->setText(FXString::null);
        }
    } else {
        dirbox->setDirectory(getDirectory());
    }
    return 1;
}


// Create new directory
long FXFileSelector::onCmdNew(FXObject*, FXSelector, void*) {
    FXString dir = filebox->getDirectory();
    FXString name = "DirectoryName";
    FXGIFIcon newdirectoryicon(getApp(), bigfolder);
    if (FXInputDialog::getString(name, this, tr("Create New Directory"), tr("Create new directory with name: "), &newdirectoryicon)) {
        FXString dirname = FXPath::absolute(dir, name);
        if (FXStat::exists(dirname)) {
            FXMessageBox::error(this, MBOX_OK, tr("Already Exists"), tr("File or directory %s already exists.\n"), dirname.text());
            return 1;
        }
        if (!FXDir::create(dirname)) {
            FXMessageBox::error(this, MBOX_OK, tr("Cannot Create"), tr("Cannot create directory %s.\n"), dirname.text());
            return 1;
        }
        setDirectory(dirname);
    }
    return 1;
}


// Update create new directory
long FXFileSelector::onUpdNew(FXObject* sender, FXSelector, void*) {
    sender->handle(this, allowNavigation() && FXStat::isWritable(getDirectory()) ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}


// Selected files and directories
FXString* FXFileSelector::getSelectedFiles() const {
    FXString* files = NULL;
    FXint i, n;
    for (i = n = 0; i < filebox->getNumItems(); i++) {
        if (filebox->isItemSelected(i) && filebox->getItemFilename(i) != ".." && filebox->getItemFilename(i) != ".") {
            n++;
        }
    }
    if (n) {
        files = new FXString [n + 1];
        for (i = n = 0; i < filebox->getNumItems(); i++) {
            if (filebox->isItemSelected(i) && filebox->getItemFilename(i) != ".." && filebox->getItemFilename(i) != ".") {
                files[n++] = filebox->getItemPathname(i);
            }
        }
        files[n] = FXString::null;
    }
    return files;
}


// Selected files only
FXString* FXFileSelector::getSelectedFilesOnly() const {
    FXString* files = NULL;
    FXint i, n;
    for (i = n = 0; i < filebox->getNumItems(); i++) {
        if (filebox->isItemSelected(i) && !filebox->isItemDirectory(i)) {
            n++;
        }
    }
    if (n) {
        files = new FXString [n + 1];
        for (i = n = 0; i < filebox->getNumItems(); i++) {
            if (filebox->isItemSelected(i) && !filebox->isItemDirectory(i)) {
                files[n++] = filebox->getItemPathname(i);
            }
        }
        files[n] = FXString::null;
    }
    return files;
}


// Copy file or directory
long FXFileSelector::onCmdCopy(FXObject*, FXSelector, void*) {
    FXString* filenamelist = getSelectedFiles();
    FXString copymessage;
    if (filenamelist) {
        for (FXint i = 0; !filenamelist[i].empty(); i++) {
            copymessage.format(tr("Copy file from location:\n\n%s\n\nto location: "), filenamelist[i].text());
            FXInputDialog inputdialog(this, tr("Copy File"), copymessage, NULL, INPUTDIALOG_STRING, 0, 0, 0, 0);
            inputdialog.setText(FXPath::absolute(FXPath::directory(filenamelist[i]), "CopyOf" + FXPath::name(filenamelist[i])));
            inputdialog.setNumColumns(60);
            if (inputdialog.execute()) {
                FXString newname = inputdialog.getText();
                if (!FXFile::copyFiles(filenamelist[i], newname, FALSE)) {
                    if (FXMessageBox::error(this, MBOX_YES_NO, tr("Error Copying File"), tr("Unable to copy file:\n\n%s  to:  %s\n\nContinue with operation?"), filenamelist[i].text(), newname.text()) == MBOX_CLICKED_NO) {
                        break;
                    }
                }
            }
        }
        delete [] filenamelist;
    }
    return 1;
}


// Move file or directory
long FXFileSelector::onCmdMove(FXObject*, FXSelector, void*) {
    FXString* filenamelist = getSelectedFiles();
    FXString movemessage;
    if (filenamelist) {
        for (FXint i = 0; !filenamelist[i].empty(); i++) {
            movemessage.format(tr("Move file from location:\n\n%s\n\nto location: "), filenamelist[i].text());
            FXInputDialog inputdialog(this, tr("Move File"), movemessage, NULL, INPUTDIALOG_STRING, 0, 0, 0, 0);
            inputdialog.setText(filenamelist[i]);
            inputdialog.setNumColumns(60);
            if (inputdialog.execute()) {
                FXString newname = inputdialog.getText();
                if (!FXFile::moveFiles(filenamelist[i], newname, FALSE)) {
                    if (FXMessageBox::error(this, MBOX_YES_NO, tr("Error Moving File"), tr("Unable to move file:\n\n%s  to:  %s\n\nContinue with operation?"), filenamelist[i].text(), newname.text()) == MBOX_CLICKED_NO) {
                        break;
                    }
                }
            }
        }
        delete [] filenamelist;
    }
    return 1;
}


// Link file or directory
long FXFileSelector::onCmdLink(FXObject*, FXSelector, void*) {
    FXString* filenamelist = getSelectedFiles();
    FXString linkmessage;
    if (filenamelist) {
        for (FXint i = 0; !filenamelist[i].empty(); i++) {
            linkmessage.format(tr("Link file from location:\n\n%s\n\nto location: "), filenamelist[i].text());
            FXInputDialog inputdialog(this, tr("Link File"), linkmessage, NULL, INPUTDIALOG_STRING, 0, 0, 0, 0);
            inputdialog.setText(FXPath::absolute(FXPath::directory(filenamelist[i]), "LinkTo" + FXPath::name(filenamelist[i])));
            inputdialog.setNumColumns(60);
            if (inputdialog.execute()) {
                FXString newname = inputdialog.getText();
                if (!FXFile::symlink(filenamelist[i], newname)) {
                    if (FXMessageBox::error(this, MBOX_YES_NO, tr("Error Linking File"), tr("Unable to link file:\n\n%s  to:  %s\n\nContinue with operation?"), filenamelist[i].text(), newname.text()) == MBOX_CLICKED_NO) {
                        break;
                    }
                }
            }
        }
        delete [] filenamelist;
    }
    return 1;
}


// Delete file or directory
long FXFileSelector::onCmdDelete(FXObject*, FXSelector, void*) {
    FXString* filenamelist = getSelectedFiles();
    FXuint answer;
    if (filenamelist) {
        for (FXint i = 0; !filenamelist[i].empty(); i++) {
            answer = FXMessageBox::warning(this, MBOX_YES_NO_CANCEL, tr("Deleting files"), tr("Are you sure you want to delete the file:\n\n%s"), filenamelist[i].text());
            if (answer == MBOX_CLICKED_CANCEL) {
                break;
            }
            if (answer == MBOX_CLICKED_NO) {
                continue;
            }
            if (!FXFile::removeFiles(filenamelist[i], TRUE)) {
                if (FXMessageBox::error(this, MBOX_YES_NO, tr("Error Deleting File"), tr("Unable to delete file:\n\n%s\n\nContinue with operation?"), filenamelist[i].text()) == MBOX_CLICKED_NO) {
                    break;
                }
            }
        }
        delete [] filenamelist;
    }
    return 1;
}


// Sensitize when files are selected
long FXFileSelector::onUpdSelected(FXObject* sender, FXSelector, void*) {
    for (FXint i = 0; i < filebox->getNumItems(); i++) {
        if (filebox->isItemSelected(i)) {
            sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
            return 1;
        }
    }
    sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}


// Change image size
long FXFileSelector::onCmdImageSize(FXObject*, FXSelector sel, void*) {
    switch (FXSELID(sel)) {
        case ID_NORMAL_SIZE:
            setImageSize(32);
            break;
        case ID_MEDIUM_SIZE:
            setImageSize(48);
            break;
        case ID_GIANT_SIZE:
            setImageSize(64);
            break;
    }
    return 1;
}


// Update image size
long FXFileSelector::onUpdImageSize(FXObject* sender, FXSelector sel, void*) {
    FXbool check = FALSE;
    switch (FXSELID(sel)) {
        case ID_NORMAL_SIZE:
            check = (getImageSize() == 32);
            break;
        case ID_MEDIUM_SIZE:
            check = (getImageSize() == 48);
            break;
        case ID_GIANT_SIZE:
            check = (getImageSize() == 64);
            break;
    }
    sender->handle(this, check ? FXSEL(SEL_COMMAND, ID_CHECK) : FXSEL(SEL_COMMAND, ID_UNCHECK), NULL);
    return 1;
}


// Popup menu for item in file list
long FXFileSelector::onPopupMenu(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    if (event->moved) {
        return 1;
    }

    FXMenuPane filemenu(this);
    new FXMenuCommand(&filemenu, tr("Up one level"), updiricon, this, ID_DIRECTORY_UP);
    new FXMenuCommand(&filemenu, tr("Home directory"), homeicon, this, ID_HOME);
    new FXMenuCommand(&filemenu, tr("Work directory"), workicon, this, ID_WORK);
    new FXMenuCommand(&filemenu, tr("Select all"), NULL, filebox, FXFileList::ID_SELECT_ALL);
    new FXMenuSeparator(&filemenu);

    FXMenuPane sortmenu(this);
    new FXMenuCascade(&filemenu, tr("Sort by"), NULL, &sortmenu);
    new FXMenuRadio(&sortmenu, tr("Name"), filebox, FXFileList::ID_SORT_BY_NAME);
    new FXMenuRadio(&sortmenu, tr("Type"), filebox, FXFileList::ID_SORT_BY_TYPE);
    new FXMenuRadio(&sortmenu, tr("Size"), filebox, FXFileList::ID_SORT_BY_SIZE);
    new FXMenuRadio(&sortmenu, tr("Time"), filebox, FXFileList::ID_SORT_BY_TIME);
    new FXMenuRadio(&sortmenu, tr("User"), filebox, FXFileList::ID_SORT_BY_USER);
    new FXMenuRadio(&sortmenu, tr("Group"), filebox, FXFileList::ID_SORT_BY_GROUP);
    new FXMenuSeparator(&sortmenu);
    new FXMenuCheck(&sortmenu, tr("Reverse"), filebox, FXFileList::ID_SORT_REVERSE);
    new FXMenuCheck(&sortmenu, tr("Ignore case"), filebox, FXFileList::ID_SORT_CASE);

    FXMenuPane viewmenu(this);
    new FXMenuCascade(&filemenu, tr("View"), NULL, &viewmenu);
    new FXMenuRadio(&viewmenu, tr("Small icons"), filebox, FXFileList::ID_SHOW_MINI_ICONS);
    new FXMenuRadio(&viewmenu, tr("Big icons"), filebox, FXFileList::ID_SHOW_BIG_ICONS);
    new FXMenuRadio(&viewmenu, tr("Details"), filebox, FXFileList::ID_SHOW_DETAILS);
    new FXMenuSeparator(&viewmenu);
    new FXMenuRadio(&viewmenu, tr("Rows"), filebox, FXFileList::ID_ARRANGE_BY_ROWS);
    new FXMenuRadio(&viewmenu, tr("Columns"), filebox, FXFileList::ID_ARRANGE_BY_COLUMNS);
    new FXMenuSeparator(&viewmenu);
    new FXMenuCheck(&viewmenu, tr("Hidden files"), filebox, FXFileList::ID_TOGGLE_HIDDEN);
    new FXMenuCheck(&viewmenu, tr("Preview images"), filebox, FXFileList::ID_TOGGLE_IMAGES);
    new FXMenuSeparator(&viewmenu);
    new FXMenuRadio(&viewmenu, tr("Normal images"), this, ID_NORMAL_SIZE);
    new FXMenuRadio(&viewmenu, tr("Medium images"), this, ID_MEDIUM_SIZE);
    new FXMenuRadio(&viewmenu, tr("Giant images"), this, ID_GIANT_SIZE);

    FXMenuPane bookmenu(this);
    FXMenuCascade* bookcasc = new FXMenuCascade(&filemenu, tr("Bookmarks"), NULL, &bookmenu);
    bookcasc->setTarget(this);
    bookcasc->setSelector(ID_BOOKMENU);
    new FXMenuCommand(&bookmenu, tr("Set bookmark"), markicon, this, ID_BOOKMARK);
    new FXMenuCommand(&bookmenu, tr("Clear bookmarks"), clearicon, &bookmarks, FXRecentFiles::ID_CLEAR);
    FXMenuSeparator* sep1 = new FXMenuSeparator(&bookmenu);
    sep1->setTarget(&bookmarks);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(&bookmenu, FXString::null, NULL, &bookmarks, FXRecentFiles::ID_FILE_10);

    new FXMenuSeparator(&filemenu);
    new FXMenuCommand(&filemenu, tr("New directory..."), newicon, this, ID_NEW);
    new FXMenuCommand(&filemenu, tr("Copy..."), copyicon, this, ID_COPY);
    new FXMenuCommand(&filemenu, tr("Move..."), moveicon, this, ID_MOVE);
    new FXMenuCommand(&filemenu, tr("Link..."), linkicon, this, ID_LINK);
    new FXMenuCommand(&filemenu, tr("Delete..."), deleteicon, this, ID_DELETE);

    filemenu.create();
    filemenu.popup(NULL, event->root_x, event->root_y);
    getApp()->runModalWhileShown(&filemenu);
    return 1;
}


// Strip pattern from text if present
FXString FXFileSelector::patternFromText(const FXString& pattern) {
    FXint beg, end;
    end = pattern.rfind(')');       // Search from the end so we can allow ( ) in the pattern name itself
    beg = pattern.rfind('(', end - 1);
    if (0 <= beg && beg < end) {
        return pattern.mid(beg + 1, end - beg - 1);
    }
    return pattern;
}


// Return the first extension "ext1" found in the pattern if the
// pattern is of the form "*.ext1,*.ext2,..." or the empty string
// if the pattern contains other wildcard combinations.
FXString FXFileSelector::extensionFromPattern(const FXString& pattern) {
    FXint beg, end, c;
    beg = 0;
    if (pattern[beg] == '*') {
        beg++;
        if (pattern[beg] == '.') {
            beg++;
            end = beg;
            while ((c = pattern[end]) != '\0' && c != ',' && c != '|') {
                if (c == '*' || c == '?' || c == '[' || c == ']' || c == '^' || c == '!') {
                    return FXString::null;
                }
                end++;
            }
            return pattern.mid(beg, end - beg);
        }
    }
    return FXString::null;
}


// Change the pattern; change the filename to the suggested extension
long FXFileSelector::onCmdFilter(FXObject*, FXSelector, void* ptr) {
    FXString pat = patternFromText((FXchar*)ptr);
    filebox->setPattern(pat);
    if (selectmode == SELECTFILE_ANY) {
        FXString ext = extensionFromPattern(pat);
        if (!ext.empty()) {
            FXString name = FXPath::stripExtension(filename->getText());
            if (!name.empty()) {
                filename->setText(name + "." + ext);
            }
        }
    }
    return 1;
}


// Set directory
void FXFileSelector::setDirectory(const FXString& path) {
    FXString abspath = FXPath::absolute(path);
    FXTRACE((100, "path=%s abspath: %s\n", path.text(), abspath.text()));
    filebox->setDirectory(abspath);
    dirbox->setDirectory(abspath);
    if (selectmode != SELECTFILE_ANY) {
        filename->setText(FXString::null);
    }
}


// Get directory
FXString FXFileSelector::getDirectory() const {
    return filebox->getDirectory();
}


// Set file name
void FXFileSelector::setFilename(const FXString& path) {
    FXString fullname(FXPath::absolute(path));
    FXString name(FXPath::name(fullname));
    filebox->setCurrentFile(fullname);
    dirbox->setDirectory(filebox->getDirectory());
    filename->setText(name);
}


// Get complete path + filename
FXString FXFileSelector::getFilename() const {
    FXint i;
    if (selectmode == SELECTFILE_MULTIPLE_ALL) {
        for (i = 0; i < filebox->getNumItems(); i++) {
            if (filebox->isItemSelected(i) && filebox->getItemFilename(i) != ".." && filebox->getItemFilename(i) != ".") {
                return FXPath::absolute(filebox->getDirectory(), filebox->getItemFilename(i));
            }
        }
    } else if (selectmode == SELECTFILE_MULTIPLE) {
        for (i = 0; i < filebox->getNumItems(); i++) {
            if (filebox->isItemSelected(i) && !filebox->isItemDirectory(i)) {
                return FXPath::absolute(filebox->getDirectory(), filebox->getItemFilename(i));
            }
        }
    } else {
        if (!filename->getText().empty()) {
            //return FXPath::absolute(filebox->getDirectory(),filename->getText());
            return FXPath::absolute(filebox->getDirectory(), FXPath::expand(filename->getText()));    // FIXME don't always want to expand!
        }
    }
    return FXString::null;
}



// Return empty-string terminated list of selected file names, or NULL
FXString* FXFileSelector::getFilenames() const {
    FXString* files;
    if (selectmode == SELECTFILE_MULTIPLE_ALL) {
        files = getSelectedFiles();
    } else {
        files = getSelectedFilesOnly();
    }
    return files;
}


// Change patterns, each pattern separated by newline
void FXFileSelector::setPatternList(const FXString& patterns) {
    FXint count;
    filefilter->clearItems();
    count = filefilter->fillItems(patterns);
    if (count == 0) {
        filefilter->appendItem(allfiles);
    }
    filefilter->setNumVisible(FXMIN(count, 12));
    setCurrentPattern(0);
}


// Return list of patterns
FXString FXFileSelector::getPatternList() const {
    FXString pat;
    for (FXint i = 0; i < filefilter->getNumItems(); i++) {
        if (!pat.empty()) {
            pat += '\n';
        }
        pat += filefilter->getItemText(i);
    }
    return pat;
}


// Set current filter pattern
void FXFileSelector::setPattern(const FXString& ptrn) {
    filefilter->setText(ptrn);
    filebox->setPattern(ptrn);
}


// Get current filter pattern
FXString FXFileSelector::getPattern() const {
    return filebox->getPattern();
}


// Set current file pattern from the list
void FXFileSelector::setCurrentPattern(FXint patno) {
    if (patno < 0 || patno >= filefilter->getNumItems()) {
        fxerror("%s::setCurrentPattern: index out of range.\n", getClassName());
    }
    filefilter->setCurrentItem(patno);
    filebox->setPattern(patternFromText(filefilter->getItemText(patno)));
}


// Return current pattern
FXint FXFileSelector::getCurrentPattern() const {
    return filefilter->getCurrentItem();
}


// Change pattern for pattern number patno
void FXFileSelector::setPatternText(FXint patno, const FXString& text) {
    if (patno < 0 || patno >= filefilter->getNumItems()) {
        fxerror("%s::setPatternText: index out of range.\n", getClassName());
    }
    filefilter->setItemText(patno, text);
    if (patno == filefilter->getCurrentItem()) {
        setPattern(patternFromText(text));
    }
}


// Return pattern text of pattern patno
FXString FXFileSelector::getPatternText(FXint patno) const {
    if (patno < 0 || patno >= filefilter->getNumItems()) {
        fxerror("%s::getPatternText: index out of range.\n", getClassName());
    }
    return filefilter->getItemText(patno);
}


// Return number of patterns
FXint FXFileSelector::getNumPatterns() const {
    return filefilter->getNumItems();
}


// Allow pattern entry
void FXFileSelector::allowPatternEntry(FXbool allow) {
    filefilter->setComboStyle(allow ? COMBOBOX_NORMAL : COMBOBOX_STATIC);
}


// Return TRUE if pattern entry is allowed
FXbool FXFileSelector::allowPatternEntry() const {
    return (filefilter->getComboStyle() != COMBOBOX_STATIC);
}


// Change space for item
void FXFileSelector::setItemSpace(FXint s) {
    filebox->setItemSpace(s);
}


// Get space for item
FXint FXFileSelector::getItemSpace() const {
    return filebox->getItemSpace();
}


// Change File List style
void FXFileSelector::setFileBoxStyle(FXuint style) {
    filebox->setListStyle((filebox->getListStyle() & ~FILESTYLEMASK) | (style & FILESTYLEMASK));
}


// Return File List style
FXuint FXFileSelector::getFileBoxStyle() const {
    return filebox->getListStyle()&FILESTYLEMASK;
}


// Change file selection mode
void FXFileSelector::setSelectMode(FXuint mode) {
    switch (mode) {
        case SELECTFILE_EXISTING:
            filebox->showOnlyDirectories(FALSE);
            filebox->setListStyle((filebox->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
        case SELECTFILE_MULTIPLE:
        case SELECTFILE_MULTIPLE_ALL:
            filebox->showOnlyDirectories(FALSE);
            filebox->setListStyle((filebox->getListStyle() & ~FILELISTMASK) | ICONLIST_EXTENDEDSELECT);
            break;
        case SELECTFILE_DIRECTORY:
            filebox->showOnlyDirectories(TRUE);
            filebox->setListStyle((filebox->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
        default:
            filebox->showOnlyDirectories(FALSE);
            filebox->setListStyle((filebox->getListStyle() & ~FILELISTMASK) | ICONLIST_BROWSESELECT);
            break;
    }
    selectmode = mode;
}


// Change wildcard matching mode
void FXFileSelector::setMatchMode(FXuint mode) {
    filebox->setMatchMode(mode);
}


// Return wildcard matching mode
FXuint FXFileSelector::getMatchMode() const {
    return filebox->getMatchMode();
}


// Return TRUE if showing hidden files
FXbool FXFileSelector::showHiddenFiles() const {
    return filebox->showHiddenFiles();
}


// Show or hide hidden files
void FXFileSelector::showHiddenFiles(FXbool showing) {
    filebox->showHiddenFiles(showing);
}


// Return TRUE if image preview on
FXbool FXFileSelector::showImages() const {
    return filebox->showImages();
}


// Show or hide preview images
void FXFileSelector::showImages(FXbool showing) {
    filebox->showImages(showing);
}


// Return images preview size
FXint FXFileSelector::getImageSize() const {
    return filebox->getImageSize();
}


// Change images preview size
void FXFileSelector::setImageSize(FXint size) {
    filebox->setImageSize(size);
}


// Show readonly button
void FXFileSelector::showReadOnly(FXbool show) {
    show ? readonly->show() : readonly->hide();
}


// Return TRUE if readonly is shown
FXbool FXFileSelector::shownReadOnly() const {
    return readonly->shown();
}



// Set initial state of readonly button
void FXFileSelector::setReadOnly(FXbool state) {
    readonly->setCheck(state);
}


// Get readonly state
FXbool FXFileSelector::getReadOnly() const {
    return readonly->getCheck();
}


// Save data
void FXFileSelector::save(FXStream& store) const {
    FXPacker::save(store);
    store << filebox;
    store << filename;
    store << filefilter;
    store << bookmarkmenu;
    store << readonly;
    store << dirbox;
    store << accept;
    store << cancel;
    store << updiricon;
    store << listicon;
    store << detailicon;
    store << iconsicon;
    store << homeicon;
    store << workicon;
    store << shownicon;
    store << hiddenicon;
    store << markicon;
    store << clearicon;
    store << newicon;
    store << deleteicon;
    store << moveicon;
    store << copyicon;
    store << linkicon;
    store << selectmode;
    store << navigable;
}


// Load data
void FXFileSelector::load(FXStream& store) {
    FXPacker::load(store);
    store >> filebox;
    store >> filename;
    store >> filefilter;
    store >> bookmarkmenu;
    store >> readonly;
    store >> dirbox;
    store >> accept;
    store >> cancel;
    store >> updiricon;
    store >> listicon;
    store >> detailicon;
    store >> iconsicon;
    store >> homeicon;
    store >> workicon;
    store >> shownicon;
    store >> hiddenicon;
    store >> markicon;
    store >> clearicon;
    store >> newicon;
    store >> deleteicon;
    store >> moveicon;
    store >> copyicon;
    store >> linkicon;
    store >> selectmode;
    store >> navigable;
}


// Cleanup; icons must be explicitly deleted
FXFileSelector::~FXFileSelector() {
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
    delete bookmarkmenu;
    delete updiricon;
    delete listicon;
    delete detailicon;
    delete iconsicon;
    delete homeicon;
    delete workicon;
    delete shownicon;
    delete hiddenicon;
    delete markicon;
    delete clearicon;
    delete newicon;
    delete deleteicon;
    delete moveicon;
    delete copyicon;
    delete linkicon;
    filebox = (FXFileList*) - 1L;
    filename = (FXTextField*) - 1L;
    filefilter = (FXComboBox*) - 1L;
    bookmarkmenu = (FXMenuPane*) - 1L;
    readonly = (FXCheckButton*) - 1L;
    dirbox = (FXDirBox*) - 1L;
    accept = (FXButton*) - 1L;
    cancel = (FXButton*) - 1L;
    updiricon = (FXIcon*) - 1L;
    listicon = (FXIcon*) - 1L;
    detailicon = (FXIcon*) - 1L;
    iconsicon = (FXIcon*) - 1L;
    homeicon = (FXIcon*) - 1L;
    workicon = (FXIcon*) - 1L;
    shownicon = (FXIcon*) - 1L;
    hiddenicon = (FXIcon*) - 1L;
    markicon = (FXIcon*) - 1L;
    clearicon = (FXIcon*) - 1L;
    newicon = (FXIcon*) - 1L;
    deleteicon = (FXIcon*) - 1L;
    moveicon = (FXIcon*) - 1L;
    copyicon = (FXIcon*) - 1L;
    linkicon = (FXIcon*) - 1L;
}

}
