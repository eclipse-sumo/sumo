//---------------------------------------------------------------------------//
//                        MSTriggeredSourceXMLHandler.cpp  -  Class to
//                        handle XML-input for the MSTriggerdSource class.
//                           -------------------
//  begin                : Wed, 03 Jul 2002
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

/** @file MSTriggeredSourceXMLHandler.cpp
    Contains the definitions of class <tt>MSTriggeredSourceXMLHandler</tt>.
    @note You can disable inlining by adding @c DISABLE_INLINE
    to your <tt>CXXFLAGS</tt>.
    @author Christian R&ouml;ssel, roessel@zpr.uni-koeln.de
    @version Revision $Revision$ from $Date$
    by $Author$
    @date Started at Wed, 03 Jul 2002
*/

// $Revision$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSTriggeredSourceXMLHandler.h"
#include "MSTriggeredSource.h"
#include "MSLane.h"
#include "MSRoute.h"
#include <vector>
#include <utility>
#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cmath>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLString.hpp>
#include <utils/convert/TplConvert.h>
#include <utils/common/UtilExceptions.h>

using namespace std;

//---------------------------------------------------------------------------//

MSTriggeredSourceXMLHandler::MSTriggeredSourceXMLHandler(
    MSTriggeredSource& aSource )
    : mySource( aSource ),
      myEmitTime(0),
      myIsParsedTriggeredSourceToken( false ),
      myIsParsedRouteDistToken( false )
{
    myRouteDistAttributes.insert( make_pair( string( "route" ),
                                             string( "" ) ) );
    myRouteDistAttributes.insert( make_pair( string( "frequency" ),
                                             string( "" ) ) );
    myEmitAttributes.insert( make_pair( string( "id" ),      string( "" ) ) );
    myEmitAttributes.insert( make_pair( string( "time" ),    string( "" ) ) );
    myEmitAttributes.insert( make_pair( string( "speed" ),   string( "" ) ) );
    myEmitAttributes.insert( make_pair( string( "vehtype" ), string( "" ) ) );
}

//---------------------------------------------------------------------------//

MSTriggeredSourceXMLHandler::~MSTriggeredSourceXMLHandler( void )
{
    myEmitAttributes.clear();
}

//---------------------------------------------------------------------------//

void
MSTriggeredSourceXMLHandler::startElement( const XMLCh* const aUri,
                                           const XMLCh* const aLocalname,
                                           const XMLCh* const aQname,
                                           const Attributes& aAttributes )
{
    // The first invocation reads the id and lane of this
    // triggeredsource. Subsequent calls read the routeDist, emit parameters
    // and will schedule emits.
    if ( ! myIsParsedTriggeredSourceToken ) {

        mySource.myIsWorking =
            isParseTriggeredSourceTokenSuccess( aLocalname, aAttributes );
        myIsParsedTriggeredSourceToken = mySource.myIsWorking;
    }
    else if ( ! mySource.myIsRouteDistParsed && mySource.myIsWorking ) {

        mySource.myIsWorking =
                 isParseRouteDistSuccess( aLocalname, aAttributes );
    }
    else if ( mySource.myIsWorking ) {


        if ( isParseEmitTokenSuccess( aLocalname, aAttributes ) ) {

            mySource.scheduleEmit( myEmitId, myEmitTime, myEmitSpeed,
                                   myEmitVehType );
        }
        else {

            mySource.readNextEmitElement();
        }
    }
    else {

        //can't happen
        assert( false );
    }
}

//---------------------------------------------------------------------------//

void
MSTriggeredSourceXMLHandler::endElement( const XMLCh* const aUri,
                                         const XMLCh* const aLocalname,
                                         const XMLCh* const aQname )
{
    if ( TplConvert<XMLCh>::_2str( aLocalname ) ==
         string( "routedist" ) ) {

        mySource.myIsRouteDistParsed = true;
        mySource.myIsWorking = true;
    }
}


//---------------------------------------------------------------------------//

void
MSTriggeredSourceXMLHandler::error( const SAXParseException& e )
{
    cout << "\nError at file " << TplConvert<XMLCh>::_2str( e.getSystemId() )
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << TplConvert<XMLCh>::_2str( e.getMessage() )
         << endl;
}

//---------------------------------------------------------------------------//

void
MSTriggeredSourceXMLHandler::fatalError( const SAXParseException& e )
{
    cout << "\nFatal Error at file "
         << TplConvert<XMLCh>::_2str( e.getSystemId() )
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << TplConvert<XMLCh>::_2str( e.getMessage() )
         << endl;
}

//---------------------------------------------------------------------------//

void
MSTriggeredSourceXMLHandler::warning( const SAXParseException& e )
{
    cout << "\nWarning at file " << TplConvert<XMLCh>::_2str( e.getSystemId() )
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << TplConvert<XMLCh>::_2str( e.getMessage() )
         << endl;
}

//---------------------------------------------------------------------------//

bool
MSTriggeredSourceXMLHandler::isProperRouteDistValues( void )
{
    // check if route exists
    string routeStr = myRouteDistAttributes.find( string( "route" ) )->second;

    MSRoute* route = MSRoute::dictionary( routeStr );
    if ( route == 0 ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Route '" << routeStr << "' does not exist."
             << endl;
        throw ProcessError();
    }

    // check frequency
    string freqStr =
        myRouteDistAttributes.find( string( "frequency" ) )->second;
    double freq;
    if ( ! isString2doubleSuccess( freqStr, freq ) ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": No conversion possible on attribute \"frequency\" "
            "with value " << freqStr << ". Quitting." << endl;
        throw ProcessError();
    }
    if ( freq < double( 0 ) ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Attribute \"frequency\" has value < 0. Quitting." << endl;
        throw ProcessError();
    }

    // Attributes ok, add to routeDist
    mySource.myRouteDist.addElement( route, freq );
    return true;
}

//---------------------------------------------------------------------------//

bool
MSTriggeredSourceXMLHandler::isProperEmitValues( void )
{

    // check and assign id
    myEmitId = mySource.getId() + string( "_" ) +
        myEmitAttributes.find( string( "id" ) )->second;

    MSVehicle* veh = MSVehicle::dictionary( myEmitId );
    if ( veh != 0 ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Vehicle " << myEmitId << " does already exist. "
            "Continuing with next element." << endl;
        throw ProcessError();
    }


    // check and assign vehicle type
    string emitType = myEmitAttributes.find( string( "vehtype" ) )->second;
    MSVehicleType* type = MSVehicleType::dictionary( emitType );
    if ( type == 0 ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Vehicle type " << emitType << " does not exist. "
            "Continuing with next element." << endl;
        throw ProcessError();
    }
    else {

        myEmitVehType = type;
    }

    // check and assign emission time
    string timeStr = myEmitAttributes.find( string( "time" ) )->second;
    double time;
    if ( ! isString2doubleSuccess( timeStr, time ) )
    {
        cerr << "MSTriggeredSource " << mySource.getId()
             << ": No conversion possible on attribute \"time\" with value "
             << timeStr << ". Continuing with next element."
             << endl;
        throw ProcessError();
    }

    MSNet::Time roundedTime = static_cast< MSNet::Time > (
            roundToNearestInt( time / double( MSNet::deltaT() ) ) );
/*    if ( roundedTime <= myEmitTime ) {
        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Emit times not sorted. Continuing with next element."
             << endl;
        throw ProcessError();
    }
    else {*/

        myEmitTime = roundedTime;
    //}

    // check and assign emission speed
    string speedStr = myEmitAttributes.find( string( "speed" ) )->second;
    double speed;
    if ( ! isString2doubleSuccess( speedStr, speed ) ) {
        cerr << "MSTriggeredSource " << mySource.getId()
             << ": No conversion possible on attribute \"speed\" with value "
             << speedStr << ". Continuing with next element."
             << endl;
        throw ProcessError();
    }
    if ( speed < 0 || speed > mySource.myLane->maxSpeed() ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Speed < 0 or > lane's max-speed. "
            "Continuing with next element." << endl;
        throw ProcessError();
    }
    else {

        myEmitSpeed = speed;
    }

    return true;
}

//---------------------------------------------------------------------------//

bool
MSTriggeredSourceXMLHandler::isParseTriggeredSourceTokenSuccess(
    const XMLCh* const aLocalname,
    const Attributes& aAttributes )
{
    assert( mySource.myIsWorking == false ); // multiple tokens
                                             // triggeredsource

    if ( TplConvert<XMLCh>::_2str( aLocalname ) !=
         string( "triggeredsource" ) ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": No parent token \"triggeredsource\" found."
             << endl;
        throw ProcessError();
    }

    // Check for the two attributes id, lane and pos
    if ( aAttributes.getLength() != 3 ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Wrong number of attributes in first token. "
             << endl;
        throw ProcessError();
    }

    // Check attributes and assign them to members
    for ( unsigned i = 0; i < aAttributes.getLength(); ++i ) {

        string attrName(
            TplConvert<XMLCh>::_2str(
                aAttributes.getLocalName( i ) ) );
        string attrValue(
            TplConvert<XMLCh>::_2str(
                aAttributes.getValue( i ) ) );

        if ( attrName == string( "id" ) ) {

            mySource.setId( attrValue );
        }
        if ( attrName == string( "lane" ) ) {

            MSLane* lane = MSLane::dictionary( attrValue );
            if ( lane != 0 ) {

                mySource.setLane( lane );
            }
            else {

                cerr << "MSTriggeredSource " << mySource.getId()
                     << ": Lane-id " << attrValue << " does not exist."
                     << endl;
                throw ProcessError();
            }
        }
        if ( attrName == string( "pos" ) ) {

            double pos;
            if ( isString2doubleSuccess( attrValue, pos ) ) {

                if ( pos >= 0 && pos <= mySource.myLane->length() ) {

                    mySource.setPos( pos );
                }
                else {

                    cerr << "MSTriggeredSource " << mySource.getId()
                         << ": Position of source not within lane."
                         << endl;
                    throw ProcessError();
                }
            }
            else {

                cerr << "MSTriggeredSource " << mySource.getId()
                     << ": No conversion possible on attribute " << attrName
                     << endl;
                throw ProcessError();
            }
        }
    }
    // Parsing of parent-token succesful
    return true;
}

//---------------------------------------------------------------------------//

bool
MSTriggeredSourceXMLHandler::isParseRouteDistSuccess(
    const XMLCh* const aLocalname,
    const Attributes& aAttributes )
{
    string elemName = TplConvert<XMLCh>::_2str( aLocalname );
    if ( elemName != string( "routedist" ) &&
         elemName != string( "routedistelem" ) ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Token name \"" << elemName
             << "\" sould be \"routedist\" or \"routedistelem\". Quitting."
             << endl;
        throw ProcessError();
    }

    if ( ! myIsParsedRouteDistToken ) {

        if ( elemName == string( "routedist" ) ) {

            myIsParsedRouteDistToken = true;
            return true;
        }
        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Token name \"" << elemName
             << "\" sould be \"routedist\". Quitting."
             << endl;
        throw ProcessError();
    }
    else {

        if ( elemName == string( "routedist" ) ) {

            cerr << "MSTriggeredSource " << mySource.getId()
                 << ": Multiple tokens \"routedist\". Quitting."
                 << endl;
            throw ProcessError();
        }

        if ( elemName == string( "routedistelem" ) ) {

            // check attributes
            if ( aAttributes.getLength() != myRouteDistAttributes.size() ) {

                cerr << "MSTriggeredSource " << mySource.getId()
                     << ": Wrong number of attributes during RouteDist"
                    " parsing. Quitting." << endl;
                throw ProcessError();
            }

            if ( ! isAttributes2mapSuccess(
                     myRouteDistAttributes, aAttributes ) ) {

                cerr << "MSTriggeredSource " << mySource.getId()
                     << ": Wrong attribute during RouteDist parsing. "
                    "Quiting." << endl;
                throw ProcessError();
            }

            return isProperRouteDistValues();
        }
        else {

            cerr << "MSTriggeredSource " << mySource.getId()
                 << ": Token name \"" << elemName
                 << "\" sould be \"routedistelem\". Quitting."
                 << endl;
            throw ProcessError();
        }
    }
}

//---------------------------------------------------------------------------//

bool
MSTriggeredSourceXMLHandler::isParseEmitTokenSuccess(
    const XMLCh* const aLocalname,
    const Attributes& aAttributes )
{
    if ( TplConvert<XMLCh>::_2str( aLocalname ) !=
         string( "emit" ) ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Token name \""
             << TplConvert<XMLCh>::_2str( aLocalname )
             << "\" sould be \"emit\". Continuing with next element."
             << endl;
        throw ProcessError();
    }
    if ( aAttributes.getLength() != myEmitAttributes.size() ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Wrong number of attributes. "
            "Continuing with next element." << endl;
        throw ProcessError();
    }

    if ( ! isAttributes2mapSuccess( myEmitAttributes, aAttributes ) ) {

        cerr << "MSTriggeredSource " << mySource.getId()
             << ": Wrong attribute. "
            "Continuing with next element." << endl;
        throw ProcessError();
    }

    mySource.myIsNewEmitFound = true;
    return isProperEmitValues();
}

