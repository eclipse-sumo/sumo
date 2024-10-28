/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GUISettingsHandler.cpp
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @author  Mirko Barthauer
/// @date    Fri, 24. Apr 2009
///
// The dialog to change the view (gui) settings.
/****************************************************************************/
#include <config.h>

#include <vector>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/RGBColor.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/xml/SUMOSAXReader.h>
#include <utils/xml/XMLSubSys.h>
#include "GUISettingsHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================
GUISettingsHandler::GUISettingsHandler(const std::string& content, bool isFile, bool netedit) :
    SUMOSAXHandler(content),
    mySettings("TEMPORARY_NAME", netedit),
    myDelay(-1), myLookFrom(-1, -1, -1), myLookAt(-1, -1, -1), myZCoordSet(true),
    myRotation(0),
    myZoom(-1),
    myCurrentColorer(SUMO_TAG_NOTHING),
    myCurrentScheme(nullptr),
    myJamSoundTime(-1) {
    if (isFile) {
        XMLSubSys::runParser(*this, content);
    } else {
        setFileName("registrySettings");
        SUMOSAXReader* reader = XMLSubSys::getSAXReader(*this);
        reader->parseString(content);
        delete reader;
    }
}


GUISettingsHandler::~GUISettingsHandler() {
}


void
GUISettingsHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    bool ok = true;
    switch (element) {
        case SUMO_TAG_BREAKPOINT:
            if (attrs.hasAttribute(SUMO_ATTR_TIME)) {
                myBreakpoints.push_back(attrs.getSUMOTimeReporting(SUMO_ATTR_TIME, nullptr, ok));
            } else {
                myBreakpoints.push_back(attrs.getSUMOTimeReporting(SUMO_ATTR_VALUE, nullptr, ok));
                WRITE_WARNING(TL("The 'value' attribute is deprecated for breakpoints. Please use 'time'."));
            }
            break;
        case SUMO_TAG_VIEWSETTINGS:
            myViewType = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, nullptr, ok, "default");
            myViewType = StringUtils::to_lower_case(myViewType);
            break;
        case SUMO_TAG_VIEWSETTINGS_3D:
            mySettings.show3DTLSLinkMarkers = StringUtils::toBool(attrs.getStringSecure("show3DTLSLinkMarkers", toString(mySettings.show3DTLSLinkMarkers)));
            mySettings.show3DTLSDomes = StringUtils::toBool(attrs.getStringSecure("show3DTLSDomes", toString(mySettings.show3DTLSDomes)));
            mySettings.show3DHeadUpDisplay = StringUtils::toBool(attrs.getStringSecure("show3DHeadUpDisplay", toString(mySettings.show3DHeadUpDisplay)));
            mySettings.generate3DTLSModels = StringUtils::toBool(attrs.getStringSecure("generate3DTLSModels", toString(mySettings.generate3DTLSModels)));
            mySettings.ambient3DLight = parseColor(attrs, "ambient3DLight", mySettings.ambient3DLight);
            mySettings.diffuse3DLight = parseColor(attrs, "diffuse3DLight", mySettings.diffuse3DLight);
            break;
        case SUMO_TAG_DELAY:
            myDelay = attrs.getOpt<double>(SUMO_ATTR_VALUE, nullptr, ok, myDelay);
            break;
        case SUMO_TAG_VIEWPORT: {
            const double x = attrs.getOpt<double>(SUMO_ATTR_X, nullptr, ok, myLookFrom.x());
            const double y = attrs.getOpt<double>(SUMO_ATTR_Y, nullptr, ok, myLookFrom.y());
            const double z = attrs.getOpt<double>(SUMO_ATTR_Z, nullptr, ok, myLookFrom.z());
            attrs.get<double>(SUMO_ATTR_Z, nullptr, myZCoordSet, false);
            myLookFrom.set(x, y, z);
            myZoom = attrs.getOpt<double>(SUMO_ATTR_ZOOM, nullptr, ok, myZoom);
            const double cx = attrs.getOpt<double>(SUMO_ATTR_CENTER_X, nullptr, ok, myLookFrom.x());
            const double cy = attrs.getOpt<double>(SUMO_ATTR_CENTER_Y, nullptr, ok, myLookFrom.y());
            const double cz = attrs.getOpt<double>(SUMO_ATTR_CENTER_Z, nullptr, ok, 0.);
            myLookAt.set(cx, cy, cz);
            myRotation = attrs.getOpt<double>(SUMO_ATTR_ANGLE, nullptr, ok, myRotation);
            break;
        }
        case SUMO_TAG_SNAPSHOT: {
            std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, nullptr, ok);
            if (file != "" && !FileHelpers::isAbsolute(file)) {
                file = FileHelpers::getConfigurationRelative(getFileName(), file);
            }
            mySnapshots[attrs.getOptSUMOTimeReporting(SUMO_ATTR_TIME, file.c_str(), ok, 0)].push_back(file);
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_SCHEME: {
            mySettings.name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, nullptr, ok, mySettings.name);
            if (gSchemeStorage.contains(mySettings.name)) {
                mySettings.copy(gSchemeStorage.get(mySettings.name));
            }
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_OPENGL:
            mySettings.dither = StringUtils::toBool(attrs.getStringSecure("dither", toString(mySettings.dither)));
            mySettings.fps = StringUtils::toBool(attrs.getStringSecure("fps", toString(mySettings.fps)));
            mySettings.trueZ = StringUtils::toBool(attrs.getStringSecure("trueZ", toString(mySettings.trueZ)));
            mySettings.drawBoundaries = StringUtils::toBool(attrs.getStringSecure("drawBoundaries", toString(mySettings.drawBoundaries)));
            mySettings.forceDrawForRectangleSelection = StringUtils::toBool(attrs.getStringSecure("forceDrawRectangleSelection", toString(mySettings.forceDrawForRectangleSelection)));
            mySettings.disableDottedContours = StringUtils::toBool(attrs.getStringSecure("disableDottedContours", toString(mySettings.disableDottedContours)));
            mySettings.geometryIndices = parseTextSettings("geometryIndices", attrs, mySettings.geometryIndices);
            break;
        case SUMO_TAG_VIEWSETTINGS_BACKGROUND:
            mySettings.backgroundColor = RGBColor::parseColorReporting(attrs.getStringSecure("backgroundColor", toString(mySettings.backgroundColor)), "background", nullptr, true, ok);
            mySettings.showGrid = StringUtils::toBool(attrs.getStringSecure("showGrid", toString(mySettings.showGrid)));
            mySettings.gridXSize = StringUtils::toDouble(attrs.getStringSecure("gridXSize", toString(mySettings.gridXSize)));
            mySettings.gridYSize = StringUtils::toDouble(attrs.getStringSecure("gridYSize", toString(mySettings.gridYSize)));
            break;
        case SUMO_TAG_VIEWSETTINGS_EDGES: {
            int laneEdgeMode = StringUtils::toInt(attrs.getStringSecure("laneEdgeMode", "0"));
            int laneEdgeScaleMode = StringUtils::toInt(attrs.getStringSecure("scaleMode", "0"));
            mySettings.laneShowBorders = StringUtils::toBool(attrs.getStringSecure("laneShowBorders", toString(mySettings.laneShowBorders)));
            mySettings.showBikeMarkings = StringUtils::toBool(attrs.getStringSecure("showBikeMarkings", toString(mySettings.showBikeMarkings)));
            mySettings.showLinkDecals = StringUtils::toBool(attrs.getStringSecure("showLinkDecals", toString(mySettings.showLinkDecals)));
            mySettings.realisticLinkRules = StringUtils::toBool(attrs.getStringSecure("realisticLinkRules", toString(mySettings.realisticLinkRules)));
            mySettings.showLinkRules = StringUtils::toBool(attrs.getStringSecure("showLinkRules", toString(mySettings.showLinkRules)));
            mySettings.showRails = StringUtils::toBool(attrs.getStringSecure("showRails", toString(mySettings.showRails)));
            mySettings.secondaryShape = StringUtils::toBool(attrs.getStringSecure("secondaryShape", toString(mySettings.secondaryShape)));
            mySettings.edgeName = parseTextSettings("edgeName", attrs, mySettings.edgeName);
            mySettings.internalEdgeName = parseTextSettings("internalEdgeName", attrs, mySettings.internalEdgeName);
            mySettings.cwaEdgeName = parseTextSettings("cwaEdgeName", attrs, mySettings.cwaEdgeName);
            mySettings.streetName = parseTextSettings("streetName", attrs, mySettings.streetName);
            mySettings.edgeValue = parseTextSettings("edgeValue", attrs, mySettings.edgeValue);
            mySettings.edgeScaleValue = parseTextSettings("edgeScaleValue", attrs, mySettings.edgeScaleValue);
            mySettings.hideConnectors = StringUtils::toBool(attrs.getStringSecure("hideConnectors", toString(mySettings.hideConnectors)));
            mySettings.laneWidthExaggeration = StringUtils::toDouble(attrs.getStringSecure("widthExaggeration", toString(mySettings.laneWidthExaggeration)));
            mySettings.laneMinSize = StringUtils::toDouble(attrs.getStringSecure("minSize", toString(mySettings.laneMinSize)));
            mySettings.showLaneDirection = StringUtils::toBool(attrs.getStringSecure("showDirection", toString(mySettings.showLaneDirection)));
            mySettings.showSublanes = StringUtils::toBool(attrs.getStringSecure("showSublanes", toString(mySettings.showSublanes)));
            mySettings.spreadSuperposed = StringUtils::toBool(attrs.getStringSecure("spreadSuperposed", toString(mySettings.spreadSuperposed)));
            mySettings.disableHideByZoom = StringUtils::toBool(attrs.getStringSecure("disableHideByZoom", toString(mySettings.disableHideByZoom)));
            mySettings.edgeParam = attrs.getStringSecure("edgeParam", mySettings.edgeParam);
            mySettings.laneParam = attrs.getStringSecure("laneParam", mySettings.laneParam);
            mySettings.vehicleParam = attrs.getStringSecure("vehicleParam", mySettings.vehicleParam);
            mySettings.vehicleScaleParam = attrs.getStringSecure("vehicleScaleParam", mySettings.vehicleScaleParam);
            mySettings.vehicleTextParam = attrs.getStringSecure("vehicleTextParam", mySettings.vehicleTextParam);
            mySettings.edgeData = attrs.getStringSecure("edgeData", mySettings.edgeData);
            mySettings.edgeDataID = attrs.getStringSecure("edgeDataID", mySettings.edgeDataID);
            mySettings.edgeDataScaling = attrs.getStringSecure("edgeDataScaling", mySettings.edgeDataScaling);
            mySettings.edgeValueRainBow = parseRainbowSettings("edgeValue", attrs, mySettings.edgeValueRainBow);
            myCurrentColorer = element;
            mySettings.edgeColorer.setActive(laneEdgeMode);
            mySettings.edgeScaler.setActive(laneEdgeScaleMode);
            mySettings.laneColorer.setActive(laneEdgeMode);
            mySettings.laneScaler.setActive(laneEdgeScaleMode);
        }
        break;
        case SUMO_TAG_COLORSCHEME: {
            myCurrentScheme = nullptr;
            myCurrentScaleScheme = nullptr;
            const std::string name = attrs.getStringSecure(SUMO_ATTR_NAME, "");
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_EDGES) {
                if (StringUtils::startsWith(name, "meso:")) {
                    // see edgeColorer.save() in GUIVisualizationSettings::save
                    myCurrentScheme = mySettings.edgeColorer.getSchemeByName(name.substr(5));
                } else {
                    myCurrentScheme = mySettings.laneColorer.getSchemeByName(name);
                }
                if (myCurrentScheme == nullptr) {
                    // legacy: meso schemes without prefix
                    myCurrentScheme = mySettings.edgeColorer.getSchemeByName(name);
                }
            }
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_VEHICLES) {
                myCurrentScheme = mySettings.vehicleColorer.getSchemeByName(name);
            }
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_PERSONS) {
                myCurrentScheme = mySettings.personColorer.getSchemeByName(name);
            }
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_JUNCTIONS) {
                myCurrentScheme = mySettings.junctionColorer.getSchemeByName(name);
            }
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_POIS) {
                myCurrentScheme = mySettings.poiColorer.getSchemeByName(name);
            }
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_POLYS) {
                myCurrentScheme = mySettings.polyColorer.getSchemeByName(name);
            }
            if (myCurrentScheme && !myCurrentScheme->isFixed()) {
                myCurrentScheme->setInterpolated(attrs.getOpt<bool>(SUMO_ATTR_INTERPOLATED, nullptr, ok, false));
                myCurrentScheme->clear();
            }
        }
        break;
        case SUMO_TAG_SCALINGSCHEME:
            myCurrentScheme = nullptr;
            myCurrentScaleScheme = nullptr;
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_EDGES) {
                myCurrentScaleScheme = mySettings.laneScaler.getSchemeByName(attrs.getStringSecure(SUMO_ATTR_NAME, ""));
                if (myCurrentScaleScheme == nullptr) {
                    myCurrentScaleScheme = mySettings.edgeScaler.getSchemeByName(attrs.getStringSecure(SUMO_ATTR_NAME, ""));
                }
            }
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_VEHICLES) {
                myCurrentScaleScheme = mySettings.vehicleScaler.getSchemeByName(attrs.getStringSecure(SUMO_ATTR_NAME, ""));
            }
            if (myCurrentScaleScheme && !myCurrentScaleScheme->isFixed()) {
                myCurrentScaleScheme->setInterpolated(attrs.getOpt<bool>(SUMO_ATTR_INTERPOLATED, nullptr, ok, false));
                myCurrentScaleScheme->clear();
            }
            break;
        case SUMO_TAG_ENTRY:
            if (myCurrentScheme != nullptr) {
                RGBColor color = attrs.get<RGBColor>(SUMO_ATTR_COLOR, nullptr, ok);
                if (myCurrentScheme->isFixed()) {
                    myCurrentScheme->setColor(attrs.getStringSecure(SUMO_ATTR_NAME, ""), color);
                } else {
                    myCurrentScheme->addColor(color,
                                              attrs.getOpt<double>(SUMO_ATTR_THRESHOLD, nullptr, ok, std::numeric_limits<double>::max()),
                                              attrs.getStringSecure(SUMO_ATTR_NAME, ""));
                }
            } else if (myCurrentScaleScheme != nullptr) {
                double scale = attrs.get<double>(SUMO_ATTR_COLOR, nullptr, ok);
                if (myCurrentScaleScheme->isFixed()) {
                    myCurrentScaleScheme->setColor(attrs.getStringSecure(SUMO_ATTR_NAME, ""), scale);
                } else {
                    myCurrentScaleScheme->addColor(scale,
                                                   attrs.getOpt<double>(SUMO_ATTR_THRESHOLD, nullptr, ok, std::numeric_limits<double>::max()),
                                                   attrs.getStringSecure(SUMO_ATTR_NAME, ""));
                }
            }
            break;
        case SUMO_TAG_VIEWSETTINGS_VEHICLES:
            mySettings.vehicleColorer.setActive(StringUtils::toInt(attrs.getStringSecure("vehicleMode", "0")));
            mySettings.vehicleScaler.setActive(StringUtils::toInt(attrs.getStringSecure("vehicleScaleMode", "0")));
            mySettings.vehicleQuality = StringUtils::toInt(attrs.getStringSecure("vehicleQuality", toString(mySettings.vehicleQuality)));
            mySettings.showBlinker = StringUtils::toBool(attrs.getStringSecure("showBlinker", toString(mySettings.showBlinker)));
            mySettings.drawMinGap = StringUtils::toBool(attrs.getStringSecure("drawMinGap", toString(mySettings.drawMinGap)));
            mySettings.drawBrakeGap = StringUtils::toBool(attrs.getStringSecure("drawBrakeGap", toString(mySettings.drawBrakeGap)));
            mySettings.showBTRange = StringUtils::toBool(attrs.getStringSecure("showBTRange", toString(mySettings.showBTRange)));
            mySettings.showRouteIndex = StringUtils::toBool(attrs.getStringSecure("showRouteIndex", toString(mySettings.showRouteIndex)));
            mySettings.scaleLength = StringUtils::toBool(attrs.getStringSecure("scaleLength", toString(mySettings.scaleLength)));
            mySettings.drawReversed = StringUtils::toBool(attrs.getStringSecure("drawReversed", toString(mySettings.drawReversed)));
            mySettings.showParkingInfo = StringUtils::toBool(attrs.getStringSecure("showParkingInfo", toString(mySettings.showParkingInfo)));
            mySettings.showChargingInfo = StringUtils::toBool(attrs.getStringSecure("showChargingInfo", toString(mySettings.showChargingInfo)));
            mySettings.vehicleSize = parseSizeSettings("vehicle", attrs, mySettings.vehicleSize);
            mySettings.vehicleName = parseTextSettings("vehicleName", attrs, mySettings.vehicleName);
            mySettings.vehicleValue = parseTextSettings("vehicleValue", attrs, mySettings.vehicleValue);
            mySettings.vehicleScaleValue = parseTextSettings("vehicleScaleValue", attrs, mySettings.vehicleScaleValue);
            mySettings.vehicleText = parseTextSettings("vehicleText", attrs, mySettings.vehicleText);
            myCurrentColorer = element;
            break;
        case SUMO_TAG_VIEWSETTINGS_PERSONS:
            mySettings.personColorer.setActive(StringUtils::toInt(attrs.getStringSecure("personMode", "0")));
            mySettings.personQuality = StringUtils::toInt(attrs.getStringSecure("personQuality", toString(mySettings.personQuality)));
            mySettings.personSize = parseSizeSettings("person", attrs, mySettings.personSize);
            mySettings.personName = parseTextSettings("personName", attrs, mySettings.personName);
            mySettings.personValue = parseTextSettings("personValue", attrs, mySettings.personValue);
            mySettings.showPedestrianNetwork = StringUtils::toBool(attrs.getStringSecure("showPedestrianNetwork", toString(mySettings.showPedestrianNetwork)));
            mySettings.pedestrianNetworkColor = RGBColor::parseColorReporting(attrs.getStringSecure("pedestrianNetworkColor", toString(mySettings.pedestrianNetworkColor)), "pedestrianNetworkColor", nullptr, true, ok);
            myCurrentColorer = element;
            break;
        case SUMO_TAG_VIEWSETTINGS_CONTAINERS:
            mySettings.containerColorer.setActive(StringUtils::toInt(attrs.getStringSecure("containerMode", "0")));
            mySettings.containerQuality = StringUtils::toInt(attrs.getStringSecure("containerQuality", toString(mySettings.containerQuality)));
            mySettings.containerSize = parseSizeSettings("container", attrs, mySettings.containerSize);
            mySettings.containerName = parseTextSettings("containerName", attrs, mySettings.containerName);
            myCurrentColorer = element;
            break;
        case SUMO_TAG_VIEWSETTINGS_JUNCTIONS:
            mySettings.junctionColorer.setActive(StringUtils::toInt(attrs.getStringSecure("junctionMode", "0")));
            mySettings.drawLinkTLIndex = parseTextSettings("drawLinkTLIndex", attrs, mySettings.drawLinkTLIndex);
            mySettings.drawLinkJunctionIndex = parseTextSettings("drawLinkJunctionIndex", attrs, mySettings.drawLinkJunctionIndex);
            mySettings.junctionID = parseTextSettings("junctionID", attrs, mySettings.junctionID);
            mySettings.junctionName = parseTextSettings("junctionName", attrs, mySettings.junctionName);
            mySettings.internalJunctionName = parseTextSettings("internalJunctionName", attrs, mySettings.internalJunctionName);
            mySettings.tlsPhaseIndex = parseTextSettings("tlsPhaseIndex", attrs, mySettings.tlsPhaseIndex);
            mySettings.tlsPhaseName = parseTextSettings("tlsPhaseName", attrs, mySettings.tlsPhaseName);
            mySettings.showLane2Lane = StringUtils::toBool(attrs.getStringSecure("showLane2Lane", toString(mySettings.showLane2Lane)));
            mySettings.drawJunctionShape = StringUtils::toBool(attrs.getStringSecure("drawShape", toString(mySettings.drawJunctionShape)));
            mySettings.drawCrossingsAndWalkingareas = StringUtils::toBool(attrs.getStringSecure(
                        "drawCrossingsAndWalkingareas", toString(mySettings.drawCrossingsAndWalkingareas)));
            mySettings.junctionSize = parseSizeSettings("junction", attrs, mySettings.junctionSize);
            mySettings.junctionValueRainBow = parseRainbowSettings("junctionValue", attrs, mySettings.junctionValueRainBow);
            myCurrentColorer = element;
            break;
        case SUMO_TAG_VIEWSETTINGS_ADDITIONALS:
            mySettings.addMode = StringUtils::toInt(attrs.getStringSecure("addMode", toString(mySettings.addMode)));
            mySettings.addSize = parseSizeSettings("add", attrs, mySettings.addSize);
            mySettings.addName = parseTextSettings("addName", attrs, mySettings.addName);
            mySettings.addFullName = parseTextSettings("addFullName", attrs, mySettings.addFullName);
            // color settings (temporal, will be integrated in a schema
            mySettings.colorSettings.busStopColorSign = parseColor(attrs, "busStopColorSign", mySettings.colorSettings.busStopColorSign);
            mySettings.colorSettings.chargingStationColor = parseColor(attrs, "chargingStationColor", mySettings.colorSettings.chargingStationColor);
            mySettings.colorSettings.chargingStationColorCharge = parseColor(attrs, "chargingStationColorCharge", mySettings.colorSettings.chargingStationColorCharge);
            mySettings.colorSettings.chargingStationColorSign = parseColor(attrs, "chargingStationColorSign", mySettings.colorSettings.chargingStationColorSign);
            mySettings.colorSettings.containerStopColor = parseColor(attrs, "containerStopColor", mySettings.colorSettings.containerStopColor);
            mySettings.colorSettings.containerStopColorSign = parseColor(attrs, "containerStopColorSign", mySettings.colorSettings.containerStopColorSign);
            mySettings.colorSettings.parkingAreaColor = parseColor(attrs, "parkingAreaColor", mySettings.colorSettings.parkingAreaColor);
            mySettings.colorSettings.parkingAreaColorSign = parseColor(attrs, "parkingAreaColorSign", mySettings.colorSettings.parkingAreaColorSign);
            mySettings.colorSettings.parkingSpaceColor = parseColor(attrs, "parkingSpaceColor", mySettings.colorSettings.parkingSpaceColor);
            mySettings.colorSettings.parkingSpaceColorContour = parseColor(attrs, "parkingSpaceColorContour", mySettings.colorSettings.parkingSpaceColorContour);
            mySettings.colorSettings.personTripColor = parseColor(attrs, "personTripColor", mySettings.colorSettings.personTripColor);
            mySettings.colorSettings.rideColor = parseColor(attrs, "rideColor", mySettings.colorSettings.rideColor);
            mySettings.colorSettings.selectedAdditionalColor = parseColor(attrs, "selectedAdditionalColor", mySettings.colorSettings.selectedAdditionalColor);
            mySettings.colorSettings.selectedConnectionColor = parseColor(attrs, "selectedConnectionColor", mySettings.colorSettings.selectedConnectionColor);
            mySettings.colorSettings.selectedContainerColor = parseColor(attrs, "selectedContainerColor", mySettings.colorSettings.selectedContainerColor);
            mySettings.colorSettings.selectedContainerPlanColor = parseColor(attrs, "selectedContainerPlanColor", mySettings.colorSettings.selectedContainerPlanColor);
            mySettings.colorSettings.selectedCrossingColor = parseColor(attrs, "selectedCrossingColor", mySettings.colorSettings.selectedCrossingColor);
            mySettings.colorSettings.selectedEdgeColor = parseColor(attrs, "selectedEdgeColor", mySettings.colorSettings.selectedEdgeColor);
            mySettings.colorSettings.selectedEdgeDataColor = parseColor(attrs, "selectedEdgeDataColor", mySettings.colorSettings.selectedEdgeDataColor);
            mySettings.colorSettings.selectedLaneColor = parseColor(attrs, "selectedLaneColor", mySettings.colorSettings.selectedLaneColor);
            mySettings.colorSettings.selectedPersonColor = parseColor(attrs, "selectedPersonColor", mySettings.colorSettings.selectedPersonColor);
            mySettings.colorSettings.selectedPersonPlanColor = parseColor(attrs, "selectedPersonPlanColor", mySettings.colorSettings.selectedPersonPlanColor);
            mySettings.colorSettings.selectedProhibitionColor = parseColor(attrs, "selectedProhibitionColor", mySettings.colorSettings.selectedProhibitionColor);
            mySettings.colorSettings.selectedRouteColor = parseColor(attrs, "selectedRouteColor", mySettings.colorSettings.selectedRouteColor);
            mySettings.colorSettings.selectedVehicleColor = parseColor(attrs, "selectedVehicleColor", mySettings.colorSettings.selectedVehicleColor);
            mySettings.colorSettings.selectionColor = parseColor(attrs, "selectionColor", mySettings.colorSettings.selectionColor);
            mySettings.colorSettings.stopColor = parseColor(attrs, "stopColor", mySettings.colorSettings.stopColor);
            mySettings.colorSettings.waypointColor = parseColor(attrs, "waypointColor", mySettings.colorSettings.waypointColor);
            mySettings.colorSettings.stopContainerColor = parseColor(attrs, "stopContainerColor", mySettings.colorSettings.stopContainerColor);
            mySettings.colorSettings.stopPersonColor = parseColor(attrs, "stopPersonColor", mySettings.colorSettings.stopPersonColor);
            mySettings.colorSettings.trainStopColor = parseColor(attrs, "trainStopColor", mySettings.colorSettings.trainStopColor);
            mySettings.colorSettings.trainStopColorSign = parseColor(attrs, "trainStopColorSign", mySettings.colorSettings.trainStopColorSign);
            mySettings.colorSettings.transhipColor = parseColor(attrs, "transhipColor", mySettings.colorSettings.transhipColor);
            mySettings.colorSettings.transportColor = parseColor(attrs, "transportColor", mySettings.colorSettings.transportColor);
            mySettings.colorSettings.vehicleTripColor = parseColor(attrs, "vehicleTripColor", mySettings.colorSettings.vehicleTripColor);
            mySettings.colorSettings.walkColor = parseColor(attrs, "walkColor", mySettings.colorSettings.walkColor);
            mySettings.widthSettings.personTripWidth = StringUtils::toDouble(attrs.getStringSecure("personTripWidth", toString(mySettings.widthSettings.personTripWidth)));
            mySettings.widthSettings.rideWidth = StringUtils::toDouble(attrs.getStringSecure("rideWidth", toString(mySettings.widthSettings.rideWidth)));
            mySettings.widthSettings.transhipWidth = StringUtils::toDouble(attrs.getStringSecure("transhipWidth", toString(mySettings.widthSettings.transhipWidth)));
            mySettings.widthSettings.transportWidth = StringUtils::toDouble(attrs.getStringSecure("transportWidth", toString(mySettings.widthSettings.transportWidth)));
            mySettings.widthSettings.tripWidth = StringUtils::toDouble(attrs.getStringSecure("tripWidth", toString(mySettings.widthSettings.tripWidth)));
            mySettings.widthSettings.walkWidth = StringUtils::toDouble(attrs.getStringSecure("walkWidth", toString(mySettings.widthSettings.walkWidth)));
            mySettings.colorSettings.busStopColor = parseColor(attrs, "busStopColor", mySettings.colorSettings.busStopColor);
            break;
        case SUMO_TAG_VIEWSETTINGS_POIS:
            mySettings.poiTextParam = attrs.getStringSecure("poiTextParam", mySettings.poiTextParam);
            mySettings.poiSize = parseSizeSettings("poi", attrs, mySettings.poiSize);
            mySettings.poiDetail = StringUtils::toInt(attrs.getStringSecure("poiDetail", toString(mySettings.poiDetail)));
            mySettings.poiName = parseTextSettings("poiName", attrs, mySettings.poiName);
            mySettings.poiType = parseTextSettings("poiType", attrs, mySettings.poiType);
            mySettings.poiText = parseTextSettings("poiText", attrs, mySettings.poiText);
            mySettings.poiColorer.setActive(StringUtils::toInt(attrs.getStringSecure("personMode", "0")));
            myCurrentColorer = element;
            break;
        case SUMO_TAG_VIEWSETTINGS_POLYS:
            mySettings.polySize = parseSizeSettings("poly", attrs, mySettings.polySize);
            mySettings.polyName = parseTextSettings("polyName", attrs, mySettings.polyName);
            mySettings.polyType = parseTextSettings("polyType", attrs, mySettings.polyType);
            mySettings.polyColorer.setActive(StringUtils::toInt(attrs.getStringSecure("personMode", "0")));
            myCurrentColorer = element;
            break;
        case SUMO_TAG_VIEWSETTINGS_LEGEND:
            mySettings.showSizeLegend = StringUtils::toBool(attrs.getStringSecure("showSizeLegend", toString(mySettings.showSizeLegend)));
            mySettings.showColorLegend = StringUtils::toBool(attrs.getStringSecure("showColorLegend", toString(mySettings.showColorLegend)));
            mySettings.showVehicleColorLegend = StringUtils::toBool(attrs.getStringSecure("showVehicleColorLegend", toString(mySettings.showVehicleColorLegend)));
            break;
        case SUMO_TAG_VIEWSETTINGS_DECAL: {
            GUISUMOAbstractView::Decal d;
            if (attrs.hasAttribute(SUMO_ATTR_FILE)) {
                d.filename = StringUtils::substituteEnvironment(attrs.get<std::string>(SUMO_ATTR_FILE, nullptr, ok));
            } else {
                d.filename = attrs.getStringSecure("filename", d.filename);
                WRITE_WARNING(TL("The 'filename' attribute is deprecated for decals. Please use 'file'."));
            }
            if (d.filename != "" && !FileHelpers::isAbsolute(d.filename)) {
                d.filename = FileHelpers::getConfigurationRelative(getFileName(), d.filename);
            }
            d.centerX = attrs.getOpt<double>(SUMO_ATTR_CENTER_X, nullptr, ok, d.centerX);
            d.centerY = attrs.getOpt<double>(SUMO_ATTR_CENTER_Y, nullptr, ok, d.centerY);
            d.centerZ = attrs.getOpt<double>(SUMO_ATTR_CENTER_Z, nullptr, ok, d.centerZ);
            d.width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, nullptr, ok, d.width);
            d.height = attrs.getOpt<double>(SUMO_ATTR_HEIGHT, nullptr, ok, d.height);
            d.altitude = StringUtils::toDouble(attrs.getStringSecure("altitude", "0"));
            d.rot = StringUtils::toDouble(attrs.getStringSecure("rotation", toString(d.rot)));
            d.tilt = StringUtils::toDouble(attrs.getStringSecure("tilt", toString(d.tilt)));
            d.roll = StringUtils::toDouble(attrs.getStringSecure("roll", toString(d.roll)));
            d.layer = attrs.getOpt<double>(SUMO_ATTR_LAYER, nullptr, ok, d.layer);
            d.screenRelative = StringUtils::toBool(attrs.getStringSecure("screenRelative", toString(d.screenRelative)));
            d.initialised = false;
            myDecals.push_back(d);
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_LIGHT: {
            GUISUMOAbstractView::Decal d;
            d.filename = "light" + attrs.getOpt<std::string>(SUMO_ATTR_INDEX, nullptr, ok, "0");
            d.centerX = attrs.getOpt<double>(SUMO_ATTR_CENTER_X, nullptr, ok, d.centerX);
            d.centerY = attrs.getOpt<double>(SUMO_ATTR_CENTER_Y, nullptr, ok, d.centerY);
            d.centerZ = attrs.getOpt<double>(SUMO_ATTR_CENTER_Z, nullptr, ok, d.centerZ);
            d.width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, nullptr, ok, d.width);
            d.height = attrs.getOpt<double>(SUMO_ATTR_HEIGHT, nullptr, ok, d.height);
            d.altitude = StringUtils::toDouble(attrs.getStringSecure("altitude", "0"));
            d.rot = StringUtils::toDouble(attrs.getStringSecure("rotation", toString(d.rot)));
            d.tilt = StringUtils::toDouble(attrs.getStringSecure("tilt", toString(d.tilt)));
            d.roll = StringUtils::toDouble(attrs.getStringSecure("roll", toString(d.roll)));
            d.layer = attrs.getOpt<double>(SUMO_ATTR_LAYER, nullptr, ok, d.layer);
            d.initialised = false;
            myDecals.push_back(d);
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_EVENT: {
            const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
            const std::string cmd = attrs.get<std::string>(SUMO_ATTR_COMMAND, nullptr, ok);
            const double prob = attrs.get<double>(SUMO_ATTR_PROB, id.c_str(), ok);
            myEventDistributions[id].add(cmd, prob);
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_EVENT_JAM_TIME:
            myJamSoundTime = attrs.get<double>(SUMO_ATTR_VALUE, nullptr, ok);
            break;
        default:
            break;
    }
}

void
GUISettingsHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_VIEWSETTINGS_SCHEME: {
            if (mySettings.name != "") {
                gSchemeStorage.add(mySettings);
                myLoadedSettingNames.push_back(mySettings.name);
            }
        }
    }
}


RGBColor
GUISettingsHandler::parseColor(const SUMOSAXAttributes& attrs, const std::string attribute, const RGBColor& defaultValue) const {
    bool ok = true;
    return RGBColor::parseColorReporting(attrs.getStringSecure(attribute, toString(defaultValue)), attribute.c_str(), nullptr, true, ok);
}


GUIVisualizationTextSettings
GUISettingsHandler::parseTextSettings(
    const std::string& prefix, const SUMOSAXAttributes& attrs,
    GUIVisualizationTextSettings defaults) {
    bool ok = true;
    return GUIVisualizationTextSettings(
               StringUtils::toBool(attrs.getStringSecure(prefix + "_show", toString(defaults.showText))),
               StringUtils::toDouble(attrs.getStringSecure(prefix + "_size", toString(defaults.size))),
               RGBColor::parseColorReporting(attrs.getStringSecure(prefix + "_color", toString(defaults.color)), "textSettings", nullptr, true, ok),
               RGBColor::parseColorReporting(attrs.getStringSecure(prefix + "_bgColor", toString(defaults.bgColor)), "textSettings", nullptr, true, ok),
               StringUtils::toBool(attrs.getStringSecure(prefix + "_constantSize", toString(defaults.constSize))),
               StringUtils::toBool(attrs.getStringSecure(prefix + "_onlySelected", toString(defaults.onlySelected))));
}


