#ifndef GUIVehicleTransfer_h
#define GUIVehicleTransfer_h
/***************************************************************************
                          GUIVehicleTransfer.h  -
  The gui-version of a mover of vehicles that got stucked due to grid locks
                             -------------------
    begin                : Sep 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel Krajzewicz@dlr.de
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
// Revision 1.2  2003/11/20 14:41:02  dkrajzew
// class documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <microsim/MSVehicleTransfer.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIVehicleTransfer
 * The gui-version of @see MSVehicleTransfer
 * Basically, this class is only needed as vehicles are deleted within the
 *  gui not in the same way as within the command line simulation
 *  This should be changed within the next time...
 */
class GUIVehicleTransfer : public MSVehicleTransfer
{
public:
    friend class GUINetBuilder;

protected:
    /// Kills a vehicle from the network
    void removeVehicle(const std::string &id);

    /// Constructor
    GUIVehicleTransfer();

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#endif

#endif

// Local Variables:
// mode:C++
// End:
