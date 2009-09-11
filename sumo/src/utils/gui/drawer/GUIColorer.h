/****************************************************************************/
/// @file    GUIColorer.h
/// @author  Michael Behrisch
/// @date    Mon, 20.07.2009
/// @version $Id$
///
//
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
#ifndef GUIColorer_h
#define GUIColorer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <fx.h>
#include <utils/foxtools/FXRealSpinDial.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


// ===========================================================================
// class definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * definition of GUIColorScheme
 * ----------------------------------------------------------------------- */
/**
 * @class GUIColorScheme
 * @brief
 */
class GUIColorScheme {
public:
    /// Constructor
    GUIColorScheme(const std::string& name, const RGBColor& baseColor,
                   const std::string& colName="", const bool isFixed=false)
            : myName(name), myIsInterpolated(false), myIsFixed(isFixed) {
        addColor(baseColor, -1, colName);
    }

    unsigned int setColor(const size_t pos, const RGBColor& color, const SUMOReal threshold) {
        const std::string& name = myNames[pos];
        removeColor(pos);
        return addColor(color, threshold, name);
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

    const RGBColor getColor(const SUMOReal value) const {
        if (myColors.size() == 1 || value <= myThresholds.front()) {
            return myColors.front();
        }
        std::vector<RGBColor>::const_iterator colIt = myColors.begin()+1;
        std::vector<SUMOReal>::const_iterator threshIt = myThresholds.begin()+1;
        while (threshIt != myThresholds.end() && (*threshIt) < value) {
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

    const bool isInterpolated() const {
        return myIsInterpolated;
    }

    const std::vector<std::string> &getNames() const {
        return myNames;
    }

    const bool isFixed() const {
        return myIsFixed;
    }

    void save(OutputDevice &dev) const {
        dev << "            <colorScheme name=\"" << myName << "\" interpolated=\"" << myIsInterpolated << "\" fixed=\"" << myIsFixed << "\">\n";
        std::vector<RGBColor>::const_iterator colIt = myColors.begin();
        std::vector<SUMOReal>::const_iterator threshIt = myThresholds.begin();
        std::vector<std::string>::const_iterator nameIt = myNames.begin();
        while (threshIt != myThresholds.end()) {
            dev << "                <color rgb=\"" << (*colIt) << "\" threshold=\"" << (*threshIt);
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


/* -------------------------------------------------------------------------
 * definition of GUIColorer
 * ----------------------------------------------------------------------- */
/**
 * @class GUIColorer
 * @brief Base class for coloring. Allows changing the used colors and sets
 *  the used color in dependence to a value or a given structure.
 */
template<class T>
class GUIColorer {
public:
    /// @brief Constructor
    GUIColorer() : myActiveScheme(0) { }

    /// @brief Destructor
    virtual ~GUIColorer() { }

    /// @brief Sets the color using a value from the given instance of T
    virtual SUMOReal getColorValue(const T& i) const = 0;

    /// @brief Sets the color using a value from the given instance of T
    void setGlColor(const T& i) const {
        const RGBColor& c = mySchemes[myActiveScheme].getColor(getColorValue(i));
        glColor3d(c.red(), c.green(), c.blue());
    }

    /// @brief Fills the given combobox with the names of available colorings
    void fill(FXComboBox &cb) {
        typename std::vector<GUIColorScheme>::iterator i;
        for (i=mySchemes.begin(); i!=mySchemes.end(); ++i) {
            cb.appendItem((*i).getName().c_str());
        }
        cb.setCurrentItem(myActiveScheme);
    }

    void setActive(size_t scheme) {
        myActiveScheme = scheme;
    }

    const size_t getActive() const {
        return myActiveScheme;
    }

    GUIColorScheme& getScheme() {
        return mySchemes[myActiveScheme];
    }

    void save(OutputDevice &dev) const {
        typename std::vector<GUIColorScheme>::const_iterator i = mySchemes.begin();
        for (; i!=mySchemes.end(); ++i) {
            i->save(dev);
        }
    }

    bool operator==(const GUIColorer &c) const {
        return myActiveScheme == c.myActiveScheme && mySchemes == c.mySchemes;
    }

protected:
    size_t myActiveScheme;
    std::vector<GUIColorScheme> mySchemes;

};


#endif

/****************************************************************************/
