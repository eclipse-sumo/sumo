/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2004-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    OutputDevice_File.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004
///
// An output device that encapsulates an ofstream
/****************************************************************************/
#pragma once
#include <config.h>

#include <iostream>
#include <memory>
#include "OutputDevice.h"
#include "StreamDevices.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDevice_File
 * @brief An output device that encapsulates an ofstream
 *
 * Please note that the device gots responsible for the stream and deletes
 *  it (it should not be deleted elsewhere).
 */
class OutputDevice_File : public OutputDevice {
public:
    /** @brief Constructor
     * @param[in] fullName The name of the output file to use
     * @param[in] compressed whether to apply gzip compression
     * @exception IOError Should not be thrown by this implementation
     */
    OutputDevice_File(const std::string& fullName, const bool compressed = false);


    /// @brief Destructor
    ~OutputDevice_File() = default;

    /** @brief returns the information whether the device will discard all output
     * @return Whether the device redirects to /dev/null
     */
    bool isNull() override {
        return myAmNull;
    }


protected:
    /// @name Methods that override/implement OutputDevice-methods
    /// @{

    /** @brief Returns the associated StreamDevice
     * @return The StreamDevice wrapping the file stream
     */
    StreamDevice& getStreamDevice() override;
    /// @}


private:
    /// The StreamDevice wrapping the file stream
    std::unique_ptr<StreamDevice> myStreamDevice;

    /// am I redirecting to /dev/null
    bool myAmNull = false;

};