GUIVisualizationSizeSettings
GUISettingsHandler::parseSizeSettings(
    const std::string& prefix, const SUMOSAXAttributes& attrs,
    GUIVisualizationSizeSettings defaults) {
    return GUIVisualizationSizeSettings(
               StringUtils::toDouble(attrs.getStringSecure(prefix + "_minSize", toString(defaults.minSize))),
               StringUtils::toDouble(attrs.getStringSecure(prefix + "_exaggeration", toString(defaults.exaggeration))),
               StringUtils::toBool(attrs.getStringSecure(prefix + "_constantSize", toString(defaults.constantSize))),
               StringUtils::toBool(attrs.getStringSecure(prefix + "_constantSizeSelected", toString(defaults.constantSizeSelected))));
}

GUIVisualizationRainbowSettings
GUISettingsHandler::parseRainbowSettings(
    const std::string& prefix, const SUMOSAXAttributes& attrs,
    GUIVisualizationRainbowSettings defaults) {
    return GUIVisualizationRainbowSettings(
               StringUtils::toBool(attrs.getStringSecure(prefix + "HideCheck", toString(defaults.hideMin))),
               StringUtils::toDouble(attrs.getStringSecure(prefix + "HideThreshold", toString(defaults.minThreshold))),
               StringUtils::toBool(attrs.getStringSecure(prefix + "HideCheck2", toString(defaults.hideMax))),
               StringUtils::toDouble(attrs.getStringSecure(prefix + "HideThreshold2", toString(defaults.maxThreshold))),
               StringUtils::toBool(attrs.getStringSecure(prefix + "SetNeutral", toString(defaults.hideMax))),
               StringUtils::toDouble(attrs.getStringSecure(prefix + "NeutralThreshold", toString(defaults.neutralThreshold))),
               StringUtils::toBool(attrs.getStringSecure(prefix + "FixRange", toString(defaults.fixRange))));
}

const std::vector<std::string>&
GUISettingsHandler::addSettings(GUISUMOAbstractView* view) const {
    if (view) {
        for (std::string name : myLoadedSettingNames) {
            FXint index = view->getColoringSchemesCombo()->appendIconItem(name.c_str());
            view->getColoringSchemesCombo()->setCurrentItem(index);
            view->setColorScheme(name);
        }
    }
    return myLoadedSettingNames;
}


void
GUISettingsHandler::applyViewport(GUISUMOAbstractView* view) const {
    if (myLookFrom.z() > 0 || myZoom > 0) {
        // z value stores zoom so we must convert first
        double z = (view->is3DView()) ? myLookFrom.z() : view->getChanger().zoom2ZPos(myZoom);
        if (view->is3DView() && !myZCoordSet) { // set view angle to ground to at least 45 degrees if no Z coordinate is given
            z = myLookFrom.distanceTo2D(myLookAt) * sin(PI * 0.25);
        }
        Position lookFrom(myLookFrom.x(), myLookFrom.y(), z);
        view->setViewportFromToRot(lookFrom, myLookAt, myRotation);
        if (view->is3DView() && !myZCoordSet) {
            view->recenterView();
        }
    }
}


void
GUISettingsHandler::setSnapshots(GUISUMOAbstractView* view) const {
    if (!mySnapshots.empty()) {
        for (auto item : mySnapshots) {
            for (auto file : item.second) {
                view->addSnapshot(item.first, file);
            }
        }
    }
}


bool
GUISettingsHandler::hasDecals() const {
    return !myDecals.empty();
}


const std::vector<GUISUMOAbstractView::Decal>&
GUISettingsHandler::getDecals() const {
    return myDecals;
}


double
GUISettingsHandler::getDelay() const {
    return myDelay;
}


std::vector<SUMOTime>
GUISettingsHandler::loadBreakpoints(const std::string& file) {
    std::vector<SUMOTime> result;
    std::ifstream strm(file.c_str());
    if (!strm.good()) {
        WRITE_ERRORF(TL("Could not open '%'."), file);
        return result;
    }
    while (strm.good()) {
        std::string val;
        strm >> val;
        if (val.length() == 0) {
            continue;
        }
        try {
            SUMOTime value = string2time(val);
            result.push_back(value);
        } catch (NumberFormatException& e) {
            WRITE_ERRORF(TL("A breakpoint value must be a time description (%)."), toString(e.what()));
        } catch (EmptyData&) {
        } catch (ProcessError&) {
            WRITE_ERRORF(TL("Could not decode breakpoint '%'."), val);
        }
    }
    return result;
}


RandomDistributor<std::string>
GUISettingsHandler::getEventDistribution(const std::string& id) {
    RandomDistributor<std::string> result = myEventDistributions[id];
    if (result.getOverallProb() > 0 && result.getOverallProb() < 1) {
        // unscaled probabilities are assumed, fill up with dummy event
        result.add("", 1. - result.getOverallProb());
    }
    return result;
}


/****************************************************************************/
