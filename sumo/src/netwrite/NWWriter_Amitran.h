/****************************************************************************/
/// @file    NWWriter_Amitran.h
/// @author  Michael Behrisch
/// @date    13.03.2014
/// @version $Id$
///
// Exporter writing networks using the Amitran format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NWWriter_Amitran_h
#define NWWriter_Amitran_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NWWriter_Amitran
 * @brief Exporter writing networks using the Amitran format
 *
 */
class NWWriter_Amitran {
public:
    /** @brief Writes the network into a Amitran-file
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void writeNetwork(const OptionsCont& oc, NBNetBuilder& nb);

};


#endif

/****************************************************************************/

