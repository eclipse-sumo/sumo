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
/// @file    GUIDialog_ViewSettings.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Mirko Barthauer
/// @date    Wed, 21. Dec 2005
///
// The dialog to change the view (gui) settings.
/****************************************************************************/
#include <config.h>

#include <fstream>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/MFXComboBoxIcon.h>
#include <utils/common/RGBColor.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringUtils.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/importio/LineReader.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/gui/div/GUIGlobalPostDrawing.h>

#include "GUIDialog_EditViewport.h"
#include "GUIDialog_ViewSettings.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GUIDialog_ViewSettings::SizePanel) GUIDialog_SizeMap[] = {
    FXMAPFUNC(SEL_CHANGED,  MID_SIMPLE_VIEW_SIZECHANGE,     GUIDialog_ViewSettings::SizePanel::onCmdSizeChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_SIZECHANGE,     GUIDialog_ViewSettings::SizePanel::onCmdSizeChange)
};

FXDEFMAP(GUIDialog_ViewSettings) GUIDialog_ViewSettingsMap[] = {
    FXMAPFUNC(SEL_CHANGED,  MID_SIMPLE_VIEW_COLORCHANGE,    GUIDialog_ViewSettings::onCmdColorChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_COLORCHANGE,    GUIDialog_ViewSettings::onCmdColorChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_NAMECHANGE,     GUIDialog_ViewSettings::onCmdNameChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_OK,                GUIDialog_ViewSettings::onCmdOk),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_CANCEL,            GUIDialog_ViewSettings::onCmdCancel),
    // settings
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_SAVE,   GUIDialog_ViewSettings::onCmdSaveSetting),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMPLE_VIEW_SAVE,   GUIDialog_ViewSettings::onUpdSaveSetting),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_DELETE, GUIDialog_ViewSettings::onCmdDeleteSetting),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMPLE_VIEW_DELETE, GUIDialog_ViewSettings::onUpdDeleteSetting),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_EXPORT, GUIDialog_ViewSettings::onCmdExportSetting),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMPLE_VIEW_EXPORT, GUIDialog_ViewSettings::onUpdExportSetting),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_IMPORT, GUIDialog_ViewSettings::onCmdImportSetting),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMPLE_VIEW_IMPORT, GUIDialog_ViewSettings::onUpdImportSetting),
    // decals
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_LOAD_DECAL,         GUIDialog_ViewSettings::onCmdLoadDecal),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_LOAD_DECALS_XML,    GUIDialog_ViewSettings::onCmdLoadXMLDecals),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_SAVE_DECALS_XML,    GUIDialog_ViewSettings::onCmdSaveXMLDecals),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_CLEAR_DECALS,       GUIDialog_ViewSettings::onCmdClearDecals),
};

FXIMPLEMENT(GUIDialog_ViewSettings,             FXDialogBox,    GUIDialog_ViewSettingsMap,  ARRAYNUMBER(GUIDialog_ViewSettingsMap))
FXIMPLEMENT(GUIDialog_ViewSettings::SizePanel,  FXObject,       GUIDialog_SizeMap,          ARRAYNUMBER(GUIDialog_SizeMap))

// ===========================================================================
// method definitions
// ===========================================================================

GUIDialog_ViewSettings::GUIDialog_ViewSettings(GUISUMOAbstractView* parent, GUIVisualizationSettings* settings) :
    FXDialogBox(parent, TL("View Settings"), GUIDesignViewSettingsMainDialog),
    GUIPersistentWindowPos(this, "VIEWSETTINGS", true, 20, 40, 700, 500, 400, 20),
    myParent(parent),
    mySettings(settings),
    myBackup(settings->name, settings->netedit) {
    // make a backup copy
    myBackup.copy(*settings);
    // create content frame
    FXVerticalFrame* contentFrame = new FXVerticalFrame(this, GUIDesignViewSettingsVerticalFrame1);
    // build header
    buildHeader(contentFrame);
    // create tabbook for every section
    FXTabBook* tabbook = new FXTabBook(contentFrame, nullptr, 0, GUIDesignViewSettingsTabBook1);
    // build background frame
    buildBackgroundFrame(tabbook);
    // build streets frame
    buildStreetsFrame(tabbook);
    // build vehicles frame
    buildVehiclesFrame(tabbook);
    // build persons frame
    buildPersonsFrame(tabbook);
    // build containers frame
    buildContainersFrame(tabbook);
    // build junctions frame
    buildJunctionsFrame(tabbook);
    // build additionals frame
    buildAdditionalsFrame(tabbook);
    // build demand frame
    if (mySettings->netedit) {
        buildDemandFrame(tabbook);
    }
    // build POIs frame
    buildPOIsFrame(tabbook);
    // build polygons frame
    buildPolygonsFrame(tabbook);
    // build selection frame (only in netedit)
    if (mySettings->netedit) {
        buildSelectionFrame(tabbook);
    }
    // build data frame (only in netedit)
    if (mySettings->netedit) {
        buildDataFrame(tabbook);
    }
    // build legend frame
    buildLegendFrame(tabbook);
    // build 3D frame
    build3DFrame(tabbook);
    if (mySettings->netedit) {
        myFrame3D->disable();
    }
    // build openGL frame
    buildOpenGLFrame(tabbook);
    // build buttons
    buildButtons(contentFrame);
    // rebuild color matrix
    rebuildColorMatrices(false);
    setIcon(GUIIconSubSys::getIcon(GUIIcon::EMPTY));
    loadWindowPos();
}


GUIDialog_ViewSettings::~GUIDialog_ViewSettings() {
    myParent->remove(this);
    // delete name panels
    delete myInternalJunctionNamePanel;
    delete myInternalEdgeNamePanel;
    delete myTLSPhaseIndexPanel;
    delete myTLSPhaseNamePanel;
    delete myCwaEdgeNamePanel;
    delete myStreetNamePanel;
    delete myEdgeValuePanel;
    delete myEdgeScaleValuePanel;
    delete myJunctionIndexPanel;
    delete myTLIndexPanel;
    delete myJunctionIDPanel;
    delete myJunctionNamePanel;
    delete myVehicleNamePanel;
    delete myVehicleValuePanel;
    delete myVehicleScaleValuePanel;
    delete myVehicleTextPanel;
    delete myPersonNamePanel;
    delete myPersonValuePanel;
    delete myAddNamePanel;
    delete myAddFullNamePanel;
    delete myPOINamePanel;
    delete myPOITypePanel;
    delete myPOITextPanel;
    delete myPolyNamePanel;
    delete myPolyTypePanel;
    delete myEdgeNamePanel;
    delete myDataValuePanel;
    delete myGeometryIndicesPanel;
    // delete size panels
    delete myVehicleSizePanel;
    delete myPersonSizePanel;
    delete myJunctionSizePanel;
    delete myPOISizePanel;
    delete myPolySizePanel;
    delete myAddSizePanel;
}


void
GUIDialog_ViewSettings::show() {
    // update buttons that can be changed externally
    myShowGrid->setCheck(mySettings->showGrid);
    myGridXSizeDialer->setValue(mySettings->gridXSize);
    myGridYSizeDialer->setValue(mySettings->gridYSize);
    // create myNewDecalsTable
    myDecalsTable->create();
    myDecalsTable->fillTable();
    FXDialogBox::show();
}


GUISUMOAbstractView*
GUIDialog_ViewSettings::getSUMOAbstractView() {
    return myParent;
}


void
GUIDialog_ViewSettings::setCurrent(GUIVisualizationSettings* settings) {
    mySettings = settings;
    myBackup.copy(*settings);
    onCmdNameChange(nullptr, 0, nullptr);
}


long
GUIDialog_ViewSettings::onCmdOk(FXObject*, FXSelector, void*) {
    hide();
    return 1;
}


long
GUIDialog_ViewSettings::onCmdCancel(FXObject*, FXSelector, void*) {
    hide();
    mySettings->copy(myBackup);
    myParent->update();
    return 1;
}


long
GUIDialog_ViewSettings::onCmdNameChange(FXObject*, FXSelector, void* ptr) {
    if (ptr != nullptr) {
        FXString dataS = (char*) ptr; // !!!unicode
        // check whether this item has been added twice
        if (dataS == mySchemeName->getItemText(mySchemeName->getNumItems() - 1)) {
            for (int i = 0; i < mySchemeName->getNumItems() - 1; ++i) {
                if (dataS == mySchemeName->getItemText(i)) {
                    mySchemeName->removeItem(i);
                }
            }
        }
        myBackup.copy(gSchemeStorage.get(dataS.text()));
        mySettings = &gSchemeStorage.get(dataS.text());
    }
    rebuildColorMatrices(true);

    myBackgroundColor->setRGBA(MFXUtils::getFXColor(mySettings->backgroundColor));

    myBusStopColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.busStopColor));
    myBusStopColorSign->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.busStopColorSign));
    myTrainStopColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.trainStopColor));
    myTrainStopColorSign->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.trainStopColorSign));
    myContainerStopColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.containerStopColor));
    myContainerStopColorSign->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.containerStopColorSign));
    myChargingStationColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.chargingStationColor));
    myChargingStationColorSign->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.chargingStationColorSign));
    if (mySettings->netedit) {
        myStopColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.stopColor));
        myWaypointColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.waypointColor));
        myVehicleTripsColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.vehicleTripColor));
        myStopPersonsColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.stopPersonColor));
        myPersonTripColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.personTripColor));
        myWalkColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.walkColor));
        myRideColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.rideColor));
        myStopContainersColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.stopContainerColor));
        myTransportColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.transportColor));
        myTranshipColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.transhipColor));

        myTripWidth->setValue(mySettings->widthSettings.tripWidth);
        myPersonTripWidth->setValue(mySettings->widthSettings.personTripWidth);
        myWalkWidth->setValue(mySettings->widthSettings.walkWidth);
        myRideWidth->setValue(mySettings->widthSettings.rideWidth);
        myTransportWidth->setValue(mySettings->widthSettings.transportWidth);
        myTranshipWidth->setValue(mySettings->widthSettings.transhipWidth);

        mySelectionColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectionColor));
        mySelectedEdgeColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedEdgeColor));
        mySelectedLaneColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedLaneColor));
        mySelectedConnectionColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedConnectionColor));
        mySelectedProhibitionColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedProhibitionColor));
        mySelectedCrossingColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedCrossingColor));
        mySelectedAdditionalColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedAdditionalColor));
        mySelectedRouteColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedRouteColor));
        mySelectedVehicleColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedVehicleColor));
        mySelectedPersonColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedPersonColor));
        mySelectedPersonPlanColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedPersonPlanColor));
        mySelectedEdgeDataColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.selectedEdgeDataColor));

        myDataValuePanel->update(mySettings->dataValue);
        myDataColorMode->setCurrentItem((FXint) mySettings->dataColorer.getActive());
        myEdgeRelationUpscaleDialer->setValue(mySettings->edgeRelWidthExaggeration);
        myTazRelationUpscaleDialer->setValue(mySettings->tazRelWidthExaggeration);
        myDataColorRainbowCheck->setCheck(mySettings->dataValueHideCheck);
        myDataColorRainbowThreshold->setValue(mySettings->dataValueHideThreshold);
    }

    myLaneEdgeColorMode->setCurrentItem((FXint) mySettings->getLaneEdgeMode());
    myLaneEdgeScaleMode->setCurrentItem((FXint) mySettings->getLaneEdgeScaleMode());
    myShowLaneBorders->setCheck(mySettings->laneShowBorders);
    myShowBikeMarkings->setCheck(mySettings->showBikeMarkings);
    myShowLaneDecals->setCheck(mySettings->showLinkDecals);
    myRealisticLinkRules->setCheck(mySettings->realisticLinkRules);
    myShowLinkRules->setCheck(mySettings->showLinkRules);
    myShowRails->setCheck(mySettings->showRails);
    mySecondaryShape->setCheck(mySettings->secondaryShape);
    myEdgeNamePanel->update(mySettings->edgeName);
    myInternalEdgeNamePanel->update(mySettings->internalEdgeName);
    myCwaEdgeNamePanel->update(mySettings->cwaEdgeName);
    myStreetNamePanel->update(mySettings->streetName);
    myEdgeValuePanel->update(mySettings->edgeValue);
    myEdgeScaleValuePanel->update(mySettings->edgeScaleValue);
    myHideMacroConnectors->setCheck(mySettings->hideConnectors);
    myShowLaneDirection->setCheck(mySettings->showLaneDirection);
    myShowSublanes->setCheck(mySettings->showSublanes);
    mySpreadSuperposed->setCheck(mySettings->spreadSuperposed);
    myLaneColorRainbowCheck->setCheck(mySettings->edgeValueHideCheck);
    myLaneColorRainbowThreshold->setValue(mySettings->edgeValueHideThreshold);
    myLaneColorRainbowCheck2->setCheck(mySettings->edgeValueHideCheck2);
    myLaneColorRainbowThreshold2->setValue(mySettings->edgeValueHideThreshold2);
    myLaneWidthUpscaleDialer->setValue(mySettings->laneWidthExaggeration);
    myLaneMinWidthDialer->setValue(mySettings->laneMinSize);

    myVehicleColorMode->setCurrentItem((FXint) mySettings->vehicleColorer.getActive());
    myVehicleScaleMode->setCurrentItem((FXint) mySettings->vehicleScaler.getActive());
    myVehicleShapeDetail->setCurrentItem(mySettings->vehicleQuality);
    myShowBlinker->setCheck(mySettings->showBlinker);
    myShowMinGap->setCheck(mySettings->drawMinGap);
    myShowBrakeGap->setCheck(mySettings->drawBrakeGap);
    myShowBTRange->setCheck(mySettings->showBTRange);
    myShowRouteIndex->setCheck(mySettings->showRouteIndex);
    myScaleLength->setCheck(mySettings->scaleLength);
    myDrawReversed->setCheck(mySettings->drawReversed);
    myShowParkingInfo->setCheck(mySettings->showParkingInfo);
    /*
    myShowLaneChangePreference->setCheck(mySettings->drawLaneChangePreference);
    */
    myVehicleNamePanel->update(mySettings->vehicleName);
    myVehicleValuePanel->update(mySettings->vehicleValue);
    myVehicleScaleValuePanel->update(mySettings->vehicleScaleValue);
    myVehicleTextPanel->update(mySettings->vehicleText);
    myVehicleSizePanel->update(mySettings->vehicleSize);

    myPersonColorMode->setCurrentItem((FXint) mySettings->personColorer.getActive());
    myPersonShapeDetail->setCurrentItem(mySettings->personQuality);
    myPersonNamePanel->update(mySettings->personName);
    myPersonValuePanel->update(mySettings->personValue);
    myPersonSizePanel->update(mySettings->personSize);

    myContainerColorMode->setCurrentItem((FXint) mySettings->containerColorer.getActive());
    myContainerShapeDetail->setCurrentItem(mySettings->containerQuality);
    myContainerNamePanel->update(mySettings->containerName);
    myContainerSizePanel->update(mySettings->containerSize);

    myJunctionColorMode->setCurrentItem((FXint) mySettings->junctionColorer.getActive());
    myTLIndexPanel->update(mySettings->drawLinkTLIndex);
    myJunctionIndexPanel->update(mySettings->drawLinkJunctionIndex);
    myJunctionIDPanel->update(mySettings->junctionID);
    myJunctionNamePanel->update(mySettings->junctionName);
    myInternalJunctionNamePanel->update(mySettings->internalJunctionName);
    myTLSPhaseIndexPanel->update(mySettings->tlsPhaseIndex);
    myTLSPhaseNamePanel->update(mySettings->tlsPhaseName);
    myJunctionSizePanel->update(mySettings->junctionSize);

    myAddNamePanel->update(mySettings->addName);
    myAddFullNamePanel->update(mySettings->addFullName);
    myAddSizePanel->update(mySettings->addSize);

    myPoiDetail->setValue(mySettings->poiDetail);
    myPOINamePanel->update(mySettings->poiName);
    myPOITypePanel->update(mySettings->poiType);
    myPOITextPanel->update(mySettings->poiText);
    myPOISizePanel->update(mySettings->poiSize);

    myPolyNamePanel->update(mySettings->polyName);
    myPolyTypePanel->update(mySettings->polyType);
    myPolySizePanel->update(mySettings->polySize);

    myShowLane2Lane->setCheck(mySettings->showLane2Lane);
    myDrawJunctionShape->setCheck(mySettings->drawJunctionShape);
    myDrawCrossingsAndWalkingAreas->setCheck(mySettings->drawCrossingsAndWalkingareas);
    myDither->setCheck(mySettings->dither);
    myFPS->setCheck(mySettings->fps);
    myDrawBoundaries->setCheck(mySettings->drawBoundaries);
    myForceDrawForPositionSelection->setCheck(mySettings->forceDrawForPositionSelection);
    myForceDrawForRectangleSelection->setCheck(mySettings->forceDrawForRectangleSelection);
    myDisableDottedContours->setCheck(mySettings->disableDottedContours);
    myGeometryIndicesPanel->update(mySettings->geometryIndices);
    myShowSizeLegend->setCheck(mySettings->showSizeLegend);
    myShowColorLegend->setCheck(mySettings->showColorLegend);
    myShowVehicleColorLegend->setCheck(mySettings->showVehicleColorLegend);

    myParent->setColorScheme(mySettings->name);
    update();
    myParent->update();
    return 1;
}


bool
GUIDialog_ViewSettings::updateColorRanges(FXObject* sender, std::vector<FXColorWell*>::const_iterator colIt,
        std::vector<FXColorWell*>::const_iterator colEnd,
        std::vector<FXRealSpinner*>::const_iterator threshIt,
        std::vector<FXRealSpinner*>::const_iterator threshEnd,
        std::vector<FXButton*>::const_iterator buttonIt,
        GUIColorScheme& scheme) {
    UNUSED_PARAMETER(threshEnd);
    int pos = 0;
    while (colIt != colEnd) {
        if (scheme.isFixed()) {
            if (sender == *colIt) {
                scheme.setColor(pos, MFXUtils::getRGBColor((*colIt)->getRGBA()));
            }
        } else {
            if (sender == *threshIt) {
                const double val = (*threshIt)->getValue();
                scheme.setThreshold(pos, val);
                return false;
            }
            if (sender == *colIt) {
                scheme.setColor(pos, MFXUtils::getRGBColor((*colIt)->getRGBA()));
                return false;
            }
            if (sender == *buttonIt) {
                scheme.addColor(MFXUtils::getRGBColor((*colIt)->getRGBA()), (*threshIt)->getValue());
                return true;
            } else if (sender == *(buttonIt + 1)) {
                scheme.removeColor(pos);
                return true;
            }
            // 2 buttons per item (add / remove)
            threshIt++;
            buttonIt += 2;
        }
        ++colIt;
        pos++;
    }
    return false;
}


bool
GUIDialog_ViewSettings::updateScaleRanges(FXObject* sender, std::vector<FXRealSpinner*>::const_iterator scaleIt,
        std::vector<FXRealSpinner*>::const_iterator scaleEnd,
        std::vector<FXRealSpinner*>::const_iterator threshIt,
        std::vector<FXRealSpinner*>::const_iterator threshEnd,
        std::vector<FXButton*>::const_iterator buttonIt,
        GUIScaleScheme& scheme) {
    int pos = 0;
    while (scaleIt != scaleEnd) {
        if (scheme.isFixed()) {
            if (sender == *scaleIt) {
                scheme.setColor(pos, (*scaleIt)->getValue());
            }
        } else {
            if (sender == *threshIt) {
                const double val = (*threshIt)->getValue();
                double lo, hi;
                if (pos != 0) {
                    threshIt--;
                    (*threshIt)->getRange(lo, hi);
                    (*threshIt)->setRange(lo, val);
                    threshIt++;
                }
                threshIt++;
                if (threshIt != threshEnd) {
                    (*threshIt)->getRange(lo, hi);
                    (*threshIt)->setRange(val, hi);
                }
                scheme.setThreshold(pos, val);
                return false;
            }
            if (sender == *scaleIt) {
                scheme.setColor(pos, (*scaleIt)->getValue());
                return false;
            }
            if (sender == *buttonIt) {
                scheme.addColor((*scaleIt)->getValue(), (*threshIt)->getValue());
                return true;
            } else if (sender == *(buttonIt + 1)) {
                scheme.removeColor(pos);
                return true;
            }
            threshIt++;
            buttonIt += 2;
        }
        ++scaleIt;
        pos++;
    }
    return false;
}


long
GUIDialog_ViewSettings::onCmdColorChange(FXObject* sender, FXSelector, void* /*val*/) {
    GUIVisualizationSettings tmpSettings(mySettings->name);
    tmpSettings.copy(*mySettings);
    int prevLaneMode = mySettings->getLaneEdgeMode();
    int prevLaneScaleMode = mySettings->getLaneEdgeScaleMode();
    int prevVehicleMode = mySettings->vehicleColorer.getActive();
    int prevVehicleScaleMode = mySettings->vehicleScaler.getActive();
    int prevPersonMode = mySettings->personColorer.getActive();
    int prevContainerMode = mySettings->containerColorer.getActive();
    int prevJunctionMode = mySettings->junctionColorer.getActive();
    int prevPOIMode = mySettings->poiColorer.getActive();
    int prevPolyMode = mySettings->polyColorer.getActive();
    int prevDataMode = mySettings->dataColorer.getActive();
    bool doRebuildColorMatrices = false;

    tmpSettings.name = mySettings->name;
    tmpSettings.backgroundColor = MFXUtils::getRGBColor(myBackgroundColor->getRGBA());

    // additionals
    tmpSettings.colorSettings.busStopColor = MFXUtils::getRGBColor(myBusStopColor->getRGBA());
    tmpSettings.colorSettings.busStopColorSign = MFXUtils::getRGBColor(myBusStopColorSign->getRGBA());
    tmpSettings.colorSettings.trainStopColor = MFXUtils::getRGBColor(myTrainStopColor->getRGBA());
    tmpSettings.colorSettings.trainStopColorSign = MFXUtils::getRGBColor(myTrainStopColorSign->getRGBA());
    tmpSettings.colorSettings.containerStopColor = MFXUtils::getRGBColor(myContainerStopColor->getRGBA());
    tmpSettings.colorSettings.containerStopColorSign = MFXUtils::getRGBColor(myContainerStopColorSign->getRGBA());
    tmpSettings.colorSettings.chargingStationColor = MFXUtils::getRGBColor(myChargingStationColor->getRGBA());
    tmpSettings.colorSettings.chargingStationColorSign = MFXUtils::getRGBColor(myChargingStationColorSign->getRGBA());
    if (mySettings->netedit) {
        tmpSettings.colorSettings.stopColor = MFXUtils::getRGBColor(myStopColor->getRGBA());
        tmpSettings.colorSettings.waypointColor = MFXUtils::getRGBColor(myWaypointColor->getRGBA());
        tmpSettings.colorSettings.vehicleTripColor = MFXUtils::getRGBColor(myVehicleTripsColor->getRGBA());
        tmpSettings.colorSettings.stopPersonColor = MFXUtils::getRGBColor(myStopPersonsColor->getRGBA());
        tmpSettings.colorSettings.personTripColor = MFXUtils::getRGBColor(myPersonTripColor->getRGBA());
        tmpSettings.colorSettings.walkColor = MFXUtils::getRGBColor(myWalkColor->getRGBA());
        tmpSettings.colorSettings.rideColor = MFXUtils::getRGBColor(myRideColor->getRGBA());
        tmpSettings.colorSettings.stopContainerColor = MFXUtils::getRGBColor(myStopContainersColor->getRGBA());
        tmpSettings.colorSettings.transportColor = MFXUtils::getRGBColor(myTransportColor->getRGBA());
        tmpSettings.colorSettings.transhipColor = MFXUtils::getRGBColor(myTranshipColor->getRGBA());

        tmpSettings.widthSettings.tripWidth = myTripWidth->getValue();
        tmpSettings.widthSettings.personTripWidth = myPersonTripWidth->getValue();
        tmpSettings.widthSettings.walkWidth = myWalkWidth->getValue();
        tmpSettings.widthSettings.rideWidth = myRideWidth->getValue();
        tmpSettings.widthSettings.transportWidth = myTransportWidth->getValue();
        tmpSettings.widthSettings.transhipWidth = myTranshipWidth->getValue();

        tmpSettings.colorSettings.selectionColor = MFXUtils::getRGBColor(mySelectionColor->getRGBA());
        tmpSettings.colorSettings.selectedEdgeColor = MFXUtils::getRGBColor(mySelectedEdgeColor->getRGBA());
        tmpSettings.colorSettings.selectedLaneColor = MFXUtils::getRGBColor(mySelectedLaneColor->getRGBA());
        tmpSettings.colorSettings.selectedConnectionColor = MFXUtils::getRGBColor(mySelectedConnectionColor->getRGBA());
        tmpSettings.colorSettings.selectedProhibitionColor = MFXUtils::getRGBColor(mySelectedProhibitionColor->getRGBA());
        tmpSettings.colorSettings.selectedCrossingColor = MFXUtils::getRGBColor(mySelectedCrossingColor->getRGBA());
        tmpSettings.colorSettings.selectedAdditionalColor = MFXUtils::getRGBColor(mySelectedAdditionalColor->getRGBA());
        tmpSettings.colorSettings.selectedRouteColor = MFXUtils::getRGBColor(mySelectedRouteColor->getRGBA());
        tmpSettings.colorSettings.selectedVehicleColor = MFXUtils::getRGBColor(mySelectedVehicleColor->getRGBA());
        tmpSettings.colorSettings.selectedPersonColor = MFXUtils::getRGBColor(mySelectedPersonColor->getRGBA());
        tmpSettings.colorSettings.selectedPersonPlanColor = MFXUtils::getRGBColor(mySelectedPersonPlanColor->getRGBA());
        tmpSettings.colorSettings.selectedEdgeDataColor = MFXUtils::getRGBColor(mySelectedEdgeDataColor->getRGBA());
    }

    tmpSettings.showGrid = (myShowGrid->getCheck() != FALSE);
    tmpSettings.gridXSize = (double) myGridXSizeDialer->getValue();
    tmpSettings.gridYSize = (double) myGridYSizeDialer->getValue();

    if (GUIVisualizationSettings::UseMesoSim) {
        tmpSettings.edgeColorer.setActive(myLaneEdgeColorMode->getCurrentItem());
        tmpSettings.edgeScaler.setActive(myLaneEdgeScaleMode->getCurrentItem());
    } else {
        tmpSettings.laneColorer.setActive(myLaneEdgeColorMode->getCurrentItem());
        tmpSettings.laneScaler.setActive(myLaneEdgeScaleMode->getCurrentItem());
    }
    tmpSettings.laneShowBorders = (myShowLaneBorders->getCheck() != FALSE);
    tmpSettings.showBikeMarkings = (myShowBikeMarkings->getCheck() != FALSE);
    tmpSettings.showLinkDecals = (myShowLaneDecals->getCheck() != FALSE);
    tmpSettings.realisticLinkRules = (myRealisticLinkRules->getCheck() != FALSE);
    tmpSettings.showLinkRules = (myShowLinkRules->getCheck() != FALSE);
    tmpSettings.showRails = (myShowRails->getCheck() != FALSE);
    tmpSettings.secondaryShape = (mySecondaryShape->getCheck() != FALSE);
    tmpSettings.edgeName = myEdgeNamePanel->getSettings();
    tmpSettings.internalEdgeName = myInternalEdgeNamePanel->getSettings();
    tmpSettings.cwaEdgeName = myCwaEdgeNamePanel->getSettings();
    tmpSettings.streetName = myStreetNamePanel->getSettings();
    tmpSettings.edgeValue = myEdgeValuePanel->getSettings();
    tmpSettings.edgeScaleValue = myEdgeScaleValuePanel->getSettings();
    tmpSettings.hideConnectors = (myHideMacroConnectors->getCheck() != FALSE);
    tmpSettings.showLaneDirection = (myShowLaneDirection->getCheck() != FALSE);
    tmpSettings.showSublanes = (myShowSublanes->getCheck() != FALSE);
    tmpSettings.spreadSuperposed = (mySpreadSuperposed->getCheck() != FALSE);
    if (sender == myParamKey) {
        if (tmpSettings.getLaneEdgeScheme().getName() == GUIVisualizationSettings::SCHEME_NAME_EDGE_PARAM_NUMERICAL) {
            tmpSettings.edgeParam = myParamKey->getText().text();
        } else if (tmpSettings.getLaneEdgeScheme().getName() == GUIVisualizationSettings::SCHEME_NAME_LANE_PARAM_NUMERICAL) {
            tmpSettings.laneParam = myParamKey->getText().text();
        } else if (tmpSettings.getLaneEdgeScheme().getName() == GUIVisualizationSettings::SCHEME_NAME_EDGEDATA_NUMERICAL) {
            tmpSettings.edgeData = myParamKey->getText().text();
        } else if (tmpSettings.getLaneEdgeScheme().getName() == GUIVisualizationSettings::SCHEME_NAME_EDGEDATA_LIVE) {
            tmpSettings.edgeData = myParamKey->getText().text();
        }
    } else if (sender == myScalingParamKey) {
        if (tmpSettings.getLaneEdgeScaleScheme().getName() == GUIVisualizationSettings::SCHEME_NAME_EDGEDATA_NUMERICAL) {
            tmpSettings.edgeDataScaling = myScalingParamKey->getText().text();
        }
    } else if (sender == myVehicleParamKey) {
        if (tmpSettings.vehicleColorer.getScheme().getName() == GUIVisualizationSettings::SCHEME_NAME_PARAM_NUMERICAL) {
            tmpSettings.vehicleParam = myVehicleParamKey->getText().text();
        }
    } else if (sender == myDataParamKey) {
        if (tmpSettings.dataColorer.getScheme().getName() == GUIVisualizationSettings::SCHEME_NAME_DATA_ATTRIBUTE_NUMERICAL) {
            tmpSettings.relDataAttr = myDataParamKey->getText().text();
        }
    } else if (sender == myVehicleTextPanel->myCheck) {
        updateVehicleParams();
    } else if (sender == myVehicleTextParamKey) {
        tmpSettings.vehicleTextParam = myVehicleTextParamKey->getText().text();
    } else if (sender == myPOITextPanel->myCheck) {
        updatePOIParams();
    } else if (sender == myPOITextParamKey) {
        tmpSettings.poiTextParam = myPOITextParamKey->getText().text();
    } else if (sender == myMeanDataID) {
        tmpSettings.edgeDataID = myMeanDataID->getText().text();
    }
    tmpSettings.edgeValueHideCheck = (myLaneColorRainbowCheck->getCheck() != FALSE);
    tmpSettings.edgeValueHideThreshold = myLaneColorRainbowThreshold->getValue();
    tmpSettings.edgeValueHideCheck2 = (myLaneColorRainbowCheck2->getCheck() != FALSE);
    tmpSettings.edgeValueHideThreshold2 = myLaneColorRainbowThreshold2->getValue();
    tmpSettings.laneWidthExaggeration = myLaneWidthUpscaleDialer->getValue();
    tmpSettings.laneMinSize = myLaneMinWidthDialer->getValue();

    tmpSettings.vehicleColorer.setActive(myVehicleColorMode->getCurrentItem());
    tmpSettings.vehicleScaler.setActive(myVehicleScaleMode->getCurrentItem());
    tmpSettings.vehicleQuality = myVehicleShapeDetail->getCurrentItem();
    tmpSettings.showBlinker = (myShowBlinker->getCheck() != FALSE);
    tmpSettings.drawMinGap = (myShowMinGap->getCheck() != FALSE);
    tmpSettings.drawBrakeGap = (myShowBrakeGap->getCheck() != FALSE);
    tmpSettings.showBTRange = (myShowBTRange->getCheck() != FALSE);
    tmpSettings.showRouteIndex = (myShowRouteIndex->getCheck() != FALSE);
    tmpSettings.scaleLength = (myScaleLength->getCheck() != FALSE);
    tmpSettings.drawReversed = (myDrawReversed->getCheck() != FALSE);
    tmpSettings.showParkingInfo = (myShowParkingInfo->getCheck() != FALSE);
    /*
    tmpSettings.drawLaneChangePreference = (myShowLaneChangePreference->getCheck() != FALSE);
    */
    tmpSettings.vehicleName = myVehicleNamePanel->getSettings();
    tmpSettings.vehicleValue = myVehicleValuePanel->getSettings();
    tmpSettings.vehicleScaleValue = myVehicleScaleValuePanel->getSettings();
    tmpSettings.vehicleText = myVehicleTextPanel->getSettings();
    tmpSettings.vehicleSize = myVehicleSizePanel->getSettings();

    tmpSettings.personColorer.setActive(myPersonColorMode->getCurrentItem());
    tmpSettings.personQuality = myPersonShapeDetail->getCurrentItem();
    tmpSettings.personName = myPersonNamePanel->getSettings();
    tmpSettings.personValue = myPersonValuePanel->getSettings();
    tmpSettings.personSize = myPersonSizePanel->getSettings();

    tmpSettings.containerColorer.setActive(myContainerColorMode->getCurrentItem());
    tmpSettings.containerQuality = myContainerShapeDetail->getCurrentItem();
    tmpSettings.containerName = myContainerNamePanel->getSettings();
    tmpSettings.containerSize = myContainerSizePanel->getSettings();

    tmpSettings.junctionColorer.setActive(myJunctionColorMode->getCurrentItem());
    tmpSettings.drawLinkTLIndex = myTLIndexPanel->getSettings();
    tmpSettings.drawLinkJunctionIndex = myJunctionIndexPanel->getSettings();
    tmpSettings.junctionID = myJunctionIDPanel->getSettings();
    tmpSettings.junctionName = myJunctionNamePanel->getSettings();
    tmpSettings.internalJunctionName = myInternalJunctionNamePanel->getSettings();
    tmpSettings.tlsPhaseIndex = myTLSPhaseIndexPanel->getSettings();
    tmpSettings.tlsPhaseName = myTLSPhaseNamePanel->getSettings();
    tmpSettings.junctionSize = myJunctionSizePanel->getSettings();

    tmpSettings.addName = myAddNamePanel->getSettings();
    tmpSettings.addFullName = myAddFullNamePanel->getSettings();
    tmpSettings.addSize = myAddSizePanel->getSettings();

    tmpSettings.poiColorer.setActive(myPOIColorMode->getCurrentItem());
    tmpSettings.poiDetail = myPoiDetail->getValue();
    tmpSettings.poiName = myPOINamePanel->getSettings();
    tmpSettings.poiType = myPOITypePanel->getSettings();
    tmpSettings.poiText = myPOITextPanel->getSettings();
    tmpSettings.poiSize = myPOISizePanel->getSettings();

    tmpSettings.polyColorer.setActive(myPolyColorMode->getCurrentItem());
    tmpSettings.polyName = myPolyNamePanel->getSettings();
    tmpSettings.polyType = myPolyTypePanel->getSettings();
    tmpSettings.polySize = myPolySizePanel->getSettings();

    if (mySettings->netedit) {
        tmpSettings.dataValue = myDataValuePanel->getSettings();
        tmpSettings.dataColorer.setActive(myDataColorMode->getCurrentItem());
        tmpSettings.dataValue = myDataValuePanel->getSettings();
        tmpSettings.tazRelWidthExaggeration = myTazRelationUpscaleDialer->getValue();
        tmpSettings.edgeRelWidthExaggeration = myEdgeRelationUpscaleDialer->getValue();
        tmpSettings.dataValueHideCheck = (myDataColorRainbowCheck->getCheck() != FALSE);
        tmpSettings.dataValueHideThreshold = myDataColorRainbowThreshold->getValue();
    }

    tmpSettings.showLane2Lane = (myShowLane2Lane->getCheck() != FALSE);
    tmpSettings.drawJunctionShape = (myDrawJunctionShape->getCheck() != FALSE);
    tmpSettings.drawCrossingsAndWalkingareas = (myDrawCrossingsAndWalkingAreas->getCheck() != FALSE);
    tmpSettings.dither = (myDither->getCheck() != FALSE);
    tmpSettings.fps = (myFPS->getCheck() != FALSE);
    tmpSettings.drawBoundaries = (myDrawBoundaries->getCheck() != FALSE);
    tmpSettings.forceDrawForPositionSelection = (myForceDrawForPositionSelection->getCheck() != FALSE);
    tmpSettings.forceDrawForRectangleSelection = (myForceDrawForRectangleSelection->getCheck() != FALSE);
    tmpSettings.disableDottedContours = (myDisableDottedContours->getCheck() != FALSE);
    tmpSettings.geometryIndices = myGeometryIndicesPanel->getSettings();
    tmpSettings.showSizeLegend = (myShowSizeLegend->getCheck() != FALSE);
    tmpSettings.showColorLegend = (myShowColorLegend->getCheck() != FALSE);
    tmpSettings.showVehicleColorLegend = (myShowVehicleColorLegend->getCheck() != FALSE);
    tmpSettings.show3DTLSDomes = (myShow3DTLSDomes->getCheck() != FALSE);
    tmpSettings.show3DTLSLinkMarkers = (myShow3DTLSLinkMarkers->getCheck() != FALSE);
    tmpSettings.show3DHeadUpDisplay = (myShow3DHeadUpDisplay->getCheck() != FALSE);
    tmpSettings.generate3DTLSModels = (myGenerate3DTLSModels->getCheck() != FALSE);
    const unsigned char lightFactor = (unsigned char)myLight3DFactor->getValue();
    tmpSettings.ambient3DLight.set(lightFactor / 2, lightFactor / 2, lightFactor / 2, 255);
    tmpSettings.diffuse3DLight.set(lightFactor, lightFactor, lightFactor, 255);
    tmpSettings.skyColor = MFXUtils::getRGBColor(mySkyColor->getRGBA());

    // lanes (colors)
    if (sender == myLaneColorRainbow) {
        myParent->buildColorRainbow(tmpSettings, tmpSettings.getLaneEdgeScheme(), tmpSettings.getLaneEdgeMode(), GLO_LANE,
                                    myLaneColorRainbowCheck->getCheck() != FALSE, myLaneColorRainbowThreshold->getValue(),
                                    myLaneColorRainbowCheck2->getCheck() != FALSE, myLaneColorRainbowThreshold2->getValue());
        doRebuildColorMatrices = true;
    } else if (sender == myJunctionColorRainbow) {
        myParent->buildColorRainbow(tmpSettings, tmpSettings.junctionColorer.getScheme(), tmpSettings.junctionColorer.getActive(), GLO_JUNCTION);
        doRebuildColorMatrices = true;
    } else if (sender == myDataColorRainbow) {
        myParent->buildColorRainbow(tmpSettings, tmpSettings.dataColorer.getScheme(), tmpSettings.dataColorer.getActive(), GLO_TAZRELDATA,
                                    myDataColorRainbowCheck->getCheck() != FALSE, myDataColorRainbowThreshold->getValue());
        doRebuildColorMatrices = true;
    }
    if (tmpSettings.getLaneEdgeMode() == prevLaneMode) {
        if (updateColorRanges(sender, myLaneColors.begin(), myLaneColors.end(),
                              myLaneThresholds.begin(), myLaneThresholds.end(), myLaneButtons.begin(),
                              tmpSettings.getLaneEdgeScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myLaneColorInterpolation) {
            tmpSettings.getLaneEdgeScheme().setInterpolated(myLaneColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // lanes (scaling)
    if (tmpSettings.getLaneEdgeScaleMode() == prevLaneScaleMode) {
        if (updateScaleRanges(sender, myLaneScales.begin(), myLaneScales.end(),
                              myLaneScaleThresholds.begin(), myLaneScaleThresholds.end(), myLaneScaleButtons.begin(),
                              tmpSettings.getLaneEdgeScaleScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myLaneScaleInterpolation) {
            tmpSettings.getLaneEdgeScaleScheme().setInterpolated(myLaneScaleInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // vehicles
    if (tmpSettings.vehicleColorer.getActive() == prevVehicleMode) {
        if (updateColorRanges(sender, myVehicleColors.begin(), myVehicleColors.end(),
                              myVehicleThresholds.begin(), myVehicleThresholds.end(), myVehicleButtons.begin(),
                              tmpSettings.vehicleColorer.getScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myVehicleColorInterpolation) {
            tmpSettings.vehicleColorer.getScheme().setInterpolated(myVehicleColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // vehicles (scaling)
    if (tmpSettings.vehicleScaler.getActive() == prevVehicleScaleMode) {
        if (updateScaleRanges(sender, myVehicleScales.begin(), myVehicleScales.end(),
                              myVehicleScaleThresholds.begin(), myVehicleScaleThresholds.end(), myVehicleScaleButtons.begin(),
                              tmpSettings.vehicleScaler.getScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myVehicleScaleInterpolation) {
            tmpSettings.vehicleScaler.getScheme().setInterpolated(myVehicleScaleInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // persons
    if (tmpSettings.personColorer.getActive() == prevPersonMode) {
        if (updateColorRanges(sender, myPersonColors.begin(), myPersonColors.end(),
                              myPersonThresholds.begin(), myPersonThresholds.end(), myPersonButtons.begin(),
                              tmpSettings.personColorer.getScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myPersonColorInterpolation) {
            tmpSettings.personColorer.getScheme().setInterpolated(myPersonColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // containers
    if (tmpSettings.containerColorer.getActive() == prevContainerMode) {
        if (updateColorRanges(sender, myContainerColors.begin(), myContainerColors.end(),
                              myContainerThresholds.begin(), myContainerThresholds.end(), myContainerButtons.begin(),
                              tmpSettings.containerColorer.getScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myContainerColorInterpolation) {
            tmpSettings.containerColorer.getScheme().setInterpolated(myContainerColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // junctions
    if (tmpSettings.junctionColorer.getActive() == prevJunctionMode) {
        if (updateColorRanges(sender, myJunctionColors.begin(), myJunctionColors.end(),
                              myJunctionThresholds.begin(), myJunctionThresholds.end(), myJunctionButtons.begin(),
                              tmpSettings.junctionColorer.getScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myJunctionColorInterpolation) {
            tmpSettings.junctionColorer.getScheme().setInterpolated(myJunctionColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // POIs
    if (tmpSettings.poiColorer.getActive() == prevPOIMode) {
        if (updateColorRanges(sender, myPOIColors.begin(), myPOIColors.end(),
                              myPOIThresholds.begin(), myPOIThresholds.end(), myPOIButtons.begin(),
                              tmpSettings.poiColorer.getScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myPOIColorInterpolation) {
            tmpSettings.poiColorer.getScheme().setInterpolated(myPOIColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // polygons
    if (tmpSettings.polyColorer.getActive() == prevPolyMode) {
        if (updateColorRanges(sender, myPolyColors.begin(), myPolyColors.end(),
                              myPolyThresholds.begin(), myPolyThresholds.end(), myPolyButtons.begin(),
                              tmpSettings.polyColorer.getScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myPolyColorInterpolation) {
            tmpSettings.polyColorer.getScheme().setInterpolated(myPolyColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // data
    if (tmpSettings.netedit) {
        if (tmpSettings.dataColorer.getActive() == prevDataMode) {
            if (updateColorRanges(sender, myDataColors.begin(), myDataColors.end(),
                                  myDataThresholds.begin(), myDataThresholds.end(), myDataButtons.begin(),
                                  tmpSettings.dataColorer.getScheme())) {
                doRebuildColorMatrices = true;
            }
            if (sender == myDataColorInterpolation) {
                tmpSettings.dataColorer.getScheme().setInterpolated(myDataColorInterpolation->getCheck() != FALSE);
                doRebuildColorMatrices = true;
            }
        } else {
            doRebuildColorMatrices = true;
        }
    }
    // openGL
    if (sender == myRecalculateBoundaries) {
        myParent->recalculateBoundaries();
    }

    if (tmpSettings == *mySettings) {
        return 1;
    }

    int index = mySchemeName->getCurrentItem();
    if (index < (int) gSchemeStorage.getNumInitialSettings()) {
        // one of the initial settings is modified
        // every time this happens we create a new scheme
        int suffix = 1;
        while (gSchemeStorage.contains("custom_" + toString(suffix))) {
            suffix++;
        }
        tmpSettings.name = "custom_" + toString(suffix);
        // the newly created settings must be entered in several places:
        // - the comboBox mySchemeName of this dialog
        // - the comboBox of the parent view (set as active)
        // - the comboBox of all other views (only append) XXX @todo
        index = mySchemeName->appendItem(tmpSettings.name.c_str());
        mySchemeName->setCurrentItem(index);
        myParent->getColoringSchemesCombo()->appendItem(tmpSettings.name.c_str());
    }
    myParent->getColoringSchemesCombo()->setCurrentItem(
        myParent->getColoringSchemesCombo()->findItem(tmpSettings.name.c_str()));
    gSchemeStorage.add(tmpSettings); // overwrites existing
    mySettings = &gSchemeStorage.get(tmpSettings.name);
    myParent->setColorScheme(tmpSettings.name);

    if (doRebuildColorMatrices) {
        rebuildColorMatrices(true);
    }
    myParent->handle(this, FXSEL(SEL_CHANGED, MID_SIMPLE_VIEW_COLORCHANGE), nullptr);
    myParent->forceRefresh();
    getApp()->forceRefresh();
    return 1;
}


void
GUIDialog_ViewSettings::loadSettings(const std::string& file) {
    GUISettingsHandler handler(file, true, mySettings->netedit);
    for (std::string settingsName : handler.addSettings(myParent)) {
        FXint index = mySchemeName->appendItem(settingsName.c_str());
        mySchemeName->setCurrentItem(index);
        mySettings = &gSchemeStorage.get(settingsName);
    }
    if (handler.hasDecals()) {
        myParent->getDecalsLockMutex().lock();
        myParent->getDecals() = handler.getDecals();
        myDecalsTable->fillTable();
        myParent->update();
        myParent->getDecalsLockMutex().unlock();
    }
    if (handler.getDelay() >= 0) {
        myParent->setDelay(handler.getDelay());
    }
    if (handler.getBreakpoints().size() > 0) {
        myParent->setBreakpoints(handler.getBreakpoints());
    }
    handler.applyViewport(myParent);
    rebuildColorMatrices(true);
}


void
GUIDialog_ViewSettings::saveDecals(OutputDevice& dev) const {
    for (const auto& decal : myParent->getDecals()) {
        // only save decals with non empty filename
        if (decal.filename.size() > 0) {
            // check if decal is a light
            const bool isLight = (decal.filename.substr(0, 5) == "light") && (decal.filename.length() == 6) && isdigit(decal.filename[5]);
            if (isLight) {
                dev.openTag(SUMO_TAG_VIEWSETTINGS_LIGHT);
                dev.writeAttr(SUMO_ATTR_INDEX, decal.filename.substr(5, 1));
            } else {
                dev.openTag(SUMO_TAG_VIEWSETTINGS_DECAL);
                dev.writeAttr("file", decal.filename);
                dev.writeAttr("screenRelative", decal.screenRelative);
            }
            dev.writeAttr(SUMO_ATTR_CENTER_X, decal.centerX);
            dev.writeAttr(SUMO_ATTR_CENTER_Y, decal.centerY);
            dev.writeAttr(SUMO_ATTR_CENTER_Z, decal.centerZ);
            dev.writeAttr(SUMO_ATTR_WIDTH, decal.width);
            dev.writeAttr(SUMO_ATTR_HEIGHT, decal.height);
            dev.writeAttr("altitude", decal.altitude);
            dev.writeAttr("rotation", decal.rot);
            dev.writeAttr("tilt", decal.tilt);
            dev.writeAttr("roll", decal.roll);
            dev.writeAttr(SUMO_ATTR_LAYER, decal.layer);
            dev.closeTag();
        }
    }
}


void
GUIDialog_ViewSettings::loadDecals(const std::string& file) {
    myParent->getDecalsLockMutex().lock();
    GUISettingsHandler handler(file);
    if (handler.hasDecals()) {
        myParent->getDecals() = handler.getDecals();
    }
    myDecalsTable->fillTable();
    myParent->update();
    myParent->getDecalsLockMutex().unlock();
}


long
GUIDialog_ViewSettings::onCmdSaveSetting(FXObject*, FXSelector, void* /*data*/) {
    int index = mySchemeName->getCurrentItem();
    if (index < (int) gSchemeStorage.getNumInitialSettings()) {
        return 1;
    }
    // get the name
    std::string name = "";
    while (name.length() == 0) {
        FXDialogBox dialog(this, TL("Enter a name"), GUIDesignViewSettingsDialog);
        FXVerticalFrame* content = new FXVerticalFrame(&dialog, GUIDesignViewSettingsVerticalFrame5);
        new FXLabel(content, TL("Please enter an alphanumeric name: "), nullptr, GUIDesignViewSettingsLabel2);
        FXTextField* text = new FXTextField(content, 40, &dialog, FXDialogBox::ID_ACCEPT, GUIDesignViewSettingsTextField1);
        new FXHorizontalSeparator(content, GUIDesignHorizontalSeparator);
        FXHorizontalFrame* buttons = new FXHorizontalFrame(content, GUIDesignViewSettingsHorizontalFrame3);
        new FXButton(buttons, TL("&OK"), nullptr, &dialog, FXDialogBox::ID_ACCEPT, GUIDesignViewSettingsButton4);
        new FXButton(buttons, TL("&Cancel"), nullptr, &dialog, FXDialogBox::ID_CANCEL, GUIDesignViewSettingsButton5);
        dialog.create();
        text->setFocus();
        if (!dialog.execute()) {
            return 1;
        }
        name = text->getText().text();
        for (int i = 0; i < (int)name.length(); ++i) {
            if (name[i] != '_' && (name[i] < 'a' || name[i] > 'z') && (name[i] < 'A' || name[i] > 'Z') && (name[i] < '0' || name[i] > '9')) {
                name = "";
                break;
            }
        }
    }
    GUIVisualizationSettings tmpSettings(mySettings->name, mySettings->netedit);
    tmpSettings.copy(*mySettings);
    tmpSettings.name = name;
    if (name == mySettings->name || StringUtils::startsWith(mySettings->name, "custom_")) {
        gSchemeStorage.remove(mySettings->name);
        myParent->getColoringSchemesCombo()->setItemText(index, name.c_str());
    } else {
        gSchemeStorage.get(mySettings->name).copy(myBackup);
        index = mySchemeName->appendItem(name.c_str());
        myParent->getColoringSchemesCombo()->appendItem(name.c_str());
        myParent->getColoringSchemesCombo()->setCurrentItem(
            myParent->getColoringSchemesCombo()->findItem(name.c_str()));
    }
    gSchemeStorage.add(tmpSettings);
    mySchemeName->setItemText(index, name.c_str());
    myParent->setColorScheme(name);
    mySettings = &gSchemeStorage.get(name);
    myBackup.copy(*mySettings);
    gSchemeStorage.writeSettings(getApp());
    return 1;
}


long
GUIDialog_ViewSettings::onUpdSaveSetting(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   mySchemeName->getCurrentItem() < (int) gSchemeStorage.getNumInitialSettings()
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdDeleteSetting(FXObject*, FXSelector, void* /*data*/) {
    int index = mySchemeName->getCurrentItem();
    if (index < (int) gSchemeStorage.getNumInitialSettings()) {
        return 1;
    }
    std::string name = mySchemeName->getItem(index).text();
    gSchemeStorage.remove(name);
    mySchemeName->removeItem(index);
    onCmdNameChange(nullptr, 0, (void*) mySchemeName->getItem(0).text());
    gSchemeStorage.writeSettings(getApp());
    return 1;
}


long
GUIDialog_ViewSettings::onUpdDeleteSetting(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   mySchemeName->getCurrentItem() < (int) gSchemeStorage.getNumInitialSettings()
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdExportSetting(FXObject*, FXSelector, void* /*data*/) {
    FXString file = MFXUtils::getFilename2Write(this, TL("Export view settings"), ".xml", GUIIconSubSys::getIcon(GUIIcon::SAVE), gCurrentFolder);
    if (file == "") {
        return 1;
    }
    try {
        OutputDevice& dev = OutputDevice::getDevice(file.text(), false);
        dev.openTag(SUMO_TAG_VIEWSETTINGS);
        if (myParent->is3DView()) {
            dev.writeAttr(SUMO_ATTR_TYPE, "osg");
        }
        mySettings->save(dev);
        if (mySaveViewPort->getCheck()) {
            myParent->getViewportEditor()->writeXML(dev);
        }
        if (mySaveDelay->getCheck()) {
            dev.openTag(SUMO_TAG_DELAY);
            dev.writeAttr(SUMO_ATTR_VALUE, myParent->getDelay());
            dev.closeTag();
        }
        if (mySaveDecals->getCheck()) {
            saveDecals(dev);
        }
        if (!mySettings->netedit && mySaveBreakpoints->getCheck()) {
            for (SUMOTime t : myParent->retrieveBreakpoints()) {
                dev.openTag(SUMO_TAG_BREAKPOINT);
                dev.writeAttr(SUMO_ATTR_TIME, time2string(t));
                dev.closeTag();
            }
        }
        dev.closeTag();
        dev.close();
    } catch (IOError& e) {
        FXMessageBox::error(this, MBOX_OK, TL("Storing failed!"), "%s", e.what());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onUpdExportSetting(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   (mySchemeName->getCurrentItem() < (int) gSchemeStorage.getNumInitialSettings()
                    && !mySaveViewPort->getCheck() && !mySaveDelay->getCheck() && !mySaveDecals->getCheck() && !mySaveBreakpoints->getCheck()) ?
                   FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdImportSetting(FXObject*, FXSelector, void* /*data*/) {
    FXFileDialog opendialog(this, TL("Import view settings"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml,*.xml.gz");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        loadSettings(opendialog.getFilename().text());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onCmdLoadDecal(FXObject*, FXSelector, void* /*data*/) {
    FXFileDialog opendialog(this, TL("Load Decals"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml,*.xml.gz");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        loadDecals(opendialog.getFilename().text());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onCmdLoadXMLDecals(FXObject*, FXSelector, void* /*data*/) {
    FXFileDialog opendialog(this, TL("Load Decals"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml,*.xml.gz");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        loadDecals(opendialog.getFilename().text());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onCmdSaveXMLDecals(FXObject*, FXSelector, void* /*data*/) {
    FXString file = MFXUtils::getFilename2Write(this, TL("Save Decals"), ".xml", GUIIconSubSys::getIcon(GUIIcon::EMPTY), gCurrentFolder);
    if (file == "") {
        return 1;
    }
    try {
        OutputDevice& dev = OutputDevice::getDevice(file.text());
        dev.openTag("decals");
        saveDecals(dev);
        dev.closeTag();
        dev.close();
    } catch (IOError& e) {
        FXMessageBox::error(myParent, MBOX_OK, TL("Storing failed!"), "%s", e.what());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onCmdClearDecals(FXObject*, FXSelector, void* /*data*/) {
    // lock decals mutex
    myParent->getDecalsLockMutex().lock();
    // clear decals
    myParent->getDecals().clear();
    // update view
    myParent->update();
    // fill table again
    myDecalsTable->fillTable();
    // unlock decals mutex
    myParent->getDecalsLockMutex().unlock();
    return 1;
}


long
GUIDialog_ViewSettings::onUpdImportSetting(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), ptr);
    return 1;
}


FXMatrix*
GUIDialog_ViewSettings::rebuildColorMatrix(FXVerticalFrame* frame,
        std::vector<FXColorWell*>& colors,
        std::vector<FXRealSpinner*>& thresholds,
        std::vector<FXButton*>& buttons,
        FXCheckButton* interpolation,
        GUIColorScheme& scheme) {
    MFXUtils::deleteChildren(frame);
    FXMatrix* m = new FXMatrix(frame, 4, GUIDesignViewSettingsMatrix4);
    colors.clear();
    thresholds.clear();
    buttons.clear();
    const bool fixed = scheme.isFixed();
    std::vector<RGBColor>::const_iterator colIt = scheme.getColors().begin();
    std::vector<double>::const_iterator threshIt = scheme.getThresholds().begin();
    std::vector<std::string>::const_iterator nameIt = scheme.getNames().begin();
    while (colIt != scheme.getColors().end()) {
        colors.push_back(new FXColorWell(m, MFXUtils::getFXColor(*colIt), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell1));
        if (fixed) {
            new FXLabel(m, nameIt->c_str());
            new FXLabel(m, "");
            new FXLabel(m, "");
        } else {
            const int dialerOptions = scheme.allowsNegativeValues() ? SPIN_NOMIN : 0;
            FXRealSpinner* threshDialer = new FXRealSpinner(m, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_THICK | FRAME_SUNKEN | LAYOUT_TOP | LAYOUT_CENTER_Y | SPIN_NOMAX | dialerOptions);
            threshDialer->setValue(*threshIt);
            thresholds.push_back(threshDialer);
            if (*threshIt == GUIVisualizationSettings::MISSING_DATA) {
                threshDialer->disable();
                threshDialer->hide();
                buttons.push_back(new FXButton(m, "", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
                buttons.back()->hide();
                buttons.push_back(new FXButton(m, TL("No Data"), nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
                buttons.back()->disable();
            } else {
                buttons.push_back(new FXButton(m, TL("Add"), nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
                buttons.push_back(new FXButton(m, TL("Remove"), nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
            }
        }
        colIt++;
        threshIt++;
        nameIt++;
    }
    interpolation->setCheck(scheme.isInterpolated());
    if (fixed) {
        interpolation->disable();
    } else {
        if (colors.size() > 1) {
            interpolation->enable();
            if (interpolation->getCheck() != FALSE) {
                thresholds.front()->enable();
            } else {
                thresholds.front()->disable();
            }
        } else {
            interpolation->disable();
            thresholds.front()->disable();
        }
    }
    return m;
}


FXMatrix*
GUIDialog_ViewSettings::rebuildScaleMatrix(FXVerticalFrame* frame,
        std::vector<FXRealSpinner*>& scales,
        std::vector<FXRealSpinner*>& thresholds,
        std::vector<FXButton*>& buttons,
        FXCheckButton* interpolation,
        GUIScaleScheme& scheme) {
    MFXUtils::deleteChildren(frame);
    FXMatrix* m = new FXMatrix(frame, 4, GUIDesignViewSettingsMatrix4);
    scales.clear();
    thresholds.clear();
    buttons.clear();
    const bool fixed = scheme.isFixed();
    std::vector<double>::const_iterator scaleIt = scheme.getColors().begin();
    std::vector<double>::const_iterator threshIt = scheme.getThresholds().begin();
    std::vector<std::string>::const_iterator nameIt = scheme.getNames().begin();
    while (scaleIt != scheme.getColors().end()) {
        FXRealSpinner* scaleDialer = new FXRealSpinner(m, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_THICK | FRAME_SUNKEN | LAYOUT_TOP | LAYOUT_CENTER_Y | SPIN_NOMAX);
        scaleDialer->setValue(*scaleIt);
        scales.push_back(scaleDialer);
        if (fixed) {
            new FXLabel(m, nameIt->c_str());
            new FXLabel(m, "");
            new FXLabel(m, "");
        } else {
            const int dialerOptions = scheme.allowsNegativeValues() ? SPIN_NOMIN : 0;
            FXRealSpinner* threshDialer = new FXRealSpinner(m, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_THICK | FRAME_SUNKEN | LAYOUT_TOP | LAYOUT_CENTER_Y | SPIN_NOMAX | dialerOptions);
            threshDialer->setValue(*threshIt);
            thresholds.push_back(threshDialer);
            if (*threshIt == GUIVisualizationSettings::MISSING_DATA) {
                threshDialer->disable();
                threshDialer->hide();
                buttons.push_back(new FXButton(m, "", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
                buttons.back()->hide();
                buttons.push_back(new FXButton(m, TL("No Data"), nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
                buttons.back()->disable();
            } else {
                buttons.push_back(new FXButton(m, TL("Add"), nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
                buttons.push_back(new FXButton(m, TL("Remove"), nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
            }
        }
        scaleIt++;
        threshIt++;
        nameIt++;
    }
    interpolation->setCheck(scheme.isInterpolated());
    if (fixed) {
        interpolation->disable();
    } else {
        if (scales.size() > 1) {
            interpolation->enable();
            if (interpolation->getCheck() != FALSE) {
                thresholds.front()->enable();
            } else {
                thresholds.front()->disable();
            }
        } else {
            interpolation->disable();
            thresholds.front()->disable();
        }
    }
    return m;
}


void
GUIDialog_ViewSettings::rebuildColorMatrices(bool doCreate) {
    FXMatrix* m = rebuildColorMatrix(myLaneColorSettingFrame, myLaneColors, myLaneThresholds, myLaneButtons, myLaneColorInterpolation, mySettings->getLaneEdgeScheme());
    if (doCreate) {
        m->create();
    }
    if (mySettings->getLaneEdgeScheme().isFixed()) {
        myLaneColorRainbow->disable();
    } else {
        myLaneColorRainbow->enable();
    }
    if (mySettings->junctionColorer.getScheme().isFixed()) {
        myJunctionColorRainbow->disable();
    } else {
        myJunctionColorRainbow->enable();
    }
    std::string activeSchemeName = myLaneEdgeColorMode->getText().text();
    std::string activeScaleSchemeName = myLaneEdgeScaleMode->getText().text();
    myParamKey->clearItems();
    myScalingParamKey->clearItems();
    myMeanDataID->clearItems();
    myMeanDataID->hide();
    if (activeSchemeName == GUIVisualizationSettings::SCHEME_NAME_EDGE_PARAM_NUMERICAL) {
        myParamKey->appendItem(mySettings->edgeParam.c_str());
        for (const std::string& attr : myParent->getEdgeLaneParamKeys(true)) {
            if (attr != mySettings->edgeParam) {
                myParamKey->appendItem(attr.c_str());
            }
        }
        myParamKey->enable();
    } else if (activeSchemeName == GUIVisualizationSettings::SCHEME_NAME_LANE_PARAM_NUMERICAL) {
        myParamKey->appendItem(mySettings->laneParam.c_str());
        for (const std::string& attr : myParent->getEdgeLaneParamKeys(false)) {
            if (attr != mySettings->laneParam) {
                myParamKey->appendItem(attr.c_str());
            }
        }
        myParamKey->enable();
    } else if (activeSchemeName == GUIVisualizationSettings::SCHEME_NAME_EDGEDATA_NUMERICAL) {
        myParamKey->appendItem(mySettings->edgeData.c_str());
        for (const std::string& attr : myParent->getEdgeDataAttrs()) {
            if (attr != mySettings->edgeData) {
                myParamKey->appendItem(attr.c_str());
            }
        }
        myParamKey->enable();
        myParamKey->setEditable(false);
    } else if (activeSchemeName == GUIVisualizationSettings::SCHEME_NAME_EDGEDATA_LIVE) {
        if (mySettings->edgeDataID != "") {
            myMeanDataID->appendItem(mySettings->edgeDataID.c_str());
        }
        for (const std::string& attr : myParent->getMeanDataIDs()) {
            if (attr != mySettings->edgeDataID) {
                myMeanDataID->appendItem(attr.c_str());
            }
        }
        if (myMeanDataID->getNumItems() > 0) {
            if (mySettings->edgeDataID == "") {
                mySettings->edgeDataID = myMeanDataID->getItemText(0).text();
            }
            myMeanDataID->enable();
            myMeanDataID->setEditable(false);
            myMeanDataID->show();
            myMeanDataID->setNumVisible(myMeanDataID->getNumItems());
            myParamKey->appendItem(mySettings->edgeData.c_str());
            for (const std::string& attr : myParent->getMeanDataAttrs(mySettings->edgeDataID)) {
                if (attr != mySettings->edgeData) {
                    myParamKey->appendItem(attr.c_str());
                }
            }
            myParamKey->enable();
            myParamKey->setEditable(false);
        }
    } else {
        myParamKey->disable();
    }

    if (activeScaleSchemeName == GUIVisualizationSettings::SCHEME_NAME_EDGEDATA_NUMERICAL) {
        myScalingParamKey->appendItem(mySettings->edgeDataScaling.c_str());
        for (const std::string& attr : myParent->getEdgeDataAttrs()) {
            if (attr != mySettings->edgeDataScaling) {
                myScalingParamKey->appendItem(attr.c_str());
            }
        }
        myScalingParamKey->enable();
        myScalingParamKey->setEditable(false);
    } else {
        myScalingParamKey->disable();
    }

    myParamKey->setNumVisible(myParamKey->getNumItems());
    myScalingParamKey->setNumVisible(myScalingParamKey->getNumItems());
    myLaneColorSettingFrame->getParent()->recalc();

    m = rebuildScaleMatrix(myLaneScaleSettingFrame, myLaneScales, myLaneScaleThresholds, myLaneScaleButtons, myLaneScaleInterpolation, mySettings->getLaneEdgeScaleScheme());
    if (doCreate) {
        m->create();
    }
    myLaneScaleSettingFrame->getParent()->recalc();

    m = rebuildColorMatrix(myVehicleColorSettingFrame, myVehicleColors, myVehicleThresholds, myVehicleButtons, myVehicleColorInterpolation, mySettings->vehicleColorer.getScheme());
    if (doCreate) {
        m->create();
    }
    activeSchemeName = myVehicleColorMode->getText().text();
    myVehicleParamKey->setEditable(true);
    if (activeSchemeName == GUIVisualizationSettings::SCHEME_NAME_PARAM_NUMERICAL) {
        updateVehicleParams();
        myVehicleParamKey->enable();
    } else {
        myVehicleParamKey->disable();
    }
    myVehicleColorSettingFrame->getParent()->recalc();

    m = rebuildScaleMatrix(myVehicleScaleSettingFrame, myVehicleScales, myVehicleScaleThresholds, myVehicleScaleButtons, myVehicleScaleInterpolation, mySettings->vehicleScaler.getScheme());
    if (doCreate) {
        m->create();
    }
    myVehicleScaleSettingFrame->getParent()->recalc();

    m = rebuildColorMatrix(myPersonColorSettingFrame, myPersonColors, myPersonThresholds, myPersonButtons, myPersonColorInterpolation, mySettings->personColorer.getScheme());
    if (doCreate) {
        m->create();
    }
    myPersonColorSettingFrame->getParent()->recalc();
    m = rebuildColorMatrix(myContainerColorSettingFrame, myContainerColors, myContainerThresholds, myContainerButtons, myContainerColorInterpolation, mySettings->containerColorer.getScheme());
    if (doCreate) {
        m->create();
    }
    myContainerColorSettingFrame->getParent()->recalc();
    m = rebuildColorMatrix(myJunctionColorSettingFrame, myJunctionColors, myJunctionThresholds, myJunctionButtons, myJunctionColorInterpolation, mySettings->junctionColorer.getScheme());
    if (doCreate) {
        m->create();
    }
    myJunctionColorSettingFrame->getParent()->recalc();
    // POIs
    m = rebuildColorMatrix(myPOIColorSettingFrame, myPOIColors, myPOIThresholds, myPOIButtons, myPOIColorInterpolation, mySettings->poiColorer.getScheme());
    if (doCreate) {
        m->create();
    }
    myPOIColorSettingFrame->getParent()->recalc();
    // polygons
    m = rebuildColorMatrix(myPolyColorSettingFrame, myPolyColors, myPolyThresholds, myPolyButtons, myPolyColorInterpolation, mySettings->polyColorer.getScheme());
    if (doCreate) {
        m->create();
    }
    myPolyColorSettingFrame->getParent()->recalc();

    // data
    if (mySettings->netedit) {
        m = rebuildColorMatrix(myDataColorSettingFrame, myDataColors, myDataThresholds, myDataButtons, myDataColorInterpolation, mySettings->dataColorer.getScheme());
        if (doCreate) {
            m->create();
        }
        activeSchemeName = myDataColorMode->getText().text();
        myDataParamKey->setEditable(true);
        if (activeSchemeName == GUIVisualizationSettings::SCHEME_NAME_DATA_ATTRIBUTE_NUMERICAL) {
            myDataParamKey->clearItems();
            myDataParamKey->appendItem(mySettings->relDataAttr.c_str());
            for (const std::string& attr : myParent->getRelDataAttrs()) {
                if (attr != mySettings->relDataAttr) {
                    myDataParamKey->appendItem(attr.c_str());
                }
            }
            myDataParamKey->enable();
        } else {
            myDataParamKey->disable();
        }
        myDataColorSettingFrame->getParent()->recalc();
    }

    layout();
    update();
}


void
GUIDialog_ViewSettings::updateVehicleParams() {
    myVehicleParamKey->clearItems();
    myVehicleTextParamKey->clearItems();
    myVehicleParamKey->appendItem(mySettings->vehicleParam.c_str());
    myVehicleTextParamKey->appendItem(mySettings->vehicleTextParam.c_str());
    for (const std::string& attr : myParent->getVehicleParamKeys(false)) {
        myVehicleParamKey->appendItem(attr.c_str());
        myVehicleTextParamKey->appendItem(attr.c_str());
    }
    myVehicleParamKey->setNumVisible(myVehicleParamKey->getNumItems());
    myVehicleTextParamKey->setNumVisible(myVehicleTextParamKey->getNumItems());
}


void
GUIDialog_ViewSettings::updatePOIParams() {
    myPOITextParamKey->clearItems();
    myPOITextParamKey->appendItem(mySettings->poiTextParam.c_str());
    for (const std::string& attr : myParent->getPOIParamKeys()) {
        myPOITextParamKey->appendItem(attr.c_str());
    }
    myPOITextParamKey->setNumVisible(myPOITextParamKey->getNumItems());
}


std::string
GUIDialog_ViewSettings::getCurrentScheme() const {
    return mySchemeName->getItem(mySchemeName->getCurrentItem()).text();
}


void
GUIDialog_ViewSettings::setCurrentScheme(const std::string& name) {
    if (name.c_str() == mySchemeName->getItemText(mySchemeName->getCurrentItem())) {
        return;
    }
    for (int i = 0; i < mySchemeName->getNumItems(); ++i) {
        if (name.c_str() == mySchemeName->getItemText(i)) {
            mySchemeName->setCurrentItem(i);
            onCmdNameChange(nullptr, 0, (void*)name.c_str());
            return;
        }
    }
}


GUIDialog_ViewSettings::NamePanel::NamePanel(
    FXMatrix* parent,
    GUIDialog_ViewSettings* target,
    const std::string& title,
    const GUIVisualizationTextSettings& settings) {
    myCheck = new FXCheckButton(parent, title.c_str(), target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myCheck->setCheck(settings.showText);
    myMatrix0 = new FXMatrix(parent, 2, GUIDesignViewSettingsMatrix5);
    mySelectedCheck = new FXCheckButton(myMatrix0, TL("Only for selected"), target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    mySelectedCheck->setCheck(settings.onlySelected);
    myConstSizeCheck = new FXCheckButton(myMatrix0, TL("constant text size"), target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myConstSizeCheck->setCheck(settings.constSize);
    FXMatrix* m1 = new FXMatrix(parent, 2, GUIDesignViewSettingsMatrix5);
    new FXLabel(m1, TL("Size"), nullptr, GUIDesignViewSettingsLabel1);
    mySizeDial = new FXRealSpinner(m1, 10, target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    mySizeDial->setRange(5, 1000);
    mySizeDial->setValue(settings.size);
    FXMatrix* m2 = new FXMatrix(parent, 4, GUIDesignViewSettingsMatrix5);
    new FXLabel(m2, TL("Color"), nullptr, GUIDesignViewSettingsLabel1);
    myColorWell = new FXColorWell(m2, MFXUtils::getFXColor(settings.color), target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell2);
    new FXLabel(m2, TL("Background"), nullptr, GUIDesignViewSettingsLabel1);
    myBGColorWell = new FXColorWell(m2, MFXUtils::getFXColor(settings.bgColor), target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell2);
}


GUIVisualizationTextSettings
GUIDialog_ViewSettings::NamePanel::getSettings() {
    return GUIVisualizationTextSettings(myCheck->getCheck() != FALSE,
                                        mySizeDial->getValue(),
                                        MFXUtils::getRGBColor(myColorWell->getRGBA()),
                                        MFXUtils::getRGBColor(myBGColorWell->getRGBA()),
                                        myConstSizeCheck->getCheck() != FALSE,
                                        mySelectedCheck->getCheck() != FALSE);
}


void
GUIDialog_ViewSettings::NamePanel::update(const GUIVisualizationTextSettings& settings) {
    myCheck->setCheck(settings.showText);
    mySizeDial->setValue(settings.size);
    myColorWell->setRGBA(MFXUtils::getFXColor(settings.color));
    myBGColorWell->setRGBA(MFXUtils::getFXColor(settings.bgColor));
    myConstSizeCheck->setCheck(settings.constSize);
    mySelectedCheck->setCheck(settings.onlySelected);
}


GUIDialog_ViewSettings::SizePanel::SizePanel(FXMatrix* parent, GUIDialog_ViewSettings* target,
        const GUIVisualizationSizeSettings& settings, GUIGlObjectType type):
    myDialogViewSettings(target),
    myType(type) {
    myCheck = new FXCheckButton(parent, TL("Draw with constant size when zoomed out"), this, MID_SIMPLE_VIEW_SIZECHANGE, GUIDesignCheckButtonViewSettings);
    myCheck->setCheck(settings.constantSize);
    myCheckSelected = new FXCheckButton(parent, TL("Only for selected"), this, MID_SIMPLE_VIEW_SIZECHANGE, GUIDesignCheckButtonViewSettings);
    myCheckSelected->setCheck(settings.constantSizeSelected);
    FXMatrix* m1 = new FXMatrix(parent, 2, GUIDesignViewSettingsMatrix5);
    new FXLabel(m1, TL("Minimum Size"), nullptr, GUIDesignViewSettingsLabel1);
    myMinSizeDial = new FXRealSpinner(m1, 10, this, MID_SIMPLE_VIEW_SIZECHANGE, GUIDesignViewSettingsSpinDial1);
    myMinSizeDial->setValue(settings.minSize);
    FXMatrix* m2 = new FXMatrix(parent, 2, GUIDesignViewSettingsMatrix5);
    new FXLabel(m2, TL("Exaggerate by"), nullptr, GUIDesignViewSettingsLabel1);
    myExaggerateDial = new FXRealSpinner(m2, 10, this, MID_SIMPLE_VIEW_SIZECHANGE, GUIDesignViewSettingsSpinDial2);
    myExaggerateDial->setRange(0, 10000);
    myExaggerateDial->setValue(settings.exaggeration);
}


GUIVisualizationSizeSettings
GUIDialog_ViewSettings::SizePanel::getSettings() {
    return GUIVisualizationSizeSettings(
               myMinSizeDial->getValue(), myExaggerateDial->getValue(),
               myCheck->getCheck() != FALSE,
               myCheckSelected->getCheck() != FALSE);
}


void
GUIDialog_ViewSettings::SizePanel::update(const GUIVisualizationSizeSettings& settings) {
    myCheck->setCheck(settings.constantSize);
    myCheckSelected->setCheck(settings.constantSizeSelected);
    myMinSizeDial->setValue(settings.minSize);
    myExaggerateDial->setValue(settings.exaggeration);
}


long
GUIDialog_ViewSettings::SizePanel::onCmdSizeChange(FXObject* obj, FXSelector sel, void* ptr) {
    // mark boundaries for recomputing
    gPostDrawing.recomputeBoundaries = myType;
    // continue as a normal change
    return myDialogViewSettings->onCmdColorChange(obj, sel, ptr);
}


void
GUIDialog_ViewSettings::buildHeader(FXVerticalFrame* contentFrame) {
    FXHorizontalFrame* horizontalFrame = new FXHorizontalFrame(contentFrame, GUIDesignViewSettingsHorizontalFrame1);
    mySchemeName = new FXComboBox(horizontalFrame, 20, this, MID_SIMPLE_VIEW_NAMECHANGE, GUIDesignViewSettingsComboBox1);
    for (const auto& name : gSchemeStorage.getNames()) {
        const int index = mySchemeName->appendItem(name.c_str());
        if (name == mySettings->name) {
            mySchemeName->setCurrentItem((FXint)index);
        }
    }
    mySchemeName->setNumVisible(5);

    new FXButton(horizontalFrame, (std::string("\t\t") + TL("Save the setting to registry")).c_str(), GUIIconSubSys::getIcon(GUIIcon::SAVE_DATABASE), this, MID_SIMPLE_VIEW_SAVE, GUIDesignButtonToolbar);
    new FXButton(horizontalFrame, (std::string("\t\t") + TL("Remove the setting from registry")).c_str(), GUIIconSubSys::getIcon(GUIIcon::REMOVEDB), this, MID_SIMPLE_VIEW_DELETE, GUIDesignButtonToolbar);
    new FXButton(horizontalFrame, (std::string("\t\t") + TL("Export setting to file")).c_str(), GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_SIMPLE_VIEW_EXPORT, GUIDesignButtonToolbar);
    new FXButton(horizontalFrame, (std::string("\t\t") + TL("Load setting from file")).c_str(), GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_SIMPLE_VIEW_IMPORT, GUIDesignButtonToolbar);

    new FXVerticalSeparator(horizontalFrame);
    new FXLabel(horizontalFrame, TL("Export includes:"), nullptr, GUIDesignViewSettingsLabel1);
    mySaveViewPort = new FXCheckButton(horizontalFrame, TL("Viewport"));
    mySaveDelay = new FXCheckButton(horizontalFrame, TL("Delay"));
    mySaveDecals = new FXCheckButton(horizontalFrame, TL("Decals"));
    mySaveBreakpoints = new FXCheckButton(horizontalFrame, TL("Breakpoints"));
    if (mySettings->netedit) {
        mySaveBreakpoints->disable();
    }
}


void
GUIDialog_ViewSettings::buildBackgroundFrame(FXTabBook* tabbook) {
    // tab for the background
    new FXTabItem(tabbook, TL("Background"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* matrixColor = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    new FXLabel(matrixColor, TL("Color"), nullptr, GUIDesignViewSettingsLabel1);
    myBackgroundColor = new FXColorWell(matrixColor, MFXUtils::getFXColor(mySettings->backgroundColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);

    FXVerticalFrame* verticalFrameDecals = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame3);
    new FXLabel(verticalFrameDecals, TL("Decals:"));
    myDecalsTable = new MFXDecalsTable(this, verticalFrameDecals);
    FXHorizontalFrame* horizontalFrameButtonsDecals = new FXHorizontalFrame(verticalFrameDecals, GUIDesignViewSettingsHorizontalFrame2);
    new FXButton(horizontalFrameButtonsDecals, TL("&Load XML Decals"), nullptr, this, MID_SIMPLE_VIEW_LOAD_DECALS_XML, GUIDesignViewSettingsButton1);
    new FXButton(horizontalFrameButtonsDecals, TL("&Save XML Decals"), nullptr, this, MID_SIMPLE_VIEW_SAVE_DECALS_XML, GUIDesignViewSettingsButton1);
    new FXButton(horizontalFrameButtonsDecals, TL("&Clear Decals"), nullptr, this, MID_SIMPLE_VIEW_CLEAR_DECALS, GUIDesignViewSettingsButton1);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* matrixGrid = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix1);
    myShowGrid = new FXCheckButton(matrixGrid, TL("Toggle grid"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myShowGrid->setCheck(mySettings->showGrid);
    new FXLabel(matrixGrid, "");
    FXMatrix* matrixGridX = new FXMatrix(matrixGrid, 2, GUIDesignViewSettingsMatrix2);
    new FXLabel(matrixGridX, TL("x-spacing"), nullptr, GUIDesignViewSettingsLabel1);
    myGridXSizeDialer = new FXRealSpinner(matrixGridX, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myGridXSizeDialer->setRange(1, 10000);
    myGridXSizeDialer->setValue(mySettings->gridXSize);
    FXMatrix* matrixGridY = new FXMatrix(matrixGrid, 2, GUIDesignViewSettingsMatrix2);
    new FXLabel(matrixGridY, TL("y-spacing"), nullptr, GUIDesignViewSettingsLabel1);
    myGridYSizeDialer = new FXRealSpinner(matrixGridY, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myGridYSizeDialer->setRange(1, 10000);
    myGridYSizeDialer->setValue(mySettings->gridYSize);
}


void
GUIDialog_ViewSettings::buildStreetsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Streets"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);
    //  ... color settings
    FXVerticalFrame* verticalFrameColor = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame6);
    FXMatrix* matrixColor = new FXMatrix(verticalFrameColor, 5, GUIDesignViewSettingsMatrix3);
    new FXLabel(matrixColor, TL("Color"), nullptr, GUIDesignViewSettingsLabel1);
    myLaneEdgeColorMode = new MFXComboBoxIcon(matrixColor, 30, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myLaneColorInterpolation = new FXCheckButton(matrixColor, TL("Interpolate"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myLaneColorSettingFrame = new FXVerticalFrame(verticalFrameColor, GUIDesignViewSettingsVerticalFrame4);
    myMeanDataID = new FXComboBox(matrixColor, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myMeanDataID->disable();
    myMeanDataID->hide();
    myMeanDataID->setEditable(false);
    myParamKey = new FXComboBox(matrixColor, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myParamKey->disable();
    myParamKey->setEditable(true);

    // rainbow settings
    FXMatrix* matrixRainbow = new FXMatrix(verticalFrameColor, 5, GUIDesignViewSettingsMatrix3);
    myLaneColorRainbow = new FXButton(matrixRainbow, TL("Recalibrate Rainbow"), nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE,
                                      (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT), 0, 0, 0, 0, 20, 20, 4, 4);
    myLaneColorRainbowCheck = new FXCheckButton(matrixRainbow, TL("hide below"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myLaneColorRainbowThreshold = new FXRealSpinner(matrixRainbow, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, REALSPIN_NOMIN | GUIDesignViewSettingsSpinDial2);
    myLaneColorRainbowThreshold->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    myLaneColorRainbowThreshold->setValue(mySettings->edgeValueHideThreshold);
    myLaneColorRainbowCheck2 = new FXCheckButton(matrixRainbow, TL("hide above"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myLaneColorRainbowThreshold2 = new FXRealSpinner(matrixRainbow, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, REALSPIN_NOMIN | GUIDesignViewSettingsSpinDial2);
    myLaneColorRainbowThreshold2->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    myLaneColorRainbowThreshold2->setValue(mySettings->edgeValueHideThreshold2);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);
    //  ... scale settings
    FXVerticalFrame* verticalFrameScale = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame6);
    FXMatrix* matrixScale = new FXMatrix(verticalFrameScale, 5, GUIDesignViewSettingsMatrix3);
    new FXLabel(matrixScale, TL("Scale width"), nullptr, GUIDesignViewSettingsLabel1);
    myLaneEdgeScaleMode = new MFXComboBoxIcon(matrixScale, 30, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myLaneScaleInterpolation = new FXCheckButton(matrixScale, TL("Interpolate"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myLaneScaleSettingFrame = new FXVerticalFrame(verticalFrameScale, GUIDesignViewSettingsVerticalFrame4);
    myScalingParamKey = new FXComboBox(matrixScale, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myScalingParamKey->disable();
    myScalingParamKey->setEditable(true);

    if (GUIVisualizationSettings::UseMesoSim) {
        mySettings->edgeColorer.fill(*myLaneEdgeColorMode);
        mySettings->edgeScaler.fill(*myLaneEdgeScaleMode);
        myLaneEdgeColorMode->setNumVisible((int)mySettings->edgeColorer.size());
        myLaneEdgeScaleMode->setNumVisible((int)mySettings->edgeScaler.size());
    } else {
        mySettings->laneColorer.fill(*myLaneEdgeColorMode);
        mySettings->laneScaler.fill(*myLaneEdgeScaleMode);
        myLaneEdgeColorMode->setNumVisible((int)mySettings->laneColorer.size());
        myLaneEdgeScaleMode->setNumVisible((int)mySettings->laneScaler.size());
    }

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);
    FXMatrix* matrixLanes = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix1);

    myShowBikeMarkings = new FXCheckButton(matrixLanes, TL("Show bike markings"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowBikeMarkings->setCheck(mySettings->showBikeMarkings);
    myShowLaneDecals = new FXCheckButton(matrixLanes, TL("Show turning arrows"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowLaneDecals->setCheck(mySettings->showLinkDecals);

    myShowLinkRules = new FXCheckButton(matrixLanes, TL("Show right-of-way rules"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowLinkRules->setCheck(mySettings->showLinkRules);
    myRealisticLinkRules = new FXCheckButton(matrixLanes, TL("Realistic stop line colors"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myRealisticLinkRules->setCheck(mySettings->realisticLinkRules);

    myShowLaneBorders = new FXCheckButton(matrixLanes, TL("Show lane borders"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowLaneBorders->setCheck(mySettings->laneShowBorders);
    myShowLaneDirection = new FXCheckButton(matrixLanes, TL("Show lane direction"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowLaneDirection->setCheck(mySettings->showLaneDirection);

    myHideMacroConnectors = new FXCheckButton(matrixLanes, TL("Hide macro connectors"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myHideMacroConnectors->setCheck(mySettings->hideConnectors);
    myShowSublanes = new FXCheckButton(matrixLanes, TL("Show sublanes"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowSublanes->setCheck(mySettings->showSublanes);

    myShowRails = new FXCheckButton(matrixLanes, TL("Show rails"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowRails->setCheck(mySettings->showRails);
    mySpreadSuperposed = new FXCheckButton(matrixLanes, (TL("Spread bidirectional railways/roads") + std::string("\t\t") + TL("Make both directional edges for a bidirectional railways or roads visible")).c_str(), this, MID_SIMPLE_VIEW_COLORCHANGE);
    mySpreadSuperposed->setCheck(mySettings->spreadSuperposed);

    mySecondaryShape = new FXCheckButton(matrixLanes, TL("Secondary shape"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    mySecondaryShape->setCheck(mySettings->secondaryShape);
    new FXLabel(matrixLanes, " ", nullptr, GUIDesignViewSettingsLabel1);

    FXMatrix* tmp0 = new FXMatrix(matrixLanes, 2, GUIDesignViewSettingsMatrix5);
    new FXLabel(tmp0, TL("Exaggerate width by"), nullptr, GUIDesignViewSettingsLabel1);
    myLaneWidthUpscaleDialer = new FXRealSpinner(tmp0, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
    myLaneWidthUpscaleDialer->setRange(0, 1000000);
    myLaneWidthUpscaleDialer->setValue(mySettings->laneWidthExaggeration);

    FXMatrix* tmp1 = new FXMatrix(matrixLanes, 2, GUIDesignViewSettingsMatrix5);
    new FXLabel(tmp1, TL("Minimum size"), nullptr, GUIDesignViewSettingsLabel1);
    myLaneMinWidthDialer = new FXRealSpinner(tmp1, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
    myLaneMinWidthDialer->setRange(0, 1000000);
    myLaneMinWidthDialer->setValue(mySettings->laneMinSize);

    // edge name
    myEdgeNamePanel = new NamePanel(matrixLanes, this, TL("Show edge id"), mySettings->edgeName);
    myStreetNamePanel = new NamePanel(matrixLanes, this, TL("Show street name"), mySettings->streetName);
    myEdgeValuePanel = new NamePanel(matrixLanes, this, TL("Show edge color value"), mySettings->edgeValue);
    myEdgeScaleValuePanel = new NamePanel(matrixLanes, this, TL("Show edge scale value"), mySettings->edgeScaleValue);
}


void
GUIDialog_ViewSettings::buildVehiclesFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Vehicles"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalframe = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* matrixShowAs = new FXMatrix(verticalframe, 2, GUIDesignViewSettingsMatrix3);
    new FXLabel(matrixShowAs, TL("Show As"), nullptr, GUIDesignViewSettingsLabel1);
    myVehicleShapeDetail = new MFXComboBoxIcon(matrixShowAs, 20, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myVehicleShapeDetail->appendIconItem(TL("'triangles'"));
    myVehicleShapeDetail->appendIconItem(TL("'boxes'"));
    myVehicleShapeDetail->appendIconItem(TL("'simple shapes'"));
    myVehicleShapeDetail->appendIconItem(TL("'raster images'"));
    myVehicleShapeDetail->appendIconItem(TL("'circles'"));
    myVehicleShapeDetail->setNumVisible(5);
    myVehicleShapeDetail->setCurrentItem(mySettings->vehicleQuality);

    new FXHorizontalSeparator(verticalframe, GUIDesignHorizontalSeparator);

    FXMatrix* matrixColor = new FXMatrix(verticalframe, 4, GUIDesignViewSettingsMatrix3);
    new FXLabel(matrixColor, TL("Color"), nullptr, GUIDesignViewSettingsLabel1);
    myVehicleColorMode = new MFXComboBoxIcon(matrixColor, 20, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->vehicleColorer.fill(*myVehicleColorMode);
    myVehicleColorMode->setNumVisible((int)mySettings->vehicleColorer.size());
    myVehicleColorInterpolation = new FXCheckButton(matrixColor, TL("Interpolate"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myVehicleParamKey = new FXComboBox(matrixColor, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myVehicleParamKey->setEditable(true);
    myVehicleParamKey->disable();

    myVehicleColorSettingFrame = new FXVerticalFrame(verticalframe, GUIDesignViewSettingsVerticalFrame4);
    new FXHorizontalSeparator(verticalframe, GUIDesignHorizontalSeparator);

    //  vehicle scale settings
    FXVerticalFrame* verticalFrameScale = new FXVerticalFrame(verticalframe, GUIDesignViewSettingsVerticalFrame6);
    FXMatrix* matrixScale = new FXMatrix(verticalFrameScale, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(matrixScale, TL("Scale size"), nullptr, GUIDesignViewSettingsLabel1);
    myVehicleScaleMode = new MFXComboBoxIcon(matrixScale, 30, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myVehicleScaleInterpolation = new FXCheckButton(matrixScale, TL("Interpolate"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myVehicleScaleSettingFrame = new FXVerticalFrame(verticalFrameScale, GUIDesignViewSettingsVerticalFrame4);
    mySettings->vehicleScaler.fill(*myVehicleScaleMode);
    myVehicleScaleMode->setNumVisible((int)mySettings->vehicleScaler.size());
    new FXHorizontalSeparator(verticalframe, GUIDesignHorizontalSeparator);

    FXMatrix* matrixVehicle = new FXMatrix(verticalframe, 2, GUIDesignMatrixViewSettings);
    myVehicleNamePanel = new NamePanel(matrixVehicle, this, TL("Show vehicle id"), mySettings->vehicleName);
    myVehicleValuePanel = new NamePanel(matrixVehicle, this, TL("Show vehicle color value"), mySettings->vehicleValue);
    myVehicleScaleValuePanel = new NamePanel(matrixVehicle, this, TL("Show vehicle scale value"), mySettings->vehicleScaleValue);
    myVehicleTextPanel = new NamePanel(matrixVehicle, this, TL("Show vehicle text param"), mySettings->vehicleText);
    myVehicleTextParamKey = new FXComboBox(myVehicleTextPanel->myMatrix0, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myVehicleTextParamKey->setEditable(true);

    //new FXHorizontalSeparator(verticalframe, GUIDesignHorizontalSeparator);

    FXMatrix* matrixShow = new FXMatrix(verticalframe, 2, GUIDesignMatrixViewSettings);
    myShowBlinker = new FXCheckButton(matrixShow, TL("Show blinker / brake lights"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowBlinker->setCheck(mySettings->showBlinker);
    myShowMinGap = new FXCheckButton(matrixShow, TL("Show minimum gap"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowMinGap->setCheck(mySettings->drawMinGap);
    myShowBrakeGap = new FXCheckButton(matrixShow, TL("Show brake gap"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowBrakeGap->setCheck(mySettings->drawBrakeGap);
    myShowBTRange = new FXCheckButton(matrixShow, TL("Show Bluetooth range"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowBTRange->setCheck(mySettings->showBTRange);
    myShowRouteIndex = new FXCheckButton(matrixShow, TL("Show route index"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowRouteIndex->setCheck(mySettings->showRouteIndex);
    myScaleLength = new FXCheckButton(matrixShow, TL("Scale length with geometry"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myScaleLength->setCheck(mySettings->scaleLength);
    myShowParkingInfo = new FXCheckButton(matrixShow, TL("Show parking info"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowParkingInfo->setCheck(mySettings->showParkingInfo);
    myDrawReversed = new FXCheckButton(matrixShow, TL("Draw reversed vehicles in reverse"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myDrawReversed->setCheck(mySettings->drawReversed);
    //new FXLabel(matrixShow, " ", nullptr, GUIDesignViewSettingsLabel1);
    //myShowLaneChangePreference = new FXCheckButton(matrixShow, TL("Show lane change preference"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    //myShowLaneChangePreference->setCheck(mySettings->drawLaneChangePreference);
    //tmpc = new FXCheckButton(matrixShow, TL("Show needed headway"), 0 ,0);
    //tmpc->disable();

    //new FXHorizontalSeparator(frame3, GUIDesignHorizontalSeparator);

    FXMatrix* matrixSize = new FXMatrix(verticalframe, 2, GUIDesignViewSettingsMatrix1);
    myVehicleSizePanel = new SizePanel(matrixSize, this, mySettings->vehicleSize, GLO_VEHICLE);
}


void
GUIDialog_ViewSettings::buildPersonsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Persons"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m101 = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix3);
    new FXLabel(m101, TL("Show As"), nullptr, GUIDesignViewSettingsLabel1);
    myPersonShapeDetail = new MFXComboBoxIcon(m101, 20, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myPersonShapeDetail->appendIconItem(TL("'triangles'"));
    myPersonShapeDetail->appendIconItem(TL("'circles'"));
    myPersonShapeDetail->appendIconItem(TL("'simple shapes'"));
    myPersonShapeDetail->appendIconItem(TL("'raster images'"));
    myPersonShapeDetail->setNumVisible(4);
    myPersonShapeDetail->setCurrentItem(mySettings->personQuality);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m102 = new FXMatrix(verticalFrame, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(m102, TL("Color"), nullptr, GUIDesignViewSettingsLabel1);
    myPersonColorMode = new MFXComboBoxIcon(m102, 20, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->personColorer.fill(*myPersonColorMode);
    myPersonColorMode->setNumVisible(mySettings->personColorer.size());
    myPersonColorInterpolation = new FXCheckButton(m102, TL("Interpolate"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);

    myPersonColorSettingFrame = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame4);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m103 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myPersonNamePanel = new NamePanel(m103, this, TL("Show person id"), mySettings->personName);
    myPersonValuePanel = new NamePanel(m103, this, TL("Show person color value"), mySettings->personValue);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m104 = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix1);
    myPersonSizePanel = new SizePanel(m104, this, mySettings->personSize, GLO_PERSON);
}


void
GUIDialog_ViewSettings::buildContainersFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Containers"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m101 = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix3);
    new FXLabel(m101, TL("Show As"), nullptr, GUIDesignViewSettingsLabel1);
    myContainerShapeDetail = new MFXComboBoxIcon(m101, 20, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myContainerShapeDetail->appendIconItem(TL("'triangles'"));
    myContainerShapeDetail->appendIconItem(TL("'boxes'"));
    myContainerShapeDetail->appendIconItem(TL("'simple shapes'"));
    myContainerShapeDetail->appendIconItem(TL("'raster images'"));
    myContainerShapeDetail->setNumVisible(4);
    myContainerShapeDetail->setCurrentItem(mySettings->containerQuality);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m102 = new FXMatrix(verticalFrame, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(m102, TL("Color"), nullptr, GUIDesignViewSettingsLabel1);
    myContainerColorMode = new MFXComboBoxIcon(m102, 20, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->containerColorer.fill(*myContainerColorMode);
    myContainerColorMode->setNumVisible(10);
    myContainerColorInterpolation = new FXCheckButton(m102, TL("Interpolate"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);

    myContainerColorSettingFrame = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame4);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m103 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myContainerNamePanel = new NamePanel(m103, this, TL("Show container id"), mySettings->containerName);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m104 = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix1);
    myContainerSizePanel = new SizePanel(m104, this, mySettings->containerSize, GLO_CONTAINER);
}


void
GUIDialog_ViewSettings::buildJunctionsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Junctions"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);
    FXMatrix* m41 = new FXMatrix(verticalFrame, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(m41, TL("Color"), nullptr, GUIDesignViewSettingsLabel1);
    myJunctionColorMode = new MFXComboBoxIcon(m41, 20, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->junctionColorer.fill(*myJunctionColorMode);
    myJunctionColorMode->setNumVisible(4);
    myJunctionColorInterpolation = new FXCheckButton(m41, TL("Interpolate"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);

    myJunctionColorSettingFrame = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame4);
    myJunctionColorRainbow = new FXButton(verticalFrame, TL("Recalibrate Rainbow"), nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE,
                                          (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT), 0, 0, 0, 0, 20, 20, 4, 4);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);
    FXMatrix* m42 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myJunctionSizePanel = new SizePanel(m42, this, mySettings->junctionSize, GLO_JUNCTION);
    myDrawJunctionShape = new FXCheckButton(m42, TL("Draw junction shape"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myDrawJunctionShape->setCheck(mySettings->drawJunctionShape);
    myDrawCrossingsAndWalkingAreas = new FXCheckButton(m42, TL("Draw crossings/walkingareas"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myDrawCrossingsAndWalkingAreas->setCheck(mySettings->drawCrossingsAndWalkingareas);
    myShowLane2Lane = new FXCheckButton(m42, TL("Show lane to lane connections"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowLane2Lane->setCheck(mySettings->showLane2Lane);
    new FXLabel(m42, " ", nullptr, GUIDesignViewSettingsLabel1);

    myTLIndexPanel = new NamePanel(m42, this, TL("Show link tls index"), mySettings->drawLinkTLIndex);
    myJunctionIndexPanel = new NamePanel(m42, this, TL("Show link junction index"), mySettings->drawLinkJunctionIndex);
    myJunctionIDPanel = new NamePanel(m42, this, TL("Show junction id"), mySettings->junctionID);
    myInternalJunctionNamePanel = new NamePanel(m42, this, TL("Show internal junction id"), mySettings->internalJunctionName);
    myInternalEdgeNamePanel = new NamePanel(m42, this, TL("Show internal edge id"), mySettings->internalEdgeName);
    myCwaEdgeNamePanel = new NamePanel(m42, this, TL("Show crossing and walkingarea id"), mySettings->cwaEdgeName);
    myTLSPhaseIndexPanel = new NamePanel(m42, this, TL("Show traffic light phase index"), mySettings->tlsPhaseIndex);
    myTLSPhaseNamePanel = new NamePanel(m42, this, TL("Show traffic light phase name"), mySettings->tlsPhaseName);
    myJunctionNamePanel = new NamePanel(m42, this, TL("Show junction name"), mySettings->junctionName);
}


void
GUIDialog_ViewSettings::buildAdditionalsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Additional"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);
    // IDs
    FXMatrix* matrixIDs = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myAddNamePanel = new NamePanel(matrixIDs, this, TL("Show object id"), mySettings->addName);
    myAddFullNamePanel = new NamePanel(matrixIDs, this, TL("Show full name"), mySettings->addFullName);
    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);
    //Sizes
    FXMatrix* matrixSizes = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myAddSizePanel = new SizePanel(matrixSizes, this, mySettings->addSize, GLO_ADDITIONALELEMENT);
    // color
    FXMatrix* matrixColor = new FXMatrix(verticalFrame, 3, GUIDesignMatrixViewSettings);
    new FXLabel(matrixColor, TL("StoppingPlace"), nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(matrixColor, TL("body"), nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(matrixColor, TL("sign"), nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(matrixColor, "busStops", nullptr, GUIDesignViewSettingsLabel1);
    myBusStopColor = new FXColorWell(matrixColor, MFXUtils::getFXColor(mySettings->colorSettings.busStopColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myBusStopColorSign = new FXColorWell(matrixColor, MFXUtils::getFXColor(mySettings->colorSettings.busStopColorSign), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(matrixColor, "trainStops", nullptr, GUIDesignViewSettingsLabel1);
    myTrainStopColor = new FXColorWell(matrixColor, MFXUtils::getFXColor(mySettings->colorSettings.trainStopColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myTrainStopColorSign = new FXColorWell(matrixColor, MFXUtils::getFXColor(mySettings->colorSettings.trainStopColorSign), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(matrixColor, "containerStops", nullptr, GUIDesignViewSettingsLabel1);
    myContainerStopColor = new FXColorWell(matrixColor, MFXUtils::getFXColor(mySettings->colorSettings.containerStopColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myContainerStopColorSign = new FXColorWell(matrixColor, MFXUtils::getFXColor(mySettings->colorSettings.containerStopColorSign), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(matrixColor, "chargingStations", nullptr, GUIDesignViewSettingsLabel1);
    myChargingStationColor = new FXColorWell(matrixColor, MFXUtils::getFXColor(mySettings->colorSettings.chargingStationColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myChargingStationColorSign = new FXColorWell(matrixColor, MFXUtils::getFXColor(mySettings->colorSettings.chargingStationColorSign), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
}


void
GUIDialog_ViewSettings::buildDemandFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Demand"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);
    // elements
    FXMatrix* demandMatrix = new FXMatrix(verticalFrame, 3, GUIDesignMatrixViewSettings);
    new FXLabel(demandMatrix, TL("element"), nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(demandMatrix, TL("color"), nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(demandMatrix, TL("width"), nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(demandMatrix, "trips", nullptr, GUIDesignViewSettingsLabel1);
    myVehicleTripsColor = new FXColorWell(demandMatrix, MFXUtils::getFXColor(mySettings->colorSettings.vehicleTripColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myTripWidth = new FXRealSpinner(demandMatrix, 10, target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myTripWidth->setValue(mySettings->widthSettings.personTripWidth);
    new FXLabel(demandMatrix, "personTrips", nullptr, GUIDesignViewSettingsLabel1);
    myPersonTripColor = new FXColorWell(demandMatrix, MFXUtils::getFXColor(mySettings->colorSettings.personTripColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myPersonTripWidth = new FXRealSpinner(demandMatrix, 10, target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myPersonTripWidth->setValue(mySettings->widthSettings.personTripWidth);
    new FXLabel(demandMatrix, "walks", nullptr, GUIDesignViewSettingsLabel1);
    myWalkColor = new FXColorWell(demandMatrix, MFXUtils::getFXColor(mySettings->colorSettings.walkColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myWalkWidth = new FXRealSpinner(demandMatrix, 10, target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myWalkWidth->setValue(mySettings->widthSettings.walkWidth);
    new FXLabel(demandMatrix, "rides", nullptr, GUIDesignViewSettingsLabel1);
    myRideColor = new FXColorWell(demandMatrix, MFXUtils::getFXColor(mySettings->colorSettings.rideColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myRideWidth = new FXRealSpinner(demandMatrix, 10, target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myRideWidth->setValue(mySettings->widthSettings.rideWidth);
    new FXLabel(demandMatrix, "transport", nullptr, GUIDesignViewSettingsLabel1);
    myTransportColor = new FXColorWell(demandMatrix, MFXUtils::getFXColor(mySettings->colorSettings.transportColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myTransportWidth = new FXRealSpinner(demandMatrix, 10, target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myTransportWidth->setValue(mySettings->widthSettings.transportWidth);
    new FXLabel(demandMatrix, "tranship", nullptr, GUIDesignViewSettingsLabel1);
    myTranshipColor = new FXColorWell(demandMatrix, MFXUtils::getFXColor(mySettings->colorSettings.transhipColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myTranshipWidth = new FXRealSpinner(demandMatrix, 10, target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myTranshipWidth->setValue(mySettings->widthSettings.transhipWidth);
    // stops
    FXMatrix* stopMatrix = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    new FXLabel(stopMatrix, "stop", nullptr, GUIDesignViewSettingsLabel1);
    myStopColor = new FXColorWell(stopMatrix, MFXUtils::getFXColor(mySettings->colorSettings.stopColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(stopMatrix, "waypoint", nullptr, GUIDesignViewSettingsLabel1);
    myWaypointColor = new FXColorWell(stopMatrix, MFXUtils::getFXColor(mySettings->colorSettings.waypointColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(stopMatrix, "stop (persons)", nullptr, GUIDesignViewSettingsLabel1);
    myStopPersonsColor = new FXColorWell(stopMatrix, MFXUtils::getFXColor(mySettings->colorSettings.stopPersonColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(stopMatrix, "stop (containers)", nullptr, GUIDesignViewSettingsLabel1);
    myStopContainersColor = new FXColorWell(stopMatrix, MFXUtils::getFXColor(mySettings->colorSettings.stopContainerColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
}


void
GUIDialog_ViewSettings::buildPOIsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("POIs"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m63 = new FXMatrix(verticalFrame, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(m63, TL("Color"), nullptr, GUIDesignViewSettingsLabel1);
    myPOIColorMode = new MFXComboBoxIcon(m63, 20, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->poiColorer.fill(*myPOIColorMode);
    myPOIColorMode->setNumVisible(3);
    myPOIColorInterpolation = new FXCheckButton(m63, TL("Interpolate"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myPOIColorSettingFrame = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame4);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m61 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);

    new FXLabel(m61, TL("POI detail"), nullptr, GUIDesignViewSettingsLabel1);
    myPoiDetail = new FXSpinner(m61, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
    myPoiDetail->setRange(3, 100);
    myPoiDetail->setValue(mySettings->poiDetail);

    myPOINamePanel = new NamePanel(m61, this, TL("Show poi id"), mySettings->poiName);
    myPOITypePanel = new NamePanel(m61, this, TL("Show poi type"), mySettings->poiType);
    myPOITextPanel = new NamePanel(m61, this, TL("Show poi text param"), mySettings->poiText);
    myPOITextParamKey = new FXComboBox(myPOITextPanel->myMatrix0, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myPOITextParamKey->setEditable(true);
    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m62 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myPOISizePanel = new SizePanel(m62, this, mySettings->poiSize, GLO_POI);
}


void
GUIDialog_ViewSettings::buildPolygonsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Polygons"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m63 = new FXMatrix(verticalFrame, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(m63, TL("Color"), nullptr, GUIDesignViewSettingsLabel1);
    myPolyColorMode = new MFXComboBoxIcon(m63, 20, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->polyColorer.fill(*myPolyColorMode);
    myPolyColorMode->setNumVisible(mySettings->polyColorer.size());
    myPolyColorInterpolation = new FXCheckButton(m63, TL("Interpolate"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myPolyColorSettingFrame = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame4);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m91 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myPolyNamePanel = new NamePanel(m91, this, TL("Show polygon id"), mySettings->polyName);
    myPolyTypePanel = new NamePanel(m91, this, TL("Show polygon types"), mySettings->polyType);
    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    myPolySizePanel = new SizePanel(m91, this, mySettings->polySize, GLO_POLYGON);
}


void
GUIDialog_ViewSettings::buildSelectionFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Selection"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m102 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    new FXLabel(m102, TL("Default Selection Color"), nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(m102, "", nullptr, GUIDesignViewSettingsLabel1);

    new FXLabel(m102, TL("Miscellaneous"), nullptr, GUIDesignViewSettingsLabel1);
    mySelectionColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectionColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "Edge", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedEdgeColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedEdgeColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "Lane Edge", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedLaneColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedLaneColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "Connection", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedConnectionColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedConnectionColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "Prohibition", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedProhibitionColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedProhibitionColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "Crossing", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedCrossingColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedCrossingColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "Additional", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedAdditionalColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedAdditionalColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "Route", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedRouteColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedRouteColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "Vehicle", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedVehicleColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedVehicleColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "Person", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedPersonColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedPersonColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "PersonPlan", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedPersonPlanColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedPersonPlanColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(m102, "edgeData", nullptr, GUIDesignViewSettingsLabel1);
    mySelectedEdgeDataColor = new FXColorWell(m102, MFXUtils::getFXColor(mySettings->colorSettings.selectedEdgeDataColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
}


void
GUIDialog_ViewSettings::buildDataFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Data"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);
    //  ... color settings
    FXVerticalFrame* verticalFrame2 = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame6);
    FXMatrix* m111 = new FXMatrix(verticalFrame2, 4, GUIDesignViewSettingsMatrix3);
    new FXLabel(m111, TL("Color"), nullptr, GUIDesignViewSettingsLabel1);
    myDataColorMode = new MFXComboBoxIcon(m111, 30, true, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myDataColorInterpolation = new FXCheckButton(m111, TL("Interpolate"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myDataColorSettingFrame = new FXVerticalFrame(verticalFrame2, GUIDesignViewSettingsVerticalFrame4);
    myDataParamKey = new FXComboBox(m111, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myDataParamKey->disable();
    myDataParamKey->setEditable(true);
    mySettings->dataColorer.fill(*myDataColorMode);
    myDataColorMode->setNumVisible((int)mySettings->dataColorer.size());

    // rainbow settings
    FXMatrix* m113 = new FXMatrix(verticalFrame2, 3, GUIDesignViewSettingsMatrix3);
    myDataColorRainbow = new FXButton(m113, TL("Recalibrate Rainbow"), nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE,
                                      (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT), 0, 0, 0, 0, 20, 20, 4, 4);
    myDataColorRainbowCheck = new FXCheckButton(m113, TL("hide below threshold"), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myDataColorRainbowThreshold = new FXRealSpinner(m113, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, REALSPIN_NOMIN | GUIDesignViewSettingsSpinDial2);
    myDataColorRainbowThreshold->setRange(-100000000, 100000000);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);
    FXMatrix* m112 = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix1);

    new FXLabel(m112, TL("Exaggerate edgeRelation width by"), nullptr, GUIDesignViewSettingsLabel1);
    myEdgeRelationUpscaleDialer = new FXRealSpinner(m112, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
    myEdgeRelationUpscaleDialer->setRange(0, 1000000);
    myEdgeRelationUpscaleDialer->setValue(mySettings->edgeRelWidthExaggeration);

    new FXLabel(m112, TL("Exaggerate tazRelation width by"), nullptr, GUIDesignViewSettingsLabel1);
    myTazRelationUpscaleDialer = new FXRealSpinner(m112, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
    myTazRelationUpscaleDialer->setRange(0, 1000000);
    myTazRelationUpscaleDialer->setValue(mySettings->tazRelWidthExaggeration);

    // text decoration
    myDataValuePanel = new NamePanel(m112, this, TL("Show data color value"), mySettings->dataValue);
}


void
GUIDialog_ViewSettings::buildLegendFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, TL("Legend"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m72 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myShowSizeLegend = new FXCheckButton(m72, TL("Show Size Legend"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowSizeLegend->setCheck(mySettings->showSizeLegend);
    new FXLabel(m72, "");
    myShowColorLegend = new FXCheckButton(m72, TL("Show Edge Color Legend"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowColorLegend->setCheck(mySettings->showColorLegend);
    new FXLabel(m72, "");
    myShowVehicleColorLegend = new FXCheckButton(m72, TL("Show Vehicle Color Legend"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowVehicleColorLegend->setCheck(mySettings->showVehicleColorLegend);
    new FXLabel(m72, "");
}


void
GUIDialog_ViewSettings::buildOpenGLFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "openGL", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m82 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myDither = new FXCheckButton(m82, TL("Dither"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myDither->setCheck(mySettings->dither);
    FXMatrix* m83 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myFPS = new FXCheckButton(m83, "FPS", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myFPS->setCheck(mySettings->fps);
    FXMatrix* m84 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myDrawBoundaries = new FXCheckButton(m84, TL("Draw boundaries"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myDrawBoundaries->setCheck(mySettings->drawBoundaries);
    FXMatrix* m85 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myForceDrawForPositionSelection = new FXCheckButton(m85, TL("Force draw for position selection"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myForceDrawForPositionSelection->setCheck(mySettings->forceDrawForPositionSelection);
    FXMatrix* m86 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myForceDrawForRectangleSelection = new FXCheckButton(m86, TL("Force draw for rectangle selection"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myForceDrawForRectangleSelection->setCheck(mySettings->forceDrawForRectangleSelection);
    FXMatrix* m87 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myDisableDottedContours = new FXCheckButton(m87, TL("Disable dotted contours during selection/deletion"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myDisableDottedContours->setCheck(mySettings->disableDottedContours);
    FXMatrix* m88 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myRecalculateBoundaries = new FXButton(m88, TL("Recalculate boundaries"), nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE,
                                           (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT), 0, 0, 0, 0, 20, 20, 4, 4);
    FXMatrix* m89 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myGeometryIndicesPanel = new NamePanel(m89, this, TL("Show geometry point indices"), mySettings->geometryIndices);
}


void
GUIDialog_ViewSettings::build3DFrame(FXTabBook* tabbook) {
    myFrame3D = new FXTabItem(tabbook, TL("3D view"), nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m82 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myShow3DTLSLinkMarkers = new FXCheckButton(m82, TL("Show TLS link markers"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShow3DTLSLinkMarkers->setCheck(mySettings->show3DTLSLinkMarkers);
    //FXMatrix* m83 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myShow3DTLSDomes = new FXCheckButton(m82, TL("Show domes around TLS models from decals"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShow3DTLSDomes->setCheck(mySettings->show3DTLSDomes);
    //FXMatrix* m84 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myGenerate3DTLSModels = new FXCheckButton(m82, TL("Show auto-generated TLS models"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myGenerate3DTLSModels->setCheck(mySettings->generate3DTLSModels);
    myShow3DHeadUpDisplay = new FXCheckButton(m82, TL("Show head-up display"), this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShow3DHeadUpDisplay->setCheck(mySettings->show3DHeadUpDisplay);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m2 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    new FXLabel(m2, TL("Sun brightness"), nullptr, GUIDesignViewSettingsLabel1);
    myLight3DFactor = new FXSpinner(m2, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myLight3DFactor->setRange(0, 255);
    myLight3DFactor->setValue(mySettings->diffuse3DLight.red());
    /*
    new FXLabel(m2, "Ambient", nullptr, GUIDesignViewSettingsLabel1);
    myAmbient3DLight = new FXColorWell(m2, MFXUtils::getFXColor(mySettings->ambient3DLight), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myAmbient3DLight->setOpaqueOnly(true);
    new FXLabel(m2, "Diffuse", nullptr, GUIDesignViewSettingsLabel1);
    myDiffuse3DLight = new FXColorWell(m2, MFXUtils::getFXColor(mySettings->diffuse3DLight), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    myDiffuse3DLight->setOpaqueOnly(true);
    */
    new FXLabel(m2, TL("Sky color"), nullptr, GUIDesignViewSettingsLabel1);
    mySkyColor = new FXColorWell(m2, MFXUtils::getFXColor(mySettings->skyColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    mySkyColor->setOpaqueOnly(true);

#ifdef HAVE_OSG
    UNUSED_PARAMETER(myFrame3D);
#else
    myFrame3D->disable();
#endif
}


void
GUIDialog_ViewSettings::buildButtons(FXVerticalFrame* contentFrame) {
    FXHorizontalFrame* horizontalFrameButtons = new FXHorizontalFrame(contentFrame, GUIDesignViewSettingsHorizontalFrame2);
    FXButton* OK = new FXButton(horizontalFrameButtons, TL("&OK"), nullptr, this, MID_SETTINGS_OK, GUIDesignViewSettingsButton2);
    new FXButton(horizontalFrameButtons, TL("&Cancel"), nullptr, this, MID_SETTINGS_CANCEL, GUIDesignViewSettingsButton3);
    OK->setFocus();
}

/****************************************************************************/
