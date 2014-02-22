/****************************************************************************/
/// @file    ROJTRFrame.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for jtr-routing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#ifndef ROJTRFrame_h
#define ROJTRFrame_h


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
 * @class ROJTRFrame
 * @brief Sets and checks options for jtr-routing
 */
class ROJTRFrame {
public:
    /** @brief Inserts options used by jtrrouter into the OptionsCont-singleton
     *
     * As jtrrouter shares several options with other routing appplications, the
     *  insertion of these is done via a call to ROFrame::fillOptions.
     *
     * jtrrouer-specific options are added afterwards.
     */
    static void fillOptions();


    /** @brief Checks set options from the OptionsCont-singleton for being valid for usage within jtrrouter
     * @return Whether all needed options are set
     * @todo probably, more things should be checked...
     */
    static bool checkOptions();

};


#endif

/****************************************************************************/

