#ifndef OutputDevice_h
#define OutputDevice_h
//---------------------------------------------------------------------------//
//                        OutputDevice.h -
//  An abstract output device that encapsulates an ostream
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// $Log$
// Revision 1.2  2004/08/02 13:01:16  dkrajzew
// documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class OutputDevice
 * An abstract output device that encapsulates an ostream
 */
class OutputDevice {
public:
    /// Constructor
    OutputDevice() { }

    /// Destructor
    virtual ~OutputDevice() { }

    /// returns the information whether one can write into the device
    virtual bool ok() = 0;

    /// Closes the device
    virtual void close() = 0;

    /// Returns the assiciated ostream
    virtual std::ostream &getOStream() = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

