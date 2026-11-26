/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEVClassesDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
///
// Dialog for edit allow VClass attribute
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNETagProperties.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/GNEHierarchicalElement.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEVClassesDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVClassesDialog::VClassRow) VClassRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWVCLASSES_TOGGLE,   GNEVClassesDialog::VClassRow::onCmdToggleVClass),
};

FXDEFMAP(GNEVClassesDialog) GNEVClassesDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWVCLASSES_SELECTALL,    GNEVClassesDialog::onCmdSelectAll),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWVCLASSES_UNSELECTALL,  GNEVClassesDialog::onCmdUnselectAll),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWVCLASSES_ONLY_ROAD,    GNEVClassesDialog::onCmdSelectOnlyRoad),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWVCLASSES_ONLY_RAIL,    GNEVClassesDialog::onCmdSelectOnlyRail),
};

// Object implementation
FXIMPLEMENT(GNEVClassesDialog::VClassRow,  FXHorizontalFrame,  VClassRowMap,               ARRAYNUMBER(VClassRowMap))
FXIMPLEMENT(GNEVClassesDialog,             GNEDialog,          GNEVClassesDialogMap,  ARRAYNUMBER(GNEVClassesDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEVClassesDialog::VClassRow - methods
// ---------------------------------------------------------------------------

GNEVClassesDialog::VClassRow::VClassRow(FXVerticalFrame* contentsFrame, SUMOVehicleClass vClass,
                                        GUIIcon vClassIcon, const std::string& description) :
    FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame),
    myVClassString(getVehicleClassNames(vClass)) {
    // add frame for vehicle icons
    FXLabel* labelVehicleIcon = new FXLabel(this, "", GUIIconSubSys::getIcon(vClassIcon), GUIDesignLabelIcon64x32Thicked);
    labelVehicleIcon->setBackColor(FXRGBA(255, 255, 255, 255));
    // create frame for information and button
    FXVerticalFrame* buttonAndInformationFrame = new FXVerticalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame* buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create button for enable/disable vehicle class
    myVClassButton = GUIDesigns::buildFXButton(buttonAndStatusFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::EMPTY),
                     this, MID_GNE_ALLOWVCLASSES_TOGGLE, GUIDesignButtonIcon);
    // create label for vehicle class name
    new FXLabel(buttonAndStatusFrame, myVClassString.c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, description.c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
}


GNEVClassesDialog::VClassRow::~VClassRow() {

}


const std::string&
GNEVClassesDialog::VClassRow::getVClassString() const {
    return myVClassString;
}


void
GNEVClassesDialog::VClassRow::setInitialStatus(const ValueStatus status) {
    myInitialStatus = status;
    resetStatus();
}


void
GNEVClassesDialog::VClassRow::resetStatus() {
    myCycleKeep = (myInitialStatus == ValueStatus::KEEP);
    updateButtonStatus(myInitialStatus);
}


void
GNEVClassesDialog::VClassRow::setVClassButtonStatus(const bool enabled) {
    myCycleKeep = false;
    updateButtonStatus(enabled ? ValueStatus::ENABLED : ValueStatus::DISABLED);
}


bool
GNEVClassesDialog::VClassRow::isVClassButtonEnabled() const {
    return (myStatus == ValueStatus::ENABLED);
}


GNEVClassesDialog::ValueStatus
GNEVClassesDialog::VClassRow::getStatus() const {
    return myStatus;
}


GNEVClassesDialog::ValueStatus
GNEVClassesDialog::VClassRow::getInitialStatus() const {
    return myInitialStatus;
}


long
GNEVClassesDialog::VClassRow::onCmdToggleVClass(FXObject*, FXSelector, void*) {
    if (myCycleKeep) {
        if (myStatus == ValueStatus::KEEP) {
            updateButtonStatus(ValueStatus::ENABLED);
        } else if (myStatus == ValueStatus::ENABLED) {
            updateButtonStatus(ValueStatus::DISABLED);
        } else {
            updateButtonStatus(ValueStatus::KEEP);
        }
    } else if (myStatus == ValueStatus::ENABLED) {
        updateButtonStatus(ValueStatus::DISABLED);
    } else {
        updateButtonStatus(ValueStatus::ENABLED);
    }
    return 1;
}


void
GNEVClassesDialog::VClassRow::updateButtonStatus(const ValueStatus status) {
    myStatus = status;
    switch (status) {
        case ValueStatus::ENABLED:
            myVClassButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::ACCEPT));
            myVClassButton->setTipText(TL("Allowed"));
            myVClassButton->setHelpText(TL("Allowed"));
            break;
        case ValueStatus::DISABLED:
            myVClassButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::CANCEL));
            myVClassButton->setTipText(TL("Disallowed"));
            myVClassButton->setHelpText(TL("Disallowed"));
            break;
        case ValueStatus::KEEP:
            myVClassButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::QUESTION_SMALL));
            myVClassButton->setTipText(TL("Different values (keep unchanged)"));
            myVClassButton->setHelpText(TL("Different values (keep unchanged)"));
            break;
    }
}

// ---------------------------------------------------------------------------
// GNEVClassesDialog - methods
// ---------------------------------------------------------------------------

GNEVClassesDialog::GNEVClassesDialog(GNEApplicationWindow* applicationWindow, SumoXMLAttr attr,
                                     const std::string originalVClasses) :
    GNEDialog(applicationWindow, TLF("Edit vClasses of attribute '%'", toString(attr)), GUIIcon::GREENVEHICLE,
              DialogType::VCLASS, GNEDialog::Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC),
    myOriginalVClasses(originalVClasses),
    myEditedVClasses(originalVClasses) {
    // build dialog
    builder();
}


GNEVClassesDialog::GNEVClassesDialog(GNEApplicationWindow* applicationWindow, GNEDialog* parentDialog,
                                     SumoXMLAttr attr, const std::string originalVClasses) :
    GNEDialog(applicationWindow, parentDialog, TLF("Edit vClasses of attribute '%'", toString(attr)), GUIIcon::GREENVEHICLE,
              DialogType::VCLASS, GNEDialog::Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC),
    myOriginalVClasses(originalVClasses),
    myEditedVClasses(originalVClasses) {
    // build dialog
    builder();
}


GNEVClassesDialog::GNEVClassesDialog(GNEApplicationWindow* applicationWindow, SumoXMLAttr attr,
                                     const std::vector<GNEAttributeCarrier*>& ACs) :
    GNEDialog(applicationWindow, TLF("Edit vClasses of attribute '%'", toString(attr)), GUIIcon::GREENVEHICLE,
              DialogType::VCLASS, GNEDialog::Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC),
    myMultipleMode(true) {
    // compute common allowed and disallowed vClasses across all edited ACs
    for (const auto& AC : ACs) {
        if ((AC->getTagProperty()->getTag() == SUMO_TAG_EDGE) && (attr == SUMO_ATTR_ALLOW) &&
                AC->getHierarchicalElement() && !AC->getHierarchicalElement()->getChildLanes().empty()) {
            for (const auto& lane : AC->getHierarchicalElement()->getChildLanes()) {
                const SVCPermissions perms = parseVehicleClasses(lane->getAttribute(attr));
                myCommonAllowed &= perms;
                myCommonDisallowed &= invertPermissions(perms);
            }
        } else if (AC->hasAttribute(attr)) {
            const SVCPermissions perms = parseVehicleClasses(AC->getAttribute(attr));
            myCommonAllowed &= perms;
            myCommonDisallowed &= invertPermissions(perms);
        }
    }
    // build dialog
    builder();
}


GNEVClassesDialog::~GNEVClassesDialog() {
}


void
GNEVClassesDialog::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    if (dialogArgument->getCustomAction() == "allVehicles") {
        onCmdSelectAll(nullptr, 0, nullptr);
    } else if (dialogArgument->getCustomAction() == "disallowAll") {
        onCmdUnselectAll(nullptr, 0, nullptr);
    } else if (dialogArgument->getCustomAction() == "onlyRoadVehicles") {
        onCmdSelectOnlyRoad(nullptr, 0, nullptr);
    } else if (dialogArgument->getCustomAction() == "onlyRailVehicles") {
        onCmdSelectOnlyRail(nullptr, 0, nullptr);
    } else if (SumoVehicleClassStrings.hasString(dialogArgument->getCustomAction())) {
        myVClassMap[getVehicleClassID(dialogArgument->getCustomAction())]->onCmdToggleVClass(nullptr, 0, nullptr);
    } else {
        WRITE_ERROR("Invalid dialog argument " + dialogArgument->getCustomAction() + " used in GNEVClassesDialog::runInternalTest");
    }
}


std::string
GNEVClassesDialog::getModifiedVClasses() const {
    return myEditedVClasses;
}


SVCPermissions
GNEVClassesDialog::getAllowedVClasses() const {
    return myAllowedVClasses;
}


SVCPermissions
GNEVClassesDialog::getDisallowedVClasses() const {
    return myDisallowedVClasses;
}


