#ifndef MSTriggeredSourceXMLHandler_H
#define MSTriggeredSourceXMLHandler_H
//---------------------------------------------------------------------------//
//                        MSTriggeredSourceXMLHandler.h  -  Class to
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

/** @file MSTriggeredSourceXMLHandler.h
    Contains the declarations of class <tt>MSTriggeredSourceXMLHandler</tt>.
    @note You can disable inlining by adding @c DISABLE_INLINE
    to your <tt>CXXFLAGS</tt>.
    @author Christian R&ouml;ssel, roessel@zpr.uni-koeln.de
    @version Revision $Revision$ from $Date$
    by $Author$
    @date Started at Wed, 03 Jul 2002 
*/

// $Revision$

#include <xercesc/sax2/DefaultHandler.hpp>
#include <map>
#include <string>
#include "MSNet.h"

class Attributes;
class MSTriggeredSource;
class MSVehicleType;

/**
   @class MSTriggeredSourceXMLHandler
   @brief Class to handle XML-input for the MSTriggerdSource class.


*/
class MSTriggeredSourceXMLHandler : public DefaultHandler
{

public:    // public methods
    /// public constructor.
    MSTriggeredSourceXMLHandler( MSTriggeredSource& aSource );

    /// Destructor.
    ~MSTriggeredSourceXMLHandler( void );

    void startElement( const XMLCh* const uri,
                       const XMLCh* const localname,
                       const XMLCh* const qname,
                       const Attributes& attrs );

    void endElement( const XMLCh* const uri,
                     const XMLCh* const localname,
                     const XMLCh* const qname );

    void warning( const SAXParseException& exception );

    void error( const SAXParseException& exception );

    void fatalError( const SAXParseException& exception );


protected: // protected methods

    bool isParseTriggeredSourceTokenSuccess( const XMLCh* const aLocalname,
                                             const Attributes& aAttributes );

    bool isParseRouteDistSuccess( const XMLCh* const aLocalname,
                                  const Attributes& aAttributes );

    bool isParseEmitTokenSuccess( const XMLCh* const aLocalname,
                                  const Attributes& aAttributes );

    typedef std::map< std::string, std::string > AttributesMap;
    bool isAttributes2mapSuccess( AttributesMap& aMap,
                                  const Attributes& aAttributes );

    bool isString2doubleSuccess( std::string toConvert,
                                 double& converted ) const;

    bool isProperRouteDistValues( void );

    bool isProperEmitValues( void );

    double roundToNearestInt( double toRound ) const;

private:   // private methods
    /// Default constructor.
    MSTriggeredSourceXMLHandler( void );

    /// Copy constructor.
    MSTriggeredSourceXMLHandler( const MSTriggeredSourceXMLHandler& );

    /// Assignment operator.
    MSTriggeredSourceXMLHandler& operator=( const MSTriggeredSourceXMLHandler& );

protected: // protected members



private:   // private members

    MSTriggeredSource& mySource;


    AttributesMap myEmitAttributes;
    AttributesMap myRouteDistAttributes;

    std::string myEmitId;

    MSNet::Time myEmitTime;

    double myEmitSpeed;

    MSVehicleType* myEmitVehType;

    bool myIsParsedTriggeredSourceToken;

    bool myIsParsedRouteDistToken;

};  // end MSTriggeredSourceXMLHandler


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//
#ifndef DISABLE_INLINE
#include "MSTriggeredSourceXMLHandler.icc"
#endif // DISABLE_INLINE


// $Log$
// Revision 1.2  2002/10/16 16:39:03  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1  2002/07/31 17:32:48  roessel
// Initial sourceforge commit.
//
// Revision 1.5  2002/07/30 15:20:21  croessel
// Made previous changes compilable.
//
// Revision 1.4  2002/07/17 16:26:31  croessel
// RouteDistribution parsing implemented.
//
// Revision 1.3  2002/07/16 13:16:28  croessel
// Declaration of new method roundToNearestInt.
//
// Revision 1.2  2002/07/09 18:38:26  croessel
// New methods isParseParentTokenSuccess and isParseEmitTokenSuccess and
// member myIsParsedParentToken.
//
// Revision 1.1  2002/07/05 14:53:26  croessel
// Initial commit.
//


// Local Variables:
// mode:C++
// End:

#endif // MSTriggeredSourceXMLHandler_H
