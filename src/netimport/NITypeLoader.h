/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NITypeLoader.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
///
// Perfoms network import
/****************************************************************************/
#ifndef NITypeLoader_h
#define NITypeLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <vector>
#include <xercesc/sax2/SAX2XMLReader.hpp>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class SUMOSAXHandler;
class NBNetBuilder;
class Position;
class PositionVector;



class NITypeLoader {
    /**
     * @class NITypeLoader
     * @brief loads a specificy type of xml file.
     * @note Extra class to simplify import * from netgenerate
     */
public:
    /** loads data from the list of xml-files of certain type */
    static bool load(SUMOSAXHandler* handler, const std::vector<std::string>& files,
                     const std::string& type, const bool stringParse = false);
};


#endif

/****************************************************************************/

