/****************************************************************************/
/// @file    WrappingCommand.h
/// @author  Christian Roessel
/// @date    Thu, 20 Dec 2001
/// @version $Id:WrappingCommand.h 4699 2007-11-09 14:05:13Z dkrajzew $
///
// simple commands, that need no parameters and no
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef WrappingCommand_h
#define WrappingCommand_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Command.h"


// ===========================================================================
// class definition
// ===========================================================================
/**
 * A command that takes no arguments. To be used with MSEventControl.
 * @see Design Patterns, Gamma et al.
 * @see Command
 * @see MSEventControl
 */
template< class T  >
class WrappingCommand : public Command
{
public:
    /// Type of the function to execute.
    typedef SUMOTime(T::* Operation)(SUMOTime);

    /**
     * Constructor.
     *
     * @param receiver Pointer to object of type T that will receive a call to
     * one of it's methods.
     * @param operation The objects' method that will be called if execute()
     * is called.
     *
     * @return Pointer to the created WrappingCommand.
     */
    WrappingCommand(T* receiver, Operation operation) throw()
            : myReceiver(receiver), myOperation(operation) {}

    /// Destructor.
    ~WrappingCommand() throw() {}

    /**
     * Execute the command and return an offset in steps for recurring
     * commands or 0 for single-execution command.
     *
     * @return The receivers operation should return the next interval
     * in steps for recurring commands and 0 for single-execution
     * commands.
     */
    SUMOTime execute(SUMOTime currentTime) throw() {
        return (myReceiver->*myOperation)(currentTime);
    }

protected:

private:
    /// The object the action is directed to.
    T* myReceiver;

    /// The object's operation to perform.
    Operation myOperation;

};


#endif

/****************************************************************************/

