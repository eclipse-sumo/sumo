#ifndef DiscreteCommand_H
#define DiscreteCommand_H
/***************************************************************************
                          DiscreteCommand.h
                             -------------------
    begin                : Fri, 23 Jan 2004
    copyright            : (C) 2004 by Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/**
 * @file   DiscreteCommand.h
 * @author Daniel Krajzewicz
 * @date   Started Fri, 23 Jan 2004
 * $Revision$ from $Date$
 *
 * @brief
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
// Revision 1.1  2004/01/26 07:30:21  dkrajzew
// discrete (non-periodic, event-driven) commands added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <microsim/MSNet.h>

/**
 * Base class for all DiscreteCommand classes. A concrete DiscreteCommand will hold a
 * receiver and an operation on the receiver. This operation will be performed
 * when execute() is called.
 * @see Design Patterns, Gamma et al.
 * @see SimpleDiscreteCommand
 * @see OneArgumentDiscreteCommand
 * @see MSEventControl
 */
class DiscreteCommand
{
public:
    /// Destructor.
    virtual ~DiscreteCommand( void ) {};

    virtual bool execute() = 0;
};

#endif

// Local Variables:
// mode:C++
// End:

