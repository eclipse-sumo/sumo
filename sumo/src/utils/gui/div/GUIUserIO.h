/****************************************************************************/
/// @file    GUIUserIO.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2006
/// @version $Id$
///
// Some OS-dependant functions to ease cliboard manipulation
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
#ifndef GUIUserIO_h
#define GUIUserIO_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIUserIO
 * @brief Some OS-dependant functions to ease cliboard manipulation
 *
 * This helper contains only one function by now. It is used to store a text
 *  snipplet permanently within Windows-clipboard. This method is necessary,
 *  because FOX only implements the Linux/UNIX-text copying scheme where
 *  a marked text is copied. This does not work as soon as the widget with
 *  the marked text is destroyed or when the selection is lost. Because this
 *  scheme differs very much from the way the clipboard is handled in windows,
 *  is is implemented "from scratch" herein.
 */
class GUIUserIO {
public:
    /** @brief Copies the given text to clipboard
     *
     * This method is only implemented for MS Windows.
     *
     * @param[in] app The application to use
     * @param[in] text The text to copy
     */
    static void copyToClipboard(const FXApp& app, const std::string& text);

    static std::string clipped;

};


#endif

/****************************************************************************/

