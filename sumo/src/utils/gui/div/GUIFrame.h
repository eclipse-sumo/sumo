/****************************************************************************/
/// @file    GUIFrame.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.04.2005
/// @version $Id$
///
// Sets and checks options for gui applications
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
#ifndef GUIFrame_h
#define GUIFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIFrame
 * @brief Sets and checks options for gui applications
 *
 * This class holds methods for initialising and checking options of gui-
 *  applications.
 *
 * @todo What about throw-declarations in methods?
 */
class GUIFrame {
public:
    /** @brief Fills the options container Singleton with gui-options
    */
    static void fillInitOptions();


    /** @brief Checks the options stored in the options container Singleton for being valid gui-options
    *
    * For gui applications,
    * @arg width and height must be > 0
    *
    * @return False, if the options are not valid
    * @todo probably, more things should be checked...
    */
    static bool checkInitOptions();

};


#endif

/****************************************************************************/

