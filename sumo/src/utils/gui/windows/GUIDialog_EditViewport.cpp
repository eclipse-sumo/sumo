/****************************************************************************/
/// @file    GUIDialog_EditViewport.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @date    Mon, 25.04.2005
/// @version $Id$
///
// A dialog to change the viewport
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

#include <utils/geom/Position.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "GUISUMOAbstractView.h"
#include <utils/gui/div/GUIIOGlobals.h>
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
                     GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, GUIDialog_EditViewport::MID_LOAD,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
        new FXButton(frame0, "\t\tSave viewport to file",
                     GUIIconSubSys::getIcon(ICON_SAVE), this, GUIDialog_EditViewport::MID_SAVE,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
    }
    FXMatrix* m1 = new FXMatrix(f1, 2, MATRIX_BY_COLUMNS);
    {
        new FXLabel(m1, "Zoom:", 0, LAYOUT_CENTER_Y);
        myZoom = new FXRealSpinDial(m1, 16, this, MID_CHANGED, LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myZoom->setRange(0.0001, 100000);
        myZoom->setNumberFormat(4);
    }
    {
        new FXLabel(m1, "X:", 0, LAYOUT_CENTER_Y);
        myXOff = new FXRealSpinDial(m1, 16, this, MID_CHANGED, LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myXOff->setRange(-1000000, 1000000);
        myXOff->setNumberFormat(4);
    }
    {
        new FXLabel(m1, "Y:", 0, LAYOUT_CENTER_Y);
        myYOff = new FXRealSpinDial(m1, 16, this, MID_CHANGED, LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myYOff->setRange(-1000000, 1000000);
        myYOff->setNumberFormat(4);
    }
#ifdef HAVE_OSG
    {
        new FXLabel(m1, "LookAtX:", 0, LAYOUT_CENTER_Y);
        myLookAtX = new FXRealSpinDial(m1, 16, this, MID_CHANGED, LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myLookAtX->setRange(-1000000, 100000);
        myLookAtX->setNumberFormat(4);
    }
    {
        new FXLabel(m1, "LookAtY:", 0, LAYOUT_CENTER_Y);
        myLookAtY = new FXRealSpinDial(m1, 16, this, MID_CHANGED, LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myLookAtY->setRange(-1000000, 1000000);
        myLookAtY->setNumberFormat(4);
    }
    {
        new FXLabel(m1, "LookAtZ:", 0, LAYOUT_CENTER_Y);
        myLookAtZ = new FXRealSpinDial(m1, 16, this, MID_CHANGED, LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myLookAtZ->setRange(-1000000, 1000000);
        myLookAtZ->setNumberFormat(4);
    }
#endif
    // ok/cancel
    new FXHorizontalSeparator(f1, SEPARATOR_GROOVE | LAYOUT_FILL_X);
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
    myParent->setViewport(Position(myXOff->getValue(), myYOff->getValue(), myZoom->getValue()),
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
    myParent->setViewport(myOldLookFrom, myOldLookAt);
    hide();
    return 1;
}


long
GUIDialog_EditViewport::onCmdChanged(FXObject*, FXSelector, void*) {
    myParent->setViewport(Position(myXOff->getValue(), myYOff->getValue(), myZoom->getValue()),
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
        Position lookFrom, lookAt;
        handler.setViewport(lookFrom, lookAt);
        if (lookFrom.z() > 0) {
            setValues(lookFrom, lookAt);
            myParent->setViewport(lookFrom, lookAt);
        }
    }
    return 1;
}


long
GUIDialog_EditViewport::onCmdSave(FXObject*, FXSelector, void* /*data*/) {
    FXFileDialog opendialog(this, "Save Viewport");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute() || !MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    try {
        OutputDevice& dev = OutputDevice::getDevice(opendialog.getFilename().text());
        dev << "<viewsettings>\n";
        dev << "    <viewport zoom=\"" << myZoom->getValue() << "\" x=\"" << myXOff->getValue() << "\" y=\"" << myYOff->getValue();
#ifdef HAVE_OSG
        dev << "\" centerX=\"" << myLookAtX->getValue() << "\" centerY=\"" << myLookAtY->getValue() << "\" centerZ=\"" << myLookAtZ->getValue();
#endif
        dev << "\"/>\n</viewsettings>\n";
        dev.close();
    } catch (IOError& e) {
        FXMessageBox::error(this, MBOX_OK, "Storing failed!", "%s", e.what());
    }
    return 1;
}


void
GUIDialog_EditViewport::setValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff) {
    myZoom->setValue(zoom);
    myXOff->setValue(xoff);
    myYOff->setValue(yoff);
}


void
GUIDialog_EditViewport::setValues(const Position& lookFrom, const Position& lookAt) {
    myZoom->setValue(lookFrom.z());
    myXOff->setValue(lookFrom.x());
    myYOff->setValue(lookFrom.y());
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

