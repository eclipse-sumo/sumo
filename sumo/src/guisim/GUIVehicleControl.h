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
// Revision 1.7  2005/12/01 07:33:44  dkrajzew
// introducing bus stops: eased building vehicles; vehicles may now have nested elements
//
// Revision 1.6  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:05:25  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2005/02/01 10:10:40  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.2  2004/04/02 11:19:16  dkrajzew
// debugging
//
// Revision 1.1  2003/12/11 06:26:27  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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
        SUMOTime departTime, const MSVehicleType* type,
        int repNo, int repOffset);

protected:
    /// Removes the vehicle
    virtual void removeVehicle(MSVehicle *v);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
