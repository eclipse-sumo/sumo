#ifndef MSLaneMeanDataValues_h
#define MSLaneMeanDataValues_h
//---------------------------------------------------------------------------//
//                        MSLaneMeanDataValues.cpp -
//  Storage for a lane's mean values
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
// Revision 1.2  2005/05/04 08:13:59  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added, new mean data functionality
//
// Revision 1.1  2004/08/02 12:05:34  dkrajzew
// moved meandata to an own folder
//
// Revision 1.1  2004/07/02 09:01:44  dkrajzew
// microsim output refactoring (moved to a subfolder)
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @struct MSLaneMeanDataValues
 * Structure holding values that describe the flow and other physical
 * properties aggregated over some seconds and normalised by the
 * aggregation period */
struct MSLaneMeanDataValues
{
    MSLaneMeanDataValues()
        : nVehEntireLane( 0 ),
        nVehContributed( 0 ),
        nVehLeftLane( 0 ),
        nVehEnteredLane( 0 ),
        contTimestepSum( 0 ),
        discreteTimestepSum( 0 ),
        speedSum( 0 ),
        speedSquareSum( 0 ),
        traveltimeStepSum( 0 ),
        haltSum(0)
        {}

    /// the number of vehicles that passed the entire lane
    unsigned nVehEntireLane;

    /// the number of vehicles that made up the aggregated data
    unsigned nVehContributed;

    /// the number of vehicles that left this lane within the
    /// sample intervall
    unsigned nVehLeftLane;

    /// the number of vehicles that entered this lane within the
    /// sample intervall
    unsigned nVehEnteredLane;

    /// the number of time steps
    double contTimestepSum;

    /// as contTimestepSum but as an integer
    unsigned discreteTimestepSum;

    /// the sum of the speeds the vehicles had ont the ...
    double speedSum;

    /// the sum of squared speeds the vehicles had ont the ...
    double speedSquareSum;

    /// traveltime sum from vehicles that entirely passed the lane
    double traveltimeStepSum;

/*    void  addVehicleData(
        double contTimesteps, unsigned discreteTimesteps, double speedSum,
        double speedSquareSum, bool hasFinishedEntireLane,
        bool hasLeftLane, bool hasEnteredLane, double travelTimesteps);
        */

    unsigned haltSum;

};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

#endif

// Local Variables:
// mode:C++
// End:

