/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @date    Wed, 21. Dec 2005
///
// The dialog to change the view (gui) settings.
/****************************************************************************/
#include <config.h>

#include <fstream>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/MFXIconComboBox.h>
#include <utils/foxtools/MFXAddEditTypedTable.h>
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
#include "GUIDialog_EditViewport.h"
#include "GUIDialog_ViewSettings.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_ViewSettings) GUIDialog_ViewSettingsMap[] = {
    FXMAPFUNC(SEL_CHANGED,  MID_SIMPLE_VIEW_COLORCHANGE,            GUIDialog_ViewSettings::onCmdColorChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_COLORCHANGE,            GUIDialog_ViewSettings::onCmdColorChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_NAMECHANGE,             GUIDialog_ViewSettings::onCmdNameChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_OK,                        GUIDialog_ViewSettings::onCmdOk),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_CANCEL,                    GUIDialog_ViewSettings::onCmdCancel),
    FXMAPFUNC(SEL_CHANGED,  MFXAddEditTypedTable::ID_TEXT_CHANGED,  GUIDialog_ViewSettings::onCmdEditTable),

    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_SAVE,   GUIDialog_ViewSettings::onCmdSaveSetting),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMPLE_VIEW_SAVE,   GUIDialog_ViewSettings::onUpdSaveSetting),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_DELETE, GUIDialog_ViewSettings::onCmdDeleteSetting),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMPLE_VIEW_DELETE, GUIDialog_ViewSettings::onUpdDeleteSetting),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_EXPORT, GUIDialog_ViewSettings::onCmdExportSetting),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMPLE_VIEW_EXPORT, GUIDialog_ViewSettings::onUpdExportSetting),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_IMPORT, GUIDialog_ViewSettings::onCmdImportSetting),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMPLE_VIEW_IMPORT, GUIDialog_ViewSettings::onUpdImportSetting),

    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_LOAD_DECALS,    GUIDialog_ViewSettings::onCmdLoadDecals),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_SAVE_DECALS,    GUIDialog_ViewSettings::onCmdSaveDecals),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_CLEAR_DECALS,   GUIDialog_ViewSettings::onCmdClearDecals),
};


FXIMPLEMENT(GUIDialog_ViewSettings, FXDialogBox, GUIDialog_ViewSettingsMap, ARRAYNUMBER(GUIDialog_ViewSettingsMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_ViewSettings::GUIDialog_ViewSettings(GUISUMOAbstractView* parent, GUIVisualizationSettings* settings, std::vector<GUISUMOAbstractView::Decal>* decals, FXMutex* decalsLock) :
    FXDialogBox(parent, "View Settings", GUIDesignViewSettingsMainDialog),
    myParent(parent), mySettings(settings), myBackup(*settings),
    myDecals(decals), myDecalsLock(decalsLock), myDecalsTable(nullptr),
    myDataValuePanel(nullptr) {
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
    // build selection frame (only in NETEDIT)
    if (mySettings->netedit) {
        buildSelectionFrame(tabbook);
    }
    // build data frame (only in NETEDIT)
    if (mySettings->netedit) {
        buildDataFrame(tabbook);
    }
    // build legend frame
    buildLegendFrame(tabbook);
    // build openGL frame
    buildOpenGLFrame(tabbook);
    // build buttons
    buildButtons(contentFrame);
    // rebuild color matrix
    rebuildColorMatrices(false);
    setIcon(GUIIconSubSys::getIcon(GUIIcon::EMPTY));
    loadWindowSize();
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
    delete myJunctionIndexPanel;
    delete myTLIndexPanel;
    delete myJunctionIDPanel;
    delete myJunctionNamePanel;
    delete myVehicleNamePanel;
    delete myVehicleValuePanel;
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
    FXDialogBox::show();
}


void
GUIDialog_ViewSettings::setCurrent(GUIVisualizationSettings* settings) {
    mySettings = settings;
    myBackup = (*settings);
    onCmdNameChange(nullptr, 0, nullptr);
}


long
GUIDialog_ViewSettings::onCmdOk(FXObject*, FXSelector, void*) {
    saveWindowSize();
    hide();
    return 1;
}


long
GUIDialog_ViewSettings::onCmdCancel(FXObject*, FXSelector, void*) {
    saveWindowSize();
    hide();
    (*mySettings) = myBackup;
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
        myBackup = gSchemeStorage.get(dataS.text());
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
        myStopsColor->setRGBA(MFXUtils::getFXColor(mySettings->colorSettings.stopColor));
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
    myEdgeNamePanel->update(mySettings->edgeName);
    myInternalEdgeNamePanel->update(mySettings->internalEdgeName);
    myCwaEdgeNamePanel->update(mySettings->cwaEdgeName);
    myStreetNamePanel->update(mySettings->streetName);
    myEdgeValuePanel->update(mySettings->edgeValue);
    myHideMacroConnectors->setCheck(mySettings->hideConnectors);
    myShowLaneDirection->setCheck(mySettings->showLaneDirection);
    myShowSublanes->setCheck(mySettings->showSublanes);
    mySpreadSuperposed->setCheck(mySettings->spreadSuperposed);
    myLaneColorRainbowCheck->setCheck(mySettings->edgeValueHideCheck);
    myLaneColorRainbowThreshold->setValue(mySettings->edgeValueHideThreshold);
    myLaneWidthUpscaleDialer->setValue(mySettings->laneWidthExaggeration);
    myLaneMinWidthDialer->setValue(mySettings->laneMinSize);

    myVehicleColorMode->setCurrentItem((FXint) mySettings->vehicleColorer.getActive());
    myVehicleShapeDetail->setCurrentItem(mySettings->vehicleQuality);
    myShowBlinker->setCheck(mySettings->showBlinker);
    myShowMinGap->setCheck(mySettings->drawMinGap);
    myShowBrakeGap->setCheck(mySettings->drawBrakeGap);
    myShowBTRange->setCheck(mySettings->showBTRange);
    myShowRouteIndex->setCheck(mySettings->showRouteIndex);
    myScaleLength->setCheck(mySettings->scaleLength);
    myShowParkingInfo->setCheck(mySettings->showParkingInfo);
    /*
    myShowLaneChangePreference->setCheck(mySettings->drawLaneChangePreference);
    */
    myVehicleNamePanel->update(mySettings->vehicleName);
    myVehicleValuePanel->update(mySettings->vehicleValue);
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
    GUIVisualizationSettings tmpSettings = *mySettings;
    int prevLaneMode = mySettings->getLaneEdgeMode();
    int prevLaneScaleMode = mySettings->getLaneEdgeScaleMode();
    int prevVehicleMode = mySettings->vehicleColorer.getActive();
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
        tmpSettings.colorSettings.stopColor = MFXUtils::getRGBColor(myStopsColor->getRGBA());
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
    tmpSettings.edgeName = myEdgeNamePanel->getSettings();
    tmpSettings.internalEdgeName = myInternalEdgeNamePanel->getSettings();
    tmpSettings.cwaEdgeName = myCwaEdgeNamePanel->getSettings();
    tmpSettings.streetName = myStreetNamePanel->getSettings();
    tmpSettings.edgeValue = myEdgeValuePanel->getSettings();
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
    }
    tmpSettings.edgeValueHideCheck = (myLaneColorRainbowCheck->getCheck() != FALSE);
    tmpSettings.edgeValueHideThreshold = myLaneColorRainbowThreshold->getValue();
    tmpSettings.laneWidthExaggeration = myLaneWidthUpscaleDialer->getValue();
    tmpSettings.laneMinSize = myLaneMinWidthDialer->getValue();

    tmpSettings.vehicleColorer.setActive(myVehicleColorMode->getCurrentItem());
    tmpSettings.vehicleQuality = myVehicleShapeDetail->getCurrentItem();
    tmpSettings.showBlinker = (myShowBlinker->getCheck() != FALSE);
    tmpSettings.drawMinGap = (myShowMinGap->getCheck() != FALSE);
    tmpSettings.drawBrakeGap = (myShowBrakeGap->getCheck() != FALSE);
    tmpSettings.showBTRange = (myShowBTRange->getCheck() != FALSE);
    tmpSettings.showRouteIndex = (myShowRouteIndex->getCheck() != FALSE);
    tmpSettings.scaleLength = (myScaleLength->getCheck() != FALSE);
    tmpSettings.showParkingInfo = (myShowParkingInfo->getCheck() != FALSE);
    /*
    tmpSettings.drawLaneChangePreference = (myShowLaneChangePreference->getCheck() != FALSE);
    */
    tmpSettings.vehicleName = myVehicleNamePanel->getSettings();
    tmpSettings.vehicleValue = myVehicleValuePanel->getSettings();
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
    tmpSettings.showSizeLegend = (myShowSizeLegend->getCheck() != FALSE);
    tmpSettings.showColorLegend = (myShowColorLegend->getCheck() != FALSE);
    tmpSettings.showVehicleColorLegend = (myShowVehicleColorLegend->getCheck() != FALSE);

    // lanes (colors)
    if (sender == myLaneColorRainbow) {
        myParent->buildColorRainbow(tmpSettings, tmpSettings.getLaneEdgeScheme(), tmpSettings.getLaneEdgeMode(), GLO_LANE,
                                    myLaneColorRainbowCheck->getCheck() != FALSE, myLaneColorRainbowThreshold->getValue());
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
        myDecalsLock->lock();
        (*myDecals) = handler.getDecals();
        rebuildDecalsTable();
        myParent->update();
        myDecalsLock->unlock();
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
    std::vector<GUISUMOAbstractView::Decal>::iterator j;
    for (j = myDecals->begin(); j != myDecals->end(); ++j) {
        GUISUMOAbstractView::Decal& d = *j;
        dev.openTag(SUMO_TAG_VIEWSETTINGS_DECAL);
        dev.writeAttr("file", d.filename);
        dev.writeAttr(SUMO_ATTR_CENTER_X, d.centerX);
        dev.writeAttr(SUMO_ATTR_CENTER_Y, d.centerY);
        dev.writeAttr(SUMO_ATTR_CENTER_Z, d.centerZ);
        dev.writeAttr(SUMO_ATTR_WIDTH, d.width);
        dev.writeAttr(SUMO_ATTR_HEIGHT, d.height);
        dev.writeAttr("altitude", d.altitude);
        dev.writeAttr("rotation", d.rot);
        dev.writeAttr("tilt", d.tilt);
        dev.writeAttr("roll", d.roll);
        dev.writeAttr(SUMO_ATTR_LAYER, d.layer);
        dev.writeAttr("screenRelative", d.screenRelative);
        dev.closeTag();
    }
}


void
GUIDialog_ViewSettings::loadDecals(const std::string& file) {
    myDecalsLock->lock();
    GUISettingsHandler handler(file);
    if (handler.hasDecals()) {
        (*myDecals) = handler.getDecals();
    }
    rebuildDecalsTable();
    myParent->update();
    myDecalsLock->unlock();
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
        FXDialogBox dialog(this, "Enter a name", GUIDesignViewSettingsDialog);
        FXVerticalFrame* content = new FXVerticalFrame(&dialog, GUIDesignViewSettingsVerticalFrame5);
        new FXLabel(content, "Please enter an alphanumeric name: ", nullptr, GUIDesignViewSettingsLabel2);
        FXTextField* text = new FXTextField(content, 40, &dialog, FXDialogBox::ID_ACCEPT, GUIDesignViewSettingsTextField1);
        new FXHorizontalSeparator(content, GUIDesignHorizontalSeparator);
        FXHorizontalFrame* buttons = new FXHorizontalFrame(content, GUIDesignViewSettingsHorizontalFrame3);
        new FXButton(buttons, "&OK", nullptr, &dialog, FXDialogBox::ID_ACCEPT, GUIDesignViewSettingsButton4);
        new FXButton(buttons, "&Cancel", nullptr, &dialog, FXDialogBox::ID_CANCEL, GUIDesignViewSettingsButton5);
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
    GUIVisualizationSettings tmpSettings = *mySettings;
    gSchemeStorage.remove(mySettings->name);
    tmpSettings.name = name;
    gSchemeStorage.add(tmpSettings);
    mySchemeName->setItemText(index, tmpSettings.name.c_str());
    myParent->getColoringSchemesCombo()->setItemText(index, tmpSettings.name.c_str());
    myParent->setColorScheme(tmpSettings.name);
    mySettings = &gSchemeStorage.get(name);
    myBackup = *mySettings;
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
    FXString file = MFXUtils::getFilename2Write(this, "Export view settings", ".xml", GUIIconSubSys::getIcon(GUIIcon::SAVE), gCurrentFolder);
    if (file == "") {
        return 1;
    }
    try {
        OutputDevice& dev = OutputDevice::getDevice(file.text());
        dev.openTag(SUMO_TAG_VIEWSETTINGS);
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
                dev.writeAttr(SUMO_ATTR_VALUE, time2string(t));
                dev.closeTag();
            }
        }
        dev.closeTag();
        dev.close();
    } catch (IOError& e) {
        FXMessageBox::error(this, MBOX_OK, "Storing failed!", "%s", e.what());
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
    FXFileDialog opendialog(this, "Import view settings");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_CONFIG));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
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
GUIDialog_ViewSettings::onCmdLoadDecals(FXObject*, FXSelector, void* /*data*/) {
    FXFileDialog opendialog(this, "Load Decals");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
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
GUIDialog_ViewSettings::onCmdSaveDecals(FXObject*, FXSelector, void* /*data*/) {
    FXString file = MFXUtils::getFilename2Write(this, "Save Decals", ".xml", GUIIconSubSys::getIcon(GUIIcon::EMPTY), gCurrentFolder);
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
        FXMessageBox::error(myParent, MBOX_OK, "Storing failed!", "%s", e.what());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onCmdClearDecals(FXObject*, FXSelector, void* /*data*/) {
    // lock decals mutex
    myDecalsLock->lock();
    // clear decals
    myDecals->clear();
    // rebuild list
    rebuildDecalsTable();
    // update view
    myParent->update();
    // unlock decals mutex
    myDecalsLock->unlock();
    return 1;
}


long
GUIDialog_ViewSettings::onUpdImportSetting(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), ptr);
    return 1;
}


