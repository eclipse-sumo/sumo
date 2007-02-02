/****************************************************************************/
/// @file    ampel.h
/// @author  Marc Gurczik
/// @date    2006-02-27
/// @version $Id$
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
#ifndef ampel_h
#define ampel_h
#pragma once

class Ampel
{
public:
    // Konstruktor
    Ampel(void);

    // Dekonstruktor
    ~Ampel(void);

    static const char SERVER_IP = "127.0.0.1";

    // Methoden
    void addTrafficLight(int objectID, float x, float y, float z, float rot);
    void setTrafficLightState(int objectID, int state);

    // Objektzähler
    static unsigned int objectCounter;

private:
    void SendPacketToServer(int objID, int objHandle, int x, int y, int z);

    int intOC;
    * ======================================================================= */
            * compiler pragmas
            /* =========================================================================
            		unsigned int time;
            };

            #endif

            /****************************************************************************/

