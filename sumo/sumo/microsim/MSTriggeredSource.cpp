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

// $Revision$

namespace
{
    const char rcsid[] =
    "$Id$";
}


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include "MSTriggeredSource.h"
#include "MSTriggeredSourceXMLHandler.h"
#include "MSVehicleType.h"
#include "MSVehicle.h"
#include "MSEventControl.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include "../utils/TplConvert.h"
#include "../helpers/Command.h"
#include "../helpers/SimpleCommand.h"
#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <functional>

using namespace std;

//---------------------------------------------------------------------------//

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
    const MSNet::Route* aRoute,
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

const MSNet::Route*
MSTriggeredSource::RouteDistribution::getRndRoute( void )
{
    // Get random value between 0 and
    // myDistribution.back().myFrequencySum and return the route of
    // the first element that has a higher frequency sum.
    double rndFreq = double( rand() ) / double( RAND_MAX ) *
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
    const RouteDistElement elem,
    double cmpFreq ) const
{
    return elem.myFrequencySum >= cmpFreq;
}

//---------------------------------------------------------------------------//

MSTriggeredSource::MSTriggeredSource(
    MSEventControl&    aEventControl,
    std::string        aXMLFilename )
    : MSSource(),
      myEventControl( aEventControl ),
      myIsWorking( false ),
      myIsRouteDistParsed( false ),
      myIsNewEmitFound( false )
{
    initParser();

    // Open the file and parse the first two tokens.
    try {

        if ( ! myParser->parseFirst( aXMLFilename.c_str(), myToken ) )
        {
            cerr << "MSTriggeredSource " << myID
                 << " scanFirst() failed. Quitting" << endl;
            XMLPlatformUtils::Terminate();
            return;
        }

        // Get first token which contains source-id and lane-id
        if ( ! myParser->parseNext( myToken ) ) {
            cerr << "MSTriggeredSource " << myID
                 << " Couldn't parse first token of file "
                 << aXMLFilename << ". Quitting." << endl;
            XMLPlatformUtils::Terminate();
            return;
        }

        // Read RouteDistribution from file.
        if ( myIsWorking ) {

            while ( ! myIsRouteDistParsed && myIsWorking ) { // !!!

                if ( ! myParser->parseNext( myToken ) ) {
                    cerr << "MSTriggeredSource " << myID
                         << " Couldn't parse RouteDistribution of file "
                         << aXMLFilename << ". Quitting." << endl;
                    XMLPlatformUtils::Terminate();
                    return;
                }
            }
        }

        // Get second token which contains the first emit parameters.
        while ( myIsWorking && ! myIsNewEmitFound ) {
            if ( ! myParser->parseNext( myToken ) ) {
                cerr << "MSTriggeredSource " << myID
                     << " Couldn't parse second token of file "
                    << aXMLFilename << ". Quitting." << endl;
                XMLPlatformUtils::Terminate();
                return;
            }
        }
    }

    catch ( const XMLException& toCatch ) {
        cerr << "An error occured: '" << aXMLFilename << "'\n"
             << "Exception message is: \n"
             << TplConvert<XMLCh>::_2str( toCatch.getMessage() )
             << "\n" << endl;
        XMLPlatformUtils::Terminate();
        return;
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
    // Initialize the XML4C2 system
    try {
        XMLPlatformUtils::Initialize();
    }

    catch ( const XMLException& toCatch ) {
        cerr << "MSTriggeredSource " << myID
             << " Error during initialization! Message:\n"
             << TplConvert<XMLCh>::_2str( toCatch.getMessage() )
             << ". Quitting." << endl;
        XMLPlatformUtils::Terminate();
        return;
    }


    //
    //  Create a SAX parser object. Then, according to what we were told on
    //  the command line, set it to validate or not.
    //
    myParser = XMLReaderFactory::createXMLReader();
    myParser->setFeature(
        XMLString::transcode(
            "http://xml.org/sax/features/namespaces" ), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/schema" ), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/schema-full-checking"),
        false );
    myParser->setFeature(
        XMLString::transcode(
            "http://xml.org/sax/features/validation"), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/dynamic" ), false );



    //
    //  Create our SAX handler object and install it on the parser, as the
    //  document and error handler.
    //
    MSTriggeredSourceXMLHandler* handler =
        new MSTriggeredSourceXMLHandler( *this ); // !!!
    myParser->setContentHandler( handler );
    myParser->setErrorHandler( handler );
}

//---------------------------------------------------------------------------//

MSNet::Time
MSTriggeredSource::emit( void )
{
//      // check whether another vehicle was already emitted within this step
//      if( myLastEmit==execTime ) {
//          return 1;
//      }
    // try to emit
    if ( myLane->isEmissionSuccess( myVehicle ) ) {
//          myLastEmit = execTime;
        myIsNewEmitFound = false;
        readNextEmitElement();
        
        return 0;
    }
    else {
        // reschedule one timestep.
        return 1;
    }
}

//---------------------------------------------------------------------------//

void
MSTriggeredSource::scheduleEmit( std::string aVehicleId,
                                 MSNet::Time aEmitTime,
                                 double      aEmitSpeed,
                                 const MSVehicleType* aVehType )
{
    if ( ! myIsWorking ) {
        cerr << "can't happen" << endl;
        assert( false );
    }

    // Schedule the emission, build a new car, insert it into the dictionary
    // and save the emitspeed.
    Command* emit = new SimpleCommand< MSTriggeredSource >(
        this, &MSTriggeredSource::emit );

  
    if( myEventControl.addEvent(
            emit, aEmitTime,
            MSEventControl::ADAPT_AFTER_EXECUTION ) ) {
        
        myVehicle = new MSVehicle(
            aVehicleId,
            const_cast< MSNet::Route* >( myRouteDist.getRndRoute() ),
            aEmitTime,
            aVehType );
        
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

                cout << "MSTriggeredSource " << myID
                     << ": Finished parsing." << endl;
                myIsWorking = false;
            }
        }

        catch ( const XMLException& toCatch ) {
            cerr << "An error occured:\n"
                 << "Exception message is: \n"
                 << TplConvert<XMLCh>::_2str( toCatch.getMessage() )
                 << "\n" << endl;
            myIsWorking = false;
            XMLPlatformUtils::Terminate();
            return;
        }
    }
}

//--------------- DO NOT DEFINE ANYTHING AFTER THIS POINT -------------------//
#ifdef DISABLE_INLINE
#include "MSTriggeredSource.icc"
#endif


// $Log$
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







