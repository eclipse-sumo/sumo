#ifndef MSTravelcostDetector_H
#define MSTravelcostDetector_H

/**
 * @file   MSTravelcostDetector.h
 * @author Christian Roessel
 * @date   Started Mon Jun  2 17:21:27 2003
 * $Revision$ from $Date$ by $Author$
 * @brief  Implementation of class MSTravelcostDetector.
 * 
 * 
 */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//


// $Log$
// Revision 1.4  2003/06/06 15:33:49  roessel
// Added documentation.
// Added maxIntervalLengthM member and assert in addSampleInterval().
//
// Revision 1.3  2003/06/06 11:00:35  roessel
// Added cast due to compiler warning.
//
// Revision 1.2  2003/06/05 12:51:26  roessel
// Modified #includes and changed getString() to toString().
//
// Revision 1.1  2003/06/05 11:16:14  roessel
// Initial commit.
//

#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "MSNet.h"
#include "MSEdge.h"
#include <utils/convert/ToString.h>
#include <helpers/OneArgumentCommand.h>

class MSEdge;
class MSLane;
class Command;

/**
 * Template-singleton class. Sets Cost detectors on all MSLane objects. You
 * can add sample intervals via addSampleInterval(). If sample time is over,
 * output is written to file. There is one file for each interval.
 *
 * The template parameter class Cost must offer following methods:
 * A constructor Cost( std::string id, MSLane* lane, double startPos,
 * double length, MSNet::Time deleteDataAfterSeconds),
 * Cost::getNamePrefix(),
 * Cost::getXMLHeader() and
 * Cost::getXMLOutput().
 * For a Cost example see MSLaneState.
 *
 * @see MSLaneState
 * @see Singleton in Design Patterns, Gamma et al.
 */
template< class Cost >
class MSTravelcostDetector
{
public:
    /// Type of the internal interval-file map.
    typedef std::map< MSNet::Time, std::ofstream* > IntervalFileMap;
    /// Type of an iterator to the interval-file map.
    typedef typename IntervalFileMap::iterator IntervalFileMapIt;
    /// Type of the internal lane-cost container.
    typedef std::vector< std::pair< MSLane*, Cost* > > laneCostCont;
    /// Type of an iterator to the lane-cost container. 
    typedef typename laneCostCont::iterator laneCostContIt;
    /// Type of the internal edge-(lane-cost) container.
    typedef std::vector< std::pair< MSEdge*, laneCostCont > > EdgeLaneCostCont;
    /// Type of an iterator to the edge-(lane-cost) container.
    typedef typename EdgeLaneCostCont::iterator EdgeLaneCostContIt;
    
    /** 
     * Create the singleton and set a maximum interval length.
     * 
     * @param maxIntervalLength Set the maximum interval length. It is not
     * allowed to call addSampleInterval() with a length higher than
     * maxIntervalLength. This restriction is caused by a periodical cleanup
     * of the Cost detector.
     *
     * @see MSLaneState
     */
    static void create( MSNet::Time maxIntervalLength )
        {
            assert( instanceM == 0 );
            instanceM = new MSTravelcostDetector( maxIntervalLength );
        }

    /**
     * Get the sole instance of this class. Instead of creating this instance
     * in the first call, here the sole instance is created by calling
     * create().
     *
     * @see create()
     * @return A pointer to the sole instance of this class.
     */    
    static MSTravelcostDetector* getInstance( void )
        {
            if ( instanceM == 0 ) {
                throw SingletonNotCreated();
            }
            return instanceM;
        }

    /** 
     * Destructor. Resets member instanceM and closes all opened files. Clears
     * containers.
     * @note The created Cost objects are not deleted. In case of MSLaneState
     * as Cost you have to get a vetcor of all MSLaneState objects via the
     * SingletonDictionary and delete them manually. It is a good idea to add
     * your own Cost objects to a SingletonDictionary too.
     * 
     */
    ~MSTravelcostDetector( void )
        {
            instanceM = 0;
            // close files
            for ( IntervalFileMapIt it = intervalsAndFilesM.begin();
                it != intervalsAndFilesM.end(); ++it ){
                delete it->second;
            }
            intervalsAndFilesM.clear();
            // Cost* should be deleteted via the SingletonDictionary
            edgeLaneCostsM.clear();
        }
    
    /** 
     * Add a sample interval. A new file for this interval is created and
     * a OneArgumentCommand is added to MSEventControl.
     * 
     * @param intervalLength Length of the sample interval.
     */
    void addSampleInterval( MSNet::Time intervalLength )
        {
            assert( maxIntervalLengthM >= intervalLength );
            if ( intervalsAndFilesM.find( intervalLength ) !=
                 intervalsAndFilesM.end() ) {
                cerr << "MSTravelcostDetector::addSampleInterval "
                    "intervalLength " << intervalLength <<
                    " already added. Ignoring." << endl;
                return;
            }
            // open file
            std::string filename = Cost::getNamePrefix() + "_" +
                toString( intervalLength );
            std::ofstream* ofs = new std::ofstream( filename.c_str() );
            assert( ofs != 0 );   
            intervalsAndFilesM.insert(
                make_pair( intervalLength, ofs ) );
            // write xml-intro
            *ofs << Cost::getXMLHeader() << std::endl;
            // add command to MSEventControl
            Command* writeData =
                new OneArgumentCommand< MSTravelcostDetector, int >
                ( this, &MSTravelcostDetector::write2file, intervalLength );
            MSEventControl::getEndOfTimestepEvents()->addEvent(
                writeData,
                static_cast<int>( intervalLength / MSNet::deltaT() ) - 1,
                MSEventControl::ADAPT_AFTER_EXECUTION );
        }

    /** 
     * Write aggregated data to file corresponding to intervalLength. The
     * aggregation is done by the Cost objects receiving the intervalLength.
     * Issue a new OneArgumentCommand when done.
     * 
     * @param intervalLength Length of the sample interval.
     * 
     * @return Length of the sample interval. This adds a new Event to
     * MSEventControl that is due in intervalLength timesteps.
     */
    MSNet::Time write2file( int intervalLength )
        {
            IntervalFileMapIt ifIt = intervalsAndFilesM.find( intervalLength );
            assert( ifIt != intervalsAndFilesM.end() );
            std::ofstream& ofs = *( ifIt->second );
            MSNet::Time step   = MSNet::getInstance()->timestep();
            double time      = static_cast< double >( step ) * MSNet::deltaT();
            ofs << "<interval begin=\""
                << time - intervalLength * MSNet::deltaT() + 1
                << "\" end=\"" << time << ">\n";     
            for ( EdgeLaneCostContIt elc = edgeLaneCostsM.begin();
                  elc != edgeLaneCostsM.end(); ++elc ) {
                ofs << "  <edge id=\"" << elc->first->id() << "\">\n";     
                for ( laneCostContIt lc = elc->second.begin();
                      lc != elc->second.end(); ++lc ) {
                    MSLane* lane = lc->first;
                    Cost*   cost = lc->second;
                    ofs << "    <lane id=\"" << lane->id() << "\" "
                        << cost->getXMLOutput( intervalLength )
                        << "/>\n";
                }
                ofs << "  </edge>\n";
            }
            ofs << "</interval>" << endl;
            return intervalLength;
        }
    
                
protected:


    /** 
     * Constructor. Creates Cost objects for each MSLane in MSNet and fills
     * the internal edgeLaneCostsM container.
     * 
     * @param maxIntervalLength Maximum length of a sample interval added by
     * addSampleInterval().
     */
    MSTravelcostDetector( MSNet::Time maxIntervalLength ) :
        intervalsAndFilesM(),
        edgeLaneCostsM(),
        maxIntervalLengthM( maxIntervalLength )
        {
            // create EdgeLaneCostCont
            typedef std::vector< MSEdge* > Edges;
            typedef typename Edges::iterator EdgeIt;
            Edges edges( MSEdge::getEdgeVector() );
   	        for ( EdgeIt edge = edges.begin(); edge != edges.end(); ++edge ) {
            	// get lanes
                MSEdge::LaneCont* lanes = (*edge)->getLanes();
                laneCostCont lc;
                for ( MSEdge::LaneCont::iterator laneIt = lanes->begin();
                      laneIt != lanes->end(); ++laneIt ){
                    MSLane* lane = *laneIt;
                    // create detector for lane
                    std::string id = Cost::getNamePrefix() + "_" + lane->id();
                    Cost* cost = new Cost( id,
                                           lane,
                                           0, lane->length(),
                                           maxIntervalLength );
                    lc.push_back( std::make_pair( lane, cost ) );
                }
                edgeLaneCostsM.push_back( std::make_pair( *edge, lc ) );
            }
        }



private:
    IntervalFileMap intervalsAndFilesM; /**< Map of interval-file pairs. */

    EdgeLaneCostCont edgeLaneCostsM; /**< Container of MSEWdge-laneCostCont
                                      * pairs. */
 
    static MSTravelcostDetector* instanceM; /**< The sole instance of this
                                             * class. */

    MSNet::Time maxIntervalLengthM; /**< Maximum interval length to be added
                                     * by addSampleInterval().*/
};


// initialize static member
template< class Cost >
MSTravelcostDetector< Cost >*
MSTravelcostDetector< Cost >::instanceM = 0;


#endif // MSTravelcostDetector_H

// Local Variables:
// mode:C++
// End:

