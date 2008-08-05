/****************************************************************************/
/// @file    ROJTRFrame.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for jtr-routing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
class ROJTRFrame
{
public:
    /** @brief Inserts options used by jtrrouter into the OptionsCont-singleton
     *
     * As jtrrouter shares several options with other routing appplications, the
     *  insertion of these is done via a call to ROFrame::fillOptions.
     *
     * jtrrouer-specific options are added afterwards via calls to "addJTROptions".
     */
    static void fillOptions();


    /** @brief Checks set options from the OptionsCont-singleton for being valid for usage within jtrrouter
     * @return Whether all needed options are set
     * @todo probably, more things should be checked...
     */
    static bool checkOptions();


protected:
    /** @brief Inserts options used by jtrrouter only
     */
    static void addJTROptions();


};


#endif

/****************************************************************************/

