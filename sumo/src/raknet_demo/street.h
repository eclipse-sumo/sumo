/****************************************************************************/
/// @file    street.h
/// @author  unknown_author
/// @date    unknown_date
/// @version $Id: $
///
// missing_desc
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
#ifndef street_h
#define street_h
#pragma once

// ===========================================================================
// used namespaces
// ===========================================================================
// namespaces in Irrlicht
//using namespace irr;

class Street
{
public:
    // Konstruktor
    Street(void);

    // Dekonstruktor
    ~Street(void);

    // Methoden
    void addLane(int objectID, int noPoints, float *streetPointsX,
                 float *streetPointsZ);

private:
    /*
    void SendPacketToServer(s32 objectID, f32 startX,
    f32 startZ, f32 destX, f32 destZ, f32 midX, f32 midZ, f32 dx, f32 dz);
    */
    static const char SERVER_IP;

    * ======================================================================= */
            * compiler pragmas
            /* =========================================================================
            	unsigned int time;
            };

            #endif

            /****************************************************************************/

