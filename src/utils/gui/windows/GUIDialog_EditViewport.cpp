/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2005-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/MsgHandler.h>
#include <utils/geom/Position.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/options/OptionsCont.h>

#include "GUISUMOAbstractView.h"
#include "GUIDialog_EditViewport.h"


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
GUIDialog_EditViewport::GUIDialog_EditViewport(GUISUMOAbstractView* parent, const char* name, int x, int y) :
    FXDialogBox(parent, name, GUIDesignDialogBox, x, y, 0, 0, 0, 0, 0, 0),
    myParent(parent) {
    // create contents frame
    FXVerticalFrame* contentsFrame = new FXVerticalFrame(this, GUIDesignContentsFrame);
    // create frame for file icons
    FXHorizontalFrame* frameFiles = new FXHorizontalFrame(contentsFrame, GUIDesignHorizontalFrameIcons);
    new FXButton(frameFiles, "Load\t\tLoad viewport from file",
                 GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, GUIDialog_EditViewport::MID_LOAD, GUIDesignButtonToolbarWithText);
    new FXButton(frameFiles, "Save\t\tSave viewport to file",
                 GUIIconSubSys::getIcon(ICON_SAVE), this, GUIDialog_EditViewport::MID_SAVE, GUIDesignButtonToolbarWithText);
    // create zoom elements
    FXHorizontalFrame* zoomFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(zoomFrame, "Zoom:", 0, GUIDesignLabelLeftThick);
    myZoom = new FXRealSpinner(zoomFrame, 16, this, MID_CHANGED, GUIDesignSpinDial);
    myZoom->setRange(0.0001, 100000);
    //myZoom->setNumberFormat(4);
    // create X elements
    FXHorizontalFrame* XFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(XFrame, "X:", 0, GUIDesignLabelLeftThick);
    myXOff = new FXRealSpinner(XFrame, 16, this, MID_CHANGED, GUIDesignSpinDial | SPIN_NOMIN | SPIN_NOMAX);
    // create Y elements
    FXHorizontalFrame* YFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(YFrame, "Y:", 0, GUIDesignLabelLeftThick);
    myYOff = new FXRealSpinner(YFrame, 16, this, MID_CHANGED, GUIDesignSpinDial | SPIN_NOMIN | SPIN_NOMAX);
    // create Z elements
    FXHorizontalFrame* ZFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(ZFrame, "Z:", 0, GUIDesignLabelLeftThick);
    myZOff = new FXRealSpinner(ZFrame, 16, this, MID_CHANGED, GUIDesignSpinDial | SPIN_NOMIN | SPIN_NOMAX);
#ifdef HAVE_OSG
    FXHorizontalFrame* lookAtXFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(lookAtXFrame, "LookAtX:", 0, GUIDesignLabelLeftThick);
    myLookAtX = new FXRealSpinner(lookAtXFrame, 16, this, MID_CHANGED, GUIDesignSpinDial | SPIN_NOMIN | SPIN_NOMAX);
    FXHorizontalFrame* lookAtYFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(lookAtYFrame, "LookAtY:", 0, GUIDesignLabelLeftThick);
    myLookAtY = new FXRealSpinner(lookAtYFrame, 16, this, MID_CHANGED, GUIDesignSpinDial | SPIN_NOMIN | SPIN_NOMAX);
    FXHorizontalFrame* lookAtZFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(lookAtZFrame, "LookAtZ:", 0, GUIDesignLabelLeftThick);
    myLookAtZ = new FXRealSpinner(lookAtZFrame, 16, this, MID_CHANGED, GUIDesignSpinDial | SPIN_NOMIN | SPIN_NOMAX);
    // update width of dialog for the new elements
    setHeight(257);
#endif
    // create buttons ok/cancel
    new FXHorizontalSeparator(contentsFrame, GUIDesignHorizontalSeparator);
    FXHorizontalFrame* frameButtons = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXHorizontalFrame(frameButtons, GUIDesignAuxiliarHorizontalFrame);
    buttonOk = new FXButton(frameButtons, "&OK\t\taccept", GUIIconSubSys::getIcon(ICON_ACCEPT), this, GUIDialog_EditViewport::MID_OK, GUIDesignButtonOK);
    new FXButton(frameButtons, "&Cancel\t\tclose", GUIIconSubSys::getIcon(ICON_CANCEL), this, GUIDialog_EditViewport::MID_CANCEL, GUIDesignButtonCancel);
    new FXHorizontalFrame(frameButtons, GUIDesignAuxiliarHorizontalFrame);
    // set dialog icon
    setIcon(GUIIconSubSys::getIcon(ICON_EDITVIEWPORT));
}


GUIDialog_EditViewport::~GUIDialog_EditViewport() {}


void
GUIDialog_EditViewport::show() {
    // If testing mode is enabled, we need to place focus in the Z dial
    if (OptionsCont::getOptions().getBool("gui-testing")) {
        myZOff->setFocus();
    } else {
        buttonOk->setFocus();
    }
    FXDialogBox::show();
}


long
GUIDialog_EditViewport::onCmdOk(FXObject*, FXSelector, void*) {
    myParent->setViewportFromTo(Position(myXOff->getValue(), myYOff->getValue(), myZOff->getValue()),
#ifdef HAVE_OSG
                                Position(myLookAtX->getValue(), myLookAtY->getValue(), myLookAtZ->getValue())
#else
                                Position::INVALID
#endif
                               );
    // write information of current zoom status
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Current Viewport values: " + toString(myXOff->getValue()) + ", " + toString(myYOff->getValue()) + ", " + toString(myZOff->getValue()) +
                      ". Zoom = '" + toString(myZoom->getValue()) + "'");
    }
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
GUIDialog_EditViewport::onCmdLoad(FXObject*, FXSelector, void*) {
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
GUIDialog_EditViewport::onCmdSave(FXObject*, FXSelector, void*) {
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
GUIDialog_EditViewport::setValues(double zoom, double xoff, double yoff) {
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
    return false;
    //return myZoom->getDial().grabbed() || myXOff->getDial().grabbed() || myYOff->getDial().grabbed();
}


/****************************************************************************/

