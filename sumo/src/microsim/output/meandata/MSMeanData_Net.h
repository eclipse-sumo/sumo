#ifndef MSMeanData_Net_h
#define MSMeanData_Net_h
//---------------------------------------------------------------------------//
//                        MSMeanData_Net.cpp -
//  Redirector for mean data output (net->edgecontrol)
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
// Revision 1.1  2004/08/02 12:05:34  dkrajzew
// moved meandata to an own folder
//
// Revision 1.1  2004/07/02 09:01:44  dkrajzew
// microsim output refactoring (moved to a subfolder)
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
 * @class MSMeanData_Net
 * The Net's meanData is a pair of an interval-length and a filehandle.
 */
class MSMeanData_Net
{
public:
    /// constructor
    MSMeanData_Net( unsigned int t, OutputDevice* of,
       bool addHeaderTail = true );

    /// destructor
    virtual ~MSMeanData_Net();

    virtual void write(unsigned int passedSteps, unsigned int start,
        unsigned int step,
        MSEdgeControl &edges, unsigned int idx);

    virtual void writeEdge(const MSEdge &edge, unsigned int idx,
        unsigned int start, unsigned int passedSteps);

    virtual void writeLane(const MSLane &lane, unsigned int idx,
        unsigned int start, unsigned int passedSteps);

    friend class MSMeanData_Net_Utils;

protected:
    /// the time interval the data shall be aggregated over (in s)
    unsigned int myInterval;

    /** @brief The file to write aggregated data into.
        For each aggregation time, a single file should be used */
    OutputDevice* myOutputDevice;

    /// Information whether the header information shall be printed
    bool myUseHeader;

};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

#endif

// Local Variables:
// mode:C++
// End:


