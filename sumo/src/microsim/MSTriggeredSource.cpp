//---------------------------------------------------------------------------//
//                        MSTriggeredSource.cpp  -  Concrete Source
//                        that reads emit-times and -speeds from a
//                        file and chooses the routes according to a
//                        distribution.
//                           -------------------
//  begin                : Wed, 12 Jun 2002
//  copyright            : (C) 2002 by Christian Roessel
//  organisation         : DLR/IVF http://ivf.dlr.de
//  email                : roessel@zpr.uni-koeln.de
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
// Revision 1.20  2005/02/01 10:10:42  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.19  2004/11/23 10:20:10  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.18  2004/01/26 07:50:16  dkrajzew
// using the xmlhelpers instead of building the parser by the object itself
//
namespace
{
    const char rcsid[] =
    "$Id$";
}


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSTriggeredSource.h"
#include "MSTriggeredSourceXMLHandler.h"
#include "MSVehicleType.h"
#include "MSVehicle.h"
#include "MSEventControl.h"
#include "MSVehicleControl.h"
#include <utils/common/MsgHandler.h>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <utils/common/XMLHelpers.h>
#include <utils/convert/TplConvert.h>
#include <helpers/Command.h>
#include <helpers/SimpleCommand.h>
#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <utils/gfx/RGBColor.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSTriggeredSource::RouteDistribution::RouteDistribution( void )
    : myDistribution()
{}

//---------------------------------------------------------------------------//

MSTriggeredSource::RouteDistribution::~RouteDistribution( void )
{
    myDistribution.clear();
}

//---------------------------------------------------------------------------//

void
MSTriggeredSource::RouteDistribution::addElement(
    MSRoute* aRoute,
    double aFrequency )
{
    assert( aFrequency > double( 0 ) );
    RouteDistElement elem;
    elem.myRoute = aRoute;
    elem.myFrequency = aFrequency;
    if ( myDistribution.size() > 0 ) {
        elem.myFrequencySum =
            myDistribution.back().myFrequencySum + aFrequency;
    }
    else {
        elem.myFrequencySum = aFrequency;
    }
    myDistribution.push_back( elem );
}

//---------------------------------------------------------------------------//

MSRoute*
MSTriggeredSource::RouteDistribution::getRndRoute( void )
{
    // Get random value between 0 and
    // myDistribution.back().myFrequencySum and return the route of
    // the first element that has a higher frequency sum.
    double rndFreq = double( rand() ) /
        ( static_cast<double>(RAND_MAX) + 1) *
        myDistribution.back().myFrequencySum;
    RouteDistCont::iterator routeElemIt =
        find_if( myDistribution.begin(),
                 myDistribution.end(),
                 bind2nd( FrequencyGreater(), rndFreq ) );

    assert( routeElemIt != myDistribution.end() );
    return ( *routeElemIt ).myRoute;
}

//---------------------------------------------------------------------------//

unsigned
MSTriggeredSource::RouteDistribution::getSize( void ) const
{
    return myDistribution.size();
}

//---------------------------------------------------------------------------//

bool
MSTriggeredSource::RouteDistribution::FrequencyGreater::operator() (
    RouteDistElement elem,
    double cmpFreq ) const
{
    return elem.myFrequencySum >= cmpFreq;
}

//---------------------------------------------------------------------------//

MSTriggeredSource::MSTriggeredSource(std::string aXMLFilename )
    : MSSource(),
      myIsWorking( false ),
      myIsRouteDistParsed( false ),
      myIsNewEmitFound( false )
{
    initParser();

    // Open the file and parse the first two tokens.
    try {

        if ( ! myParser->parseFirst( aXMLFilename.c_str(), myToken ) )
        {
            MsgHandler::getErrorInstance()->inform(
                string("MSTriggeredSource ") + myID
                + string(" scanFirst() failed. Quitting"));
            throw ProcessError();
        }

        // Get first token which contains source-id and lane-id
        if ( ! myParser->parseNext( myToken ) ) {
            MsgHandler::getErrorInstance()->inform(
                string("MSTriggeredSource ") + myID
                + string(" Couldn't parse first token of file ")
                + aXMLFilename);
            throw ProcessError();
        }

        // Read RouteDistribution from file.
        if ( myIsWorking ) {

            while ( ! myIsRouteDistParsed && myIsWorking ) { // !!!

                if ( ! myParser->parseNext( myToken ) ) {
                    MsgHandler::getErrorInstance()->inform(
                        string("MSTriggeredSource ") + myID
                        + string(" Couldn't parse RouteDistribution of file ")
                        + aXMLFilename);
                    throw ProcessError();
                }
            }
        }

        // Get second token which contains the first emit parameters.
        while ( myIsWorking && ! myIsNewEmitFound ) {
            if ( ! myParser->parseNext( myToken ) ) {
                MsgHandler::getErrorInstance()->inform(
                    string("MSTriggeredSource ") + myID
                    + string(" Couldn't parse second token of file ")
                    + aXMLFilename);
                throw ProcessError();
            }
        }
    }

    catch ( const XMLException& toCatch ) {
        MsgHandler::getErrorInstance()->inform(
            string("An error occured: '") + aXMLFilename
            + string("'\n") + string("Exception message is: \n")
            + TplConvert<XMLCh>::_2str( toCatch.getMessage() ));
        throw ProcessError();
    }
}


//---------------------------------------------------------------------------//


MSTriggeredSource::~MSTriggeredSource( void )
{
//    delete myRouteDist;

    delete myParser; // Deletes handler???
}

//---------------------------------------------------------------------------//

void
MSTriggeredSource::initParser( void )
{
    //
    //  Create our SAX handler object and install it on the parser, as the
    //  document and error handler.
    //
    MSTriggeredSourceXMLHandler* handler =
        new MSTriggeredSourceXMLHandler( *this ); // !!!
    //
    //  Create a SAX parser object. Then, according to what we were told on
    //  the command line, set it to validate or not.
    //
    myParser = XMLHelpers::getSAXReader(*handler);
}

//---------------------------------------------------------------------------//

SUMOTime
MSTriggeredSource::emit( void )
{
//      // check whether another vehicle was already emitted within this step
//      if( myLastEmit==execTime ) {
//          return 1;
//      }
    // try to emit
    if ( myLane->isEmissionSuccess( myVehicle ) ) {
        MSNet::getInstance()->getVehicleControl().vehiclesEmitted(1);
        myIsNewEmitFound = false;
        readNextEmitElement();
        return 0;
    }
    else {
        // reschedule one timestep.
//        cout << MSNet::globaltime << "<->" << myVehicle->desiredDepart()
//            << "\t" << (MSNet::globaltime - myVehicle->desiredDepart()) << endl;
        return 1;
    }
}

//---------------------------------------------------------------------------//

void
MSTriggeredSource::scheduleEmit( std::string aVehicleId,
                                 SUMOTime aEmitTime,
                                 double      aEmitSpeed,
                                 const MSVehicleType* aVehType )
{
    if ( ! myIsWorking ) {
        MsgHandler::getErrorInstance()->inform("can't happen");
        assert( false );
    }

    // Schedule the emission, build a new car, insert it into the dictionary
    // and save the emitspeed.
    Command* emit = new SimpleCommand< MSTriggeredSource >(
        this, &MSTriggeredSource::emit );


    if( MSEventControl::getBeginOfTimestepEvents()->addEvent(
            emit, aEmitTime,
            MSEventControl::ADAPT_AFTER_EXECUTION ) ) {

        myVehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(
            aVehicleId, myRouteDist.getRndRoute(), aEmitTime,
            aVehType, 0, 0, RGBColor(1, 1, 1));
        MSNet::getInstance()->getVehicleControl().newUnbuildVehicleBuild();
        if ( MSVehicle::dictionary( aVehicleId, myVehicle ) == false ) {
            delete myVehicle;
            assert( false ); // !!!
        }

        // Actually there is no move performed, just an assignement of state.
        myVehicle->moveSetState( MSVehicle::State( myPos, aEmitSpeed ) );
    }
}

//---------------------------------------------------------------------------//

void
MSTriggeredSource::readNextEmitElement( void )
{
    while ( myIsWorking && ! myIsNewEmitFound ) {

        try {
            if ( myParser->parseNext( myToken ) == false ) {
                WRITE_MESSAGE(string("MSTriggeredSource ") + myID + string(": Finished parsing."));
                myIsWorking = false;
            }
        }

        catch ( const XMLException& toCatch ) {
            MsgHandler::getErrorInstance()->inform(
                string("An error occured:\n")
                + string("Exception message is: \n")
                + TplConvert<XMLCh>::_2str( toCatch.getMessage() ));
            myIsWorking = false;
            throw ProcessError();
        }
    }
}

