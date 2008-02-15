/****************************************************************************/
/// @file    DiscreteCommand.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 23 Jan 2004
/// @version $Id:DiscreteCommand.h 4699 2007-11-09 14:05:13Z dkrajzew $
///
// -------------------
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
#ifndef DiscreteCommand_h
#define DiscreteCommand_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSNet.h>

/**
 * Base class for all DiscreteCommand classes. A concrete DiscreteCommand will hold a
 * receiver and an operation on the receiver. This operation will be performed
 * when execute() is called.
 * @see Design Patterns, Gamma et al.
 * @see MSEventControl
 */
class DiscreteCommand
{
public:
    /// Destructor.
    virtual ~DiscreteCommand(void) throw() {};

    virtual bool execute() throw() = 0;
};


#endif

/****************************************************************************/

