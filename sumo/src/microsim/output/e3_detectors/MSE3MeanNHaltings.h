#ifndef MSE3MEANNHALTINGS_H
#define MSE3MEANNHALTINGS_H

///
/// @file    MSE3MeanNHaltings.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Fri Nov 28 2003 12:27 CET
/// @version
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

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <microsim/output/MSDetectorHaltingContainerWrapper.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSE3MeanNHaltings
{
public:

protected:
    typedef double DetectorAggregate;
    typedef DetectorContainer::HaltingsMap Container;
    typedef Container::InnerContainer HaltingsMap;

    MSE3MeanNHaltings( const Container& container )
        :
        containerM( container )
        {}

    virtual ~MSE3MeanNHaltings( void )
        {}

    bool hasVehicle( MSVehicle& veh ) const
        {
            return containerM.hasVehicle( &veh );
        }

    DetectorAggregate getValue( MSVehicle& veh )
        {
            HaltingsMap::const_iterator pair =
                containerM.containerM.find( &veh );
            assert( pair != containerM.containerM.end() );
            return pair->second.nHalts;
        }

    static std::string getDetectorName( void )
        {
            return "meanNHaltsPerVehicle";
        }

private:
    const Container& containerM;

    MSE3MeanNHaltings();
    MSE3MeanNHaltings( const MSE3MeanNHaltings& );
    MSE3MeanNHaltings& operator=( const MSE3MeanNHaltings& );
};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/



// Local Variables:
// mode:C++
// End:

#endif // MSE3MEANNHALTINGS_H
