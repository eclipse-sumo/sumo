#ifndef GUIInterface_GridSpeedUp_h
#define GUIInterface_GridSpeedUp_h
//---------------------------------------------------------------------------//
//                        GUIInterface_GridSpeedUp.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.3  2005/09/23 06:11:14  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.2  2005/07/12 12:52:07  dkrajzew
// code style adapted
//
// Revision 1.2  2005/06/14 11:29:51  dksumo
// documentation added
//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)




/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUIInterface_GridSpeedUp {
public:
	GUIInterface_GridSpeedUp() { }

	virtual ~GUIInterface_GridSpeedUp() { }

    virtual void get(int what,
        SUMOReal x, SUMOReal y, SUMOReal xoff, SUMOReal yoff,
		size_t *setEdges, size_t *setJunctions, size_t *setAdditional) { }

    /// returns the number of cells in x-direction
    int getNoXCells() const;

    /// returns the number of cells in y-direction
    int getNoYCells() const;

    /// returns the size of each cell in x-direction
    SUMOReal getXCellSize() const;

    /// returns the size of each cell in y-direction
    SUMOReal getYCellSize() const;


};/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
