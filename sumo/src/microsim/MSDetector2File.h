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

#include "MSUnit.h"
#include "MSDetectorFileOutput.h"

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
    /// Shortening alias .
    typedef MSDetectorFileOutput Detector;
    /// A pair of a Detector with it's associated file-stream.
    typedef std::pair< Detector*, std::ofstream* > DetectorFilePair;
    /// Container holding DetectorFilePair (with the same interval).
    typedef std::vector< DetectorFilePair > DetectorFileVec;
    /// Association of intervals to DetectorFilePair containers.
    typedef std::map< MSUnit::IntSteps, DetectorFileVec > Intervals;

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
     * @param intervalInSeconds Interval at which output is written.
     */
    void addDetectorAndInterval( Detector* det,
                                 const std::string& filename,
                                 MSUnit::Seconds intervalInSeconds );
    // statt intervalInSeconds zwei Parameter, einen fuer die messintervalllaenge und einen fuer das outputintervall, z.b. gib alle 60 s die werte fuer die letzten 5 min aus. dazu muss ein TwoArgumentCommand gebaut werden
    
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
    MSUnit::IntSteps write2file( MSUnit::IntSteps interval );


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
        public std::binary_function< DetectorFilePair, Detector*, bool >
    {
        bool operator()( const DetectorFilePair& pair,
                         const Detector* det ) const
            {
                return pair.first == det;
            }
    };
    
            
private:
    Intervals intervalsM;       /**< Map that hold DetectorFileVec for
                                 * given intervals. */
    
    static MSDetector2File* instanceM; /**< The sole instance of this
                                        * class. */
};


#endif // MSDetector2File_H

// Local Variables:
// mode:C++
// End:
