/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOSAXHandler.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// SAX-handler base for SUMO-files
/****************************************************************************/
#ifndef SUMOSAXHandler_h
#define SUMOSAXHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <utils/xml/GenericSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOSAXHandler
 * @brief SAX-handler base for SUMO-files
 *
 * This class is a GenericSAXHandler which knows all tags SUMO uses, so all
 *  SUMO-XML - loading classes should be derived from it.
 */
class SUMOSAXHandler : public GenericSAXHandler {
public:
    /**
     * @brief Constructor
     *
     * @param[in] file The name of the processed file
     */
    SUMOSAXHandler(const std::string& file = "", const std::string& expectedRoot = "");


    /// Destructor
    virtual ~SUMOSAXHandler();


private:
    /// @brief invalidated copy constructor
    SUMOSAXHandler(const SUMOSAXHandler& s);

    /// @brief invalidated assignment operator
    const SUMOSAXHandler& operator=(const SUMOSAXHandler& s);

};


#endif

/****************************************************************************/

