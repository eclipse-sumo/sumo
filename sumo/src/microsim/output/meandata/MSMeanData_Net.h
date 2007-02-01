/****************************************************************************/
/// @file    MSMeanData_Net.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
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
#ifndef MSMeanData_Net_h
#define MSMeanData_Net_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// imported modules
// ===========================================================================
// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <microsim/output/MSDetectorFileOutput.h>
#include "MSLaneMeanDataValues.h"
#include <limits>


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
 * @class MSMeanData_Net
 */
class MSMeanData_Net : public MSDetectorFileOutput
{
public:
    /// constructor
    MSMeanData_Net(unsigned int t, unsigned int index,
                   MSEdgeControl &edges, const std::vector<int> &dumpBegins,
                   const std::vector<int> &dumpEnds, bool useLanes,
                   bool addHeaderTail = true);

    /// destructor
    virtual ~MSMeanData_Net();

    virtual void write(XMLDevice &dev,
                       SUMOTime startTime, SUMOTime stopTime);

    virtual void writeEdge(XMLDevice &dev,
                           const MSEdge &edge,
                           SUMOTime startTime, SUMOTime stopTime);

    virtual void writeLane(XMLDevice &dev,
                           const MSLane &lane,
                           SUMOTime startTime, SUMOTime stopTime);

    friend class MSMeanData_Net_Utils;

    void writeXMLHeader(XMLDevice &dev) const;
    virtual void writeXMLOutput(XMLDevice &dev,
                                SUMOTime startTime, SUMOTime stopTime);
    void writeXMLDetectorInfoStart(XMLDevice &dev) const;
    void writeXMLDetectorInfoEnd(XMLDevice &dev) const;
    SUMOTime getDataCleanUpSteps(void) const;

protected:
    void resetOnly(SUMOTime stopTime);
    void resetOnly(const MSEdge &edge, SUMOTime stopTime);

    inline void conv(
        const MSLaneMeanDataValues &values, SUMOTime period,
        SUMOReal laneLength, SUMOReal laneVMax,
        SUMOReal &traveltime, SUMOReal &meanSpeed,
        SUMOReal &meanDensity, SUMOReal &meanOccupancy)
    {

        if (values.nSamples==0) {
            assert(laneVMax>=0);
            traveltime = laneLength / laneVMax;
            meanSpeed = laneVMax;
            meanDensity = 0;
            meanOccupancy = 0;
        } else {
            meanSpeed = values.speedSum / (SUMOReal) values.nSamples;
            if (meanSpeed==0) {
                traveltime = 1000000;//std::numeric_limits<SUMOReal>::max() / (SUMOReal) 100.;
            } else {
                traveltime = laneLength / meanSpeed;
            }
            assert(period!=0);
            assert(laneLength!=0);
            meanDensity = (SUMOReal) values.nSamples /
                          (SUMOReal) period * (SUMOReal) 1000. / (SUMOReal) laneLength;
            meanOccupancy = (SUMOReal) values.vehLengthSum /
                            (SUMOReal) period / (SUMOReal) laneLength;
        }
    }

protected:
    /// the time interval the data shall be aggregated over (in s)
    unsigned int myInterval;

    /// Information whether the header information shall be printed
    bool myUseHeader;

    /// The mean data index of this output
    unsigned int myIndex;

    /// The edgecontrol to use
    MSEdgeControl &myEdges;

    /// Information whether the output shall be edge-based (not lane-based)
    bool myAmEdgeBased;

    /// first and last time step to write information (-1 indicates always)
    std::vector<int> myDumpBegins, myDumpEnds;

};


#endif

/****************************************************************************/

