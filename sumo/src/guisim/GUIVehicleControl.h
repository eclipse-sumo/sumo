/****************************************************************************/
/// @file    GUIVehicleControl.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles within the gui
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
#ifndef GUIVehicleControl_h
#define GUIVehicleControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <microsim/MSVehicleControl.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h> 



// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIVehicleControl
 * The gui-version of MSVehicleControl.
 * This version throws an exception if the color is not supplied and the
 *  vehicle removal differs from the microsim version: as a vehicle may have
 *  a tracker open, it is not always possible to delete the vehicle straight
 *  forward.
 */
class GUIVehicleControl : public MSVehicleControl
{
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

    /// Removes the vehicle
    virtual void deleteVehicle(MSVehicle *v);

    /** Returns the list of all known vehicles by name */
    std::vector<std::string> getVehicleNames();

    /** Returns the list of all known vehicles by gl-id */
    std::vector<GLuint> getVehicleIDs();

};


#endif

/****************************************************************************/

