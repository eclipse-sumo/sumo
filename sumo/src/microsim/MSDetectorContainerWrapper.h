#ifndef MSDETECTORCONTAINERWRAPPER_H
#define MSDETECTORCONTAINERWRAPPER_H

/**
 * @file   MSDetectorContainerWrapper.h
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

#include "MSDetectorContainerWrapperBase.h"
#include "MSVehicle.h"
#include "MSPredicates.h"
#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <utility>

template< class WrappedContainer >
struct MSDetectorContainerWrapper : public MSDetectorContainerWrapperBase
{
    typedef typename WrappedContainer::const_iterator ContainerConstIt;
    typedef WrappedContainer InnerContainer;
    typedef typename WrappedContainer::value_type ContainerItem;
    typedef typename WrappedContainer::iterator ContainerIt;
    typedef typename Predicate::PosGreaterC< ContainerItem >
    PosGreaterPredicate;
    typedef typename Predicate::VehEqualsC< ContainerItem >
    ErasePredicate;

    void enterDetectorByMove( MSVehicle* veh )
        {
            containerM.push_front( ContainerItem( veh ) );
        }

    void enterDetectorByEmitOrLaneChange( MSVehicle* veh )
        {
            ContainerIt insertIt =
                std::find_if( containerM.begin(), containerM.end(),
                              std::bind2nd(
                                  PosGreaterPredicate(), veh->pos() ) );
            containerM.insert( insertIt, ContainerItem( veh ) );
        }

    void leaveDetectorByMove( MSVehicle* veh )
        {
            ContainerIt eraseIt =
                std::find_if( containerM.begin(), containerM.end(),
                              std::bind2nd(
                                  ErasePredicate(), veh ) );
            assert(containerM.size()>0);
            assert(eraseIt!=containerM.end());
            containerM.erase( eraseIt );
        }

    void leaveDetectorByLaneChange( MSVehicle* veh )
        {
            leaveDetectorByMove( veh );
        }

    MSDetectorContainerWrapper()
        : MSDetectorContainerWrapperBase(),
          containerM()
        {}
    
    MSDetectorContainerWrapper(
        const MSDetectorOccupancyCorrection& occupancyCorrection )
        : MSDetectorContainerWrapperBase( occupancyCorrection ),
          containerM()
        {}

    virtual ~MSDetectorContainerWrapper( void )
        {
            containerM.clear();
        }

    WrappedContainer containerM;
};




class MSVehicle;

template< class T >
struct MSDetectorMapWrapper
    : public MSDetectorContainerWrapperBase
{
    typedef T WrappedContainer;
    typedef WrappedContainer InnerContainer;
    typedef typename WrappedContainer::iterator ContainerIt;
    typedef typename WrappedContainer::data_type Data;

    bool hasVehicle( MSVehicle* veh ) const
        {
            return containerM.find( veh ) != containerM.end();
        }
    
    virtual void enterDetectorByMove( MSVehicle* veh )
        {
            assert( ! hasVehicle( veh ) );
            containerM.insert( std::make_pair( veh, Data() ) );
        }

    void enterDetectorByEmitOrLaneChange( MSVehicle* veh )
        {
            enterDetectorByMove( veh );
        }

    void leaveDetectorByMove( MSVehicle* veh )
        {
            // There may be Vehicles that pass the detector-end but
            // had not passed the beginning (emit or lanechange).
            //assert( hasVehicle( veh ) );
            containerM.erase( veh );
        }

    void leaveDetectorByLaneChange( MSVehicle* veh )
        {
            leaveDetectorByMove( veh );
        }

    MSDetectorMapWrapper()
        : MSDetectorContainerWrapperBase(),
          containerM()
        {}

    MSDetectorMapWrapper(
        const MSDetectorOccupancyCorrection& occupancyCorrection )
        : MSDetectorContainerWrapperBase( occupancyCorrection ),
          containerM()
        {}

    virtual ~MSDetectorMapWrapper( void )
        {
            containerM.clear();
        }

    WrappedContainer containerM;
};



namespace DetectorContainer
{
    typedef MSDetectorContainerWrapper<
        std::list< MSVehicle* > > VehiclesList;
    
    typedef MSDetectorMapWrapper<
        std::map< MSVehicle*, MSUnit::Seconds > > TimeMap;
    
    class EmptyType{};
    
    typedef MSDetectorMapWrapper<
        std::map< MSVehicle*, EmptyType > > VehicleMap;

}

#endif // MSDETECTORCONTAINERWRAPPER_H

// Local Variables:
// mode:C++
// End:
