/****************************************************************************/
/// @file    GUIDialog_EditViewport.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Mon, 25.04.2005
/// @version $Id$
///
// A dialog to change the viewport
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/geom/Position.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "GUISUMOAbstractView.h"
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include "GUIDialog_EditViewport.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_EditViewport) GUIDialog_EditViewportMap[] = {
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_CHANGED, GUIDialog_EditViewport::onCmdChanged),
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_OK,      GUIDialog_EditViewport::onCmdOk),
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_CANCEL,  GUIDialog_EditViewport::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_LOAD,    GUIDialog_EditViewport::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_SAVE,    GUIDialog_EditViewport::onCmdSave),
};


// Object implementation
FXIMPLEMENT(GUIDialog_EditViewport, FXDialogBox, GUIDialog_EditViewportMap, ARRAYNUMBER(GUIDialog_EditViewportMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_EditViewport::GUIDialog_EditViewport(GUISUMOAbstractView* parent,
        const char* name, int x, int y)
    : FXDialogBox(parent, name, DECOR_TITLE | DECOR_BORDER, x, y, 0, 0),
      myParent(parent) {
    FXVerticalFrame* f1 = new FXVerticalFrame(this, LAYOUT_TOP | FRAME_NONE | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 1, 1);
    {
        FXHorizontalFrame* frame0 =
            new FXHorizontalFrame(f1, FRAME_THICK, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);
        new FXButton(frame0, "\t\tLoad viewport from file",
                     GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, GUIDialog_EditViewport::MID_LOAD, GUIDesignButtonToolbar);
        new FXButton(frame0, "\t\tSave viewport to file",
                     GUIIconSubSys::getIcon(ICON_SAVE), this, GUIDialog_EditViewport::MID_SAVE, GUIDesignButtonToolbar);
    }
    FXMatrix* m1 = new FXMatrix(f1, 2, MATRIX_BY_COLUMNS);
    new FXLabel(m1, "Zoom:", 0, LAYOUT_CENTER_Y);
    myZoom = new FXRealSpinDial(m1, 16, this, MID_CHANGED, LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
    myZoom->setRange(0.0001, 100000);
    myZoom->setNumberFormat(4);
    new FXLabel(m1, "X:", 0, LAYOUT_CENTER_Y);
    myXOff = new FXRealSpinDial(m1, 16, this, MID_CHANGED, GUIDesignSpinDial | SPINDIAL_NOMIN | SPINDIAL_NOMAX);
    new FXLabel(m1, "Y:", 0, LAYOUT_CENTER_Y);
    myYOff = new FXRealSpinDial(m1, 16, this, MID_CHANGED, GUIDesignSpinDial | SPINDIAL_NOMIN | SPINDIAL_NOMAX);
    new FXLabel(m1, "Z:", 0, LAYOUT_CENTER_Y);
    myZOff = new FXRealSpinDial(m1, 16, this, MID_CHANGED, GUIDesignSpinDial | SPINDIAL_NOMIN | SPINDIAL_NOMAX);
#ifdef HAVE_OSG
    new FXLabel(m1, "LookAtX:", 0, LAYOUT_CENTER_Y);
    myLookAtX = new FXRealSpinDial(m1, 16, this, MID_CHANGED, GUIDesignSpinDial | SPINDIAL_NOMIN | SPINDIAL_NOMAX);
    new FXLabel(m1, "LookAtY:", 0, LAYOUT_CENTER_Y);
    myLookAtY = new FXRealSpinDial(m1, 16, this, MID_CHANGED, GUIDesignSpinDial | SPINDIAL_NOMIN | SPINDIAL_NOMAX);
    new FXLabel(m1, "LookAtZ:", 0, LAYOUT_CENTER_Y);
    myLookAtZ = new FXRealSpinDial(m1, 16, this, MID_CHANGED, GUIDesignSpinDial | SPINDIAL_NOMIN | SPINDIAL_NOMAX);
#endif
    // ok/cancel
    new FXHorizontalSeparator(f1, GUIDesignHorizontalSeparator);
    FXHorizontalFrame* f6 = new FXHorizontalFrame(f1, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH, 0, 0, 0, 0, 10, 10, 5, 0);
    FXButton* initial =
        new FXButton(f6, "&OK", NULL, this, GUIDialog_EditViewport::MID_OK,
                     BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X,
                     0, 0, 0, 0,  4, 4, 3, 3);
    new FXButton(f6, "&Cancel", NULL, this, GUIDialog_EditViewport::MID_CANCEL,
                 FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X,
                 0, 0, 0, 0,  4, 4, 3, 3);
    initial->setFocus();
    setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
}


GUIDialog_EditViewport::~GUIDialog_EditViewport() {}


long
GUIDialog_EditViewport::onCmdOk(FXObject*, FXSelector, void*) {
    myParent->setViewportFromTo(Position(myXOff->getValue(), myYOff->getValue(), myZOff->getValue()),
#ifdef HAVE_OSG
                                Position(myLookAtX->getValue(), myLookAtY->getValue(), myLookAtZ->getValue())
#else
                                Position::INVALID
#endif
                               );
    hide();
    return 1;
}


long
GUIDialog_EditViewport::onCmdCancel(FXObject*, FXSelector, void*) {
    myParent->setViewportFromTo(myOldLookFrom, myOldLookAt);
    hide();
    return 1;
}


long
GUIDialog_EditViewport::onCmdChanged(FXObject* o, FXSelector, void*) {
    if (o == myZOff) {
        myZoom->setValue(myParent->getChanger().zPos2Zoom(myZOff->getValue()));
    } else if (o == myZoom) {
        myZOff->setValue(myParent->getChanger().zoom2ZPos(myZoom->getValue()));
    }
    myParent->setViewportFromTo(Position(myXOff->getValue(), myYOff->getValue(), myZOff->getValue()),
#ifdef HAVE_OSG
                                Position(myLookAtX->getValue(), myLookAtY->getValue(), myLookAtZ->getValue())
#else
                                Position::INVALID
#endif
                               );
    return 1;
}


long
GUIDialog_EditViewport::onCmdLoad(FXObject*, FXSelector, void* /*data*/) {
    FXFileDialog opendialog(this, "Load Viewport");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        GUISettingsHandler handler(opendialog.getFilename().text());
        handler.applyViewport(myParent);
        setValues(myParent->getChanger().getZoom(), myParent->getChanger().getXPos(), myParent->getChanger().getYPos());
    }
    return 1;
}


long
GUIDialog_EditViewport::onCmdSave(FXObject*, FXSelector, void* /*data*/) {
    FXString file = MFXUtils::getFilename2Write(this, "Save Viewport", ".xml", GUIIconSubSys::getIcon(ICON_EMPTY), gCurrentFolder);
    if (file == "") {
        return 1;
    }
    try {
        OutputDevice& dev = OutputDevice::getDevice(file.text());
        dev.openTag(SUMO_TAG_VIEWSETTINGS);
        writeXML(dev);
        dev.closeTag();
        dev.close();
    } catch (IOError& e) {
        FXMessageBox::error(this, MBOX_OK, "Storing failed!", "%s", e.what());
    }
    return 1;
}


void
GUIDialog_EditViewport::writeXML(OutputDevice& dev) {
    dev.openTag(SUMO_TAG_VIEWPORT);
    dev.writeAttr(SUMO_ATTR_ZOOM, myZoom->getValue());
    dev.writeAttr(SUMO_ATTR_X, myXOff->getValue());
    dev.writeAttr(SUMO_ATTR_Y, myYOff->getValue());
#ifdef HAVE_OSG
    dev.writeAttr(SUMO_ATTR_CENTER_X, myLookAtX->getValue());
    dev.writeAttr(SUMO_ATTR_CENTER_Y, myLookAtY->getValue());
    dev.writeAttr(SUMO_ATTR_CENTER_Z, myLookAtZ->getValue());
#endif
    dev.closeTag();
}

void
GUIDialog_EditViewport::setValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff) {
    myZoom->setValue(zoom);
    myXOff->setValue(xoff);
    myYOff->setValue(yoff);
    myZOff->setValue(myParent->getChanger().zoom2ZPos(zoom));
}


void
GUIDialog_EditViewport::setValues(const Position& lookFrom, const Position& lookAt) {
    myXOff->setValue(lookFrom.x());
    myYOff->setValue(lookFrom.y());
    myZOff->setValue(lookFrom.z());
    myZoom->setValue(myParent->getChanger().zPos2Zoom(lookFrom.z()));
#ifdef HAVE_OSG
    myLookAtX->setValue(lookAt.x());
    myLookAtY->setValue(lookAt.y());
    myLookAtZ->setValue(lookAt.z());
#else
    UNUSED_PARAMETER(lookAt);
#endif
}


void
GUIDialog_EditViewport::setOldValues(const Position& lookFrom, const Position& lookAt) {
    setValues(lookFrom, lookAt);
    myOldLookFrom = lookFrom;
    myOldLookAt = lookAt;
}


bool
GUIDialog_EditViewport::haveGrabbed() const {
    return myZoom->getDial().grabbed() || myXOff->getDial().grabbed() || myYOff->getDial().grabbed();
}


/****************************************************************************/

