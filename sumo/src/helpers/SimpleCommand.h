#ifndef SimpleCommand_H
#define SimpleCommand_H
/***************************************************************************
                          SimpleCommand.h  -  Command-pattern-class for
                          simple commands, that need no parameters and no
                          undo.
                             -------------------
    begin                : Thu, 20 Dec 2001
    copyright            : (C) 2001 by Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/**
 * @file   SimpleCommand.h
 * @author Christian Roessel
 * @date   Started Thu, 20 Dec 2001
 * $Revision$ from $Date$ by $Author$
 *
 * @brief  Contains the implementation of SimpleCommand
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
// Revision 1.6  2003/08/06 16:52:04  roessel
// Better distinction between steps and seconds added.
//
// Revision 1.5  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.4  2003/06/06 14:43:35  roessel
// Moved implementation from .icc to .h
// Added documentation.
//
// Revision 1.3  2003/02/07 10:40:13  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:53:12  dkrajzew
// global inclusion
//
// Revision 1.1  2002/10/16 14:44:45  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.3  2002/07/31 17:35:56  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.5  2002/07/30 15:10:36  croessel
// Undid previous commit
//
// Revision 1.3  2002/07/23 14:02:53  croessel
// Switched order of members.
//
// Revision 1.2  2002/06/18 16:35:37  croessel
// Moved deinition to cpp or icc in order to make files cfront compliant.
//
// Revision 1.1.1.1  2002/04/08 07:21:22  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:13  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/01/30 11:15:51  croessel
// Documentation added.
//
// Revision 1.4  2002/01/17 15:19:32  croessel
// Changed the return-type of the possible operations from void to
// MSNet::Time. Valuable for periodic commands, i.e. traffic-light
// switches.
//
// Revision 1.3  2002/01/17 15:09:45  croessel
// Parse error fixed.
//
// Revision 1.2  2002/01/17 14:57:43  croessel
// Missing execute() implementation added.
//
// Revision 1.1  2002/01/09 15:05:31  croessel
// Initial commit.
//


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "Command.h"


/**
 * A command that takes no arguments. To be used with MSEventControl.
 * @see Design Patterns, Gamma et al.
 * @see Command
 * @see MSEventControl
 */
template< class T  >
class SimpleCommand : public Command
{
public:
    /// Type of the function to execute.
    typedef MSNet::Time ( T::* Operation )();

    /**
     * Constructor.
     *
     * @param receiver Pointer to object of type T that will receive a call to
     * one of it's methods.
     * @param operation The objects' method that will be called if execute()
     * is called.
     *
     * @return Pointer to the created SimpleCommand.
     */
    SimpleCommand( T* receiver, Operation operation ) :
        myReceiver( receiver ),
        myOperation( operation )
        {}

    /// Destructor.
    ~SimpleCommand()
        {}

    /**
     * Execute the command and return an offset in steps for recurring
     * commands or 0 for single-execution command.
     *
     * @return The receivers operation should return the next interval
     * in steps for recurring commands and 0 for single-execution
     * commands.
     */
    MSNet::Time execute()
        {
            return ( myReceiver->*myOperation )();
        }

protected:

private:
    /// The object the action is directed to.
    T* myReceiver;

    /// The object's operation to perform.
    Operation myOperation;
};

#endif // SimpleCommand_H

// Local Variables:
// mode:C++
// End:
