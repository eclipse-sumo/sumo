#ifndef OutputDevice_COUT_h
#define OutputDevice_COUT_h
//---------------------------------------------------------------------------//
//                        OutputDevice_COUT.h -
//  An output device that encapsulates cout
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
#include "OutputDevice.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class OutputDevice_COUT
 * An output device that encapsulates cout
 */
class OutputDevice_COUT : public OutputDevice {
public:
    /// Constructor
    OutputDevice_COUT();

    /// Destructor
    ~OutputDevice_COUT();

    /// returns the information whether one can write into the device
    bool ok();

    /// Closes the device
    void close();

    /// Returns the assiciated ostream
    std::ostream &getOStream();

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

