/***************************************************************************
                          SimpleCommand.h  -  Command-pattern-class for 
                          simple commands, that need no parameters and no 
                          undo.
                             -------------------
    begin                : Thu, 20 Dec 2001
    copyright            : (C) 2001 by Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
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

#ifndef SimpleCommand_H
#define SimpleCommand_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "Command.h"


/**
   See Design-Patterns, Gamma et al.
   
   Usage: 
   Create a Command-object, that executes the method "action()" of class T:
   T* receiver = new T;
   ...
   Command* aCommand = new SimpleCommand< T >( receiver, &T::action );
   ...
   aCommand->execute();  
   
 */
template< class T  >
class SimpleCommand : public Command
{
public:

    typedef MSNet::Time ( T::* Operation )();

    SimpleCommand( T* receiver, Operation op );
     

    ~SimpleCommand();

    /** Execute the command and return an offset for recurring commands
        or 0 for single-execution commands. */    
    MSNet::Time execute();

protected:

private:

    T* myReceiver;
    Operation myOperation;    
};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//
#ifndef DISABLE_INLINE
#include "SimpleCommand.icc"
#endif // DISABLE_INLINE

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#include "SimpleCommand.cpp"
#endif // EXTERNAL_TEMPLATE_DEFINITION

#endif // SimpleCommand_H

// Local Variables:
// mode:C++
// End:











