/****************************************************************************/
/// @file    OutputDevice_String.h
/// @author  Michael Behrisch
/// @date    2009
/// @version $Id: OutputDevice_String.h 7845 2009-10-10 20:02:17Z behrisch $
///
// An output device that encapsulates a stringstream
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef OutputDeviceMock_h
#define OutputDeviceMock_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <utils/iodevices/OutputDevice.h>


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
class OutputDeviceMock : public OutputDevice {
public:
    /** @brief Constructor
     * @exception IOError Should not be thrown by this implementation
     */
	OutputDeviceMock() throw(IOError){}


    /// @brief Destructor
	~OutputDeviceMock() throw() {}


    /** @brief Returns the current content as a string
     * @return The content as string
     */
     std::string OutputDeviceMock::getString() throw() {
    return myStream.str();
}

protected:
    /// @name Methods that override/implement OutputDevice-methods
    /// @{

    /** @brief Returns the associated ostream
     * @return The used stream
     */
	std::ostream & OutputDeviceMock::getOStream() throw() {
		return myStream;
	}


private:
    /// The wrapped ofstream
    std::ostringstream myStream;

};


#endif

/****************************************************************************/

