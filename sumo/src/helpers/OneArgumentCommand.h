#ifndef OneArgumentCommand_H
#define OneArgumentCommand_H

/**
 * @file   OneArgumentCommand.h
 * @author Christian Roessel
 * @date   Mon Jun  2 17:15:00 2003
 * 
 * @brief  
 * 
 * 
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.1  2003/06/05 11:20:52  roessel
// Initial commit.
//

#include "Command.h"

template< class T, typename Arg  >
class OneArgumentCommand : public Command
{
public:
    /// definition of the execution function
    typedef MSNet::Time ( T::* Operation )( Arg );

    /// constructor
    OneArgumentCommand( T* receiver, Operation op, Arg argument ) :
        myReceiver( receiver ),
        myOperation( op  ),
        myArgument( argument )
        {
        }

    /// destructor
    ~OneArgumentCommand()
        {
        }

    /** Execute the command and return an offset for recurring commands
        or 0 for single-execution commands. */
    MSNet::Time execute()
        {
            return ( myReceiver->*myOperation )( myArgument );
        }

protected:

private:
    /// the object the action is directed to
    T* myReceiver;

    /// the object's performing operation
    Operation myOperation;

    /// the argument that is passed to the receiver-operation
    Arg myArgument;
};

#endif // OneArgumentCommand_H

// Local Variables:
// mode:C++
// End:
