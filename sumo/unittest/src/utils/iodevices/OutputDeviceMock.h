/****************************************************************************/
/// @file    OutputDeviceMock.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @date    2009-11-23
/// @version $Id$
///
// 
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
 * @class OutputDeviceMock
 * Mock Implementation for Unit Tests 
 *
 */
class OutputDeviceMock : public OutputDevice {
public:
    /** @brief Constructor    
     */
	OutputDeviceMock() throw(IOError){}

    /// @brief Destructor
	~OutputDeviceMock() throw() {}


    /** @brief Returns the current content as a string   
     */
    std::string getString() throw() {
        return myStream.str();
    }

protected:    
    /** @brief Returns the associated ostream    
     */
    std::ostream &getOStream() throw() {
        return myStream;
    }

private:
    /// the string stream
    std::ostringstream myStream;

};


#endif

/****************************************************************************/

