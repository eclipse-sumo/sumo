/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    PCTypeDefHandler.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A handler for loading polygon type maps
/****************************************************************************/
#ifndef PCTypeDefHandler_h
#define PCTypeDefHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/options/OptionsCont.h>
#include "PCTypeDefHandler.h"


// ===========================================================================
// class declarations
// ===========================================================================
class PCTypeToDef;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PCTypeDefHandler
 * @brief A handler for loading polygon type maps
 */
class PCTypeDefHandler : public SUMOSAXHandler {
public:
    /** @brief Constructor
     * @param[in] oc The options used while loading the type map
     * @param[out] con The container to fill
     */
    PCTypeDefHandler(OptionsCont& oc, PCTypeMap& con);


    /// @brief Destructor
    virtual ~PCTypeDefHandler();


protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     * @todo Completely unsecure currently (invalid values may force abortion with no error message)
     */
    void myStartElement(int element,
                        const SUMOSAXAttributes& attrs);
    //@}


protected:
    /// @brief The options (program settings)
    OptionsCont& myOptions;

    /// @brief The type map to fill
    PCTypeMap& myContainer;


private:
    /// @brief Invalidated copy constructor
    PCTypeDefHandler(const PCTypeDefHandler& src);

    /// @brief Invalidated assignment operator
    PCTypeDefHandler& operator=(const PCTypeDefHandler& src);

};


#endif

/****************************************************************************/

