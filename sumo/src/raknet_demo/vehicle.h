/****************************************************************************/
/// @file    vehicle.h
/// @author  Marc Gurczik
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
#ifndef vehicle_h
#define vehicle_h
#pragma once

class Vehicle
{
public:
    // Konstruktor
    Vehicle(void);

    // Dekonstruktor
    ~Vehicle(void);

    static const char SERVER_IP;

    // Methoden
    void setStartPosition(int x, int y, int z);
    void setDestPosition(int x, int y, int z);
    void setViaPoint(int x, int y, int z);
    void setVelocity(double speed);
    void calculateRoute();
    void setPosition(int x, int y, int z);
    static void removeFromClient(int oc);

    // Objektzähler
    static unsigned int objectCounter;

public:
    static void SendPacketToServer(int objID, int objHandle, int x, int y, int z);

    int intOC;
    * ======================================================================= */
            * compiler pragmas
            /* =========================================================================
            		unsigned int time;
            };


            #endif

            /****************************************************************************/

