#ifndef MSDetector2File_H
#define MSDetector2File_H

/**
 * @file   MSDetector2File.h
 * @author Christian Roessel <christian.roessel@dlr.de>
 * @date   Started Mon Jul  7 16:16:26 2003
 * @version $Id$
 * @brief  Implementation of class MSDetector2File.
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

#include "MSNet.h"
#include "MSEventControl.h"
#include <helpers/SingletonDictionary.h>
#include <string>
#include <cassert>
#include <utility>
#include <iostream>
#include <helpers/OneArgumentCommand.h>

/**
 * Template-singleton class, that controls file-output of instances of
 * class Detector. Just add a existing detector, a filename (maybe
 * this will change in future) and an interval and the detector-output
 * will be written to filename at given interval. This process is
 * triggered by the MSEventControl mechanism.
 *
 * The output is in XML-format.
 *
 * The template parameter class Detector must offer following methods:
 * Detector::getNamePrefix()
 * Detector::getXMLHeader()
 * Detector::getXMLOutput()
 * Detector::getXMLDetectorInfoStart()
 * Detector::getXMLDetectorInfoEnd()
 *
 * For a Detector example see MSInductLoop
 */
template< class Detector >
class MSDetector2File
{
public:
    /// Type of the Detector's singleton-dictionary.
    typedef SingletonDictionary< std::string, Detector* > DetectorDict;
    /// A pair of a Detector with it's associated file-stream.
    typedef std::pair< Detector*, std::ofstream* > DetectorFilePair;
    /// Container holding DetectorFilePair (with the same interval).
    typedef std::vector< DetectorFilePair > DetectorFileVec;
    /// Association of intervals to DetectorFilePair containers.
    typedef std::map< MSNet::Time, DetectorFileVec > Intervals;

    /** 
     * Return and/or create the sole instance of this class.
     * 
     * @return Sole instance pointer of class MSDetector2File.
     */
    static MSDetector2File* getInstance( void )
        {
            if ( instanceM == 0 ) {
                instanceM = new MSDetector2File();
            }
            return instanceM;
        }

    /** 
     * Destructor. Closes all file-streams, resets instance pointer
     * and clears the interval-DetectorFilePair map.
     * 
     */
    ~MSDetector2File( void )
        {
            instanceM = 0;
            // close files
            for ( typename Intervals::iterator it = intervalsM.begin();
                  it != intervalsM.end(); ++it ) {
                for( typename DetectorFileVec::iterator df =
                         it->second.begin(); df != it->second.end(); ++df ) {
                    *(df->second) << Detector::getXMLDetectorInfoEnd()
                                  << std::endl;
                    delete df->second;
                }
            }
            intervalsM.clear();
            // Detector* should be deleted via the SingletonDictionary
        }

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
                                 MSNet::Time intervalInSeconds )
        {
            MSNet::Time intervalInSteps( MSNet::getSteps( intervalInSeconds ));
            assert( intervalInSteps >= 1 );

/*            Detector* det = 
                DetectorDict::getInstance()->getValue( detectorId );*/
/*            std::string filename = det->getNamePrefix() + "_" +
                toString( intervalInSeconds ) + ".xml";*/

            if ( det->getDataCleanUpSteps() < intervalInSteps ) {
                intervalInSteps = det->getDataCleanUpSteps();
                std::cerr << "MSDetector2File::addDetectorAndInterval: "
                    "intervalInSeconds greater than\ndetectors clean-up "
                    "interval. Reducing intervalInSeconds to clean-up "
                    "interval." << std::endl;
            }
            std::ofstream* ofs = 0;
            typename Intervals::iterator it =
                intervalsM.find( intervalInSteps );
            if ( it == intervalsM.end() ) {
                DetectorFileVec detAndFileVec;
                ofs = new std::ofstream( filename.c_str() );
                assert( ofs != 0 );
                detAndFileVec.push_back( std::make_pair( det, ofs ) );
                intervalsM.insert(
                    std::make_pair( intervalInSteps, detAndFileVec ) );

                // Add command for given interval only once to MSEventControl
                Command* writeData =
                    new OneArgumentCommand< MSDetector2File, MSNet::Time >
                    ( this, &MSDetector2File::write2file, intervalInSteps );
                MSEventControl::getEndOfTimestepEvents()->addEvent(
                    writeData,
                    intervalInSteps - 1,
                    MSEventControl::ADAPT_AFTER_EXECUTION );
            }
            else {
                DetectorFileVec& detAndFileVec = it->second;
                if ( find_if( detAndFileVec.begin(),
                              detAndFileVec.end(),
                              bind2nd( detectorEquals(), det ) )
                     == detAndFileVec.end() ) {
                    ofs = new std::ofstream( filename.c_str() );
                    assert( ofs != 0 );
                    detAndFileVec.push_back( std::make_pair( det, ofs ) );    
                }
                else {
                    // detector already in container. Don't add several times
                    std::cerr << "MSDetector2File::addDetectorAndInterval: "
                        "detector already in container. Ignoring."<< std::endl;
                    return;
                }
            }
            
            // write xml-intro
            *ofs << Detector::getXMLHeader()
                 << det->getXMLDetectorInfoStart() << std::endl;
        }

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
     * @param intervalInSteps The interval that is due.
     * 
     * @return intervalInSteps to reactivate the event.
     */
    MSNet::Time write2file( MSNet::Time intervalInSteps )
        {
            typename Intervals::iterator intervalIt =
                intervalsM.find( intervalInSteps);
            assert( intervalIt != intervalsM.end() );
            DetectorFileVec dfVec = intervalIt->second;
            for ( typename DetectorFileVec::iterator it = dfVec.begin();
                  it != dfVec.end(); ++it ) {
                double time = MSNet::getInstance()->simSeconds();
                *(it->second) << "<interval start=\""
                              << time - MSNet::getSeconds( intervalInSteps) + 1
                              << "\" stop=\"" << time << "\" "
                              << it->first->getXMLOutput( intervalInSteps )
                              << " />" << std::endl;
            }
            return intervalInSteps;
        }

    /// Default constructor.
    MSDetector2File( void )
        {}

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



// initialize static member
template< class Detector >
MSDetector2File< Detector >*
MSDetector2File< Detector >::instanceM = 0;
    
#endif // MSDetector2File_H

// Local Variables:
// mode:C++
// End:
