/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
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
/// @file    OutputDevice_String.h
/// @author  Michael Behrisch
/// @date    2009
///
// An output device that encapsulates a stringstream
/****************************************************************************/
#pragma once
#include <config.h>

#include <fstream>
#include "OutputDevice.h"
#include <memory>
#include "StreamDevices.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDevice_String
 * @brief An output device that encapsulates a stringstream
 *
 * The device manages the underlying string stream through a StreamDevice wrapper
 * and provides access to the string content.
 */
class OutputDevice_String : public OutputDevice {
public:
    /** @brief Constructor
     * @exception IOError Should not be thrown by this implementation
     */
    OutputDevice_String(const int defaultIndentation = 0);


    /// @brief Destructor
    ~OutputDevice_String() = default;


    /** @brief Returns the current content as a string
     * @return The content as string
     */
    std::string getString() const;

protected:
    /// @name Methods that override/implement OutputDevice-methods
    /// @{

    /** @brief Returns the associated StreamDevice
     * @return The StreamDevice wrapping the string stream
     */
    StreamDevice& getStreamDevice();
    /// @}


private:
    /// The wrapped stringstream (raw pointer for getString() access)
    std::ostringstream* myStream;

    /// The StreamDevice wrapping the string stream
    std::unique_ptr<StreamDevice> myStreamDevice;

};