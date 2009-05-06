/****************************************************************************/
/// @file    GUICompleteSchemeStorage.h
/// @author  Daniel Krajzewicz
/// @date    :find(mySortedSchemeNames.begin(), mySortedSchemeNames.end(), name)==mySortedSchemeNames.end()) {
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
#ifndef GUICompleteSchemeStorage_h
#define GUICompleteSchemeStorage_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <utils/gui/windows/GUISUMOAbstractView.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUICompleteSchemeStorage
 * @brief This class contains all available visualization settings
 */
class GUICompleteSchemeStorage {
public:
    /// Constructor
    GUICompleteSchemeStorage();

    /// Destructor
    ~GUICompleteSchemeStorage();

    /// Adds a visualization setting
    void add(GUIVisualizationSettings &scheme);

    /// Returns the named setting
    GUIVisualizationSettings &get(const std::string &name);

    /// Returns the information whether a setting with the given name is stored
    bool contains(const std::string &name) const;

    /// Returns the information whether a setting with the given name is stored
    void remove(const std::string &name);

    /// Returns a list of stored settings names
    const std::vector<std::string> &getNames() const;

    /// initialises the storage with some default settings
    void init(FXApp *app);

    /// Returns all settings
    const std::map<std::string, GUIVisualizationSettings> &getItems() const;

protected:
    /// A map of settings referenced by their names
    std::map<std::string, GUIVisualizationSettings> mySettings;

    /// List of known setting names
    std::vector<std::string> mySortedSchemeNames;

};

extern GUICompleteSchemeStorage gSchemeStorage;


#endif

/****************************************************************************/

