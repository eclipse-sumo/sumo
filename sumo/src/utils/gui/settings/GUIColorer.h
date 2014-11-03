/****************************************************************************/
/// @file    GUIColorer.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @date    Mon, 20.07.2009
/// @version $Id$
///
//
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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIPropertySchemeStorage
 * @brief Base class for coloring. Allows changing the used colors and sets
 *  the used color in dependence to a value
 */
template<class T>
class GUIPropertySchemeStorage {
public:
    /// @brief Constructor
    GUIPropertySchemeStorage() : myActiveScheme(0) { }

    /// @brief Destructor
    virtual ~GUIPropertySchemeStorage() { }

    /// @brief Fills the given combobox with the names of available colorings
    void fill(FXComboBox& cb) {
        for (typename std::vector<T>::iterator i = mySchemes.begin(); i != mySchemes.end(); ++i) {
            cb.appendItem((*i).getName().c_str());
        }
        cb.setCurrentItem((FXint)myActiveScheme);
    }

    void setActive(size_t scheme) {
        if (scheme < mySchemes.size()) {
            myActiveScheme = scheme;
        }
    }

    size_t getActive() const {
        return myActiveScheme;
    }

    T& getScheme() {
        return mySchemes[myActiveScheme];
    }

    const T& getScheme() const {
        return mySchemes[myActiveScheme];
    }

    T* getSchemeByName(std::string name) {
        for (typename std::vector<T>::iterator i = mySchemes.begin(); i != mySchemes.end(); ++i) {
            if ((*i).getName() == name) {
                return &(*i);
            }
        }
        return 0;
    }

    void save(OutputDevice& dev) const {
        for (typename std::vector<T>::const_iterator i = mySchemes.begin(); i != mySchemes.end(); ++i) {
            i->save(dev);
        }
    }

    bool operator==(const GUIPropertySchemeStorage& c) const {
        return myActiveScheme == c.myActiveScheme && mySchemes == c.mySchemes;
    }


    void addScheme(T scheme) {
        mySchemes.push_back(scheme);
    }

    size_t size() const {
        return mySchemes.size();
    }


protected:
    size_t myActiveScheme;
    std::vector<T> mySchemes;

};

typedef GUIPropertySchemeStorage<GUIColorScheme> GUIColorer;
typedef GUIPropertySchemeStorage<GUIScaleScheme> GUIScaler;

#endif

/****************************************************************************/
