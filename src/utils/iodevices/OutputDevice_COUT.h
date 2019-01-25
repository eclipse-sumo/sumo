/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    OutputDevice_COUT.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates cout
/****************************************************************************/
#ifndef OutputDevice_COUT_h
#define OutputDevice_COUT_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "OutputDevice.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDevice_COUT
 * @brief An output device that encapsulates cout
 */
class OutputDevice_COUT : public OutputDevice {
public:
    /** @brief Returns the single cout instance
     *
     * Creates and returns the cout device.
     */
    static OutputDevice* getDevice();


protected:
    /// @name Methods that override/implement OutputDevice-methods
    /// @{

    /** @brief Returns the associated ostream
     * @return cout
     */
    std::ostream& getOStream();


    /** @brief Called after every write access.
     *
     * Calls flush on stdout.
     */
    virtual void postWriteHook();
    /// @}


private:
    /** @brief Constructor
     * @exception IOError Should not be thrown by this implementation
     */
    OutputDevice_COUT();


    /// @brief Destructor
    ~OutputDevice_COUT();


private:
    /// @brief my singular instance
    static OutputDevice* myInstance;


};


#endif

/****************************************************************************/

