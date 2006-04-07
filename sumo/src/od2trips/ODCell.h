#ifndef ODCell_h
#define ODCell_h
//---------------------------------------------------------------------------//
//                        ODCell.h -
//  A single cell within the od-matrix
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 05. Apr. 2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.1  2006/04/07 05:25:15  dkrajzew
// complete od2trips rework
//
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


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * struct ODCell
 * This class represents a single od-matrix cell
 */
struct ODCell {
    /// The number of vehicles
    SUMOReal vehicleNumber;
    /// The begin time this cell describes
    SUMOTime begin;
    /// The end time this cell describes
    SUMOTime end;
    /// Name of the origin district
    std::string origin;
    /// Name of the destination district
    std::string destination;
    /// Name of the vehicle type
    std::string vehicleType;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

