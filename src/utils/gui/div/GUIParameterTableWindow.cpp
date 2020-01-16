/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIParameterTableWindow.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
///
// The window that holds the table of an object's parameter
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <fx.h>
#include "GUIParameterTableWindow.h"
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/common/ToString.h>
#include <utils/common/Parameterised.h>
#include <utils/gui/div/GUIParam_PopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableItem.h>


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIParameterTableWindow) GUIParameterTableWindowMap[] = {
    FXMAPFUNC(SEL_COMMAND,          MID_SIMSTEP,    GUIParameterTableWindow::onSimStep),
    FXMAPFUNC(SEL_SELECTED,         MID_TABLE,      GUIParameterTableWindow::onTableSelected),
    FXMAPFUNC(SEL_DESELECTED,       MID_TABLE,      GUIParameterTableWindow::onTableDeselected),
    FXMAPFUNC(SEL_RIGHTBUTTONPRESS, MID_TABLE,      GUIParameterTableWindow::onRightButtonPress),
};

FXIMPLEMENT(GUIParameterTableWindow, FXMainWindow, GUIParameterTableWindowMap, ARRAYNUMBER(GUIParameterTableWindowMap))


// ===========================================================================
// static value definitions
// ===========================================================================
FXMutex GUIParameterTableWindow::myGlobalContainerLock;
std::vector<GUIParameterTableWindow*> GUIParameterTableWindow::myContainer;

// ===========================================================================
// method definitions
// ===========================================================================
GUIParameterTableWindow::GUIParameterTableWindow(GUIMainWindow& app, GUIGlObject& o) :
    FXMainWindow(app.getApp(), (o.getFullName() + " Parameter").c_str(), nullptr, nullptr, DECOR_ALL, 20, 20, 200, 500),
    myObject(&o),
    myApplication(&app),
    myCurrentPos(0) 
{
    myTable = new FXTable(this, this, MID_TABLE, TABLE_COL_SIZABLE | TABLE_ROW_SIZABLE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    myTable->setTableSize(1, 3);
    myTable->setVisibleColumns(3);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->setColumnText(0, "Name");
    myTable->setColumnText(1, "Value");
    myTable->setColumnText(2, "Dynamic");
    myTable->getRowHeader()->setWidth(0);
    FXHeader* header = myTable->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 240);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(1, 120);
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, 60);
    setIcon(GUIIconSubSys::getIcon(ICON_APP_TABLE));
    myLock.lock();
    myObject->addParameterTable(this);
    myLock.unlock();
    FXMutexLock locker(myGlobalContainerLock);
    myContainer.push_back(this);
    // Table cannot be editable
    myTable->setEditable(FALSE);
}

GUIParameterTableWindow::~GUIParameterTableWindow() {
    myApplication->removeChild(this);
    myLock.lock();
    for (std::vector<GUIParameterTableItemInterface*>::iterator i = myItems.begin(); i != myItems.end(); ++i) {
        delete (*i);
    }
    if (myObject != nullptr) {
        myObject->removeParameterTable(this);
    }
    myLock.unlock();
    FXMutexLock locker(myGlobalContainerLock);
    std::vector<GUIParameterTableWindow*>::iterator i = std::find(myContainer.begin(), myContainer.end(), this);
    if (i != myContainer.end()) {
        myContainer.erase(i);
    }
}


void
GUIParameterTableWindow::removeObject(GUIGlObject* /*i*/) {
    FXMutexLock locker(myLock);
    myObject = nullptr;
}


long
GUIParameterTableWindow::onSimStep(FXObject*, FXSelector, void*) {
    // table values are updated in GUINet::guiSimulationStep()
    updateTable();
    update();
    return 1;
}


long
GUIParameterTableWindow::onTableSelected(FXObject*, FXSelector, void*) {
    return 1;
}


long
GUIParameterTableWindow::onTableDeselected(FXObject*, FXSelector, void*) {
    return 1;
}


long
GUIParameterTableWindow::onRightButtonPress(FXObject* sender, FXSelector sel, void* eventData) {
    // check which value entry was pressed
    myTable->onLeftBtnPress(sender, sel, eventData);
    int row = myTable->getCurrentRow();
    if (row == -1 || row >= (int)(myItems.size())) {
        return 1;
    }
    GUIParameterTableItemInterface* i = myItems[row];
    if (!i->dynamic()) {
        return 1;
    }
    if (myObject == nullptr) {
        return 1;
    }

    ValueSource<double>* doubleSource = i->getdoubleSourceCopy();
    if (doubleSource != nullptr) {
        GUIParam_PopupMenuInterface* p = new GUIParam_PopupMenuInterface(*myApplication, *this, *myObject, i->getName(), doubleSource);
        new FXMenuCommand(p, "Open in new Tracker", nullptr, p, MID_OPENTRACKER);
        // set geometry
        p->setX(static_cast<FXEvent*>(eventData)->root_x);
        p->setY(static_cast<FXEvent*>(eventData)->root_y);
        p->create();
        // show
        p->show();
    }
    return 1;
}


void
GUIParameterTableWindow::mkItem(const char* name, bool dynamic, std::string value) {
    myTable->insertRows(myItems.size() + 1);
    GUIParameterTableItemInterface* i = new GUIParameterTableItem<std::string>(myTable, myCurrentPos++, name, dynamic, value);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::mkItem(const char* name, bool dynamic, double value) {
    myTable->insertRows(myItems.size() + 1);
    GUIParameterTableItemInterface* i = new GUIParameterTableItem<double>(myTable, myCurrentPos++, name, dynamic, value);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::mkItem(const char* name, bool dynamic, unsigned value) {
    myTable->insertRows(myItems.size() + 1);
    GUIParameterTableItemInterface* i = new GUIParameterTableItem<unsigned>(myTable, myCurrentPos++, name, dynamic, value);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::mkItem(const char* name, bool dynamic, int value) {
    myTable->insertRows(myItems.size() + 1);
    GUIParameterTableItemInterface* i = new GUIParameterTableItem<int>(myTable, myCurrentPos++, name, dynamic, value);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::mkItem(const char* name, bool dynamic, long long int value) {
    myTable->insertRows(myItems.size() + 1);
    GUIParameterTableItemInterface* i = new GUIParameterTableItem<long long int>(myTable, myCurrentPos++, name, dynamic, value);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::updateTable() {
    FXMutexLock locker(myLock);
    if (myObject == nullptr) {
        return;
    }
    for (std::vector<GUIParameterTableItemInterface*>::iterator i = myItems.begin(); i != myItems.end(); i++) {
        (*i)->update();
    }
}


void
GUIParameterTableWindow::closeBuilding(const Parameterised* p) {
    // add generic paramters if available
    if (p == nullptr) {
        p = dynamic_cast<const Parameterised*>(myObject);
    }
    if (p != nullptr) {
        const std::map<std::string, std::string>& map = p->getParametersMap();
        for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); ++it) {
            mkItem(("param:" + it->first).c_str(), false, it->second);
        }
    }
    const int rows = myItems.size() + 1;
    setHeight(rows * 20 + 40);
    myTable->fitColumnsToContents(1);
    setWidth(myTable->getContentWidth() + 40);
    myTable->setVisibleRows(rows);
    myApplication->addChild(this);
    create();
    show();
}


int
GUIParameterTableWindow::numParams(const GUIGlObject* obj) {
    const Parameterised* p = dynamic_cast<const Parameterised*>(obj);
    return p != nullptr ? (int)p->getParametersMap().size() : 0;
}


/****************************************************************************/

