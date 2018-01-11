/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOSAXHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// SAX-handler base for SUMO-files
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/xml/GenericSAXHandler.h>
#include "SUMOXMLDefinitions.h"
#include "SUMOSAXHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================
SUMOSAXHandler::SUMOSAXHandler(const std::string& file)
    : GenericSAXHandler(SUMOXMLDefinitions::tags, SUMO_TAG_NOTHING,
                        SUMOXMLDefinitions::attrs, SUMO_ATTR_NOTHING, file) {}


SUMOSAXHandler::~SUMOSAXHandler() {}


/****************************************************************************/

