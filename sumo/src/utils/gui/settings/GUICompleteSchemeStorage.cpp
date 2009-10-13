/****************************************************************************/
/// @file    GUICompleteSchemeStorage.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-01-09
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
GUICompleteSchemeStorage::add(GUIVisualizationSettings &scheme) {
    std::string name = scheme.name;
    if (std::find(mySortedSchemeNames.begin(), mySortedSchemeNames.end(), name)==mySortedSchemeNames.end()) {
        mySortedSchemeNames.push_back(name);
    }
    mySettings[name] = scheme;
}


GUIVisualizationSettings &
GUICompleteSchemeStorage::get(const std::string &name) {
    return mySettings.find(name)->second;
}


GUIVisualizationSettings &
GUICompleteSchemeStorage::getDefault() {
    return mySettings.find(myDefaultSettingName)->second;
}


bool
GUICompleteSchemeStorage::contains(const std::string &name) const {
    return mySettings.find(name)!=mySettings.end();
}


void
GUICompleteSchemeStorage::remove(const std::string &name) {
    if (!contains(name)) {
        return;
    }
    mySortedSchemeNames.erase(find(mySortedSchemeNames.begin(), mySortedSchemeNames.end(), name));
    mySettings.erase(mySettings.find(name));
}


void
GUICompleteSchemeStorage::setDefault(const std::string &name) {
    if (!contains(name)) {
        return;
    }
    myDefaultSettingName = name;
}


const std::vector<std::string> &
GUICompleteSchemeStorage::getNames() const {
    return mySortedSchemeNames;
}


size_t
GUICompleteSchemeStorage::getNumInitialSettings() const {
    return myNumInitialSettings;
}


RGBColor
convert(const FXColor c) {
    return RGBColor(
               (SUMOReal) FXREDVAL(c) / (SUMOReal) 255.,
               (SUMOReal) FXGREENVAL(c) / (SUMOReal) 255.,
               (SUMOReal) FXBLUEVAL(c) / (SUMOReal) 255.);
}


void
GUICompleteSchemeStorage::init(FXApp *app) {
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
        vs.backgroundColor = RGBColor((SUMOReal) .2, (SUMOReal) .5, (SUMOReal) .2);
        vs.laneShowBorders = true;
        vs.hideConnectors = true;
        vs.minVehicleSize = 0;
        gSchemeStorage.add(vs);
    }
    myNumInitialSettings = mySortedSchemeNames.size();
    // add saved settings
    int noSaved = app->reg().readIntEntry("VisualizationSettings", "settingNo", 0);
    for (int i=0; i<noSaved; ++i) {
        std::string name = "visset#" + toString(i);
        std::string setting = app->reg().readStringEntry("VisualizationSettings",name.c_str(), "");
        if (setting!="") {
            size_t j, k;
            GUIVisualizationSettings vs;

            vs.name = setting;
            app->reg().readStringEntry("VisualizationSettings",name.c_str(), "");

            // add saved xml setting
            int xmlSize = app->reg().readIntEntry(name.c_str(), "xmlSize", 0);
            std::string content = "";
            int index = 0;
            while (xmlSize > 0) {
                std::string part = app->reg().readStringEntry(name.c_str(), ("xml"+toString(index)).c_str(), "");
                if (part == "") {
                    break;
                }
                content += part;
                xmlSize -= part.size();
                index++;
            }
            if (content != "" && xmlSize == 0) {
                GUISettingsHandler handler(content, false);
                handler.addSettings();
            }
        }
    }
    myDefaultSettingName = mySortedSchemeNames[0];
    myX = myY = myZoom = 0;
}


void
GUICompleteSchemeStorage::writeSettings(FXApp *app) throw() {
    const std::vector<std::string> &names = getNames();
    app->reg().writeIntEntry("VisualizationSettings", "settingNo", (FXint) names.size()-myNumInitialSettings);
    size_t gidx = 0;
    for (std::vector<std::string>::const_iterator i=names.begin()+myNumInitialSettings; i!=names.end(); ++i, ++gidx) {
        const GUIVisualizationSettings &item = mySettings.find(*i)->second;
        std::string sname = "visset#" + toString(gidx);

        app->reg().writeStringEntry("VisualizationSettings", sname.c_str(), item.name.c_str());
        OutputDevice_String dev;
        item.save(dev);
        std::string content = dev.getString();
        app->reg().writeIntEntry(sname.c_str(), "xmlSize", content.size());
        const unsigned maxSize = 1900; // this is a fox limitation for windows registry entries
        for (unsigned int i = 0; i < content.size(); i+=maxSize) {
            const std::string b = content.substr(i, maxSize);
            app->reg().writeStringEntry(sname.c_str(), ("xml" + toString(i/maxSize)).c_str(), b.c_str());
        }
    }
}


void
GUICompleteSchemeStorage::saveViewport(const SUMOReal x, const SUMOReal y, const SUMOReal zoom) {
    myX = x;
    myY = y;
    myZoom = zoom;
}


void
GUICompleteSchemeStorage::setViewport(GUISUMOAbstractView* view) throw() {
    if (myZoom > 0) {
        view->setViewport(myZoom, myX, myY);
    }
}


/****************************************************************************/

