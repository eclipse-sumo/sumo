/****************************************************************************/
/// @file    GUISelectionLoader.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Helper for loading selections
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
#ifndef GUISelectionLoader_h
#define GUISelectionLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUISelectionLoader
 * @brief Helper for loading selections
 */
class GUISelectionLoader {
public:
    /** @brief Loads a selection from a given file
     * @param[in] file The file to load the selection from
     * @param[out] msg The error message
     * @return Whether the selection could be loaded (fills the error message if not)
     */
    static bool loadSelection(const std::string &file, std::string &msg) throw();


protected:
    /// @brief invalidated constructor
    GUISelectionLoader() throw() {}

    /// @brief invalidated destructor
    ~GUISelectionLoader() throw() {}

};


#endif

/****************************************************************************/

