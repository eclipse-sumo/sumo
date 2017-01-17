/****************************************************************************/
/// @file    GUISettingsHandler.cpp
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @date    Fri, 24. Apr 2009
/// @version $Id$
///
// The dialog to change the view (gui) settings.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <utils/common/TplConvert.h>
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUISettingsHandler::GUISettingsHandler(const std::string& content, bool isFile)
    : SUMOSAXHandler(content), myDelay(-1), myLookFrom(-1, -1, -1), myLookAt(-1, -1, -1),
      myCurrentColorer(SUMO_TAG_NOTHING), myCurrentScheme(0), myJamSoundTime(-1) {
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
GUISettingsHandler::myStartElement(int element,
                                   const SUMOSAXAttributes& attrs) {
    bool ok = true;
    switch (element) {
        case SUMO_TAG_BREAKPOINTS_FILE: {
            std::string file = attrs.get<std::string>(SUMO_ATTR_VALUE, 0, ok);
            myBreakpoints = loadBreakpoints(file);
        }
        break;
        case SUMO_TAG_VIEWSETTINGS:
            myViewType = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, 0, ok, "default");
            std::transform(myViewType.begin(), myViewType.end(), myViewType.begin(), tolower);
            break;
        case SUMO_TAG_DELAY:
            myDelay = attrs.getOpt<SUMOReal>(SUMO_ATTR_VALUE, 0, ok, myDelay);
            break;
        case SUMO_TAG_VIEWPORT: {
            const SUMOReal x = attrs.getOpt<SUMOReal>(SUMO_ATTR_X, 0, ok, myLookFrom.x());
            const SUMOReal y = attrs.getOpt<SUMOReal>(SUMO_ATTR_Y, 0, ok, myLookFrom.y());
            const SUMOReal z = attrs.getOpt<SUMOReal>(SUMO_ATTR_ZOOM, 0, ok, myLookFrom.z());
            myLookFrom.set(x, y, z);
            const SUMOReal cx = attrs.getOpt<SUMOReal>(SUMO_ATTR_CENTER_X, 0, ok, myLookAt.x());
            const SUMOReal cy = attrs.getOpt<SUMOReal>(SUMO_ATTR_CENTER_Y, 0, ok, myLookAt.y());
            const SUMOReal cz = attrs.getOpt<SUMOReal>(SUMO_ATTR_CENTER_Z, 0, ok, myLookAt.z());
            myLookAt.set(cx, cy, cz);
            break;
        }
        case SUMO_TAG_SNAPSHOT: {
            bool ok = true;
            std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, 0, ok);
            if (file != "" && !FileHelpers::isAbsolute(file)) {
                file = FileHelpers::getConfigurationRelative(getFileName(), file);
            }
            mySnapshots[attrs.getOptSUMOTimeReporting(SUMO_ATTR_TIME, file.c_str(), ok, 0)] = file;
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_SCHEME: {
            bool ok = true;
            mySettings.name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, 0, ok, mySettings.name);
            if (gSchemeStorage.contains(mySettings.name)) {
                mySettings = gSchemeStorage.get(mySettings.name);
            }
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_OPENGL:
            mySettings.antialiase = TplConvert::_2bool(attrs.getStringSecure("antialiase", toString(mySettings.antialiase)).c_str());
            mySettings.dither = TplConvert::_2bool(attrs.getStringSecure("dither", toString(mySettings.dither)).c_str());
            break;
        case SUMO_TAG_VIEWSETTINGS_BACKGROUND: {
            bool ok = true;
            mySettings.backgroundColor = RGBColor::parseColorReporting(attrs.getStringSecure("backgroundColor", toString(mySettings.backgroundColor)), "background", 0, true, ok);
            mySettings.showGrid = TplConvert::_2bool(attrs.getStringSecure("showGrid", toString(mySettings.showGrid)).c_str());
            mySettings.gridXSize = TplConvert::_2SUMOReal(attrs.getStringSecure("gridXSize", toString(mySettings.gridXSize)).c_str());
            mySettings.gridYSize = TplConvert::_2SUMOReal(attrs.getStringSecure("gridYSize", toString(mySettings.gridYSize)).c_str());
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_EDGES: {
            int laneEdgeMode = TplConvert::_2int(attrs.getStringSecure("laneEdgeMode", "0").c_str());
            int laneEdgeScaleMode = TplConvert::_2int(attrs.getStringSecure("scaleMode", "0").c_str());
            mySettings.laneShowBorders = TplConvert::_2bool(attrs.getStringSecure("laneShowBorders", toString(mySettings.laneShowBorders)).c_str());
            mySettings.showLinkDecals = TplConvert::_2bool(attrs.getStringSecure("showLinkDecals", toString(mySettings.showLinkDecals)).c_str());
            mySettings.showLinkRules = TplConvert::_2bool(attrs.getStringSecure("showLinkRules", toString(mySettings.showLinkRules)).c_str());
            mySettings.showRails = TplConvert::_2bool(attrs.getStringSecure("showRails", toString(mySettings.showRails)).c_str());
            mySettings.edgeName = parseTextSettings("edgeName", attrs, mySettings.edgeName);
            mySettings.internalEdgeName = parseTextSettings("internalEdgeName", attrs, mySettings.internalEdgeName);
            mySettings.cwaEdgeName = parseTextSettings("cwaEdgeName", attrs, mySettings.cwaEdgeName);
            mySettings.streetName = parseTextSettings("streetName", attrs, mySettings.streetName);
            mySettings.hideConnectors = TplConvert::_2bool(attrs.getStringSecure("hideConnectors", toString(mySettings.hideConnectors)).c_str());
            mySettings.laneWidthExaggeration = TplConvert::_2SUMOReal(attrs.getStringSecure("widthExaggeration", toString(mySettings.laneWidthExaggeration)).c_str());
            mySettings.laneMinSize = TplConvert::_2SUMOReal(attrs.getStringSecure("minSize", toString(mySettings.laneWidthExaggeration)).c_str());
            mySettings.showLaneDirection = TplConvert::_2bool(attrs.getStringSecure("showDirection", toString(mySettings.showLaneDirection)).c_str());
            mySettings.showSublanes = TplConvert::_2bool(attrs.getStringSecure("showSublanes", toString(mySettings.showSublanes)).c_str());
            myCurrentColorer = element;
            mySettings.edgeColorer.setActive(laneEdgeMode);
            mySettings.edgeScaler.setActive(laneEdgeScaleMode);
            mySettings.laneColorer.setActive(laneEdgeMode);
            mySettings.laneScaler.setActive(laneEdgeScaleMode);
        }
        break;
        case SUMO_TAG_COLORSCHEME:
            myCurrentScheme = 0;
            myCurrentScaleScheme = 0;
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_EDGES) {
                myCurrentScheme = mySettings.laneColorer.getSchemeByName(attrs.getStringSecure(SUMO_ATTR_NAME, ""));
                if (myCurrentScheme == 0) {
                    myCurrentScheme = mySettings.edgeColorer.getSchemeByName(attrs.getStringSecure(SUMO_ATTR_NAME, ""));
                }
            }
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_VEHICLES) {
                myCurrentScheme = mySettings.vehicleColorer.getSchemeByName(attrs.getStringSecure(SUMO_ATTR_NAME, ""));
            }
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_PERSONS) {
                myCurrentScheme = mySettings.personColorer.getSchemeByName(attrs.getStringSecure(SUMO_ATTR_NAME, ""));
            }
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_JUNCTIONS) {
                myCurrentScheme = mySettings.junctionColorer.getSchemeByName(attrs.getStringSecure(SUMO_ATTR_NAME, ""));
            }
            if (myCurrentScheme && !myCurrentScheme->isFixed()) {
                bool ok = true;
                myCurrentScheme->setInterpolated(attrs.getOpt<bool>(SUMO_ATTR_INTERPOLATED, 0, ok, false));
                myCurrentScheme->clear();
            }
            break;
        case SUMO_TAG_SCALINGSCHEME:
            myCurrentScheme = 0;
            myCurrentScaleScheme = 0;
            if (myCurrentColorer == SUMO_TAG_VIEWSETTINGS_EDGES) {
                myCurrentScaleScheme = mySettings.laneScaler.getSchemeByName(attrs.getStringSecure(SUMO_ATTR_NAME, ""));
                if (myCurrentScaleScheme == 0) {
                    myCurrentScaleScheme = mySettings.edgeScaler.getSchemeByName(attrs.getStringSecure(SUMO_ATTR_NAME, ""));
                }
            }
            if (myCurrentScaleScheme && !myCurrentScaleScheme->isFixed()) {
                bool ok = true;
                myCurrentScaleScheme->setInterpolated(attrs.getOpt<bool>(SUMO_ATTR_INTERPOLATED, 0, ok, false));
                myCurrentScaleScheme->clear();
            }
            break;

        case SUMO_TAG_ENTRY:
            if (myCurrentScheme != 0) {
                bool ok = true;
                RGBColor color = attrs.get<RGBColor>(SUMO_ATTR_COLOR, 0, ok);
                if (myCurrentScheme->isFixed()) {
                    myCurrentScheme->setColor(attrs.getStringSecure(SUMO_ATTR_NAME, ""), color);
                } else {
                    myCurrentScheme->addColor(color, attrs.getOpt<SUMOReal>(SUMO_ATTR_THRESHOLD, 0, ok, 0));
                }
            } else if (myCurrentScaleScheme != 0) {
                bool ok = true;
                SUMOReal scale = attrs.get<SUMOReal>(SUMO_ATTR_COLOR, 0, ok);
                if (myCurrentScaleScheme->isFixed()) {
                    myCurrentScaleScheme->setColor(attrs.getStringSecure(SUMO_ATTR_NAME, ""), scale);
                } else {
                    myCurrentScaleScheme->addColor(scale, attrs.getOpt<SUMOReal>(SUMO_ATTR_THRESHOLD, 0, ok, 0));
                }
            }
            break;
        case SUMO_TAG_VIEWSETTINGS_VEHICLES:
            mySettings.vehicleColorer.setActive(TplConvert::_2int(attrs.getStringSecure("vehicleMode", "0").c_str()));
            mySettings.vehicleQuality = TplConvert::_2int(attrs.getStringSecure("vehicleQuality", toString(mySettings.vehicleQuality)).c_str());
            mySettings.showBlinker = TplConvert::_2bool(attrs.getStringSecure("showBlinker", toString(mySettings.showBlinker)).c_str());
            mySettings.vehicleSize = parseSizeSettings("vehicle", attrs, mySettings.vehicleSize);
            mySettings.vehicleName = parseTextSettings("vehicleName", attrs, mySettings.vehicleName);
            myCurrentColorer = element;
            break;
        case SUMO_TAG_VIEWSETTINGS_PERSONS:
            mySettings.personColorer.setActive(TplConvert::_2int(attrs.getStringSecure("personMode", "0").c_str()));
            mySettings.personQuality = TplConvert::_2int(attrs.getStringSecure("personQuality", toString(mySettings.personQuality)).c_str());
            mySettings.personSize = parseSizeSettings("person", attrs, mySettings.personSize);
            mySettings.personName = parseTextSettings("personName", attrs, mySettings.personName);
            myCurrentColorer = element;
            break;
        case SUMO_TAG_VIEWSETTINGS_CONTAINERS:
            mySettings.containerColorer.setActive(TplConvert::_2int(attrs.getStringSecure("containerMode", "0").c_str()));
            mySettings.containerQuality = TplConvert::_2int(attrs.getStringSecure("containerQuality", toString(mySettings.containerQuality)).c_str());
            mySettings.containerSize = parseSizeSettings("container", attrs, mySettings.containerSize);
            mySettings.containerName = parseTextSettings("containerName", attrs, mySettings.containerName);
            myCurrentColorer = element;
            break;
        case SUMO_TAG_VIEWSETTINGS_JUNCTIONS:
            mySettings.junctionColorer.setActive(TplConvert::_2int(attrs.getStringSecure("junctionMode", "0").c_str()));
            mySettings.drawLinkTLIndex = parseTextSettings("drawLinkTLIndex", attrs, mySettings.drawLinkTLIndex);
            mySettings.drawLinkJunctionIndex = parseTextSettings("drawLinkJunctionIndex", attrs, mySettings.drawLinkJunctionIndex);
            mySettings.junctionName = parseTextSettings("junctionName", attrs, mySettings.junctionName);
            mySettings.internalJunctionName = parseTextSettings("internalJunctionName", attrs, mySettings.internalJunctionName);
            mySettings.showLane2Lane = TplConvert::_2bool(attrs.getStringSecure("showLane2Lane", toString(mySettings.showLane2Lane)).c_str());
            mySettings.drawJunctionShape = TplConvert::_2bool(attrs.getStringSecure("drawShape", toString(mySettings.drawJunctionShape)).c_str());
            mySettings.drawCrossingsAndWalkingareas = TplConvert::_2bool(attrs.getStringSecure(
                        "drawCrossingsAndWalkingareas", toString(mySettings.drawCrossingsAndWalkingareas)).c_str());
            mySettings.junctionSize = parseSizeSettings("junction", attrs, mySettings.junctionSize);
            myCurrentColorer = element;
            break;
        case SUMO_TAG_VIEWSETTINGS_ADDITIONALS:
            mySettings.addMode = TplConvert::_2int(attrs.getStringSecure("addMode", toString(mySettings.addMode)).c_str());
            mySettings.addSize = parseSizeSettings("add", attrs, mySettings.addSize);
            mySettings.addName = parseTextSettings("addName", attrs, mySettings.addName);
            break;
        case SUMO_TAG_VIEWSETTINGS_POIS:
            mySettings.poiSize = parseSizeSettings("poi", attrs, mySettings.poiSize);
            mySettings.poiName = parseTextSettings("poiName", attrs, mySettings.poiName);
            break;
        case SUMO_TAG_VIEWSETTINGS_POLYS:
            mySettings.polySize = parseSizeSettings("poly", attrs, mySettings.polySize);
            mySettings.polyName = parseTextSettings("polyName", attrs, mySettings.polyName);
            break;
        case SUMO_TAG_VIEWSETTINGS_LEGEND:
            mySettings.showSizeLegend = TplConvert::_2bool(attrs.getStringSecure("showSizeLegend", toString(mySettings.showSizeLegend)).c_str());
            break;
        case SUMO_TAG_VIEWSETTINGS_DECAL: {
            GUISUMOAbstractView::Decal d;
            d.filename = attrs.getStringSecure("filename", d.filename);
            if (d.filename != "" && !FileHelpers::isAbsolute(d.filename)) {
                d.filename = FileHelpers::getConfigurationRelative(getFileName(), d.filename);
            }
            d.centerX = attrs.getOpt<SUMOReal>(SUMO_ATTR_CENTER_X, 0, ok, d.centerX);
            d.centerY = attrs.getOpt<SUMOReal>(SUMO_ATTR_CENTER_Y, 0, ok, d.centerY);
            d.centerZ = attrs.getOpt<SUMOReal>(SUMO_ATTR_CENTER_Z, 0, ok, d.centerZ);
            d.width = attrs.getOpt<SUMOReal>(SUMO_ATTR_WIDTH, 0, ok, d.width);
            d.height = attrs.getOpt<SUMOReal>(SUMO_ATTR_HEIGHT, 0, ok, d.height);
            d.altitude = TplConvert::_2SUMOReal(attrs.getStringSecure("altitude", toString(d.height)).c_str());
            d.rot = TplConvert::_2SUMOReal(attrs.getStringSecure("rotation", toString(d.rot)).c_str());
            d.tilt = TplConvert::_2SUMOReal(attrs.getStringSecure("tilt", toString(d.tilt)).c_str());
            d.roll = TplConvert::_2SUMOReal(attrs.getStringSecure("roll", toString(d.roll)).c_str());
            d.layer = attrs.getOpt<SUMOReal>(SUMO_ATTR_LAYER, 0, ok, d.layer);
            d.screenRelative = TplConvert::_2bool(attrs.getStringSecure("screenRelative", toString(d.screenRelative)).c_str());
            d.initialised = false;
            myDecals.push_back(d);
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_LIGHT: {
            GUISUMOAbstractView::Decal d;
            d.filename = "light" + attrs.getOpt<std::string>(SUMO_ATTR_INDEX, 0, ok, "0");
            d.centerX = attrs.getOpt<SUMOReal>(SUMO_ATTR_CENTER_X, 0, ok, d.centerX);
            d.centerY = attrs.getOpt<SUMOReal>(SUMO_ATTR_CENTER_Y, 0, ok, d.centerY);
            d.centerZ = attrs.getOpt<SUMOReal>(SUMO_ATTR_CENTER_Z, 0, ok, d.centerZ);
            d.width = attrs.getOpt<SUMOReal>(SUMO_ATTR_WIDTH, 0, ok, d.width);
            d.height = attrs.getOpt<SUMOReal>(SUMO_ATTR_HEIGHT, 0, ok, d.height);
            d.altitude = TplConvert::_2SUMOReal(attrs.getStringSecure("altitude", toString(d.height)).c_str());
            d.rot = TplConvert::_2SUMOReal(attrs.getStringSecure("rotation", toString(d.rot)).c_str());
            d.tilt = TplConvert::_2SUMOReal(attrs.getStringSecure("tilt", toString(d.tilt)).c_str());
            d.roll = TplConvert::_2SUMOReal(attrs.getStringSecure("roll", toString(d.roll)).c_str());
            d.layer = attrs.getOpt<SUMOReal>(SUMO_ATTR_LAYER, 0, ok, d.layer);
            d.initialised = false;
            myDecals.push_back(d);
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_EVENT: {
            const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
            const std::string cmd = attrs.get<std::string>(SUMO_ATTR_COMMAND, 0, ok);
            const SUMOReal prob = attrs.get<SUMOReal>(SUMO_ATTR_PROB, id.c_str(), ok);
            myEventDistributions[id].add(prob, cmd);
        }
        break;
        case SUMO_TAG_VIEWSETTINGS_EVENT_JAM_TIME:
            myJamSoundTime = attrs.get<SUMOReal>(SUMO_ATTR_VALUE, 0, ok);
            break;
        default:
            break;
    }
}


GUIVisualizationTextSettings
GUISettingsHandler::parseTextSettings(
    const std::string& prefix, const SUMOSAXAttributes& attrs,
    GUIVisualizationTextSettings defaults) {
    bool ok = true;
    return GUIVisualizationTextSettings(
               TplConvert::_2bool(attrs.getStringSecure(prefix + "_show", toString(defaults.show)).c_str()),
               TplConvert::_2SUMOReal(attrs.getStringSecure(prefix + "_size", toString(defaults.size)).c_str()),
               RGBColor::parseColorReporting(attrs.getStringSecure(prefix + "_color", toString(defaults.color)), "edges", 0, true, ok));
}


GUIVisualizationSizeSettings
GUISettingsHandler::parseSizeSettings(
    const std::string& prefix, const SUMOSAXAttributes& attrs,
    GUIVisualizationSizeSettings defaults) {
    return GUIVisualizationSizeSettings(
               TplConvert::_2SUMOReal(attrs.getStringSecure(prefix + "_minSize", toString(defaults.minSize)).c_str()),
               TplConvert::_2SUMOReal(attrs.getStringSecure(prefix + "_exaggeration", toString(defaults.exaggeration)).c_str()),
               TplConvert::_2bool(attrs.getStringSecure(prefix + "_constantSize", toString(defaults.constantSize)).c_str()));
}


std::string
GUISettingsHandler::addSettings(GUISUMOAbstractView* view) const {
    if (mySettings.name != "") {
        gSchemeStorage.add(mySettings);
        if (view) {
            FXint index = view->getColoringSchemesCombo().appendItem(mySettings.name.c_str());
            view->getColoringSchemesCombo().setCurrentItem(index);
            view->setColorScheme(mySettings.name);
        }
    }
    return mySettings.name;
}


void
GUISettingsHandler::applyViewport(GUISUMOAbstractView* view) const {
    if (myLookFrom.z() > 0) {
        // z value stores zoom so we must convert first
        Position lookFrom(myLookFrom.x(), myLookFrom.y(), view->getChanger().zoom2ZPos(myLookFrom.z()));
        view->setViewportFromTo(lookFrom, myLookAt);
    }
}


void
GUISettingsHandler::setSnapshots(GUISUMOAbstractView* view) const {
    if (!mySnapshots.empty()) {
        view->setSnapshots(mySnapshots);
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


SUMOReal
GUISettingsHandler::getDelay() const {
    return myDelay;
}


std::vector<SUMOTime>
GUISettingsHandler::loadBreakpoints(const std::string& file) {
    std::vector<SUMOTime> result;
    std::ifstream strm(file.c_str());
    while (strm.good()) {
        std::string val;
        strm >> val;
        if (val.length() == 0) {
            continue;
        }
        try {
            SUMOTime value = string2time(val);
            result.push_back(value);
        } catch (NumberFormatException&) {
            WRITE_ERROR(" A breakpoint-value must be an int, is:" + val);
        }  catch (ProcessError&) {
            WRITE_ERROR(" Could not decode breakpoint '" + val + "'");
        } catch (EmptyData&) {}
    }
    return result;
}


RandomDistributor<std::string>
GUISettingsHandler::getEventDistribution(const std::string& id) {
    RandomDistributor<std::string> result = myEventDistributions[id];
    if (result.getOverallProb() > 0 && result.getOverallProb() < 1) {
        // unscaled probabilities are assumed, fill up with dummy event
        result.add(1 - result.getOverallProb(), "");
    }
    return result;
}


/****************************************************************************/