void
GUIDialog_ViewSettings::rebuildDecalsTable() {
    myDecalsTable->clearItems();
    const int cols = 8;
    // set table attributes
    const int numRows = MAX2((int)10, (int)myDecals->size() + 1);
    myDecalsTable->setTableSize(numRows, cols);
    myDecalsTable->setColumnText(0, "file");
    myDecalsTable->setColumnText(1, "centerX");
    myDecalsTable->setColumnText(2, "centerY");
    myDecalsTable->setColumnText(3, "width");
    myDecalsTable->setColumnText(4, "height");
    myDecalsTable->setColumnText(5, "rotation");
    myDecalsTable->setColumnText(6, "layer");
    myDecalsTable->setColumnText(7, "relative");
    FXHeader* header = myDecalsTable->getColumnHeader();
    header->setHeight(getApp()->getNormalFont()->getFontHeight() + getApp()->getNormalFont()->getFontAscent());
    int k;
    for (k = 0; k < cols; k++) {
        header->setItemJustify(k, GUIDesignViewSettingsMFXTableJustify);
        header->setItemSize(k, 60);
    }
    header->setItemSize(0, 150);
    // insert already known decals information into table
    FXint row = 0;
    std::vector<GUISUMOAbstractView::Decal>::iterator j;
    for (j = myDecals->begin(); j != myDecals->end(); ++j) {
        GUISUMOAbstractView::Decal& d = *j;
        myDecalsTable->setItemText(row, 0, d.filename.c_str());
        myDecalsTable->setItemText(row, 1, toString<double>(d.centerX).c_str());
        myDecalsTable->setItemText(row, 2, toString<double>(d.centerY).c_str());
        myDecalsTable->setItemText(row, 3, toString<double>(d.width).c_str());
        myDecalsTable->setItemText(row, 4, toString<double>(d.height).c_str());
        myDecalsTable->setItemText(row, 5, toString<double>(d.rot).c_str());
        myDecalsTable->setItemText(row, 6, toString<double>(d.layer).c_str());
        myDecalsTable->setItemText(row, 7, toString<double>(d.screenRelative).c_str());
        row++;
    }
    // insert dummy last field
    for (k = 0; k < 7; k++) {
        myDecalsTable->setItemText(row, k, " ");
    }
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
                buttons.push_back(new FXButton(m, "No Data", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
                buttons.back()->disable();
            } else {
                buttons.push_back(new FXButton(m, "Add", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
                buttons.push_back(new FXButton(m, "Remove", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
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
            buttons.push_back(new FXButton(m, "Add", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
            buttons.push_back(new FXButton(m, "Remove", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsButton1));
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
    // decals
    delete myDecalsTable;
    myDecalsTable = new MFXAddEditTypedTable(myDecalsFrame, this, MID_TABLE, GUIDesignViewSettingsMFXTable);
    myDecalsTable->setVisibleRows(5);
    myDecalsTable->setVisibleColumns(7);
    myDecalsTable->setTableSize(5, 7);
    myDecalsTable->setBackColor(FXRGB(255, 255, 255));
    myDecalsTable->getRowHeader()->setWidth(0);
    for (int i = 1; i <= 5; ++i) {
        myDecalsTable->setCellType(i, CT_REAL);
        myDecalsTable->setNumberCellParams(i, -10000000, 10000000, 1, 10, 100, "%.2f");
    }
    rebuildDecalsTable();
    if (doCreate) {
        myDecalsTable->create();
    }
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
    myParamKey->clearItems();
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
    } else {
        myParamKey->disable();
    }
    myParamKey->setNumVisible(myParamKey->getNumItems());
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

long
GUIDialog_ViewSettings::onCmdEditTable(FXObject*, FXSelector, void* ptr) {
    MFXEditedTableItem* i = (MFXEditedTableItem*) ptr;
    std::string value = i->item->getText().text();
    // check whether the inserted value is empty
    if (value.find_first_not_of(" ") == std::string::npos) {
        return 1;
    }
    GUISUMOAbstractView::Decal d;
    int row = i->row;
    // check whether we add a new entry or edit an existing entry
    if (row == static_cast<int>(myDecals->size())) {
        d.filename = "";
        d.centerX = double(myParent->getGridWidth() / 2.);
        d.centerY = double(myParent->getGridHeight() / 2.);
        d.width = 0.;
        d.height = 0.;
        d.initialised = false;
        d.rot = 0;
        d.layer = 0;
        d.screenRelative = false;
        myDecalsLock->lock();
        myDecals->push_back(d);
        myDecalsLock->unlock();
    } else if (row > static_cast<int>(myDecals->size())) {
        // ignore clicks two lines below existing entries
        return 1;
    } else {
        d = (*myDecals)[row];
    }

    switch (i->col) {
        case 0:
            if (d.initialised && d.filename != value) {
                d.initialised = false;
            }
            d.filename = StringUtils::trim(value);
            break;
        case 1:
            try {
                d.centerX = StringUtils::toDouble(value);
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 2:
            try {
                d.centerY = StringUtils::toDouble(value);
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 3:
            try {
                d.width = StringUtils::toDouble(value);
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 4:
            try {
                d.height = StringUtils::toDouble(value);
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 5:
            try {
                d.rot = StringUtils::toDouble(value);
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 6:
            try {
                d.layer = StringUtils::toDouble(value);
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 7:
            try {
                d.screenRelative = StringUtils::toBool(value);
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a bool, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        default:
            throw 1;
    }
    (*myDecals)[row] = d;
    if (!i->updateOnly) {
        rebuildDecalsTable();
    }
    myParent->update();
    return 1;
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
    mySelectedCheck = new FXCheckButton(myMatrix0, "Only for selected", target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    mySelectedCheck->setCheck(settings.onlySelected);
    myConstSizeCheck = new FXCheckButton(myMatrix0, "constant text size", target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myConstSizeCheck->setCheck(settings.constSize);
    FXMatrix* m1 = new FXMatrix(parent, 2, GUIDesignViewSettingsMatrix5);
    new FXLabel(m1, "Size", nullptr, GUIDesignViewSettingsLabel1);
    mySizeDial = new FXRealSpinner(m1, 10, target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    mySizeDial->setRange(5, 1000);
    mySizeDial->setValue(settings.size);
    FXMatrix* m2 = new FXMatrix(parent, 4, GUIDesignViewSettingsMatrix5);
    new FXLabel(m2, "Color", nullptr, GUIDesignViewSettingsLabel1);
    myColorWell = new FXColorWell(m2, MFXUtils::getFXColor(settings.color), target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell2);
    new FXLabel(m2, "Background", nullptr, GUIDesignViewSettingsLabel1);
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


GUIDialog_ViewSettings::SizePanel::SizePanel(
    FXMatrix* parent,
    GUIDialog_ViewSettings* target,
    const GUIVisualizationSizeSettings& settings) {
    myCheck = new FXCheckButton(parent, "Draw with constant size when zoomed out", target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myCheck->setCheck(settings.constantSize);
    myCheckSelected = new FXCheckButton(parent, "Only for selected", target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myCheckSelected->setCheck(settings.constantSizeSelected);
    FXMatrix* m1 = new FXMatrix(parent, 2, GUIDesignViewSettingsMatrix5);
    new FXLabel(m1, "Minimum Size", nullptr, GUIDesignViewSettingsLabel1);
    myMinSizeDial = new FXRealSpinner(m1, 10, target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myMinSizeDial->setValue(settings.minSize);
    FXMatrix* m2 = new FXMatrix(parent, 2, GUIDesignViewSettingsMatrix5);
    new FXLabel(m2, "Exaggerate by", nullptr, GUIDesignViewSettingsLabel1);
    myExaggerateDial = new FXRealSpinner(m2, 10, target, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
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


void
GUIDialog_ViewSettings::saveWindowSize() {
    getApp()->reg().writeIntEntry("VIEWSETTINGS", "x", getX());
    getApp()->reg().writeIntEntry("VIEWSETTINGS", "y", getY());
    getApp()->reg().writeIntEntry("VIEWSETTINGS", "width", getWidth());
    getApp()->reg().writeIntEntry("VIEWSETTINGS", "height", getHeight());
}

void
GUIDialog_ViewSettings::loadWindowSize() {
    // ensure window is visible after switching screen resolutions
    const FXint minSize = 400;
    const FXint minTitlebarHeight = 20;
    setX(MAX2(0, MIN2(getApp()->reg().readIntEntry("VIEWSETTINGS", "x", 150),
                      getApp()->getRootWindow()->getWidth() - minSize)));
    setY(MAX2(minTitlebarHeight,
              MIN2(getApp()->reg().readIntEntry("VIEWSETTINGS", "y", 150),
                   getApp()->getRootWindow()->getHeight() - minSize)));
    setWidth(MAX2(getApp()->reg().readIntEntry("VIEWSETTINGS", "width", 700), minSize));
    setHeight(MAX2(getApp()->reg().readIntEntry("VIEWSETTINGS", "height", 500), minSize));
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

    new FXButton(horizontalFrame, "\t\tSave the setting to registry", GUIIconSubSys::getIcon(GUIIcon::SAVEDB), this, MID_SIMPLE_VIEW_SAVE, GUIDesignButtonToolbar);
    new FXButton(horizontalFrame, "\t\tRemove the setting from registry", GUIIconSubSys::getIcon(GUIIcon::REMOVEDB), this, MID_SIMPLE_VIEW_DELETE, GUIDesignButtonToolbar);
    new FXButton(horizontalFrame, "\t\tExport setting to file", GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_SIMPLE_VIEW_EXPORT, GUIDesignButtonToolbar);
    new FXButton(horizontalFrame, "\t\tLoad setting from file", GUIIconSubSys::getIcon(GUIIcon::OPEN_CONFIG), this, MID_SIMPLE_VIEW_IMPORT, GUIDesignButtonToolbar);

    new FXVerticalSeparator(horizontalFrame);
    new FXLabel(horizontalFrame, "Export includes:", nullptr, GUIDesignViewSettingsLabel1);
    mySaveViewPort = new FXCheckButton(horizontalFrame, "Viewport");
    mySaveDelay = new FXCheckButton(horizontalFrame, "Delay");
    mySaveDecals = new FXCheckButton(horizontalFrame, "Decals");
    mySaveBreakpoints = new FXCheckButton(horizontalFrame, "Breakpoints");
    if (mySettings->netedit) {
        mySaveBreakpoints->disable();
    }
}


void
GUIDialog_ViewSettings::buildBackgroundFrame(FXTabBook* tabbook) {
    // tab for the background
    new FXTabItem(tabbook, "Background", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* matrixColor = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    new FXLabel(matrixColor, "Color", nullptr, GUIDesignViewSettingsLabel1);
    myBackgroundColor = new FXColorWell(matrixColor, MFXUtils::getFXColor(mySettings->backgroundColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);

    FXVerticalFrame* verticalFrameDecals = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame3);
    new FXLabel(verticalFrameDecals, "Decals:");
    myDecalsFrame = new FXVerticalFrame(verticalFrameDecals);
    FXHorizontalFrame* horizontalFrameButtonsDecals = new FXHorizontalFrame(verticalFrameDecals, GUIDesignViewSettingsHorizontalFrame2);
    new FXButton(horizontalFrameButtonsDecals, "&Load Decals", nullptr, this, MID_SIMPLE_VIEW_LOAD_DECALS, GUIDesignViewSettingsButton1);
    new FXButton(horizontalFrameButtonsDecals, "&Save Decals", nullptr, this, MID_SIMPLE_VIEW_SAVE_DECALS, GUIDesignViewSettingsButton1);
    new FXButton(horizontalFrameButtonsDecals, "&Clear Decals", nullptr, this, MID_SIMPLE_VIEW_CLEAR_DECALS, GUIDesignViewSettingsButton1);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* matrixGrid = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix1);
    myShowGrid = new FXCheckButton(matrixGrid, "Toggle grid", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myShowGrid->setCheck(mySettings->showGrid);
    new FXLabel(matrixGrid, "");
    FXMatrix* matrixGridX = new FXMatrix(matrixGrid, 2, GUIDesignViewSettingsMatrix2);
    new FXLabel(matrixGridX, "x-spacing", nullptr, GUIDesignViewSettingsLabel1);
    myGridXSizeDialer = new FXRealSpinner(matrixGridX, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myGridXSizeDialer->setRange(1, 10000);
    myGridXSizeDialer->setValue(mySettings->gridXSize);
    FXMatrix* matrixGridY = new FXMatrix(matrixGrid, 2, GUIDesignViewSettingsMatrix2);
    new FXLabel(matrixGridY, "y-spacing", nullptr, GUIDesignViewSettingsLabel1);
    myGridYSizeDialer = new FXRealSpinner(matrixGridY, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial1);
    myGridYSizeDialer->setRange(1, 10000);
    myGridYSizeDialer->setValue(mySettings->gridXSize);
}


void
GUIDialog_ViewSettings::buildStreetsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "Streets", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);
    //  ... color settings
    FXVerticalFrame* verticalFrameColor = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame6);
    FXMatrix* matrixColor = new FXMatrix(verticalFrameColor, 4, GUIDesignViewSettingsMatrix3);
    new FXLabel(matrixColor, "Color", nullptr, GUIDesignViewSettingsLabel1);
    myLaneEdgeColorMode = new MFXIconComboBox(matrixColor, 30, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myLaneColorInterpolation = new FXCheckButton(matrixColor, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myLaneColorSettingFrame = new FXVerticalFrame(verticalFrameColor, GUIDesignViewSettingsVerticalFrame4);
    myParamKey = new FXComboBox(matrixColor, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myParamKey->disable();
    myParamKey->setEditable(true);

    // rainbow settings
    FXMatrix* matrixRainbow = new FXMatrix(verticalFrameColor, 3, GUIDesignViewSettingsMatrix3);
    myLaneColorRainbow = new FXButton(matrixRainbow, "Recalibrate Rainbow", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE,
                                      (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT), 0, 0, 0, 0, 20, 20, 4, 4);
    myLaneColorRainbowCheck = new FXCheckButton(matrixRainbow, "hide below threshold", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myLaneColorRainbowThreshold = new FXRealSpinner(matrixRainbow, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, REALSPIN_NOMIN | GUIDesignViewSettingsSpinDial2);
    myLaneColorRainbowThreshold->setRange(-100000, 100000);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);
    //  ... scale settings
    FXVerticalFrame* verticalFrameScale = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame6);
    FXMatrix* matrixScale = new FXMatrix(verticalFrameScale, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(matrixScale, "Scale width", nullptr, GUIDesignViewSettingsLabel1);
    myLaneEdgeScaleMode = new MFXIconComboBox(matrixScale, 30, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myLaneScaleInterpolation = new FXCheckButton(matrixScale, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myLaneScaleSettingFrame = new FXVerticalFrame(verticalFrameScale, GUIDesignViewSettingsVerticalFrame4);

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

    myShowLaneBorders = new FXCheckButton(matrixLanes, "Show lane borders", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowLaneBorders->setCheck(mySettings->laneShowBorders);
    myShowBikeMarkings = new FXCheckButton(matrixLanes, "Show bike markings", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowBikeMarkings->setCheck(mySettings->showBikeMarkings);

    myShowLaneDecals = new FXCheckButton(matrixLanes, "Show turning arrows", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowLaneDecals->setCheck(mySettings->showLinkDecals);
    new FXLabel(matrixLanes, " ", nullptr, GUIDesignViewSettingsLabel1);

    myShowLinkRules = new FXCheckButton(matrixLanes, "Show right-of-way rules", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowLinkRules->setCheck(mySettings->showLinkRules);
    myRealisticLinkRules = new FXCheckButton(matrixLanes, "Realistic stop line colors", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myRealisticLinkRules->setCheck(mySettings->realisticLinkRules);

    myShowRails = new FXCheckButton(matrixLanes, "Show rails", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowRails->setCheck(mySettings->showRails);
    new FXLabel(matrixLanes, " ", nullptr, GUIDesignViewSettingsLabel1);
    myHideMacroConnectors = new FXCheckButton(matrixLanes, "Hide macro connectors", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myHideMacroConnectors->setCheck(mySettings->hideConnectors);
    new FXLabel(matrixLanes, " ", nullptr, GUIDesignViewSettingsLabel1);
    myShowLaneDirection = new FXCheckButton(matrixLanes, "Show lane direction", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowLaneDirection->setCheck(mySettings->showLaneDirection);
    new FXLabel(matrixLanes, " ", nullptr, GUIDesignViewSettingsLabel1);
    myShowSublanes = new FXCheckButton(matrixLanes, "Show sublanes", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowSublanes->setCheck(mySettings->showSublanes);
    mySpreadSuperposed = new FXCheckButton(matrixLanes, "Spread bidirectional railway\t\tMake both directional edges for a bidirectional railway visible", this, MID_SIMPLE_VIEW_COLORCHANGE);
    mySpreadSuperposed->setCheck(mySettings->spreadSuperposed);
    new FXLabel(matrixLanes, "Exaggerate width by", nullptr, GUIDesignViewSettingsLabel1);
    myLaneWidthUpscaleDialer = new FXRealSpinner(matrixLanes, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
    myLaneWidthUpscaleDialer->setRange(0, 1000000);
    myLaneWidthUpscaleDialer->setValue(mySettings->laneWidthExaggeration);

    new FXLabel(matrixLanes, "Minimum size", nullptr, GUIDesignViewSettingsLabel1);
    myLaneMinWidthDialer = new FXRealSpinner(matrixLanes, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
    myLaneMinWidthDialer->setRange(0, 1000000);
    myLaneMinWidthDialer->setValue(mySettings->laneMinSize);

    // edge name
    myEdgeNamePanel = new NamePanel(matrixLanes, this, "Show edge id", mySettings->edgeName);
    myStreetNamePanel = new NamePanel(matrixLanes, this, "Show street name", mySettings->streetName);
    myEdgeValuePanel = new NamePanel(matrixLanes, this, "Show edge color value", mySettings->edgeValue);
}


void
GUIDialog_ViewSettings::buildVehiclesFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "Vehicles", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalframe = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* matrixShowAs = new FXMatrix(verticalframe, 2, GUIDesignViewSettingsMatrix3);
    new FXLabel(matrixShowAs, "Show As", nullptr, GUIDesignViewSettingsLabel1);
    myVehicleShapeDetail = new MFXIconComboBox(matrixShowAs, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myVehicleShapeDetail->appendIconItem("'triangles'");
    myVehicleShapeDetail->appendIconItem("'boxes'");
    myVehicleShapeDetail->appendIconItem("'simple shapes'");
    myVehicleShapeDetail->appendIconItem("'raster images'");
    myVehicleShapeDetail->appendIconItem("'circles'");
    myVehicleShapeDetail->setNumVisible(5);
    myVehicleShapeDetail->setCurrentItem(mySettings->vehicleQuality);

    new FXHorizontalSeparator(verticalframe, GUIDesignHorizontalSeparator);

    FXMatrix* matrixColor = new FXMatrix(verticalframe, 4, GUIDesignViewSettingsMatrix3);
    new FXLabel(matrixColor, "Color", nullptr, GUIDesignViewSettingsLabel1);
    myVehicleColorMode = new MFXIconComboBox(matrixColor, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->vehicleColorer.fill(*myVehicleColorMode);
    myVehicleColorMode->setNumVisible((int)mySettings->vehicleColorer.size());
    myVehicleColorInterpolation = new FXCheckButton(matrixColor, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myVehicleParamKey = new FXComboBox(matrixColor, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myVehicleParamKey->setEditable(true);
    myVehicleParamKey->disable();

    myVehicleColorSettingFrame = new FXVerticalFrame(verticalframe, GUIDesignViewSettingsVerticalFrame4);

    new FXHorizontalSeparator(verticalframe, GUIDesignHorizontalSeparator);

    FXMatrix* matrixVehicle = new FXMatrix(verticalframe, 2, GUIDesignMatrixViewSettings);
    myVehicleNamePanel = new NamePanel(matrixVehicle, this, "Show vehicle id", mySettings->vehicleName);
    myVehicleValuePanel = new NamePanel(matrixVehicle, this, "Show vehicle color value", mySettings->vehicleValue);
    myVehicleTextPanel = new NamePanel(matrixVehicle, this, "Show vehicle text param", mySettings->vehicleText);
    myVehicleTextParamKey = new FXComboBox(myVehicleTextPanel->myMatrix0, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myVehicleTextParamKey->setEditable(true);

    //new FXHorizontalSeparator(verticalframe, GUIDesignHorizontalSeparator);

    FXMatrix* matrixShow = new FXMatrix(verticalframe, 2, GUIDesignMatrixViewSettings);
    myShowBlinker = new FXCheckButton(matrixShow, "Show blinker / brake lights", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowBlinker->setCheck(mySettings->showBlinker);
    myShowMinGap = new FXCheckButton(matrixShow, "Show minimum gap", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowMinGap->setCheck(mySettings->drawMinGap);
    myShowBrakeGap = new FXCheckButton(matrixShow, "Show brake gap", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowBrakeGap->setCheck(mySettings->drawBrakeGap);
    myShowBTRange = new FXCheckButton(matrixShow, "Show Bluetooth range", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowBTRange->setCheck(mySettings->showBTRange);
    myShowRouteIndex = new FXCheckButton(matrixShow, "Show route index", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowRouteIndex->setCheck(mySettings->showRouteIndex);
    myScaleLength = new FXCheckButton(matrixShow, "Scale length with geometry", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myScaleLength->setCheck(mySettings->scaleLength);
    myShowParkingInfo = new FXCheckButton(matrixShow, "Show parking info", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowParkingInfo->setCheck(mySettings->showParkingInfo);
    //new FXLabel(matrixShow, " ", nullptr, GUIDesignViewSettingsLabel1);
    //myShowLaneChangePreference = new FXCheckButton(matrixShow, "Show lane change preference", this, MID_SIMPLE_VIEW_COLORCHANGE);
    //myShowLaneChangePreference->setCheck(mySettings->drawLaneChangePreference);
    //tmpc = new FXCheckButton(matrixShow, "Show needed headway", 0 ,0);
    //tmpc->disable();

    //new FXHorizontalSeparator(frame3, GUIDesignHorizontalSeparator);

    FXMatrix* matrixSize = new FXMatrix(verticalframe, 2, GUIDesignViewSettingsMatrix1);
    myVehicleSizePanel = new SizePanel(matrixSize, this, mySettings->vehicleSize);
}


void
GUIDialog_ViewSettings::buildPersonsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "Persons", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m101 = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix3);
    new FXLabel(m101, "Show As", nullptr, GUIDesignViewSettingsLabel1);
    myPersonShapeDetail = new MFXIconComboBox(m101, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myPersonShapeDetail->appendIconItem("'triangles'");
    myPersonShapeDetail->appendIconItem("'circles'");
    myPersonShapeDetail->appendIconItem("'simple shapes'");
    myPersonShapeDetail->appendIconItem("'raster images'");
    myPersonShapeDetail->setNumVisible(4);
    myPersonShapeDetail->setCurrentItem(mySettings->personQuality);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m102 = new FXMatrix(verticalFrame, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(m102, "Color", nullptr, GUIDesignViewSettingsLabel1);
    myPersonColorMode = new MFXIconComboBox(m102, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->personColorer.fill(*myPersonColorMode);
    myPersonColorMode->setNumVisible(mySettings->personColorer.size());
    myPersonColorInterpolation = new FXCheckButton(m102, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);

    myPersonColorSettingFrame = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame4);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m103 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myPersonNamePanel = new NamePanel(m103, this, "Show person id", mySettings->personName);
    myPersonValuePanel = new NamePanel(m103, this, "Show person color value", mySettings->personValue);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m104 = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix1);
    myPersonSizePanel = new SizePanel(m104, this, mySettings->personSize);
}


void
GUIDialog_ViewSettings::buildContainersFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "Containers", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m101 = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix3);
    new FXLabel(m101, "Show As", nullptr, GUIDesignViewSettingsLabel1);
    myContainerShapeDetail = new MFXIconComboBox(m101, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myContainerShapeDetail->appendIconItem("'triangles'");
    myContainerShapeDetail->appendIconItem("'boxes'");
    myContainerShapeDetail->appendIconItem("'simple shapes'");
    myContainerShapeDetail->appendIconItem("'raster images'");
    myContainerShapeDetail->setNumVisible(4);
    myContainerShapeDetail->setCurrentItem(mySettings->containerQuality);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m102 = new FXMatrix(verticalFrame, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(m102, "Color", nullptr, GUIDesignViewSettingsLabel1);
    myContainerColorMode = new MFXIconComboBox(m102, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->containerColorer.fill(*myContainerColorMode);
    myContainerColorMode->setNumVisible(10);
    myContainerColorInterpolation = new FXCheckButton(m102, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);

    myContainerColorSettingFrame = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame4);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m103 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myContainerNamePanel = new NamePanel(m103, this, "Show container id", mySettings->containerName);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m104 = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix1);
    myContainerSizePanel = new SizePanel(m104, this, mySettings->containerSize);
}


void
GUIDialog_ViewSettings::buildJunctionsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "Junctions", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);
    FXMatrix* m41 = new FXMatrix(verticalFrame, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(m41, "Color", nullptr, GUIDesignViewSettingsLabel1);
    myJunctionColorMode = new MFXIconComboBox(m41, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->junctionColorer.fill(*myJunctionColorMode);
    myJunctionColorMode->setNumVisible(4);
    myJunctionColorInterpolation = new FXCheckButton(m41, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);

    myJunctionColorSettingFrame = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame4);
    myJunctionColorRainbow = new FXButton(verticalFrame, "Recalibrate Rainbow", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE,
                                          (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT), 0, 0, 0, 0, 20, 20, 4, 4);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);
    FXMatrix* m42 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myJunctionSizePanel = new SizePanel(m42, this, mySettings->junctionSize);
    myDrawJunctionShape = new FXCheckButton(m42, "Draw junction shape", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myDrawJunctionShape->setCheck(mySettings->drawJunctionShape);
    myDrawCrossingsAndWalkingAreas = new FXCheckButton(m42, "Draw crossings/walkingareas", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myDrawCrossingsAndWalkingAreas->setCheck(mySettings->drawCrossingsAndWalkingareas);
    myShowLane2Lane = new FXCheckButton(m42, "Show lane to lane connections", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowLane2Lane->setCheck(mySettings->showLane2Lane);
    new FXLabel(m42, " ", nullptr, GUIDesignViewSettingsLabel1);

    myTLIndexPanel = new NamePanel(m42, this, "Show link tls index", mySettings->drawLinkTLIndex);
    myJunctionIndexPanel = new NamePanel(m42, this, "Show link junction index", mySettings->drawLinkJunctionIndex);
    myJunctionIDPanel = new NamePanel(m42, this, "Show junction id", mySettings->junctionID);
    myInternalJunctionNamePanel = new NamePanel(m42, this, "Show internal junction id", mySettings->internalJunctionName);
    myInternalEdgeNamePanel = new NamePanel(m42, this, "Show internal edge id", mySettings->internalEdgeName);
    myCwaEdgeNamePanel = new NamePanel(m42, this, "Show crossing and walkingarea id", mySettings->cwaEdgeName);
    myTLSPhaseIndexPanel = new NamePanel(m42, this, "Show traffic light phase index", mySettings->tlsPhaseIndex);
    myTLSPhaseNamePanel = new NamePanel(m42, this, "Show traffic light phase name", mySettings->tlsPhaseName);
    myJunctionNamePanel = new NamePanel(m42, this, "Show junction name", mySettings->junctionName);
}


void
GUIDialog_ViewSettings::buildAdditionalsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "Additional", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);
    // IDs
    FXMatrix* matrixIDs = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myAddNamePanel = new NamePanel(matrixIDs, this, "Show object id", mySettings->addName);
    myAddFullNamePanel = new NamePanel(matrixIDs, this, "Show full name", mySettings->addFullName);
    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);
    //Sizes
    FXMatrix* matrixSizes = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myAddSizePanel = new SizePanel(matrixSizes, this, mySettings->addSize);
    // color
    FXMatrix* matrixColor = new FXMatrix(verticalFrame, 3, GUIDesignMatrixViewSettings);
    new FXLabel(matrixColor, "StoppingPlace", nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(matrixColor, "body", nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(matrixColor, "sign", nullptr, GUIDesignViewSettingsLabel1);
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
    new FXTabItem(tabbook, "Demand", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);
    // elements
    FXMatrix* demandMatrix = new FXMatrix(verticalFrame, 3, GUIDesignMatrixViewSettings);
    new FXLabel(demandMatrix, "element", nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(demandMatrix, "color", nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(demandMatrix, "width", nullptr, GUIDesignViewSettingsLabel1);
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
    new FXLabel(stopMatrix, "color", nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(stopMatrix, "vehicles", nullptr, GUIDesignViewSettingsLabel1);
    myStopsColor = new FXColorWell(stopMatrix, MFXUtils::getFXColor(mySettings->colorSettings.stopColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(stopMatrix, "persons", nullptr, GUIDesignViewSettingsLabel1);
    myStopPersonsColor = new FXColorWell(stopMatrix, MFXUtils::getFXColor(mySettings->colorSettings.stopPersonColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
    new FXLabel(stopMatrix, "containers", nullptr, GUIDesignViewSettingsLabel1);
    myStopContainersColor = new FXColorWell(stopMatrix, MFXUtils::getFXColor(mySettings->colorSettings.stopContainerColor), this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsColorWell);
}


void
GUIDialog_ViewSettings::buildPOIsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "POIs", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m63 = new FXMatrix(verticalFrame, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(m63, "Color", nullptr, GUIDesignViewSettingsLabel1);
    myPOIColorMode = new MFXIconComboBox(m63, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->poiColorer.fill(*myPOIColorMode);
    myPOIColorMode->setNumVisible(3);
    myPOIColorInterpolation = new FXCheckButton(m63, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myPOIColorSettingFrame = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame4);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m61 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);

    new FXLabel(m61, "POI detail", nullptr, GUIDesignViewSettingsLabel1);
    myPoiDetail = new FXSpinner(m61, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
    myPoiDetail->setRange(3, 100);
    myPoiDetail->setValue(mySettings->poiDetail);

    myPOINamePanel = new NamePanel(m61, this, "Show poi id", mySettings->poiName);
    myPOITypePanel = new NamePanel(m61, this, "Show poi type", mySettings->poiType);
    myPOITextPanel = new NamePanel(m61, this, "Show poi text param", mySettings->poiText);
    myPOITextParamKey = new FXComboBox(myPOITextPanel->myMatrix0, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myPOITextParamKey->setEditable(true);
    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m62 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myPOISizePanel = new SizePanel(m62, this, mySettings->poiSize);
}


void
GUIDialog_ViewSettings::buildPolygonsFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "Polygons", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m63 = new FXMatrix(verticalFrame, 3, GUIDesignViewSettingsMatrix3);
    new FXLabel(m63, "Color", nullptr, GUIDesignViewSettingsLabel1);
    myPolyColorMode = new MFXIconComboBox(m63, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    mySettings->polyColorer.fill(*myPolyColorMode);
    myPolyColorMode->setNumVisible(3);
    myPolyColorInterpolation = new FXCheckButton(m63, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myPolyColorSettingFrame = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame4);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    FXMatrix* m91 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myPolyNamePanel = new NamePanel(m91, this, "Show polygon id", mySettings->polyName);
    myPolyTypePanel = new NamePanel(m91, this, "Show polygon types", mySettings->polyType);
    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);

    myPolySizePanel = new SizePanel(m91, this, mySettings->polySize);
}


void
GUIDialog_ViewSettings::buildSelectionFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "Selection", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m102 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    new FXLabel(m102, "Default Selection Color", nullptr, GUIDesignViewSettingsLabel1);
    new FXLabel(m102, "", nullptr, GUIDesignViewSettingsLabel1);

    new FXLabel(m102, "Miscellaneous", nullptr, GUIDesignViewSettingsLabel1);
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
    new FXTabItem(tabbook, "Data", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);
    //  ... color settings
    FXVerticalFrame* verticalFrame2 = new FXVerticalFrame(verticalFrame, GUIDesignViewSettingsVerticalFrame6);
    FXMatrix* m111 = new FXMatrix(verticalFrame2, 4, GUIDesignViewSettingsMatrix3);
    new FXLabel(m111, "Color", nullptr, GUIDesignViewSettingsLabel1);
    myDataColorMode = new MFXIconComboBox(m111, 30, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myDataColorInterpolation = new FXCheckButton(m111, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myDataColorSettingFrame = new FXVerticalFrame(verticalFrame2, GUIDesignViewSettingsVerticalFrame4);
    myDataParamKey = new FXComboBox(m111, 1, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignComboBoxStatic);
    myDataParamKey->disable();
    myDataParamKey->setEditable(true);
    mySettings->dataColorer.fill(*myDataColorMode);
    myDataColorMode->setNumVisible((int)mySettings->dataColorer.size());

    // rainbow settings
    FXMatrix* m113 = new FXMatrix(verticalFrame2, 3, GUIDesignViewSettingsMatrix3);
    myDataColorRainbow = new FXButton(m113, "Recalibrate Rainbow", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE,
                                      (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT), 0, 0, 0, 0, 20, 20, 4, 4);
    myDataColorRainbowCheck = new FXCheckButton(m113, "hide below threshold", this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignCheckButtonViewSettings);
    myDataColorRainbowThreshold = new FXRealSpinner(m113, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, REALSPIN_NOMIN | GUIDesignViewSettingsSpinDial2);
    myDataColorRainbowThreshold->setRange(-100000000, 100000000);

    new FXHorizontalSeparator(verticalFrame, GUIDesignHorizontalSeparator);
    FXMatrix* m112 = new FXMatrix(verticalFrame, 2, GUIDesignViewSettingsMatrix1);

    new FXLabel(m112, "Exaggerate edgeRelation width by", nullptr, GUIDesignViewSettingsLabel1);
    myEdgeRelationUpscaleDialer = new FXRealSpinner(m112, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
    myEdgeRelationUpscaleDialer->setRange(0, 1000000);
    myEdgeRelationUpscaleDialer->setValue(mySettings->edgeRelWidthExaggeration);

    new FXLabel(m112, "Exaggerate tazRelation width by", nullptr, GUIDesignViewSettingsLabel1);
    myTazRelationUpscaleDialer = new FXRealSpinner(m112, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, GUIDesignViewSettingsSpinDial2);
    myTazRelationUpscaleDialer->setRange(0, 1000000);
    myTazRelationUpscaleDialer->setValue(mySettings->tazRelWidthExaggeration);

    // text decoration
    myDataValuePanel = new NamePanel(m112, this, "Show data color value", mySettings->dataValue);
}


void
GUIDialog_ViewSettings::buildLegendFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "Legend", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m72 = new FXMatrix(verticalFrame, 2, GUIDesignMatrixViewSettings);
    myShowSizeLegend = new FXCheckButton(m72, "Show Size Legend", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowSizeLegend->setCheck(mySettings->showSizeLegend);
    new FXLabel(m72, "");
    myShowColorLegend = new FXCheckButton(m72, "Show Edge Color Legend", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowColorLegend->setCheck(mySettings->showColorLegend);
    new FXLabel(m72, "");
    myShowVehicleColorLegend = new FXCheckButton(m72, "Show Vehicle Color Legend", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myShowVehicleColorLegend->setCheck(mySettings->showVehicleColorLegend);
    new FXLabel(m72, "");
}


void
GUIDialog_ViewSettings::buildOpenGLFrame(FXTabBook* tabbook) {
    new FXTabItem(tabbook, "openGL", nullptr, GUIDesignViewSettingsTabItemBook1);
    FXScrollWindow* scrollWindow = new FXScrollWindow(tabbook);
    FXVerticalFrame* verticalFrame = new FXVerticalFrame(scrollWindow, GUIDesignViewSettingsVerticalFrame2);

    FXMatrix* m82 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myDither = new FXCheckButton(m82, "Dither", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myDither->setCheck(mySettings->dither);
    FXMatrix* m83 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myFPS = new FXCheckButton(m83, "FPS", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myFPS->setCheck(mySettings->fps);
    FXMatrix* m84 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myDrawBoundaries = new FXCheckButton(m84, "Draw boundaries", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myDrawBoundaries->setCheck(mySettings->drawBoundaries);
    FXMatrix* m85 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myForceDrawForPositionSelection = new FXCheckButton(m85, "Force draw for position selection", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myForceDrawForPositionSelection->setCheck(mySettings->forceDrawForPositionSelection);
    FXMatrix* m86 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myForceDrawForRectangleSelection = new FXCheckButton(m86, "Force draw for rectangle selection", this, MID_SIMPLE_VIEW_COLORCHANGE);
    myForceDrawForRectangleSelection->setCheck(mySettings->forceDrawForRectangleSelection);
    FXMatrix* m87 = new FXMatrix(verticalFrame, 1, GUIDesignMatrixViewSettings);
    myRecalculateBoundaries = new FXButton(m87, "Recalcule boundaries", nullptr, this, MID_SIMPLE_VIEW_COLORCHANGE,
                                           (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT), 0, 0, 0, 0, 20, 20, 4, 4);
}


void
GUIDialog_ViewSettings::buildButtons(FXVerticalFrame* contentFrame) {
    FXHorizontalFrame* horizontalFrameButtons = new FXHorizontalFrame(contentFrame, GUIDesignViewSettingsHorizontalFrame2);
    FXButton* OK = new FXButton(horizontalFrameButtons, "&OK", nullptr, this, MID_SETTINGS_OK, GUIDesignViewSettingsButton2);
    new FXButton(horizontalFrameButtons, "&Cancel", nullptr, this, MID_SETTINGS_CANCEL, GUIDesignViewSettingsButton3);
    OK->setFocus();
}

/****************************************************************************/
