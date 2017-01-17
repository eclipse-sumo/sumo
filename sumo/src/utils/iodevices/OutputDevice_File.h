/****************************************************************************/
/// @file    OutputDevice_File.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates an ofstream
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef OutputDevice_File_h
#define OutputDevice_File_h


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
 * @class OutputDevice_File
 * @brief An output device that encapsulates an ofstream
 *
 * Please note that the device gots responsible for the stream and deletes
 *  it (it should not be deleted elsewhere).
 */
class OutputDevice_File : public OutputDevice {
public:
    /** @brief Constructor
     * @param[in] strm The output stream to used
     * @exception IOError Should not be thrown by this implementation
     */
    OutputDevice_File(const std::string& fullName, const bool binary);


    /// @brief Destructor
    ~OutputDevice_File();


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
    std::ofstream* myFileStream;

};


#endif

/****************************************************************************/

