/****************************************************************************/
/// @file    GUIColorScheme.h
/// @author  Michael Behrisch
/// @date    Mon, 20.07.2009
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIColorScheme_h
#define GUIColorScheme_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <vector>
#include <utils/common/RGBColor.h>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIColorScheme
 * @brief
 */
class GUIColorScheme {
public:
    /// Constructor
    GUIColorScheme(const std::string& name, const RGBColor& baseColor,
                   const std::string& colName="", const bool isFixed=false)
            : myName(name), myIsInterpolated(!isFixed), myIsFixed(isFixed) {
        addColor(baseColor, 0, colName);
    }

    void setThreshold(const size_t pos, const SUMOReal threshold) {
        myThresholds[pos] = threshold;
    }

    void setColor(const size_t pos, const RGBColor& color) {
        myColors[pos] = color;
    }

    bool setColor(const std::string& name, const RGBColor& color) {
        std::vector<std::string>::iterator nameIt = myNames.begin();
        std::vector<RGBColor>::iterator colIt = myColors.begin();
        for (; nameIt != myNames.end(); ++nameIt, ++colIt) {
            if (*nameIt == name) {
                (*colIt) = color;
                return true;
            }
        }
        return false;
    }

    unsigned int addColor(const RGBColor& color, const SUMOReal threshold, const std::string& name="") {
        std::vector<RGBColor>::iterator colIt = myColors.begin();
        std::vector<SUMOReal>::iterator threshIt = myThresholds.begin();
        std::vector<std::string>::iterator nameIt = myNames.begin();
        unsigned int pos = 0;
        while (threshIt != myThresholds.end() && (*threshIt) < threshold) {
            ++threshIt;
            ++colIt;
            ++nameIt;
            pos++;
        }
        myColors.insert(colIt, color);
        myThresholds.insert(threshIt, threshold);
        myNames.insert(nameIt, name);
        return pos;
    }

    void removeColor(const size_t pos) {
        assert(pos < myColors.size());
        myColors.erase(myColors.begin()+pos);
        myThresholds.erase(myThresholds.begin()+pos);
        myNames.erase(myNames.begin()+pos);
    }

    void clear() {
        myColors.clear();
        myThresholds.clear();
        myNames.clear();
    }

    const RGBColor getColor(const SUMOReal value) const {
        if (myColors.size() == 1 || value < myThresholds.front()) {
            return myColors.front();
        }
        std::vector<RGBColor>::const_iterator colIt = myColors.begin()+1;
        std::vector<SUMOReal>::const_iterator threshIt = myThresholds.begin()+1;
        while (threshIt != myThresholds.end() && (*threshIt) <= value) {
            ++threshIt;
            ++colIt;
        }
        if (threshIt == myThresholds.end()) {
            return myColors.back();
        }
        if (!myIsInterpolated) {
            return *(colIt-1);
        }
        SUMOReal lowVal = *(threshIt-1);
        return RGBColor::interpolate(*(colIt-1), *colIt, (value-lowVal)/((*threshIt)-lowVal));
    }

    void setInterpolated(const bool interpolate, SUMOReal interpolationStart=0.f) {
        myIsInterpolated = interpolate;
        if (interpolate) {
            myThresholds[0] = interpolationStart;
        }
    }

    const std::string &getName() const {
        return myName;
    }

    const std::vector<RGBColor> &getColors() const {
        return myColors;
    }

    const std::vector<SUMOReal> &getThresholds() const {
        return myThresholds;
    }

    bool isInterpolated() const {
        return myIsInterpolated;
    }

    const std::vector<std::string> &getNames() const {
        return myNames;
    }

    bool isFixed() const {
        return myIsFixed;
    }

    void save(OutputDevice &dev) const {
        dev << "            <colorScheme name=\"" << myName;
        if (!myIsFixed) {
            dev << "\" interpolated=\"" << myIsInterpolated;
        }
        dev << "\">\n";
        std::vector<RGBColor>::const_iterator colIt = myColors.begin();
        std::vector<SUMOReal>::const_iterator threshIt = myThresholds.begin();
        std::vector<std::string>::const_iterator nameIt = myNames.begin();
        while (threshIt != myThresholds.end()) {
            dev << "                <entry color=\"" << (*colIt);
            if (!myIsFixed) {
                dev << "\" threshold=\"" << (*threshIt);
            }
            if ((*nameIt) != "") {
                dev << "\" name=\"" << (*nameIt);
            }
            dev << "\"/>\n";
            ++threshIt;
            ++colIt;
            ++nameIt;
        }
        dev << "            </colorScheme>\n";
    }

    bool operator==(const GUIColorScheme &c) const {
        return myName == c.myName && myColors == c.myColors && myThresholds == c.myThresholds && myIsInterpolated == c.myIsInterpolated;
    }

private:
    std::string myName;
    std::vector<RGBColor> myColors;
    std::vector<SUMOReal> myThresholds;
    bool myIsInterpolated;
    std::vector<std::string> myNames;
    bool myIsFixed;

};

#endif

/****************************************************************************/
