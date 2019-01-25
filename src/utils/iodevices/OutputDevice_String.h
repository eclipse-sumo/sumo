/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    OutputDevice_String.h
/// @author  Michael Behrisch
/// @date    2009
/// @version $Id$
///
// An output device that encapsulates a stringstream
/****************************************************************************/
#ifndef OutputDevice_String_h
#define OutputDevice_String_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fstream>
#include "OutputDevice.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDevice_String
 * @brief An output device that encapsulates an ofstream
 *
 * Please note that the device gots responsible for the stream and deletes
 *  it (it should not be deleted elsewhere).
 */
class OutputDevice_String : public OutputDevice {
public:
    /** @brief Constructor
     * @exception IOError Should not be thrown by this implementation
     */
    OutputDevice_String(const bool binary = false, const int defaultIndentation = 0);


    /// @brief Destructor
    ~OutputDevice_String();


    /** @brief Returns the current content as a string
     * @return The content as string
     */
    std::string getString() const;

protected:
    /// @name Methods that override/implement OutputDevice-methods
    /// @{

    /** @brief Returns the associated ostream
     * @return The used stream
     */
    std::ostream& getOStream();
    /// @}


private:
    /// The wrapped ofstream
    std::ostringstream myStream;

};


#endif

/****************************************************************************/

