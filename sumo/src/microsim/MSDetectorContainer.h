#ifndef MSDETECTORCONTAINER_H
#define MSDETECTORCONTAINER_H

/**
 * @file   MSDetectorContainer.h
 * @author Christian Roessel
 * @date   Started Fri Sep 26 19:11:26 2003
 * @version $Id$
 * @brief  
 * 
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

#include "MSDetectorContainerBase.h"
#include "MSVehicle.h"
#include "MSPredicates.h"
//#include "MS_E2_ZS_Collector.h"
#include <algorithm>
#include <functional>
#include <list>

template< typename InnerContainer >
struct MSDetectorContainer : public MSDetectorContainerBase
{
    typedef typename InnerContainer::value_type ContainerItem;
    typedef typename InnerContainer::const_iterator ContainerConstIt;
    typedef InnerContainer InnerCont;
    
    void enterDetectorByMove( MSVehicle& veh )
        {
            containerM.push_front( ContainerItem( veh ) );
        }
    
    void enterDetectorByEmitOrLaneChange( MSVehicle& veh )
        {
            typedef typename InnerContainer::iterator ContainerIt;
            typedef typename Predicate::PosGreater< ContainerItem >
                PosGreaterPredicate;
            ContainerIt insertIt =
                std::find_if( containerM.begin(), containerM.end(),
                              std::bind2nd(
                                  PosGreaterPredicate(), veh.pos() ) );
            containerM.insert( insertIt, ContainerItem( veh ) );
        }
    
    void leaveDetectorByMove( void )
        {
            containerM.pop_back();
        }
    
    void leaveDetectorByLaneChange( MSVehicle& veh )
        {
            typedef typename InnerContainer::iterator ContainerIt;
            typedef typename Predicate::VehEquals< ContainerItem >
                ErasePredicate;
            ContainerIt eraseIt =
                std::find_if( containerM.begin(), containerM.end(),
                              std::bind2nd(
                                  VehEqualsPredicate(), &veh ) );
            containerM.erase( eraseIt );
        }
    
    virtual void update( void )
        {}

    MSDetectorContainer( void ) 
        : containerM()
        {}
    
    virtual ~MSDetectorContainer( void )
        {
            containerM.clear();
        }
    
    InnerContainer containerM;
    
};


namespace DetectorContainer
{
    typedef MSDetectorContainer< double > Counter;
    typedef MSDetectorContainer< std::list< MSVehicle* > > Vehicles;
}

// Spezialization
template<>
struct DetectorContainer::Counter : public MSDetectorContainerBase
{
    typedef double Container;
    
    void enterDetectorByMove( MSVehicle& veh )
        {
            ++containerM;
        }
    
    void enterDetectorByEmitOrLaneChange( MSVehicle& veh )
        {
            ++containerM;
        }
    
    void leaveDetectorByMove( void )
        {
            --containerM;
        }
    
    void leaveDetectorByLaneChange( MSVehicle& veh )
        {
            --containerM;
        }
    
    void update( void )
        {}

    MSDetectorContainer( void ) 
        : containerM( 0 )
        {}
    
    virtual ~MSDetectorContainer( void )
        {}

    Container containerM;
};



#endif // MSDETECTORCONTAINER_H

// Local Variables:
// mode:C++
// End:
