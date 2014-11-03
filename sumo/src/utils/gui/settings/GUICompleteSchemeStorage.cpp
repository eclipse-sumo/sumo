/****************************************************************************/
/// @file    GUICompleteSchemeStorage.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    2006-01-09
/// @version $Id$
///
// Storage for available visualization settings
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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

#include "GUICompleteSchemeStorage.h"
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/RGBColor.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/iodevices/OutputDevice_String.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static variable definitions
// ===========================================================================
GUICompleteSchemeStorage gSchemeStorage;


// ===========================================================================
// method definitions
// ===========================================================================
GUICompleteSchemeStorage::GUICompleteSchemeStorage() { }


GUICompleteSchemeStorage::~GUICompleteSchemeStorage() { }



void
GUICompleteSchemeStorage::add(const GUIVisualizationSettings& scheme) {
    std::string name = scheme.name;
    if (std::find(mySortedSchemeNames.begin(), mySortedSchemeNames.end(), name) == mySortedSchemeNames.end()) {
        mySortedSchemeNames.push_back(name);
    }
    mySettings[name] = scheme;
}


GUIVisualizationSettings&
GUICompleteSchemeStorage::get(const std::string& name) {
    return mySettings.find(name)->second;
}


GUIVisualizationSettings&
GUICompleteSchemeStorage::getDefault() {
    return mySettings.find(myDefaultSettingName)->second;
}


bool
GUICompleteSchemeStorage::contains(const std::string& name) const {
    return mySettings.find(name) != mySettings.end();
}


void
GUICompleteSchemeStorage::remove(const std::string& name) {
    if (!contains(name)) {
        return;
    }
    mySortedSchemeNames.erase(find(mySortedSchemeNames.begin(), mySortedSchemeNames.end(), name));
    mySettings.erase(mySettings.find(name));
}


void
GUICompleteSchemeStorage::setDefault(const std::string& name) {
    if (!contains(name)) {
        return;
    }
    myDefaultSettingName = name;
}


const std::vector<std::string>&
GUICompleteSchemeStorage::getNames() const {
    return mySortedSchemeNames;
}


unsigned int
GUICompleteSchemeStorage::getNumInitialSettings() const {
    return myNumInitialSettings;
}


void
GUICompleteSchemeStorage::init(FXApp* app) {
    {
        GUIVisualizationSettings vs;
        vs.name = "standard";
        gSchemeStorage.add(vs);
    }
    {
        GUIVisualizationSettings vs;
        vs.name = "faster standard";
        vs.showLinkDecals = false;
        vs.showRails = false;
        gSchemeStorage.add(vs);
    }
    {
        GUIVisualizationSettings vs;
        vs.name = "real world";
        vs.vehicleQuality = 2;
        vs.backgroundColor = RGBColor(51, 128, 51, 255);
        vs.laneShowBorders = true;
        vs.hideConnectors = true;
        vs.vehicleSize.minSize = 0;
        vs.personQuality = 2;
        gSchemeStorage.add(vs);
    }
    myNumInitialSettings = (unsigned int) mySortedSchemeNames.size();
    // add saved settings
    int noSaved = app->reg().readIntEntry("VisualizationSettings", "settingNo", 0);
    for (int i = 0; i < noSaved; ++i) {
        std::string name = "visset#" + toString(i);
        std::string setting = app->reg().readStringEntry("VisualizationSettings", name.c_str(), "");
        if (setting != "") {
            GUIVisualizationSettings vs;

            vs.name = setting;
            app->reg().readStringEntry("VisualizationSettings", name.c_str(), "");

            // add saved xml setting
            int xmlSize = app->reg().readIntEntry(name.c_str(), "xmlSize", 0);
            std::string content = "";
            int index = 0;
            while (xmlSize > 0) {
                std::string part = app->reg().readStringEntry(name.c_str(), ("xml" + toString(index)).c_str(), "");
                if (part == "") {
                    break;
                }
                content += part;
                xmlSize -= (int) part.size();
                index++;
            }
            if (content != "" && xmlSize == 0) {
                try {
                    GUISettingsHandler handler(content, false);
                    handler.addSettings();
                } catch (ProcessError) { }
            }
        }
    }
    myDefaultSettingName = mySortedSchemeNames[0];
    myLookFrom.set(0, 0, 0);
}


void
GUICompleteSchemeStorage::writeSettings(FXApp* app) {
    const std::vector<std::string>& names = getNames();
    app->reg().writeIntEntry("VisualizationSettings", "settingNo", (FXint) names.size() - myNumInitialSettings);
    size_t gidx = 0;
    for (std::vector<std::string>::const_iterator i = names.begin() + myNumInitialSettings; i != names.end(); ++i, ++gidx) {
        const GUIVisualizationSettings& item = mySettings.find(*i)->second;
        std::string sname = "visset#" + toString(gidx);

        app->reg().writeStringEntry("VisualizationSettings", sname.c_str(), item.name.c_str());
        OutputDevice_String dev;
        item.save(dev);
        std::string content = dev.getString();
        app->reg().writeIntEntry(sname.c_str(), "xmlSize", (FXint)(content.size()));
        const unsigned maxSize = 1500; // this is a fox limitation for registry entries
        for (unsigned int i = 0; i < content.size(); i += maxSize) {
            const std::string b = content.substr(i, maxSize);
            app->reg().writeStringEntry(sname.c_str(), ("xml" + toString(i / maxSize)).c_str(), b.c_str());
        }
    }
}


void
GUICompleteSchemeStorage::saveViewport(const SUMOReal x, const SUMOReal y, const SUMOReal zoom) {
    myLookFrom.set(x, y, zoom);
}


void
GUICompleteSchemeStorage::setViewport(GUISUMOAbstractView* view) {
    if (myLookFrom.z() > 0) {
        view->setViewport(myLookFrom, myLookAt);
    } else {
        view->recenterView();
    }
}


/****************************************************************************/

