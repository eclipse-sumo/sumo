#ifndef OneArgumentCommand_H
#define OneArgumentCommand_H

/**
 * @file   OneArgumentCommand.h
 * @author Christian Roessel
 * @date   Started on Mon Jun  2 17:15:00 2003
 * $Revision$ from $Date$ by $Author$
 *
 * @brief Contains the implementation of OneArgumentCommand
 *
 *
 */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//


// $Log$
// Revision 1.5  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.4  2003/07/10 16:20:11  roessel
// Bug fix: variable name must correspond to prototype.
//
// Revision 1.3  2003/06/06 15:36:15  roessel
// Refined documentation.
//
// Revision 1.2  2003/06/06 14:09:21  roessel
// Documentation added.
//
// Revision 1.1  2003/06/05 11:20:52  roessel
// Initial commit.
//

#include "Command.h"

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
    typedef MSNet::Time ( T::* Operation )( Arg );

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
    OneArgumentCommand( T* receiver, Operation operation, Arg argument ) :
        myReceiver( receiver ),
        myOperation( operation ),
        myArgument( argument )
        {
        }

    /// Destructor.
    ~OneArgumentCommand()
        {
        }

    /**
     * Execute the command and return an offset for recurring commands
     * or 0 for single-execution command.
     *
     * @return The receivers operation should return the next interval for
     * recurring commands and 0 for single-execution commands.
     */
    MSNet::Time execute()
        {
            return ( myReceiver->*myOperation )( myArgument );
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

#endif // OneArgumentCommand_H

// Local Variables:
// mode:C++
// End:
