#ifndef MSDETECTORHALTINGCONTAINERWRAPPER_H
#define MSDETECTORHALTINGCONTAINERWRAPPER_H

/**
 * @file   MSDetectorHaltingContainerWrapper.h
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


#include "MSDetectorContainerWrapper.h"
#include "MSDetectorOccupancyCorrection.h"
#include "MSUnit.h"
#include "MSUpdateEachTimestep.h"
#include <list>
#include <map>
#include "MSSubject.h"

class MSVehicle;

namespace DetectorContainer
{
    struct Halting
    {
        Halting( MSVehicle* veh )
            : vehM( veh ),
              posM( vehM->pos() ),
              timeBelowSpeedThresholdM( 0 ),
              isHaltingM( false ),
              isInJamM( false ),
              haltingDurationM( 0 )
            {}
        MSVehicle* vehM;
        MSUnit::Cells posM; 
        MSUnit::Steps timeBelowSpeedThresholdM;
        bool isHaltingM;
        bool isInJamM;
        MSUnit::Steps haltingDurationM;
    };

    struct E3Halting
    {
        E3Halting( MSVehicle* veh )
            :
            posM( veh->pos() )
            , timeBelowSpeedThresholdM( 0 )
            , isHaltingM( false )
            , haltingDurationM( 0 )
            , nHalts( 0 )
            {}
        MSUnit::Cells posM; 
        MSUnit::Steps timeBelowSpeedThresholdM;
        bool isHaltingM;
        MSUnit::Steps haltingDurationM;
        unsigned nHalts;
    };    
}

namespace halt
{
    class BeginOfHalt{};
    class EndOfHalt{};
    
    typedef MSSubjectPassesObserved<
        const DetectorContainer::Halting, BeginOfHalt > HaltBeginSubject;
    typedef MSSubjectPassesObserved<
        const DetectorContainer::Halting, EndOfHalt >   HaltEndSubject;

    typedef HaltBeginSubject::Observer HaltBeginObserver;
    typedef HaltEndSubject::Observer   HaltEndObserver;
}


template< class WrappedContainer >
struct MSDetectorHaltingContainerWrapper
    :
    public MSDetectorContainerWrapper< WrappedContainer >
    , public MSUpdateEachTimestep<
          MSDetectorHaltingContainerWrapper< WrappedContainer > >
    , public halt::HaltBeginSubject
    , public halt::HaltEndSubject
{
    typedef typename WrappedContainer::iterator HaltingsIt;
    typedef typename WrappedContainer::const_iterator HaltingsConstIt;
    typedef WrappedContainer InnerContainer;
    typedef halt::HaltBeginSubject HaltBeginSubject;
    typedef halt::HaltEndSubject HaltEndSubject;
    typedef halt::HaltBeginObserver HaltBeginObserver;
    typedef halt::HaltEndObserver HaltEndObserver;

    MSDetectorHaltingContainerWrapper( void )
        {}
    
    MSDetectorHaltingContainerWrapper(
        MSUnit::Steps timeThreshold,
        MSUnit::CellsPerStep speedThreshold,
        MSUnit::Cells jamDistThreshold )
        : MSDetectorContainerWrapper< WrappedContainer >(),
          MSUpdateEachTimestep<
          MSDetectorHaltingContainerWrapper< WrappedContainer > >(),
          timeThresholdM( timeThreshold ),
          speedThresholdM( speedThreshold ),
          jamDistThresholdM( jamDistThreshold )
        {}
    
    MSDetectorHaltingContainerWrapper(
        const MSDetectorOccupancyCorrection& occupancyCorrection,
        MSUnit::Steps timeThreshold,
        MSUnit::CellsPerStep speedThreshold,
        MSUnit::Cells jamDistThreshold )
        : MSDetectorContainerWrapper< WrappedContainer >( occupancyCorrection),
          MSUpdateEachTimestep<
          MSDetectorHaltingContainerWrapper< WrappedContainer > >(),
          timeThresholdM( timeThreshold ),
          speedThresholdM( speedThreshold ),
          jamDistThresholdM( jamDistThreshold )
        {}

    bool updateEachTimestep( void )
        {
            // set posM, isHaltingM and haltingDurationM
            for ( HaltingsIt haltIt = containerM.begin();
                  haltIt != containerM.end(); ++haltIt ) {
                haltIt->posM += haltIt->vehM->getMovedDistance();
                if ( haltIt->vehM->speed() >= speedThresholdM ) {
                    if ( haltIt->isHaltingM ) {
                        HaltEndSubject::notify( *haltIt );
                    }
                    haltIt->timeBelowSpeedThresholdM = 0;
                    haltIt->isHaltingM = false;
                    haltIt->haltingDurationM = 0.0;
                }
                else {
                    haltIt->timeBelowSpeedThresholdM++;
                    if ( haltIt->timeBelowSpeedThresholdM > timeThresholdM ) {
                        if ( ! haltIt->isHaltingM ) {
                            haltIt->isHaltingM = true;
                            // time to detect halting contributes to
                            // halting-duration
                            haltIt->haltingDurationM =
                                haltIt->timeBelowSpeedThresholdM++;
                            HaltBeginSubject::notify( *haltIt );
                        }
                        else {
                            haltIt->haltingDurationM++;
                        }
                    }
                }
            }

            // set isInJamM
            if ( containerM.size() >= 1 ) {
                containerM.begin()->isInJamM = false;
            }
            if ( containerM.size() > 1 ) {
                for ( HaltingsIt jamIt = containerM.begin();
                      jamIt != --containerM.end(); /* empty */ ){
                    HaltingsIt rearIt = jamIt;
                    HaltingsIt frontIt = ++jamIt;
                    if ( ! rearIt->isHaltingM || ! frontIt->isHaltingM ) {
                        continue;
                    }
                    if ( frontIt->posM - frontIt->vehM->length()
                         - rearIt->posM <= jamDistThresholdM ) {
                        if ( rearIt == containerM.begin() ) {
                            rearIt->isInJamM = true;
                        }
                        frontIt->isInJamM = true;
                    }
                    else {
                        frontIt->isInJamM = false;
                    }
                }
            }
            return false;
        }

    void attach( HaltBeginObserver* toAttach )
        {
            HaltBeginSubject::attach( toAttach );
        }
    void detach( HaltBeginObserver* toDetach )
        {
            HaltBeginSubject::detach( toDetach );
        }    
    void attach( HaltEndObserver* toAttach )
        {
            HaltEndSubject::attach( toAttach );
        }
    void detach( HaltEndObserver* toDetach )
        {
            HaltEndSubject::detach( toDetach );
        }    
    
    MSUnit::Steps timeThresholdM;
    MSUnit::CellsPerStep speedThresholdM;
    MSUnit::Cells jamDistThresholdM;
};


