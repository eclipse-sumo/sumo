/****************************************************************************/
/// @file    GUIColoringSchemesMap.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A mapping between coloring schemes, their enumerations and int values
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
#ifndef GUIColoringSchemesMap_h
#define GUIColoringSchemesMap_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <fx.h>
#include "GUIBaseColorer.h"


// ===========================================================================
// class definitions
// ===========================================================================
class BaseSchemeInfoSource {
public:
    virtual ~BaseSchemeInfoSource() { }

    virtual void fill(FXComboBox &cb) = 0;
    virtual ColorSetType getColorSetType(size_t index) const = 0;
    virtual GUIBaseColorerInterface *getColorerInterface(size_t index) const = 0;

};


/**
 * @class GUIColoringSchemesMap
 * To allow a variable usage of coloring schemes - as not always all are available
 * - this structure holds the mapping between their enumeration representations
 * as understood by the corresponding drawer classes, their names and their
 * numerical representation within a filled chooser widget.
 */
template<class T1>
class GUIColoringSchemesMap : public BaseSchemeInfoSource {
public:
    /// Constructor
    GUIColoringSchemesMap() { }

    /// Destructor
    ~GUIColoringSchemesMap() {
        typename std::vector<ColorMapping>::iterator i;
        for (i=myAvailableSchemes.begin(); i!=myAvailableSchemes.end(); ++i) {
            delete(*i).colorer;
        }
        myAvailableSchemes.clear();
    }

    /// Adds a named coloring scheme identifier to the list of allowed coloring schemes
    void add(const std::string &name, GUIBaseColorer<T1> *colorer) {
        ColorMapping cm;
        cm.name = name;
        cm.colorer = colorer;
        myAvailableSchemes.push_back(cm);
    }

    /** @brief Fills the given combobox with the names of available coloring
        The indices of the coloring schemes are stored within this structures
        in order to allow the retrieval of their enumeration values. */
    void fill(FXComboBox &cb) {
        typename std::vector<ColorMapping>::iterator i;
        for (i=myAvailableSchemes.begin(); i!=myAvailableSchemes.end(); ++i) {
            cb.appendItem((*i).name.c_str());
        }
    }

    /* @brief Fills the given popup with the names of available coloring
        The indices of the coloring schemes are stored within this structures
        in order to allow the retrieval of their enumeration values. */
    /*
    void fill(FXPopup &p, FXObject *target, int selector) {
        typename std::vector<ColorMapping>::iterator i;
        for(i=myAvailableSchemes.begin(); i!=myAvailableSchemes.end(); ++i) {
            new FXButton(&p, (*i).name.c_str(),
                NULL,target,selector+choserValue-1,FRAME_THICK|FRAME_RAISED);
        }
    }
    */

    /// Returns the enumeration value for a previously given int value
    /*
    E1 getEnumValue(int choserValue) const {
        typename std::vector<ColorMapping>::const_iterator i;
        for(i=myAvailableSchemes.begin(); i!=myAvailableSchemes.end(); ++i) {
            if((*i).choserValue==choserValue) {
                return (*i).enumValue;
            }
        }
        throw 1; // !!!
    }
    */

    /// Returns the number of available coloring schemes
    size_t size() const {
        return myAvailableSchemes.size();
    }

    GUIBaseColorer<T1> *getColorer(size_t index) const {
        return myAvailableSchemes[index].colorer;
    }

    GUIBaseColorerInterface *getColorerInterface(size_t index) const {
        return myAvailableSchemes[index].colorer;
    }

    ColorSetType getColorSetType(size_t index) const {
        return myAvailableSchemes[index].colorer->getSetType();
    }

private:
    /**
     * @struct ColorMapping
     * This structure holds - for each coloring scheme - its name, numerical index
     *  within the filled structure and the enumeration type.
     */
    struct ColorMapping {
        /// The name of the coloring scheme
        std::string name;

        // The coloring scheme's enumeration index
        //int choserValue;

        /// The colorer to use
        GUIBaseColorer<T1> *colorer;

    };

    /// The list of available schemes
    std::vector<ColorMapping> myAvailableSchemes;

};


#endif

/****************************************************************************/

