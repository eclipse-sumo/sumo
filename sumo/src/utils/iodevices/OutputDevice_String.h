/****************************************************************************/
/// @file    OutputDevice_String.h
/// @author  Michael Behrisch
/// @date    2009
/// @version $Id$
///
// An output device that encapsulates a stringstream
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2009-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef OutputDevice_String_h
#define OutputDevice_String_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
    OutputDevice_String(const bool binary = false, const unsigned int defaultIndentation = 0);


    /// @brief Destructor
    ~OutputDevice_String();


    /** @brief Returns the current content as a string
     * @return The content as string
     */
    std::string getString();

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