//---------------------------------------------------------------------------//

bool
MSTriggeredSourceXMLHandler::isAttributes2mapSuccess(
    AttributesMap& aMap,
    const Attributes& aAttributes )
{
    // read all attribute values into attributesMap. Quit on error.
    for ( unsigned i = 0; i < aAttributes.getLength(); ++i ) {

        string key( TplConvert<XMLCh>::_2str( aAttributes.getLocalName( i ) ) );
        AttributesMap::iterator attrIt =
            aMap.find( key );

        if ( attrIt == myEmitAttributes.end() ) {

            throw ProcessError();
        }

        // We now have a valid attribute.
        // attrIt->second = ... gives a segfault :-(

        aMap[ key ] = TplConvert<XMLCh>::_2str( aAttributes.getValue( i ) );
    }
    return true;
}

//---------------------------------------------------------------------------//

bool
MSTriggeredSourceXMLHandler::isString2doubleSuccess(
    string toConv,
    double& converted ) const
{
    try {
        converted = TplConvert< char >::_2float( toConv.c_str() );
        return true;
    }
    catch ( NumberFormatException ) {
        return false;
    }
//      const char* toConvert = toConv.c_str();
//      char** endptr = 0;
//      errno = 0;
//      converted = strtod( toConvert, endptr );

//      // conversion performed?
//      if ( converted == double(0) && toConvert == *endptr ) {

//          return false;
//      }
//      // over- or underflow?
//      if ( errno == ERANGE ) {

//          return false;
//      }
//      return true;
}

//---------------------------------------------------------------------------//

double
MSTriggeredSourceXMLHandler::roundToNearestInt( double aValue ) const
{
    double integralPart;
    if ( modf( aValue, &integralPart ) * double( 2 ) > double( 1 ) ) {

        return ceil( aValue );
    }
    else {

        return floor( aValue );
    }
}


//--------------- DO NOT DEFINE ANYTHING AFTER THIS POINT -------------------//
#ifdef DISABLE_INLINE
#include "MSTriggeredSourceXMLHandler.icc"
#endif

// $Log$
// Revision 1.5  2002/10/28 12:58:58  dkrajzew
// errors on parsing the Source-XML-description cause throwing of ProcessErrors now
//
// Revision 1.4  2002/10/21 09:55:40  dkrajzew
// begin of the implementation of multireferenced, dynamically loadable routes
//
// Revision 1.3  2002/10/17 13:36:27  dkrajzew
// debug informations removed
//
// Revision 1.2  2002/10/16 16:39:03  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.2  2002/07/31 17:50:02  roessel
// Removed unneede include.
//
// Revision 1.1  2002/07/31 17:32:48  roessel
// Initial sourceforge commit.
//
// Revision 1.12  2002/07/31 17:10:52  croessel
// CHanged keyword route to routeid.
//
// Revision 1.11  2002/07/31 16:21:49  croessel
// Changed attrIt->second to aMap[key] due to segfault.
//
// Revision 1.10  2002/07/30 15:20:21  croessel
// Made previous changes compilable.
//
// Revision 1.9  2002/07/26 11:18:38  dkrajzew
// debugged; report in emails...
//
// Revision 1.8  2002/07/23 16:39:39  croessel
// "Un"const Route* in isProperRouteDistValues.
// Changed aAttributes.getLocalName() to getValue() in
// isParseTriggeredSourceTokenSuccess().
//
// Revision 1.7  2002/07/23 15:28:10  croessel
// Added parsing of attribute pos to isParseTriggeredSourceTokenSuccess.
//
// Revision 1.6  2002/07/23 10:50:21  croessel
// Check for duplicate vehicle ids added in isProperEmitValues().
//
// Revision 1.5  2002/07/23 10:03:21  croessel
// Changed XMLCh2local to TplConvert.
//
// Revision 1.4  2002/07/17 16:26:30  croessel
// RouteDistribution parsing implemented.
//
// Revision 1.3  2002/07/16 13:15:44  croessel
// Changed #include <x> to relative path "../x" for sumo-includes.
// Definition of new method roundToNearestInt.
//
// Revision 1.2  2002/07/09 18:40:43  croessel
// Reimplementation of startElement.
// Implementation of isProperValues, isParseParentTokenSuccess and
// isParseEmitTokenSuccess.
//
// Revision 1.1  2002/07/05 14:53:26  croessel
// Initial commit.
//

// Local Variables:
// mode:C++
// End:


