/****************************************************************************/
/// @file    MSXMLRawOut.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// »missingDescription«
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
#ifndef MSXMLRawOut_h
#define MSXMLRawOut_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSEdgeControl;
class MSEdge;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSXMLRawOut
 * The Net's meanData is a pair of an interval-length and a filehandle.
 */
class MSXMLRawOut
{
public:
    /// constructor
    MSXMLRawOut();

    /// destructor
    virtual ~MSXMLRawOut();

    static void write(OutputDevice* of, const MSEdgeControl &ec,
                      SUMOTime timestep, unsigned int intend);

    static void writeEdge(OutputDevice* of, const MSEdge &edge,
                          unsigned int intend);

    static void writeLane(OutputDevice* of, const MSLane &lane,
                          unsigned int intend);

    static void writeVehicle(OutputDevice* of, const MSVehicle &veh,
                             unsigned int intend);


};


#endif

/****************************************************************************/

