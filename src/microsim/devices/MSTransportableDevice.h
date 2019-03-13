/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSTransportableDevice.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// Abstract in-person device
/****************************************************************************/
#ifndef MSTransportableDevice_h
#define MSTransportableDevice_h


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
 * @class MSTransportableDevice
 * @brief Abstract in-person device
 *
 * The MSTransportableDevice-interface brings the following interfaces to a vehicle that
 *  may be overwritten by real devices:
 * @arg Retrieval of the person that holds the device
 */
class MSTransportableDevice : public MSDevice {
public:
    /** @brief Constructor
     *
     * @param[in] holder The person that holds this device
     * @param[in] id The ID of the device
     */
    MSTransportableDevice(MSTransportable& holder, const std::string& id) :
        MSDevice(id), myHolder(holder) {
    }


    /// @brief Destructor
    virtual ~MSTransportableDevice() { }


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
    MSTransportableDevice(const MSTransportableDevice&);

    /// @brief Invalidated assignment operator.
    MSTransportableDevice& operator=(const MSTransportableDevice&);

};


#endif

/****************************************************************************/
