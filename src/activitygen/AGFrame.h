/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AGFrame.h
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @date    Mon, 13 Sept 2010
/// @version $Id$
///
// Configuration of the options of ActivityGen
/****************************************************************************/
#ifndef AGFRAME_H
#define AGFRAME_H


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AGFrame
 * @brief Sets and checks options for ActivityGen
 */
class AGFrame {
public:
    /** @brief Inserts options used by ActivityGen into the OptionsCont singleton
     */
    static void fillOptions();


    /** @brief Checks set options from the OptionsCont singleton for being valid
     * for usage within ActivityGen
     *
     * @return Whether all needed options are set
     * @todo Not implemented yet
     */
    static bool checkOptions();

};

#endif /* AGFRAME_H */
