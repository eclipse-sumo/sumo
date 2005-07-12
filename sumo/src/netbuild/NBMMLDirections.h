#ifndef NBMMLDirection_h
#define NBMMLDirection_h
//---------------------------------------------------------------------------//
//                        NBMMLDirection.h -
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
// Revision 1.2  2005/07/12 12:32:47  dkrajzew
// code style adapted; guessing of ramps and unregulated near districts implemented; debugging
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * enumerations
 * ======================================================================= */
enum NBMMLDirection {
    MMLDIR_NODIR,
    MMLDIR_STRAIGHT,
    MMLDIR_TURN,
    MMLDIR_LEFT,
    MMLDIR_RIGHT,
    MMLDIR_PARTLEFT,
    MMLDIR_PARTRIGHT
};

#endif
