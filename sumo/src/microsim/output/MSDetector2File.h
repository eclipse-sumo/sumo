#ifndef MSDetector2File_H
#define MSDetector2File_H

/**
 * @file   MSDetector2File.h
 * @author Christian Roessel <christian.roessel@dlr.de>
 * @date   Started Mon Jul  7 16:16:26 2003
 * @version $Id$
 * @brief  Declaration of class MSDetector2File.
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Id$
// $Log$
// Revision 1.3  2004/11/25 16:26:48  dkrajzew
// consolidated and debugged some detectors and their usage
//
// Revision 1.2  2004/11/25 11:53:49  dkrajzew
// patched the bug on false intervals stamps if begin!=0
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <microsim/MSUnit.h>
#include "MSDetectorFileOutput.h"
#include <map>
#include <utility>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OutputDevice;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * Singleton class, that controls file-output of instances of class
 * MSDetectorFileOutput. Just add a existing detector, a filename
 * (maybe this will change in future) and an interval and the
 * detector-output will be written to filename at given interval. This
 * process is triggered by the MSEventControl mechanism.
 *
 * The output is in XML-format, but controlled mostly by the instances
 * of MSDetectorFileOutput
 *
 * For a Detector example
 * @see MSInductLoop
 * @see MSLaneState
 */
class MSDetector2File
{
public:
    /// A pair of a Detector with it's associated file-stream.
    typedef std::pair< MSDetectorFileOutput*, OutputDevice* > DetectorFilePair;
    /// Container holding DetectorFilePair (with the same interval).
    typedef std::vector< DetectorFilePair > DetectorFileVec;
    typedef std::pair< MSUnit::IntSteps, MSUnit::IntSteps > IntervalsKey;
    /// Association of intervals to DetectorFilePair containers.
    typedef std::map< IntervalsKey, DetectorFileVec > Intervals;

    /**
     * Return and/or create the sole instance of this class.
     *
     * @return Sole instance pointer of class MSDetector2File.
     */
    static MSDetector2File* getInstance( void );


    /**
     * Destructor. Closes all file-streams, resets instance pointer
     * and clears the interval-DetectorFilePair map.
     *
     */
    ~MSDetector2File( void );


    /**
     * Prepare a detector for file output at given interval. The
     * output is triggered by MSEventControl via OneArgumentCommand.
     *
     * @see MSEventControl
     * @see OneArgumentCommand
     *
     * @param det Existing detector that shall report it's data.
     * @param filename File where the output shall go.
     * @param sampleInterval Sample interval length
     * @param write2fileInterval Interval at which output is written.
     */
    void addDetectorAndInterval( MSDetectorFileOutput* det,
                                 OutputDevice *device,
                                 MSUnit::Seconds sampleInterval,
                                 MSUnit::Seconds write2fileInterval );

protected:
    /**
     * When interval is over, search interval in map and write data of
     * all detectors that are associated to this interval to
     * file. This method is called by an instance of
     * OneArgumentCommand via MSEventControl.
     *
     * @see MSEventControl
     * @see OneArgumentCommand
     *
     * @param interval The interval that is due.
     *
     * @return intervalInSteps to reactivate the event.
     */
    MSUnit::IntSteps write2file( IntervalsKey key );


    /// Default constructor.
    MSDetector2File( void );


    /**
     * Binary predicate that compares the passed DetectorFilePair's
     * detector to a fixed one. Returns true if detectors are
     * equal. (Used to prevent multiple inclusion of a detector for
     * the same interval.)
     *
     * @see addDetectorAndInterval
     */
    struct detectorEquals :
        public std::binary_function< DetectorFilePair, MSDetectorFileOutput*, bool >
    {
        bool operator()( const DetectorFilePair& pair,
                         const MSDetectorFileOutput* det ) const
            {
                return pair.first == det;
            }
    };


private:
    Intervals intervalsM;       /**< Map that hold DetectorFileVec for
                                 * given intervals. */

    static MSDetector2File* instanceM; /**< The sole instance of this
                                        * class. */

    /// A map from sample intervals to the times the interval was called the last time
    typedef std::map<int, int> LastCallsMap;

    /// The map that holds the last call for each sample interval
    LastCallsMap myLastCalls; // !!! ok, we don not have an offset by now, if one is introduced, both the sample interval and the offset must be used as key

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
