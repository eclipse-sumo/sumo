#ifndef MSTriggeredSource_H
#define MSTriggeredSource_H
//---------------------------------------------------------------------------//
//                        MSTriggeredSource.h  -  Concrete Source that
//                        reads emit-times and -speeds from a file and
//                        chooses the routes according to a
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

#include "MSSource.h"
#include "MSNet.h"
#include <string>
#include <vector>
#include <xercesc/sax2/XMLReaderFactory.hpp>

class MSEventControl;
class MSVehicleType;
class SAX2XMLReader;
class MSVehicle;

/**
   @class MSTriggeredSource
   Concrete Source that reads emit-times and -speeds from a
   file and chooses the routes according to a distribution.
   @see MSSource

*/
class MSTriggeredSource : public MSSource
{
public:
    friend class MSTriggeredSourceXMLHandler;

    /** Customized construtor.
        @param aEventControl Pointer to EventController that schedules the
        emits.
        @param aFilename Name of the file that contains the starting
        times and speeds.
        @param aRouteDist RouteDistribution-container, should not be
        empty. The sum of @ref myFrequency is not bound
        to a fixed value.
    */
    MSTriggeredSource(
        MSEventControl&    aEventControl,
        std::string        aXMLFilename );


    /// Destructor.
    ~MSTriggeredSource( void );


    /** Emit a scheduled vehicle on the corresponding myEmitLane. This
        method is called by MSEventControl at the proper time.
        @return Returns 0 if emission was successful, otherwise 1 to be
        rescheduled at the next timestep.
    */
    MSNet::Time emit( void );



protected: // protected methods
    void initParser( void );

    void scheduleEmit( std::string aVehicleId,
                       MSNet::Time aEmitTime,
                       double      aEmitSpeed,
                       const MSVehicleType* aVehType );

    void readNextEmitElement( void );

private:   // private methods
    /// Default constructor.
    MSTriggeredSource( void );

    /// Copy constructor.
    MSTriggeredSource( const MSTriggeredSource& );

    /// Assignment operator.
    MSTriggeredSource& operator=( MSTriggeredSource& );

protected: // protected members



private:   // private members
    /**

    */
    class RouteDistribution
    {
    public:
        /// Default constructor.
        RouteDistribution( void );

        /// Destructor.
        ~RouteDistribution( void );
        void addElement( const MSNet::Route* aRoute,
                         double aFrequency );
        const MSNet::Route* getRndRoute( void );
        unsigned getSize( void ) const;
    protected:
    private:
        /// Copy constructor.
        RouteDistribution( const RouteDistribution& );

        /// Assignment operator.
        RouteDistribution& operator=( RouteDistribution& );
    protected:
    private:
        /**
         */
        struct RouteDistElement
        {
            /// Pointer to route. Shouldn't be 0.
            const MSNet::Route* myRoute;

            /** Route-pointers frequency.
                Frequency with which the vehicles will get the
                corresponding route-pointer. Is not bound to a maximum
                value, but should be > 0.
            */
            double myFrequency;

            double myFrequencySum;
        };

        typedef std::vector< RouteDistElement > RouteDistCont;
        RouteDistCont myDistribution;

        /**
         */
        struct FrequencyGreater :
            public std::binary_function< RouteDistElement,
                                         double, bool >
        {
            bool operator() ( RouteDistElement elem,
                              double cmpFreq ) const;
        };

    }; // end class RouteDistribution

    MSEventControl& myEventControl;
    RouteDistribution myRouteDist;
    bool myIsWorking;
    bool myIsRouteDistParsed;
    bool myIsNewEmitFound;
    MSVehicle* myVehicle;
//    MSNet::Time myLastEmit;

    SAX2XMLReader* myParser;
    XMLPScanToken  myToken;

};  // end MSTriggeredSource


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//
#ifndef DISABLE_INLINE
#include "MSTriggeredSource.icc"
#endif // DISABLE_INLINE


// $Log$
// Revision 1.5  2002/09/25 18:15:55  roessel
// Types in FrequencyGreater::operator() need not be const for g++-3.2
//
// Revision 1.4  2002/07/31 17:32:48  roessel
// Initial sourceforge commit.
//
// Revision 1.16  2002/07/30 15:20:20  croessel
// Made previous changes compilable.
//
// Revision 1.15  2002/07/26 11:18:38  dkrajzew
// debugged; report in emails...
//
// Revision 1.14  2002/07/23 16:44:48  croessel
// "Un"const Route* in RouteDistribution.
// Removed member myEmitSpeed because vehicle will get it's state immediately.
//
// Revision 1.13  2002/07/23 14:04:23  croessel
// scheduleEmit() implemented.
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
// Revision 1.7  2002/07/11 09:10:21  croessel
// Readding file to repository.
//
// Revision 1.5  2002/07/09 18:31:02  croessel
// Made getRndRoute and getSize const.
// Parameter change in constructor.
// Removed getId, use the one in base class.
// Introduced new members parser and token.
//
// Revision 1.4  2002/07/05 14:52:34  croessel
// Changes in members and constructor. Methods added.
//
// Revision 1.3  2002/06/18 10:14:45  croessel
// Not needed for release 0.7
//
// Revision 1.2  2002/06/12 19:41:06  croessel
// Added a emission lane.
//
// Revision 1.1  2002/06/12 19:15:12  croessel
// Initial commit.
//


// Local Variables:
// mode:C++
// End:

#endif // MSTriggeredSource_H
