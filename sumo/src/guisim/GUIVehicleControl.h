#ifndef GUIVehicleControl_h
#define GUIVehicleControl_h
/***************************************************************************
                          GUIVehicleControl.h  -
 The class responsible for building and deletion of vehicles within the gui
                             -------------------
    begin                : Wed, 10. Dec 2003
    copyright            : (C) 2002 by DLR http://ivf.dlr.de
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
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
// Revision 1.2  2004/04/02 11:19:16  dkrajzew
// debugging
//
// Revision 1.1  2003/12/11 06:26:27  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <microsim/MSVehicleControl.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIVehicleControl
 * The gui-version of MSVehicleControl.
 * This version throws an exception if the color is not supplied and the
 *  vehicle removal differs from the microsim version: as a vehicle may have
 *  a tracker open, it is not always possible to delete the vehicle straight
 *  forward.
 */
class GUIVehicleControl : public MSVehicleControl {
public:
    /// Constructor
    GUIVehicleControl();

    /// Destructor
    ~GUIVehicleControl();

    /** @brief Throws an exception
        Actually, this polymorph throws an exception  as each vehicle within
        the gui-version should have a color */
    MSVehicle *buildVehicle(std::string id, MSRoute* route,
        MSNet::Time departTime, const MSVehicleType* type,
        int repNo, int repOffset);

    /// Builds a vehicle in the case his color is given
    MSVehicle *buildVehicle(std::string id, MSRoute* route,
        MSNet::Time departTime, const MSVehicleType* type,
        int repNo, int repOffset, const RGBColor &col);

    /// Removes the vehicle
    void scheduleVehicleRemoval(MSVehicle *v);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#endif

#endif

// Local Variables:
// mode:C++
// End:
