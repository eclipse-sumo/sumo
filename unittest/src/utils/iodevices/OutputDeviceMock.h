/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    OutputDeviceMock.h
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @date    2009-11-23
/// @version $Id$
///
//
/****************************************************************************/

#ifndef OutputDeviceMock_h
#define OutputDeviceMock_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fstream>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDeviceMock
 * Mock Implementation for Unit Tests
 *
 */
class OutputDeviceMock : public OutputDevice {
public:
    /** @brief Constructor
     */
    OutputDeviceMock() {}

    /// @brief Destructor
    ~OutputDeviceMock()  {}


    /** @brief Returns the current content as a string
     */
    std::string getString()  {
        return myStream.str();
    }

protected:
    /** @brief Returns the associated ostream
     */
    std::ostream& getOStream()  {
        return myStream;
    }

private:
    /// the string stream
    std::ostringstream myStream;

};


#endif

/****************************************************************************/

