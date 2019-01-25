/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSPersonDevice.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// Abstract in-person device
/****************************************************************************/
#ifndef MSPersonDevice_h
#define MSPersonDevice_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <random>
#include "MSDevice.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSTransportable;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPersonDevice
 * @brief Abstract in-person device
 *
 * The MSPersonDevice-interface brings the following interfaces to a vehicle that
 *  may be overwritten by real devices:
 * @arg Retrieval of the person that holds the device
 */
class MSPersonDevice : public MSDevice {
public:
    /** @brief Constructor
     *
     * @param[in] holder The person that holds this device
     * @param[in] id The ID of the device
     */
    MSPersonDevice(MSTransportable& holder, const std::string& id) :
        MSDevice(id), myHolder(holder) {
    }


    /// @brief Destructor
    virtual ~MSPersonDevice() { }


    /** @brief Returns the person that holds this device
     *
     * @return The person that holds this device
     */
    MSTransportable& getHolder() const {
        return myHolder;
    }

protected:
    /// @brief The person that stores the device
    MSTransportable& myHolder;

private:
    /// @brief Invalidated copy constructor.
    MSPersonDevice(const MSPersonDevice&);

    /// @brief Invalidated assignment operator.
    MSPersonDevice& operator=(const MSPersonDevice&);

};


#endif

/****************************************************************************/