long
GNEVClassesDialog::onCmdSelectAll(FXObject*, FXSelector, void*) {
    // change all icons to accept
    for (const auto& vClass : myVClassMap) {
        vClass.second->setVClassButtonStatus(true);
    }
    return 1;
}


long
GNEVClassesDialog::onCmdUnselectAll(FXObject*, FXSelector, void*) {
    // change all icons to cancel
    for (const auto& vClass : myVClassMap) {
        vClass.second->setVClassButtonStatus(false);
    }
    return 1;
}


long
GNEVClassesDialog::onCmdSelectOnlyRoad(FXObject*, FXSelector, void*) {
    // change all non-road icons to disallow, and allow for the rest
    for (const auto& vClass : myVClassMap) {
        if ((vClass.first & (SVC_PEDESTRIAN | SVC_NON_ROAD)) == 0) {
            vClass.second->setVClassButtonStatus(true);
        } else {
            vClass.second->setVClassButtonStatus(false);
        }
    }
    return 1;
}


long
GNEVClassesDialog::onCmdSelectOnlyRail(FXObject*, FXSelector, void*) {
    // change all non-road icons to disallow, and allow for the rest
    for (const auto& vClass : myVClassMap) {
        if ((vClass.first & SVC_RAIL_CLASSES) != 0) {
            vClass.second->setVClassButtonStatus(true);
        } else {
            vClass.second->setVClassButtonStatus(false);
        }
    }
    return 1;
}


long
GNEVClassesDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    bool allSelected = true;
    bool changed = false;
    myAllowedVClasses = 0;
    myDisallowedVClasses = 0;
    // clear allow and disallow VClasses
    std::vector<std::string> allowedVehicles;
    for (const auto& vClass : myVClassMap) {
        if (vClass.second->getStatus() != vClass.second->getInitialStatus()) {
            changed = true;
        }
        // check if vehicle is allowed depending on status
        if (vClass.second->getStatus() == ValueStatus::ENABLED) {
            myAllowedVClasses |= vClass.first;
            allowedVehicles.push_back(vClass.second->getVClassString());
        } else {
            allSelected = false;
            if (vClass.second->getStatus() == ValueStatus::DISABLED) {
                myDisallowedVClasses |= vClass.first;
            }
        }
    }
    if (allSelected) {
        myEditedVClasses = "all";
    } else {
        myEditedVClasses = joinToString(allowedVehicles, " ");
    }
    // Stop Modal if we have different classes from original
    if (myMultipleMode ? changed : (myEditedVClasses != myOriginalVClasses)) {
        return closeDialogAccepting();
    } else {
        return closeDialogCanceling();
    }
}


long
GNEVClassesDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // reset vClasses
    myEditedVClasses = myOriginalVClasses;
    myAllowedVClasses = 0;
    myDisallowedVClasses = 0;
    // cancel
    return closeDialogCanceling();
}


long
GNEVClassesDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset vClasses
    myEditedVClasses = myOriginalVClasses;
    for (const auto& vClass : myVClassMap) {
        vClass.second->resetStatus();
    }
    return 1;
}


