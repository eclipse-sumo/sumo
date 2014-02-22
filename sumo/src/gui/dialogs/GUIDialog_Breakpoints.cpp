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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utils/foxtools/MFXAddEditTypedTable.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/iodevices/OutputDevice.h>
#include "GUIDialog_Breakpoints.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// definitions
// ===========================================================================
#define INVALID_VALUE -1
#define INVALID_VALUE_STR "-1"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_Breakpoints) GUIDialog_BreakpointsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,                   GUIDialog_Breakpoints::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,                   GUIDialog_Breakpoints::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,                  GUIDialog_Breakpoints::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,                         GUIDialog_Breakpoints::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MFXAddEditTypedTable::ID_TEXT_CHANGED,  GUIDialog_Breakpoints::onCmdEditTable),
};


FXIMPLEMENT(GUIDialog_Breakpoints, FXMainWindow, GUIDialog_BreakpointsMap, ARRAYNUMBER(GUIDialog_BreakpointsMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_Breakpoints::GUIDialog_Breakpoints(GUIMainWindow* parent)
    : FXMainWindow(parent->getApp(), "Breakpoints Editor", NULL, NULL, DECOR_ALL, 20, 20, 300, 300),
      myParent(parent) {
    FXHorizontalFrame* hbox = new FXHorizontalFrame(this, LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);

    // build the table
    myTable = new MFXAddEditTypedTable(hbox, this, MID_TABLE, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    myTable->setVisibleRows(20);
    myTable->setVisibleColumns(1);
    myTable->setTableSize(20, 1);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->setCellType(0, CT_REAL);
    SUMOTime begin = string2time(OptionsCont::getOptions().getString("begin"));
    SUMOTime end = string2time(OptionsCont::getOptions().getString("end"));
    if (end < 0) {
        end = SUMOTime_MAX;
    }
    myTable->setNumberCellParams(0, begin / 1000, end / 1000, 1, 10, 100, "%.2f");
    myTable->getRowHeader()->setWidth(0);
    rebuildList();
    // build the layout
    FXVerticalFrame* layout = new FXVerticalFrame(hbox, LAYOUT_TOP, 0, 0, 0, 0, 4, 4, 4, 4);
    // "Load"
    new FXButton(layout, "Load\t\t", 0, this, MID_CHOOSEN_LOAD, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 4, 4, 3, 3);
    // "Save"
    new FXButton(layout, "Save\t\t", 0, this, MID_CHOOSEN_SAVE, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 4, 4, 3, 3);
    new FXHorizontalSeparator(layout, SEPARATOR_GROOVE | LAYOUT_FILL_X);
    // "Clear List"
    new FXButton(layout, "Clear\t\t", 0, this, MID_CHOOSEN_CLEAR, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 4, 4, 3, 3);
    new FXHorizontalSeparator(layout, SEPARATOR_GROOVE | LAYOUT_FILL_X);
    // "Close"
    new FXButton(layout, "Close\t\t", 0, this, MID_CANCEL, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 4, 4, 3, 3);
    //
    setIcon(GUIIconSubSys::getIcon(ICON_APP_BREAKPOINTS));
    myParent->addChild(this);
}


GUIDialog_Breakpoints::~GUIDialog_Breakpoints() {
    myParent->removeChild(this);
}


void
GUIDialog_Breakpoints::rebuildList() {
    myTable->clearItems();
    sort(GUIGlobals::gBreakpoints.begin(), GUIGlobals::gBreakpoints.end());
    // set table attributes
    myTable->setTableSize((FXint) GUIGlobals::gBreakpoints.size() + 1, 1);
    myTable->setColumnText(0, "Time");
    FXHeader* header = myTable->getColumnHeader();
    header->setHeight(getApp()->getNormalFont()->getFontHeight() + getApp()->getNormalFont()->getFontAscent());
    int k;
    for (k = 0; k < 1; k++) {
        header->setItemJustify(k, JUSTIFY_CENTER_X);
    }
    // insert into table
    FXint row = 0;
    std::vector<int>::iterator j;
    for (j = GUIGlobals::gBreakpoints.begin(); j != GUIGlobals::gBreakpoints.end(); ++j) {
        myTable->setItemText(row, 0, time2string(*j).c_str());
        row++;
    }
    // insert dummy last field
    for (k = 0; k < 1; k++) {
        myTable->setItemText(row, k, " ");
    }
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
        GUIGlobals::gBreakpoints = GUISettingsHandler::loadBreakpoints(file);
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
    std::ostringstream strm;
    std::sort(GUIGlobals::gBreakpoints.begin(), GUIGlobals::gBreakpoints.end());
    for (std::vector<int>::iterator j = GUIGlobals::gBreakpoints.begin(); j != GUIGlobals::gBreakpoints.end(); ++j) {
        if ((*j) != INVALID_VALUE) {
            strm << time2string(*j) << std::endl;
        }
    }
    return strm.str();
}


long
GUIDialog_Breakpoints::onCmdClear(FXObject*, FXSelector, void*) {
    GUIGlobals::gBreakpoints.clear();
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
    MFXEditedTableItem* i = (MFXEditedTableItem*) data;
    std::string value = i->item->getText().text();
    // check whether the inserted value is empty
    if (value.find_first_not_of(" ") == std::string::npos) {
        // replace by invalid if so
        value = INVALID_VALUE_STR;
    }
    int row = i->row;
    if (row == (int) GUIGlobals::gBreakpoints.size()) {
        GUIGlobals::gBreakpoints.push_back(INVALID_VALUE);
    }

    switch (i->col) {
        case 0:
            try {
                GUIGlobals::gBreakpoints[row] = string2time(value);
            } catch (NumberFormatException&) {
                std::string msg = "The value must be an int, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        default:
            break;
    }
    if (!i->updateOnly) {
        rebuildList();
    }
    return 1;
}


/****************************************************************************/

