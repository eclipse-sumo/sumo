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
 * Template-singleton class. 
 * 
 */
template< class Detector >
class MSDetector2File
{
public:
    typedef SingletonDictionary< std::string, Detector* > DetectorDict;
    typedef std::pair< Detector*, std::ofstream* > DetectorFilePair;
    typedef std::vector< DetectorFilePair > DetectorFileVec;
    typedef std::map< MSNet::Time, DetectorFileVec > Intervals;
    
    static MSDetector2File* getInstance( void )
        {
            if ( instanceM == 0 ) {
                instanceM = new MSDetector2File();
            }
            return instanceM;
        }

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
                cerr << "MSDetector2File::addDetectorAndInterval: "
                    "intervalInSeconds greater than\ndetectors clean-up "
                    "interval. Reducing intervalInSeconds to clean-up "
                    "interval." << endl;
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
                        "detector already in container. Ignoring." << endl;
                    return;
                }
            }
            
            // write xml-intro
            *ofs << Detector::getXMLHeader()
                 << det->getXMLDetectorInfoStart() << std::endl;
        }

    
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

    
protected:
    MSDetector2File( void )
        {}
                                  
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
    Intervals intervalsM;
    
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
