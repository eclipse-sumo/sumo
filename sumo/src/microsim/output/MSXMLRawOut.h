#ifndef MSXMLRawOut_h
#define MSXMLRawOut_h
//---------------------------------------------------------------------------//
//                        MSXMLRawOut.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 10.05.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.1  2004/08/02 12:05:54  dkrajzew
// raw-output added
//
//
/* =========================================================================
 * class declarations
 * ======================================================================= */
class OutputDevice;
class MSEdgeControl;
class MSEdge;
class MSLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
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
        unsigned int timestep, unsigned int intend);

    static void writeEdge(OutputDevice* of, const MSEdge &edge,
        unsigned int intend);

    static void writeLane(OutputDevice* of, const MSLane &lane,
        unsigned int intend);

    static void writeVehicle(OutputDevice* of, const MSVehicle &veh,
        unsigned int intend);


};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

#endif

// Local Variables:
// mode:C++
// End:


