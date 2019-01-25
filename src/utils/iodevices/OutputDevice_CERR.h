/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    OutputDevice_CERR.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates cerr
/****************************************************************************/
#ifndef OutputDevice_CERR_h
#define OutputDevice_CERR_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "OutputDevice.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDevice_CERR
 * @brief An output device that encapsulates cerr
 */
class OutputDevice_CERR : public OutputDevice {
public:
    /** @brief Returns the single cerr instance
     *
     * Creates and returns the cerr device.
     */
    static OutputDevice* getDevice();


protected:
    /// @name Methods that override/implement OutputDevice-methods
    /// @{

    /** @brief Returns the associated ostream
     * @return cerr
     */
    std::ostream& getOStream();


    /** @brief Called after every write access.
     *
     * Calls flush on stderr.
     */
    virtual void postWriteHook();
    /// @}


private:
    /** @brief Constructor
     * @exception IOError Should not be thrown by this implementation
     */
    OutputDevice_CERR();


    /// @brief Destructor
    ~OutputDevice_CERR();


private:
    /// @brief my singular instance
    static OutputDevice* myInstance;


};


#endif

/****************************************************************************/

