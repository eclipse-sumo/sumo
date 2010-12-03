/****************************************************************************/
/// @file    GUIDialog_EditViewport.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 25.04.2005
/// @version $Id$
///
// A dialog to change the viewport
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "GUIDialog_EditViewport.h"
#include <utils/gui/windows/GUIAppGlobals.h>
#include "GUISUMOAbstractView.h"
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/foxtools/MFXUtils.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_EditViewport) GUIDialog_EditViewportMap[]={
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_CHANGED, GUIDialog_EditViewport::onCmdChanged),
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_OK,      GUIDialog_EditViewport::onCmdOk),
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_CANCEL,  GUIDialog_EditViewport::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_LOAD,    GUIDialog_EditViewport::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_SAVE,    GUIDialog_EditViewport::onCmdSave),
};


// Object implementation
FXIMPLEMENT(GUIDialog_EditViewport,FXDialogBox,GUIDialog_EditViewportMap, ARRAYNUMBER(GUIDialog_EditViewportMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_EditViewport::GUIDialog_EditViewport(GUISUMOAbstractView* parent,
        const char* name, SUMOReal zoom, SUMOReal xoff, SUMOReal yoff, int x, int y) throw()
        : FXDialogBox(parent, name, DECOR_TITLE|DECOR_BORDER, x, y, 0, 0),
        myParent(parent), myOldZoom(zoom), myOldXOff(xoff), myOldYOff(yoff) {
    FXVerticalFrame *f1 = new FXVerticalFrame(this, LAYOUT_TOP|FRAME_NONE|LAYOUT_FILL_X, 0,0,0,0, 0,0,1,1);
    {
        FXHorizontalFrame *frame0 =
            new FXHorizontalFrame(f1,FRAME_THICK, 0,0,0,0, 0,0,0,0, 2,2);
        new FXButton(frame0,"\t\tLoad viewport from file",
                     GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, GUIDialog_EditViewport::MID_LOAD,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(frame0,"\t\tSave viewport to file",
                     GUIIconSubSys::getIcon(ICON_SAVE), this, GUIDialog_EditViewport::MID_SAVE,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    }
    FXMatrix *m1 = new FXMatrix(f1, 2, MATRIX_BY_COLUMNS);
    {
        new FXLabel(m1, "Zoom:", 0, LAYOUT_CENTER_Y);
        myZoom = new FXRealSpinDial(m1, 16, this, MID_CHANGED, LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myZoom->setRange(0.0001, 100000);
        myZoom->setNumberFormat(4);
        myZoom->setValue(zoom);
    }
    {
        new FXLabel(m1, "X Offset:", 0, LAYOUT_CENTER_Y);
        myXOff = new FXRealSpinDial(m1, 16, this, MID_CHANGED, LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myXOff->setRange(-1000000, 1000000);
        myXOff->setNumberFormat(4);
        myXOff->setValue(xoff);
    }
    {
        new FXLabel(m1, "Y Offset:", 0, LAYOUT_CENTER_Y);
        myYOff = new FXRealSpinDial(m1, 16, this, MID_CHANGED, LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myYOff->setRange(-1000000, 1000000);
        myYOff->setNumberFormat(4);
        myYOff->setValue(yoff);
    }
    // ok/cancel
    new FXHorizontalSeparator(f1,SEPARATOR_GROOVE|LAYOUT_FILL_X);
    FXHorizontalFrame *f6 = new FXHorizontalFrame(f1, LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH, 0,0,0,0, 10,10,5,0);
    FXButton *initial =
        new FXButton(f6, "&OK", NULL, this, GUIDialog_EditViewport::MID_OK,
                     BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,
                     0,0,0,0,  4,4,3,3);
    new FXButton(f6,"&Cancel", NULL, this, GUIDialog_EditViewport::MID_CANCEL,
                 FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,
                 0,0,0,0,  4,4,3,3);
    initial->setFocus();
    setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
}


GUIDialog_EditViewport::~GUIDialog_EditViewport() throw() {}


long
GUIDialog_EditViewport::onCmdOk(FXObject*,FXSelector,void*) {
    myParent->setViewport((SUMOReal) myZoom->getValue(), (SUMOReal) myXOff->getValue(), (SUMOReal) myYOff->getValue());
    hide();
    return 1;
}


long
GUIDialog_EditViewport::onCmdCancel(FXObject*,FXSelector,void*) {
    myParent->setViewport(myOldZoom, myOldXOff, myOldYOff);
    hide();
    return 1;
}


long
GUIDialog_EditViewport::onCmdChanged(FXObject*,FXSelector,void*) {
    myParent->setViewport((SUMOReal) myZoom->getValue(), (SUMOReal) myXOff->getValue(), (SUMOReal) myYOff->getValue());
    return 1;
}


long
GUIDialog_EditViewport::onCmdLoad(FXObject*,FXSelector,void*/*data*/) {
    FXFileDialog opendialog(this, "Load Viewport");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        GUISettingsHandler handler(opendialog.getFilename().text());
        SUMOReal zoom, xoff, yoff;
        handler.setViewport(zoom, xoff, yoff);
        if (zoom>0) {
            setValues(zoom, xoff, yoff);
            myParent->setViewport(zoom, xoff, yoff);
        }
    }
    return 1;
}


long
GUIDialog_EditViewport::onCmdSave(FXObject*,FXSelector,void*/*data*/) {
    FXFileDialog opendialog(this, "Save Viewport");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute()||!MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    try {
        OutputDevice &dev = OutputDevice::getDevice(opendialog.getFilename().text());
        dev << "<viewsettings>\n";
        dev << "    <viewport zoom=\"" << myZoom->getValue() << "\" x=\"" << myXOff->getValue() << "\" y=\"" << myYOff->getValue() << "\"/>\n";
        dev << "</viewsettings>\n";
        dev.close();
    } catch (IOError &e) {
        FXMessageBox::error(this, MBOX_OK, "Storing failed!", e.what());
    }
    return 1;
}


void
GUIDialog_EditViewport::setValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff) throw() {
    myZoom->setValue(zoom);
    myXOff->setValue(xoff);
    myYOff->setValue(yoff);
}


void
GUIDialog_EditViewport::setOldValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff) throw() {
    myZoom->setValue(zoom);
    myXOff->setValue(xoff);
    myYOff->setValue(yoff);
    myOldZoom = zoom;
    myOldXOff = xoff;
    myOldYOff = yoff;
}


bool
GUIDialog_EditViewport::haveGrabbed() const throw() {
    return myZoom->getDial().grabbed()||myXOff->getDial().grabbed()||myYOff->getDial().grabbed();
}


/****************************************************************************/

