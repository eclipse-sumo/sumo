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
                   const bool interpolate=false)
                   : myName(name), myIsInterpolated(interpolate) {
        addColor(baseColor, -1);
    }

    void setColor(const size_t pos, const RGBColor& color, const SUMOReal threshold) {
        assert(pos < myColors.size());
        myColors[pos] = color;
        myThresholds[pos] = threshold;
    }

    void addColor(const RGBColor& color, const SUMOReal threshold) {
        std::vector<RGBColor>::iterator colIt = myColors.begin();
        std::vector<SUMOReal>::iterator threshIt = myThresholds.begin();
        while (threshIt != myThresholds.end() && (*threshIt) < threshold) {
            threshIt++;
            colIt++;
        }
        myColors.insert(colIt, color);
        myThresholds.insert(threshIt, threshold);
    }

    void removeColor(const size_t pos) {
        assert(pos < myColors.size());
        myColors.erase(myColors.begin()+pos);
        myThresholds.erase(myThresholds.begin()+pos);
    }

    RGBColor getColor(const SUMOReal value) {
        std::vector<RGBColor>::iterator colIt = myColors.begin();
        std::vector<SUMOReal>::iterator threshIt = myThresholds.begin();
        while (threshIt != myThresholds.end() && (*threshIt) < value) {
            threshIt++;
            colIt++;
        }
        if (!myIsInterpolated) {
            return (*colIt);
        }
        if (threshIt == myThresholds.begin()) {
            return (*colIt);
        }
        if (threshIt == myThresholds.end()) {
            return myColors.back();
        }
        SUMOReal lowVal = *(threshIt-1);
        return RGBColor::interpolate(*(colIt-1), *colIt, (value-lowVal)/((*threshIt)-lowVal));
    }

    void setInterpolated(const bool interpolate) {
        myIsInterpolated = interpolate;
    }

private:
    std::string myName;
    std::vector<RGBColor> myColors;
    std::vector<SUMOReal> myThresholds;
    bool myIsInterpolated;

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
    /// Constructor
    GUIColorer() { }

    /// Destructor
    virtual ~GUIColorer() { }

    /// Sets the color using a value from the given instance of T
    virtual SUMOReal getColorValue(const T& i) const = 0;

    /// Sets the color using a value from the given instance of T
    void setGlColor(const T& i) const {
        RGBColor& c = mySchemes[myActiveScheme].getColor(getColorValue(i));
        glColor3d(c.red(), c.green(), c.blue());
    }

protected:
    size_t myActiveScheme;
    std::vector<GUIColorScheme> mySchemes;

};


#endif

/****************************************************************************/
