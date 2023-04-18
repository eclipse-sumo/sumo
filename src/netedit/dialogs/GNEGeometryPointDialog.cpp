/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEGeometryPointDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2021
///
// A dialog for set Geometry Points
/****************************************************************************/

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>

#include "GNEGeometryPointDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEGeometryPointDialog) GNEGeometryPointDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_SET_ATTRIBUTE,  GNEGeometryPointDialog::onCmdChangeGeometryPoint),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_ACCEPT,  GNEGeometryPointDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_CANCEL,  GNEGeometryPointDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_RESET,   GNEGeometryPointDialog::onCmdReset),
    FXMAPFUNC(SEL_CLOSE,        0,                      GNEGeometryPointDialog::onCmdCancel),
};

// Object abstract implementation
FXIMPLEMENT_ABSTRACT(GNEGeometryPointDialog, FXTopWindow, GNEGeometryPointDialogMap, ARRAYNUMBER(GNEGeometryPointDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEGeometryPointDialog::GNEGeometryPointDialog(GNEViewNet* viewNet, Position* pos) :
    FXTopWindow(viewNet, "Custom Geometry Point", GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), GUIDesignDialogBoxExplicit(320, 80)),
    myViewNet(viewNet),
    myPos(pos),
    myOriginalPos(*pos),
    myGeo(GeoConvHelper::getFinal().getProjString() != "!") {
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create frame for X,Y
    FXHorizontalFrame* XYFrame = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(XYFrame, "X,Y,[Z]", nullptr, GUIDesignLabelThickedFixed(75));
    myTextFieldXY = new FXTextField(XYFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextFieldXY->setText(toString(*pos).c_str());
    // create frame for lon,lat
    FXHorizontalFrame* lonLatFrame = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(lonLatFrame, "lon,lat,[Z]", nullptr, GUIDesignLabelThickedFixed(75));
    myTextFieldLonLat = new FXTextField(lonLatFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // check if enable geo coordinates
    if (myGeo) {
        Position geoPos = *pos;
        GeoConvHelper::getFinal().cartesian2geo(geoPos);
        myTextFieldLonLat->setText(toString(geoPos, gPrecisionGeo).c_str());
    } else {
        myTextFieldLonLat->disable();
    }
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, (std::string("\t\t") + TL("close accepting changes")).c_str(),  GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonCustomWidth(43));
    myCancelButton = new FXButton(buttonsFrame, (std::string("\t\t") + TL("close discarding changes")).c_str(), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCustomWidth(43));
    myResetButton = new FXButton(buttonsFrame, (std::string("\t\t") + TL("reset to previous values")).c_str(),  GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_BUTTON_RESET, GUIDesignButtonCustomWidth(43));
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // create
    create();
    // show in the given position
    show(PLACEMENT_SCREEN);
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(this);
}


GNEGeometryPointDialog::~GNEGeometryPointDialog() {
    // return focus to GNEViewNet to avoid minimization
    getParent()->setFocus();
}


long
GNEGeometryPointDialog::onCmdChangeGeometryPoint(FXObject* sender, FXSelector, void*) {
    // check text field
    if (sender == myTextFieldXY) {
        // check if position can be parsed
        if (GNEAttributeCarrier::canParse<Position>(myTextFieldXY->getText().text())) {
            // set valid color and kill focus
            myTextFieldXY->setTextColor(FXRGB(0, 0, 0));
            myTextFieldXY->killFocus();
            // obtain position
            *myPos = GNEAttributeCarrier::parse<Position>(myTextFieldXY->getText().text());
            // check if there is geo coordinates
            if (myGeo) {
                // calculate geo position
                Position geoPos = *myPos;
                GeoConvHelper::getFinal().cartesian2geo(geoPos);
                // set geo position in myTextFieldLonLat
                myTextFieldLonLat->setText(toString(geoPos).c_str(), FALSE);
                myTextFieldLonLat->setTextColor(FXRGB(0, 0, 0));
            }
        } else {
            // set invalid color
            myTextFieldXY->setTextColor(FXRGB(255, 0, 0));
        }
    } else {
        // check if position can be parsed
        if (GNEAttributeCarrier::canParse<Position>(myTextFieldLonLat->getText().text())) {
            // set valid color and kill focus
            myTextFieldLonLat->setTextColor(FXRGB(0, 0, 0));
            myTextFieldLonLat->killFocus();
            // obtain geo position
            Position geoPo = GNEAttributeCarrier::parse<Position>(myTextFieldLonLat->getText().text());
            // calculate cartesian position
            *myPos = geoPo;
            GeoConvHelper::getFinal().x2cartesian_const(*myPos);
            // set geo position in myTextFieldXY
            myTextFieldXY->setText(toString(*myPos).c_str(), FALSE);
            myTextFieldXY->setTextColor(FXRGB(0, 0, 0));
        } else {
            // set invalid color
            myTextFieldLonLat->setTextColor(FXRGB(255, 0, 0));
        }
    }
    return 1;
}


long
GNEGeometryPointDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // stop modal
    getApp()->stopModal(this);
    return 1;
}


long
GNEGeometryPointDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // set original position
    *myPos = myOriginalPos;
    // stop modal
    getApp()->stopModal(this);
    return 1;
}


long
GNEGeometryPointDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // set original position
    *myPos = myOriginalPos;
    // calculate geo position
    Position geoPos = *myPos;
    GeoConvHelper::getFinal().cartesian2geo(geoPos);
    // set valid colors
    myTextFieldXY->setTextColor(FXRGB(0, 0, 0));
    // check geo
    if (myGeo) {
        myTextFieldLonLat->setTextColor(FXRGB(0, 0, 0));
    }
    // set text field
    myTextFieldXY->setText(toString(*myPos).c_str(), FALSE);
    // check geo
    if (myGeo) {
        myTextFieldLonLat->setText(toString(geoPos).c_str(), FALSE);
    }
    return 1;
}


GNEGeometryPointDialog::GNEGeometryPointDialog() :
    myViewNet(nullptr),
    myOriginalPos(),
    myGeo(false) {
}

/****************************************************************************/
