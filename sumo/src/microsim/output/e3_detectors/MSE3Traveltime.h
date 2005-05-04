#ifndef MSE3TRAVELTIME_H
#define MSE3TRAVELTIME_H

///
/// @file    MSE3Traveltime.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Thu Nov 27 2003 15:25 CET
/// @version $Id$
///
/// @brief
///
///

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <microsim/output/MSDetectorContainerWrapper.h>
#include <string>
#include <microsim/MSUnit.h>
#include <microsim/MSNet.h>

class MSVehicle;

class MSE3Traveltime
{
private:

protected:

    typedef double DetectorAggregate;
    typedef DetectorContainer::TimeMap Container;
    typedef Container::InnerContainer TravelTimeMap;

    MSE3Traveltime( const Container& container )
        :
        containerM( container )
        {}

    virtual ~MSE3Traveltime( void )
        {}

    bool hasVehicle( MSVehicle& veh ) const
        {
            return containerM.hasVehicle( &veh );
        }

    DetectorAggregate getValue( MSVehicle& veh ) // [s]
        {
            TravelTimeMap::const_iterator pair =
                containerM.containerM.find( &veh );
            assert( pair != containerM.containerM.end() );
            MSUnit::Seconds startTime = pair->second;
            MSUnit::Seconds time = MSNet::getInstance()->simSeconds();
            assert( time > startTime );
            return  time - startTime;
        }

    static std::string getDetectorName( void )
        {
            return "meanTraveltime";
        }

private:
    const Container& containerM;

    MSE3Traveltime();
    MSE3Traveltime( const MSE3Traveltime& );
    MSE3Traveltime& operator=( const MSE3Traveltime& );

};

// Local Variables:
// mode:C++
// End:

#endif // MSE3TRAVELTIME_H
