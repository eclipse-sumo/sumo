/***************************************************************************
                          Command.h  -  Command-pattern-class.
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
// Revision 1.1  2002/04/08 07:21:22  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:13  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/01/30 11:15:38  croessel
// Documentation added.
//
// Revision 1.2  2002/01/17 15:19:32  croessel
// Changed the return-type of the possible operations from void to
// MSNet::Time. Valuable for periodic commands, i.e. traffic-light
// switches.
//
// Revision 1.1  2002/01/09 15:05:30  croessel
// Initial commit.
//

#ifndef Command_H
#define Command_H

#include "../microsim/MSNet.h"

/**
   See Design-Patterns, Gamma et al.
 */
class Command
{
public:
    /// Destructor.
    virtual ~Command();

    /** Execute the command and return an offset for recurring commands
        or 0 for single-execution commands. */
    virtual MSNet::Time execute() = 0;

protected:
    /// Default constructor.
    Command();
    
private:

};

#endif

// Local Variables:
// mode:C++
// End:










