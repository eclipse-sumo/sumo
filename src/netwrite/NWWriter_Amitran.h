/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NWWriter_Amitran.h
/// @author  Michael Behrisch
/// @date    13.03.2014
/// @version $Id$
///
// Exporter writing networks using the Amitran format
/****************************************************************************/
#ifndef NWWriter_Amitran_h
#define NWWriter_Amitran_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


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

