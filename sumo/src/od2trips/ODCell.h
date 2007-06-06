/****************************************************************************/
/// @file    ODCell.h
/// @author  Peter Mieth
/// @date    Sept 2002
/// @version $Id$
///
// A single cell within the od-matrix
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
#ifndef ODCell_h
#define ODCell_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * struct ODCell
 * This class represents a single od-matrix cell
 */
struct ODCell
{
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


#endif

/****************************************************************************/