//--------------- DO NOT DEFINE ANYTHING AFTER THIS POINT -------------------//
#ifdef DISABLE_INLINE
#include "MSTriggeredSource.icc"
#endif


// $Log$
// Revision 1.20  2005/02/01 10:10:42  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.2  2004/10/29 05:52:34  dksumo
// fastened up the output of warnings and messages
//
// Revision 1.1  2004/10/22 12:49:26  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.18  2004/01/26 07:50:16  dkrajzew
// using the xmlhelpers instead of building the parser by the object itself
//
// Revision 1.17  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.16  2003/11/20 13:28:38  dkrajzew
// loading and using of a predefined vehicle color added
//
// Revision 1.15  2003/10/31 08:03:39  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
// Revision 1.14  2003/10/15 11:42:35  dkrajzew
// false usage of rand() patched
//
// Revision 1.13  2003/09/30 14:46:59  dkrajzew
// only some make-up made
//
// Revision 1.12  2003/09/17 10:13:03  dkrajzew
// missing report about vehicle emission added
//
// Revision 1.11  2003/08/04 11:35:52  dkrajzew
// only GUIVehicles need a color definition; process of building cars changed
//
// Revision 1.10  2003/07/21 11:00:38  dkrajzew
// informing the network about vehicles still left within the emitters added
//
// Revision 1.9  2003/06/24 14:34:24  dkrajzew
// errors are now reported to the MsgHandler
//
// Revision 1.8  2003/06/06 10:39:17  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.7  2003/04/09 15:36:16  dkrajzew
// debugging of emitters: forgotten release of vehicles (gui) debugged; forgotten initialisation of logger-members debuggt; error managament corrected
//
// Revision 1.6  2003/03/20 16:21:12  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.5  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.4  2002/10/21 09:55:40  dkrajzew
// begin of the implementation of multireferenced, dynamically loadable routes
//
// Revision 1.3  2002/10/17 10:46:21  dkrajzew
// unneeded initialisation and false termination of the XML-system removed
//
// Revision 1.2  2002/10/16 16:39:03  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.5  2002/09/25 18:15:55  roessel
// Types in FrequencyGreater::operator() need not be const for g++-3.2
//
// Revision 1.4  2002/07/31 17:50:02  roessel
// Removed unneede include.
//
// Revision 1.3  2002/07/31 17:32:48  roessel
// Initial sourceforge commit.
//
// Revision 1.18  2002/07/30 15:20:20  croessel
// Made previous changes compilable.
//
// Revision 1.17  2002/07/26 11:44:29  dkrajzew
// Adaptation of past event execution time implemented
//
// Revision 1.16  2002/07/26 11:18:38  dkrajzew
// debugged; report in emails...
//
// Revision 1.15  2002/07/23 16:46:53  croessel
// "Un"const Route*.
// Implemented emit().
// Changes in schedule emit: Vehicle gets it's state immediately.
//
// Revision 1.14  2002/07/23 14:04:23  croessel
// scheduleEmit() implemented.
//
// Revision 1.13  2002/07/23 10:41:14  croessel
// Added try/catch block in readNextElement().
//
// Revision 1.12  2002/07/23 10:03:21  croessel
// Changed XMLCh2local to TplConvert.
//
// Revision 1.11  2002/07/17 16:43:34  croessel
// Moved parser initialization into seperate method.
//
// Revision 1.10  2002/07/17 16:26:30  croessel
// RouteDistribution parsing implemented.
//
// Revision 1.9  2002/07/17 13:38:26  croessel
// RouteDist methods implemented.
//
// Revision 1.8  2002/07/17 10:57:40  croessel
// Removed routeDistribution from constructor, made routeDistribution
// private and updated destructor.
//
// Revision 1.7  2002/07/16 13:13:26  croessel
// Added using namespace std;
// Added #include <cassert>
// Changed #include <x> to relative path "../x" for sumo-includes.
//
// Revision 1.6  2002/07/11 09:10:22  croessel
// Readding file to repository.
//
// Revision 1.4  2002/07/09 18:33:48  croessel
// Implementation of consstructor and readNextElement. Removed getId.
//
// Revision 1.3  2002/07/05 14:52:34  croessel
// Changes in members and constructor. Methods added.
//
// Revision 1.2  2002/06/18 10:14:45  croessel
// Not needed for release 0.7
//
// Revision 1.1  2002/06/12 19:15:12  croessel
// Initial commit.
//


// Local Variables:
// mode:C++
// End:
