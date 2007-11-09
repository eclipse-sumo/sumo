/****************************************************************************/
/// @file    OneArgumentCommand.h
/// @author  Christian Roessel
/// @date    on Mon Jun  2 17:15:00 2003
/// @version $Id$
///
// * @author Christian Roessel
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
#ifndef OneArgumentCommand_h
#define OneArgumentCommand_h



// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Command.h"
#include "microsim/MSUnit.h"

/**
 * A command that takes one constant argument. To be used with MSEventControl.
 * E.g. pass a detector-interval as argument.
 * @see Design Patterns, Gamma et al.
 * @see Commad
 * @see MSEventControl
 * @see MSTravelcostDetector
 * @see MSDetector2File
 *
 */
template< class T, typename Arg  >
class OneArgumentCommand : public Command
{
public:
    /// Type of the function to execute.
    typedef MSUnit::IntSteps(T::* Operation)(Arg);

    /**
     * Constructor.
     *
     * @param receiver Pointer to object of type T that will receive a call to
     * one of it's methods.
     * @param operation The objects' method that will be called if execute()
     * is called.
     * @param argument The argument that will be passed to operation.
     *
     * @return Pointer to the created OneArgumentCommand.
     */
    OneArgumentCommand(T* receiver, Operation operation, Arg argument)
            : myReceiver(receiver), myOperation(operation), myArgument(argument) {}

    /// Destructor.
    ~OneArgumentCommand() {}

    /**
     * Execute the command and return an offset in steps for recurring
     * commands or 0 for single-execution command.
     *
     * @return The receivers operation should return the next interval
     * in steps for recurring commands and 0 for single-execution
     * commands.
     */
    SUMOTime execute(SUMOTime) {
        return (myReceiver->*myOperation)(myArgument);
    }

protected:

private:
    /// The object the action is directed to.
    T* myReceiver;

    /// The object's operation to perform.
    Operation myOperation;

    /// The argument that is passed to the receiver-operation
    Arg myArgument;
};


#endif

/****************************************************************************/

