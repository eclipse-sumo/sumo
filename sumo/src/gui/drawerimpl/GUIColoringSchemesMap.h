#ifndef GUIColoringSchemesMap_h
#define GUIColoringSchemesMap_h
//---------------------------------------------------------------------------//
//                        GUIColoringSchemesMap.h -
//  A mapping between coloring schemes, their enumerations and int values
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.1  2004/08/02 11:38:18  dkrajzew
// variable coloring schemes added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <string>

#include <fx.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIColoringSchemesMap
 * To allow a variable usage of coloring schemes - as not always all are available
 * - this structure holds the mapping between their enumeration representations
 * as understood by the corresponding drawer classes, their names and their
 * numerical representation within a filled chooser widget.
 */
template<typename T>
class GUIColoringSchemesMap {
public:
    /// Constructor
    GUIColoringSchemesMap()
        { }

    /// Destructor
    ~GUIColoringSchemesMap()
        { }

    /// Adds a named coloring scheme identifier to the list of allowed coloring schemes
    void add(const std::string &name, T enumValue) {
        ColorMapping cm;
        cm.name = name;
        cm.enumValue = enumValue;
        myAvailableSchemes.push_back(cm);
    }

    /** @brief Fills the given combobox with the names of available coloring
        The indices of the coloring schemes are stored within this structures
        in order to allow the retrieval of their enumeration values. */
    void fill(FXComboBox &cb) {
        int choserValue = 0;
        std::vector<ColorMapping>::iterator i;
        for(i=myAvailableSchemes.begin(); i!=myAvailableSchemes.end(); ++i) {
            (*i).choserValue = choserValue++;
            cb.appendItem((*i).name.c_str());
        }
    }

    /** @brief Fills the given popup with the names of available coloring
        The indices of the coloring schemes are stored within this structures
        in order to allow the retrieval of their enumeration values. */
    void fill(FXPopup &p, FXObject *target, int selector) {
        int choserValue = 0;
        std::vector<ColorMapping>::iterator i;
        for(i=myAvailableSchemes.begin(); i!=myAvailableSchemes.end(); ++i) {
            (*i).choserValue = choserValue++;
            new FXButton(&p, (*i).name.c_str(),
                NULL,target,selector+choserValue-1,FRAME_THICK|FRAME_RAISED);
        }
    }

    /// Returns the enumeration value for a previously given int value
    T getEnumValue(int choserValue) const {
        std::vector<ColorMapping>::const_iterator i;
        for(i=myAvailableSchemes.begin(); i!=myAvailableSchemes.end(); ++i) {
            if((*i).choserValue==choserValue) {
                return (*i).enumValue;
            }
        }
        throw 1; // !!!
    }

    /// Returns the number of available coloring schemes
    size_t size() const {
        return myAvailableSchemes.size();
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

        /// The coloring scheme's enumeration value
        T enumValue;

        /// The coloring scheme's enumeration index
        int choserValue;

    };

    /// The list of available schemes
    std::vector<ColorMapping> myAvailableSchemes;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

