/****************************************************************************/
/// @file    MSCORN.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// A storage for optional things to compute
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
#ifndef MSCORN_h
#define MSCORN_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCORN
 * Helper calls to store information about what shall be computed
 *  (CORN == COmpute Or Not)
 */
class MSCORN {
public:
    /**
     * @enum Function
     * A list of functions
     */
    enum Function {
        CORN_VEH_SAVEREROUTING,

        CORN_MAX
    };

    enum Pointer {
        CORN_P_VEH_PASSENGER,
        CORN_P_MAX
    };

    static void init();
    static bool wished(Function f);
    static void setWished(Function f, bool value=true);


private:
    static bool myWished[CORN_MAX];
    static bool myFirstCall[CORN_MAX];

private:
    MSCORN();
    ~MSCORN();

};


#endif

/****************************************************************************/