// template< class T >
struct MSDetectorHaltingMapWrapper
    :
//     public MSDetectorMapWrapper< T >
//     , public MSUpdateEachTimestep< MSDetectorHaltingMapWrapper< T > >
    public MSDetectorMapWrapper< std::map< MSVehicle*, DetectorContainer::E3Halting > >
    , public MSUpdateEachTimestep< MSDetectorHaltingMapWrapper >
{
    typedef std::map< MSVehicle*, DetectorContainer::E3Halting > WrappedContainer;
    typedef WrappedContainer::iterator HaltingsIt;
    typedef WrappedContainer::const_iterator HaltingsConstIt;
    typedef WrappedContainer InnerContainer;

    MSDetectorHaltingMapWrapper(
        MSUnit::Steps timeThreshold,
        MSUnit::CellsPerStep speedThreshold
        )
        :
        MSDetectorMapWrapper< WrappedContainer >(),
        MSUpdateEachTimestep<
            MSDetectorHaltingMapWrapper >(),
        timeThresholdM( timeThreshold ),
        speedThresholdM( speedThreshold )
        {}
    
    MSDetectorHaltingMapWrapper(
        const MSDetectorOccupancyCorrection& occupancyCorrection,
        MSUnit::Steps timeThreshold,
        MSUnit::CellsPerStep speedThreshold
        )
        :
        MSDetectorMapWrapper< WrappedContainer >( occupancyCorrection),
        MSUpdateEachTimestep<
            MSDetectorHaltingMapWrapper >(),
        timeThresholdM( timeThreshold ),
        speedThresholdM( speedThreshold )
        {}

    void enterDetectorByMove( MSVehicle* veh )
        {
            assert( ! hasVehicle( veh ) );
            containerM.insert( std::make_pair(
                                   veh, DetectorContainer::E3Halting( veh ) ) );
        }
    
    bool updateEachTimestep( void )
        {
            for ( HaltingsIt pair = containerM.begin();
                  pair != containerM.end(); ++pair ) {
                MSVehicle* veh = pair->first;
                DetectorContainer::E3Halting& halting = pair->second;
                halting.posM += veh->getMovedDistance();
                if ( veh->speed() >= speedThresholdM ) {
                    halting.timeBelowSpeedThresholdM = 0;
                    halting.isHaltingM = false;
                    halting.haltingDurationM = 0.0;
                }
                else {
                    halting.timeBelowSpeedThresholdM++;
                    if ( halting.timeBelowSpeedThresholdM > timeThresholdM ) {
                        if ( ! halting.isHaltingM ) {
                            // beginning of new halt detected
                            halting.isHaltingM = true;
                            // time to detect halting contributes to
                            // halting-duration
                            halting.haltingDurationM =
                                halting.timeBelowSpeedThresholdM++;
                            halting.nHalts++;
                        }
                        else {
                            halting.haltingDurationM++;
                        }
                    }
                }
            }
            return false; // to please MSVC++
        }

    MSUnit::Steps timeThresholdM;
    MSUnit::CellsPerStep speedThresholdM;
    MSUnit::Cells jamDistThresholdM;
};




namespace DetectorContainer
{
    typedef MSDetectorHaltingContainerWrapper<
        std::list< Halting > > HaltingsList;

    typedef MSUpdateEachTimestep< HaltingsList > UpdateHaltings;

//     typedef MSDetectorHaltingMapWrapper<
//         std::map< MSVehicle*, E3Halting > > HaltingsMap;
    typedef MSDetectorHaltingMapWrapper HaltingsMap;

    typedef MSUpdateEachTimestep< HaltingsMap > UpdateE3Haltings;   
}


namespace Predicate
{
	// specialization
    template<>
    struct PosGreaterC< DetectorContainer::Halting > :
        public std::binary_function< DetectorContainer::Halting,
                                     double, bool >
    {
        bool operator() ( const DetectorContainer::Halting& item,
                          double pos ) const {
            return item.posM > pos;
        }
    };

    // specialization
    template<>
    struct VehEqualsC< DetectorContainer::Halting > :
        public std::binary_function< DetectorContainer::Halting,
                                     MSVehicle*, bool >
    {
        bool operator() ( DetectorContainer::Halting item,
                          const MSVehicle* veh ) const {
            return item.vehM == veh;
        }
    };
}


#endif // MSDETECTORHALTINGCONTAINER_H

// Local Variables:
// mode:C++
// End:
