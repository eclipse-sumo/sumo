/****************************************************************************/
/// @file    GUIPropertyScheme.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Mon, 20.07.2009
/// @version $Id$
///
//
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
#ifndef GUIPropertyScheme_h
#define GUIPropertyScheme_h


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
 * @class GUIPropertyScheme
 * This class provides a mapping from real values to properties (mainly colors).
 * Each color is stored along with a threshold value.
 * Color values between thresholds are obtained by interpolation
 */

template<class T>
class GUIPropertyScheme {
public:
    /// Constructor
    GUIPropertyScheme(const std::string& name, const T& baseColor,
                      const std::string& colName = "", const bool isFixed = false, SUMOReal baseValue = 0) :
        myName(name), myIsInterpolated(!isFixed),
        myIsFixed(isFixed),
        myAllowNegativeValues(false) {
        addColor(baseColor, baseValue, colName);
    }

    void setThreshold(const int pos, const SUMOReal threshold) {
        myThresholds[pos] = threshold;
    }

    void setColor(const int pos, const T& color) {
        myColors[pos] = color;
    }

    bool setColor(const std::string& name, const T& color) {
        std::vector<std::string>::iterator nameIt = myNames.begin();
        typename std::vector<T>::iterator colIt = myColors.begin();
        for (; nameIt != myNames.end(); ++nameIt, ++colIt) {
            if (*nameIt == name) {
                (*colIt) = color;
                return true;
            }
        }
        return false;
    }

    int addColor(const T& color, const SUMOReal threshold, const std::string& name = "") {
        typename std::vector<T>::iterator colIt = myColors.begin();
        std::vector<SUMOReal>::iterator threshIt = myThresholds.begin();
        std::vector<std::string>::iterator nameIt = myNames.begin();
        int pos = 0;
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

    void removeColor(const int pos) {
        assert(pos < (int)myColors.size());
        myColors.erase(myColors.begin() + pos);
        myThresholds.erase(myThresholds.begin() + pos);
        myNames.erase(myNames.begin() + pos);
    }

    void clear() {
        myColors.clear();
        myThresholds.clear();
        myNames.clear();
    }

    const T getColor(const SUMOReal value) const {
        if (myColors.size() == 1 || value < myThresholds.front()) {
            return myColors.front();
        }
        typename std::vector<T>::const_iterator colIt = myColors.begin() + 1;
        std::vector<SUMOReal>::const_iterator threshIt = myThresholds.begin() + 1;
        while (threshIt != myThresholds.end() && (*threshIt) <= value) {
            ++threshIt;
            ++colIt;
        }
        if (threshIt == myThresholds.end()) {
            return myColors.back();
        }
        if (!myIsInterpolated) {
            return *(colIt - 1);
        }
        SUMOReal lowVal = *(threshIt - 1);
        return interpolate(*(colIt - 1), *colIt, (value - lowVal) / ((*threshIt) - lowVal));
    }

    void setInterpolated(const bool interpolate, SUMOReal interpolationStart = 0.f) {
        myIsInterpolated = interpolate;
        if (interpolate) {
            myThresholds[0] = interpolationStart;
        }
    }

    const std::string& getName() const {
        return myName;
    }

    const std::vector<T>& getColors() const {
        return myColors;
    }

    const std::vector<SUMOReal>& getThresholds() const {
        return myThresholds;
    }

    bool isInterpolated() const {
        return myIsInterpolated;
    }

    const std::vector<std::string>& getNames() const {
        return myNames;
    }

    bool isFixed() const {
        return myIsFixed;
    }

    bool allowsNegativeValues() const {
        return myAllowNegativeValues;
    }

    void setAllowsNegativeValues(bool value) {
        myAllowNegativeValues = value;
    }

    void save(OutputDevice& dev) const {
        const std::string tag = getTagName(myColors);

        dev.openTag(tag);
        dev.writeAttr(SUMO_ATTR_NAME, myName);
        if (!myIsFixed) {
            dev.writeAttr(SUMO_ATTR_INTERPOLATED, myIsInterpolated);
        }
        typename std::vector<T>::const_iterator colIt = myColors.begin();
        std::vector<SUMOReal>::const_iterator threshIt = myThresholds.begin();
        std::vector<std::string>::const_iterator nameIt = myNames.begin();
        while (threshIt != myThresholds.end()) {
            dev.openTag(SUMO_TAG_ENTRY);
            dev.writeAttr(SUMO_ATTR_COLOR, *colIt);
            if (!myIsFixed) {
                dev.writeAttr(SUMO_ATTR_THRESHOLD, *threshIt);
            }
            if ((*nameIt) != "") {
                dev.writeAttr(SUMO_ATTR_NAME, *nameIt);
            }
            dev.closeTag();
            ++threshIt;
            ++colIt;
            ++nameIt;
        }
        dev.closeTag();
    }

    bool operator==(const GUIPropertyScheme& c) const {
        return myName == c.myName && myColors == c.myColors && myThresholds == c.myThresholds && myIsInterpolated == c.myIsInterpolated;
    }


    /// @brief specializations for GUIColorScheme
    RGBColor interpolate(const RGBColor& min, const RGBColor& max, SUMOReal weight) const {
        return RGBColor::interpolate(min, max, weight);
    }

    std::string getTagName(std::vector<RGBColor>) const {
        return toString(SUMO_TAG_COLORSCHEME);
    }


    /// @brief specializations for GUIScaleScheme
    SUMOReal interpolate(const SUMOReal& min, const SUMOReal& max, SUMOReal weight) const {
        return min + (max - min) * weight;
    }

    std::string getTagName(std::vector<SUMOReal>) const {
        return toString(SUMO_TAG_SCALINGSCHEME);
    }


private:
    std::string myName;
    std::vector<T> myColors;
    std::vector<SUMOReal> myThresholds;
    bool myIsInterpolated;
    std::vector<std::string> myNames;
    bool myIsFixed;
    bool myAllowNegativeValues;

};

typedef GUIPropertyScheme<RGBColor> GUIColorScheme;
typedef GUIPropertyScheme<SUMOReal> GUIScaleScheme;

#endif

/****************************************************************************/