void
GNEVClassesDialog::builder() {
    // label for selection options
    new FXLabel(getContentFrame(), TL("Selection options"), nullptr, GUIDesignLabel(JUSTIFY_NORMAL));
    // horizontal frame uniform for options
    FXHorizontalFrame* optionsFrame = new FXHorizontalFrame(getContentFrame(), GUIDesignAuxiliarHorizontalFrameUniform);
    // allow all
    GUIDesigns::buildFXButton(optionsFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::OK), this, MID_GNE_ALLOWVCLASSES_SELECTALL, GUIDesignButtonIcon);
    new FXLabel(optionsFrame, TL("Allow all vehicles"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // only road
    GUIDesigns::buildFXButton(optionsFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::OK), this, MID_GNE_ALLOWVCLASSES_ONLY_ROAD, GUIDesignButtonIcon);
    new FXLabel(optionsFrame, TL("Allow only road vehicles"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // only rail
    GUIDesigns::buildFXButton(optionsFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::OK), this, MID_GNE_ALLOWVCLASSES_ONLY_RAIL, GUIDesignButtonIcon);
    new FXLabel(optionsFrame, TL("Allow only rail vehicles"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // disallow all
    GUIDesigns::buildFXButton(optionsFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_ALLOWVCLASSES_UNSELECTALL, GUIDesignButtonIcon);
    new FXLabel(optionsFrame, TL("Disallow all vehicles"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // label for select vClasses
    new FXLabel(getContentFrame(), TL("Select vClasses"), nullptr, GUIDesignLabel(JUSTIFY_NORMAL));
    // Create frame for vehicles's columns
    FXHorizontalFrame* myVehiclesFrame = new FXHorizontalFrame(getContentFrame(), GUIDesignContentsFrame);
    // create left frame and fill it
    FXVerticalFrame* vehiclesLeftFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    myVClassMap[SVC_PASSENGER] = new VClassRow(vehiclesLeftFrame, SVC_PASSENGER, GUIIcon::VCLASS_PASSENGER, TL("Default vehicle class"));
    myVClassMap[SVC_PRIVATE] = new VClassRow(vehiclesLeftFrame, SVC_PRIVATE, GUIIcon::VCLASS_PRIVATE, TL("A passenger car assigned for private use"));
    myVClassMap[SVC_TAXI] = new VClassRow(vehiclesLeftFrame, SVC_TAXI, GUIIcon::VCLASS_TAXI, TL("Vehicle for hire with a driver"));
    myVClassMap[SVC_BUS] = new VClassRow(vehiclesLeftFrame, SVC_BUS, GUIIcon::VCLASS_BUS, TL("Urban line traffic"));
    myVClassMap[SVC_COACH] = new VClassRow(vehiclesLeftFrame, SVC_COACH, GUIIcon::VCLASS_COACH, TL("Overland transport"));
    myVClassMap[SVC_DELIVERY] = new VClassRow(vehiclesLeftFrame, SVC_DELIVERY, GUIIcon::VCLASS_DELIVERY, TL("Vehicles specialized in delivering goods"));
    myVClassMap[SVC_TRUCK] = new VClassRow(vehiclesLeftFrame, SVC_TRUCK, GUIIcon::VCLASS_TRUCK, TL("Vehicle designed to transport cargo"));
    myVClassMap[SVC_TRAILER] = new VClassRow(vehiclesLeftFrame, SVC_TRAILER, GUIIcon::VCLASS_TRAILER, TL("Truck with trailer"));
    myVClassMap[SVC_EMERGENCY] = new VClassRow(vehiclesLeftFrame, SVC_EMERGENCY, GUIIcon::VCLASS_EMERGENCY, TL("Vehicle designated to respond to an emergency"));
    myVClassMap[SVC_MOTORCYCLE] = new VClassRow(vehiclesLeftFrame, SVC_MOTORCYCLE, GUIIcon::VCLASS_MOTORCYCLE, TL("Two- or three-wheeled motor vehicle"));
    myVClassMap[SVC_MOPED] = new VClassRow(vehiclesLeftFrame, SVC_MOPED, GUIIcon::VCLASS_MOPED, TL("Motorcycle not allowed in motorways"));
    // create center frame and fill it
    FXVerticalFrame* vehiclesCenterFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    myVClassMap[SVC_BICYCLE] = new VClassRow(vehiclesCenterFrame, SVC_BICYCLE, GUIIcon::VCLASS_BICYCLE, TL("Human-powered, pedal-driven vehicle"));
    myVClassMap[SVC_SCOOTER] = new VClassRow(vehiclesCenterFrame, SVC_SCOOTER, GUIIcon::VCLASS_SCOOTER, TL("An electric scooter or a kick scooter"));
    myVClassMap[SVC_PEDESTRIAN] = new VClassRow(vehiclesCenterFrame, SVC_PEDESTRIAN, GUIIcon::VCLASS_PEDESTRIAN, TL("Person traveling on foot"));
    myVClassMap[SVC_WHEELCHAIR] = new VClassRow(vehiclesCenterFrame, SVC_WHEELCHAIR, GUIIcon::VCLASS_WHEELCHAIR, TL("A mobility impaired person"));
    myVClassMap[SVC_TRAM] = new VClassRow(vehiclesCenterFrame, SVC_TRAM, GUIIcon::VCLASS_TRAM, TL("Rail vehicle which runs on tracks"));
    myVClassMap[SVC_RAIL_ELECTRIC] = new VClassRow(vehiclesCenterFrame, SVC_RAIL_ELECTRIC, GUIIcon::VCLASS_RAIL_ELECTRIC, TL("Rail electric vehicle"));
    myVClassMap[SVC_RAIL_FAST] = new VClassRow(vehiclesCenterFrame, SVC_RAIL_FAST, GUIIcon::VCLASS_RAIL_FAST, TL("High-speed rail vehicle"));
    myVClassMap[SVC_RAIL_URBAN] = new VClassRow(vehiclesCenterFrame, SVC_RAIL_URBAN, GUIIcon::VCLASS_RAIL_URBAN, TL("Heavier than tram"));
    myVClassMap[SVC_RAIL] = new VClassRow(vehiclesCenterFrame, SVC_RAIL, GUIIcon::VCLASS_RAIL, TL("Heavy rail vehicle"));
    myVClassMap[SVC_CABLE_CAR] = new VClassRow(vehiclesCenterFrame, SVC_CABLE_CAR, GUIIcon::VCLASS_CABLE_CAR, TL("A conveyance suspended on a cable"));
    myVClassMap[SVC_SUBWAY] = new VClassRow(vehiclesCenterFrame, SVC_SUBWAY, GUIIcon::VCLASS_SUBWAY, TL("A railway that mostly runs underground"));
    // create right frame and fill it  (8 vehicles)
    FXVerticalFrame* vehiclesRightFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    myVClassMap[SVC_E_VEHICLE] = new VClassRow(vehiclesRightFrame, SVC_E_VEHICLE, GUIIcon::VCLASS_EVEHICLE, TL("Future electric mobility vehicles"));
    myVClassMap[SVC_ARMY] = new VClassRow(vehiclesRightFrame, SVC_ARMY, GUIIcon::VCLASS_ARMY, TL("Vehicle designed for military forces"));
    myVClassMap[SVC_SHIP] = new VClassRow(vehiclesRightFrame, SVC_SHIP, GUIIcon::VCLASS_SHIP, TL("Basic class for navigating waterway"));
    myVClassMap[SVC_AUTHORITY] = new VClassRow(vehiclesRightFrame, SVC_AUTHORITY, GUIIcon::VCLASS_AUTHORITY, TL("Vehicle of a governmental security agency"));
    myVClassMap[SVC_VIP] = new VClassRow(vehiclesRightFrame, SVC_VIP, GUIIcon::VCLASS_VIP, TL("A civilian security armored car used by VIPs"));
    myVClassMap[SVC_HOV] = new VClassRow(vehiclesRightFrame, SVC_HOV, GUIIcon::VCLASS_HOV, TL("High-Occupancy Vehicle (two or more passengers)"));
    myVClassMap[SVC_CONTAINER] = new VClassRow(vehiclesRightFrame, SVC_CONTAINER, GUIIcon::VCLASS_CONTAINER, TL("A transport container"));
    myVClassMap[SVC_AIRCRAFT] = new VClassRow(vehiclesRightFrame, SVC_AIRCRAFT, GUIIcon::VCLASS_AIRCRAFT, TL("An airplane"));
    myVClassMap[SVC_DRONE] = new VClassRow(vehiclesRightFrame, SVC_DRONE, GUIIcon::VCLASS_DRONE, TL("A small unmanned robot"));
    myVClassMap[SVC_CUSTOM1] = new VClassRow(vehiclesRightFrame, SVC_CUSTOM1, GUIIcon::VCLASS_CUSTOM1, TL("Reserved for user-defined semantics"));
    myVClassMap[SVC_CUSTOM2] = new VClassRow(vehiclesRightFrame, SVC_CUSTOM2, GUIIcon::VCLASS_CUSTOM2, TL("Reserved for user-defined semantics"));
    // initialize initial statuses
    if (myMultipleMode) {
        for (const auto& vClass : myVClassMap) {
            if ((myCommonAllowed & vClass.first) != 0) {
                vClass.second->setInitialStatus(ValueStatus::ENABLED);
            } else if ((myCommonDisallowed & vClass.first) != 0) {
                vClass.second->setInitialStatus(ValueStatus::DISABLED);
            } else {
                vClass.second->setInitialStatus(ValueStatus::KEEP);
            }
        }
    } else if (myOriginalVClasses == "all") {
        for (const auto& vClass : myVClassMap) {
            vClass.second->setInitialStatus(ValueStatus::ENABLED);
        }
    } else {
        const std::vector<std::string>& allowStringVector = StringTokenizer(myOriginalVClasses).getVector();
        const std::set<std::string> allowSet(allowStringVector.begin(), allowStringVector.end());
        for (const auto& vClass : myVClassMap) {
            if (allowSet.count(getVehicleClassNames(vClass.first)) > 0) {
                vClass.second->setInitialStatus(ValueStatus::ENABLED);
            } else {
                vClass.second->setInitialStatus(ValueStatus::DISABLED);
            }
        }
    }
    // reset dialog
    onCmdReset(nullptr, 0, nullptr);
    // open modal dialog
    openDialog();
}

/****************************************************************************/
