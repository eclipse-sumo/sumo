#ifndef MSTravelcostDetector_H
#define MSTravelcostDetector_H

/**
 * @file   MSTravelcostDetector.h
 * @author Christian Roessel
 * @date   Mon Jun  2 17:21:27 2003
 * 
 * @brief  
 * 
 * 
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
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
#include "../helpers/convert.h"
#include "../helpers/OneArgumentCommand.h"

class MSEdge;
class MSLane;
class Command;

template< class Cost >
class MSTravelcostDetector
{
public:
    typedef std::map< MSNet::Time, std::ofstream* > IntervalFileMap;
    typedef typename IntervalFileMap::iterator IntervalFileMapIt;
    
    typedef std::vector< std::pair< MSLane*, Cost* > > laneCostCont;
    typedef typename laneCostCont::iterator laneCostContIt;

    typedef std::vector< std::pair< MSEdge*, laneCostCont > > EdgeLaneCostCont;
    typedef typename EdgeLaneCostCont::iterator EdgeLaneCostContIt;
    
    
    static void create( MSNet::Time maxIntervalLength )
        {
            assert( instanceM == 0 );
            instanceM = new MSTravelcostDetector( maxIntervalLength );
        }

    static MSTravelcostDetector* getInstance( void )
        {
            if ( instanceM == 0 ) {
                throw SingletonNotCreated();
            }
            return instanceM;
        }

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
    
    
    void addSampleInterval( MSNet::Time intervalLength )
        {
            if ( intervalsAndFilesM.find( intervalLength ) !=
                 intervalsAndFilesM.end() ) {
                cerr << "MSTravelcostDetector::addSampleInterval "
                    "intervalLength " << intervalLength <<
                    " already added. Ignoring." << endl;
                return;
            }
            // open file
            std::string filename = Cost::getNamePrefix() + "_" +
                getString( intervalLength );
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
                intervalLength / MSNet::deltaT() - 1,
                MSEventControl::ADAPT_AFTER_EXECUTION );
        }


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


    /// ctor
    MSTravelcostDetector( MSNet::Time maxIntervalLength ) :
        intervalsAndFilesM(),
        edgeLaneCostsM()
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
    IntervalFileMap intervalsAndFilesM;   

    EdgeLaneCostCont edgeLaneCostsM;
 
    static MSTravelcostDetector* instanceM;
};


// initialize static member
template< class Cost >
MSTravelcostDetector< Cost >*
MSTravelcostDetector< Cost >::instanceM = 0;


#endif // MSTravelcostDetector_H

// Local Variables:
// mode:C++
// End:
