/****************************************************************************/
/// @file    CommandMock.h
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @date    2009-11-30
/// @version $Id$
///
// 
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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

#ifndef Command_Mock_h
#define Command_Mock_h

#include <utils/common/Command.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CommandMock
 * Mock Implementation for Unit Tests
 */
class CommandMock : public Command {
public:
    /** @brief Constructor
     */
	CommandMock() throw(){
		hasRun = false;
	}

    /// @brief Destructor
	~CommandMock() throw(){}

	/** @brief Executes the command.*/
    SUMOTime execute(SUMOTime currentTime) throw(ProcessError) {
		hasRun = true;
		return currentTime;
	} 

	/** return true, when execute was called, otherwise false */
	bool isExecuteCalled() {
		return hasRun;
	}

private:
	bool hasRun;  

};


#endif

/****************************************************************************/

