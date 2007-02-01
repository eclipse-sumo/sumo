/****************************************************************************/
/// @file    GUIInterface_GridSpeedUp.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: $
///
//
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
#ifndef GUIInterface_GridSpeedUp_h
#define GUIInterface_GridSpeedUp_h

// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif



// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class GUIInterface_GridSpeedUp
{
public:
    GUIInterface_GridSpeedUp()
    { }

    virtual ~GUIInterface_GridSpeedUp()
    { }

    virtual void get(int what,
                         SUMOReal x, SUMOReal y, SUMOReal xoff, SUMOReal yoff,
                         size_t *setEdges, size_t *setJunctions, size_t *setAdditional)
    { }

    /// returns the number of cells in x-direction
    int getNoXCells() const;

    /// returns the number of cells in y-direction
    int getNoYCells() const;

    /// returns the size of each cell in x-direction
    SUMOReal getXCellSize() const;

    /// returns the size of each cell in y-direction
    SUMOReal getYCellSize() const;


}
;/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/


#endif

/****************************************************************************/

