/****************************************************************************/
/// @file    GUIColorer.h
/// @author  Michael Behrisch
/// @date    Mon, 20.07.2009
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include <utils/iodevices/OutputDevice.h>
#include "GUIColorScheme.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


// ===========================================================================
// class definitions
// ===========================================================================
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

    /// @brief Sets the color using a function call for the given instance of T
    virtual bool setFunctionalColor(const T& i) const {
		UNUSED_PARAMETER(i);
        return false;
    }

    /// @brief Sets the color using a value from the given instance of T
    void setGlColor(const T& i) const {
        if (!setFunctionalColor(i)) {
            const RGBColor& c = mySchemes[myActiveScheme].getColor(getColorValue(i));
            glColor3d(c.red(), c.green(), c.blue());
        }
    }

    /// @brief Fills the given combobox with the names of available colorings
    void fill(FXComboBox &cb) {
        typename std::vector<GUIColorScheme>::iterator i;
        for (i=mySchemes.begin(); i!=mySchemes.end(); ++i) {
            cb.appendItem((*i).getName().c_str());
        }
        cb.setCurrentItem((FXint)myActiveScheme);
    }

    void setActive(size_t scheme) {
        if (scheme < mySchemes.size()) {
            myActiveScheme = scheme;
        }
    }

    const size_t getActive() const {
        return myActiveScheme;
    }

    GUIColorScheme& getScheme() {
        return mySchemes[myActiveScheme];
    }

    GUIColorScheme* getSchemeByName(std::string name) {
        typename std::vector<GUIColorScheme>::iterator i;
        for (i=mySchemes.begin(); i!=mySchemes.end(); ++i) {
            if ((*i).getName() == name) {
                return &(*i);
            }
        }
        return 0;
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
