/****************************************************************************/
/// @file    GUIDialog_Breakpoints.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 17 Jun 2004
/// @version $Id$
///
// Editor for simulation breakpoints
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/iodevices/OutputDevice.h>
#include "GUIDialog_Breakpoints.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_Breakpoints) GUIDialog_BreakpointsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,  GUIDialog_Breakpoints::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,  GUIDialog_Breakpoints::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR, GUIDialog_Breakpoints::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,        GUIDialog_Breakpoints::onCmdClose),
    FXMAPFUNC(SEL_REPLACED, MID_TABLE,         GUIDialog_Breakpoints::onCmdEditTable),
};


FXIMPLEMENT(GUIDialog_Breakpoints, FXMainWindow, GUIDialog_BreakpointsMap, ARRAYNUMBER(GUIDialog_BreakpointsMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_Breakpoints::GUIDialog_Breakpoints(GUIMainWindow* parent, std::vector<SUMOTime>& breakpoints, FXMutex& breakpointLock) :
    FXMainWindow(parent->getApp(), "Breakpoints Editor", NULL, NULL, DECOR_ALL, 20, 20, 170, 300),
    myParent(parent), myBreakpoints(&breakpoints), myBreakpointLock(&breakpointLock) {
    FXHorizontalFrame* hbox = new FXHorizontalFrame(this, GUIDesignAuxiliarFrame);

    // build the table
    myTable = new FXTable(hbox, this, MID_TABLE, GUIDesignTable);
    myTable->setVisibleRows(20);
    myTable->setVisibleColumns(1);
    myTable->setTableSize(20, 1);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->getRowHeader()->setWidth(0);
    myBreakpointLock->lock();
    rebuildList();
    myBreakpointLock->unlock();
    // build the layout
    FXVerticalFrame* layout = new FXVerticalFrame(hbox, LAYOUT_TOP, 0, 0, 0, 0, 4, 4, 4, 4);

    // create buttons ('&' in the label creates a hot key)
    // "Load"
    new FXButton(layout, "&Load\t\t", 0, this, MID_CHOOSEN_LOAD, GUIDesignButtonBreakpoint);
    // "Save"
    new FXButton(layout, "&Save\t\t", 0, this, MID_CHOOSEN_SAVE, GUIDesignButtonBreakpoint);
    new FXHorizontalSeparator(layout, GUIDesignHorizontalSeparator);
    // "Clear List"
    new FXButton(layout, "Clea&r\t\t", 0, this, MID_CHOOSEN_CLEAR, GUIDesignButtonBreakpoint);
    new FXHorizontalSeparator(layout, GUIDesignHorizontalSeparator);
    // "Close"
    new FXButton(layout, "&Close\t\t", 0, this, MID_CANCEL, GUIDesignButtonBreakpoint);

    //
    setIcon(GUIIconSubSys::getIcon(ICON_APP_BREAKPOINTS));
    myParent->addChild(this);
}


GUIDialog_Breakpoints::~GUIDialog_Breakpoints() {
    myParent->removeChild(this);
}


void
GUIDialog_Breakpoints::show() {
    FXMainWindow::show();
    myTable->startInput((int)myBreakpoints->size(), 0);
}


void
GUIDialog_Breakpoints::rebuildList() {
    myTable->clearItems();
    sort(myBreakpoints->begin(), myBreakpoints->end());
    // set table attributes
    myTable->setTableSize((FXint)myBreakpoints->size() + 1, 1);
    myTable->setColumnText(0, "Time");
    FXHeader* header = myTable->getColumnHeader();
    header->setHeight(getApp()->getNormalFont()->getFontHeight() + getApp()->getNormalFont()->getFontAscent());
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    // insert into table
    for (int row = 0; row < (int)myBreakpoints->size(); row++) {
        myTable->setItemText(row, 0, time2string((*myBreakpoints)[row]).c_str());
    }
    // insert dummy last field
    myTable->setItemText((int)myBreakpoints->size(), 0, " ");
}


long
GUIDialog_Breakpoints::onCmdLoad(FXObject*, FXSelector, void*) {
    FXFileDialog opendialog(this, "Load Breakpoints");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.txt");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        std::vector<SUMOTime> newBreakpoints = GUISettingsHandler::loadBreakpoints(file);
        FXMutexLock lock(*myBreakpointLock);
        myBreakpoints->assign(newBreakpoints.begin(), newBreakpoints.end());
        rebuildList();
    }
    return 1;
}


long
GUIDialog_Breakpoints::onCmdSave(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(this, "Save Breakpoints", ".txt", GUIIconSubSys::getIcon(ICON_EMPTY), gCurrentFolder);
    if (file == "") {
        return 1;
    }
    std::string content = encode2TXT();
    try {
        OutputDevice& dev = OutputDevice::getDevice(file.text());
        dev << content;
        dev.close();
    } catch (IOError& e) {
        FXMessageBox::error(this, MBOX_OK, "Storing failed!", "%s", e.what());
    }
    return 1;
}


std::string
GUIDialog_Breakpoints::encode2TXT() {
    FXMutexLock lock(*myBreakpointLock);
    std::ostringstream strm;
    std::sort(myBreakpoints->begin(), myBreakpoints->end());
    for (std::vector<SUMOTime>::iterator j = myBreakpoints->begin(); j != myBreakpoints->end(); ++j) {
        strm << time2string(*j) << std::endl;
    }
    return strm.str();
}


long
GUIDialog_Breakpoints::onCmdClear(FXObject*, FXSelector, void*) {
    FXMutexLock lock(*myBreakpointLock);
    myBreakpoints->clear();
    rebuildList();
    return 1;
}



long
GUIDialog_Breakpoints::onCmdClose(FXObject*, FXSelector, void*) {
    close(true);
    return 1;
}


long
GUIDialog_Breakpoints::onCmdEditTable(FXObject*, FXSelector, void* data) {
    FXMutexLock lock(*myBreakpointLock);
    const FXTablePos* const i = (FXTablePos*) data;
    const std::string value = myTable->getItemText(i->row, i->col).text();
    // check whether the inserted value is empty
    const bool empty = value.find_first_not_of(" ") == std::string::npos;
    try {
        if (i->row == (int)myBreakpoints->size()) {
            if (!empty) {
                myBreakpoints->push_back(string2time(value));
            }
        } else {
            if (empty) {
                myBreakpoints->erase(myBreakpoints->begin() + i->row);
            } else {
                (*myBreakpoints)[i->row] = string2time(value);
            }
        }
    } catch (NumberFormatException&) {
        std::string msg = "The value must be a number, is:" + value;
        FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
    }
    rebuildList();
    return 1;
}


/****************************************************************************/

