//---------------------------------------------------------------------------//
//                        MSTriggeredReader.cpp -
//  The basic class for classes that read triggers
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2003/09/22 14:56:06  dkrajzew
// base debugging
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <string>
#include <microsim/MSNet.h>
#include <helpers/PreStartInitialised.h>
#include "MSTriggeredReader.h"



MSTriggeredReader::MSTriggerCommand::MSTriggerCommand(MSTriggeredReader &parent)
    : _parent(parent)
{
}


MSTriggeredReader::MSTriggerCommand::~MSTriggerCommand( void )
{
}


MSNet::Time
MSTriggeredReader::MSTriggerCommand::execute()
{
    _parent.processNext();
    _parent.readNextTriggered();
    return _parent._offset;
}


MSTriggeredReader::MSTriggeredReader(MSNet &net)
    : PreStartInitialised(net), _offset(0)
{
}

MSTriggeredReader::~MSTriggeredReader()
{
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSTriggeredReader.icc"
//#endif

// Local Variables:
// mode:C++
// End:


