/****************************************************************************/
/// @file    MSDetectorHaltingContainerWrapper.h
/// @author  Christian Roessel
/// @date    Fri Sep 26 19:11:26 2003
/// @version $Id$
///
//	»missingDescription«
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSDetectorHaltingContainerWrapper_h
#define MSDetectorHaltingContainerWrapper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDetectorContainerWrapper.h"
#include "MSDetectorOccupancyCorrection.h"
#include <microsim/MSUnit.h>
#include <microsim/MSUpdateEachTimestep.h>
#include <list>
#include <map>
#include "MSSubject.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDetectorHaltingContainerWrapper
 */
namespace DetectorContainer
{
struct Halting
{
    Halting(MSVehicle* veh)
            : vehM(veh),
            posM(vehM->getPositionOnLane()),
            timeBelowSpeedThresholdM(0),
            isHaltingM(false),
            isInJamM(false),
            haltingDurationM(0)
    {}
    MSVehicle* vehM;
    SUMOReal posM;
    MSUnit::Steps timeBelowSpeedThresholdM;
    bool isHaltingM;
    bool isInJamM;
    MSUnit::Steps haltingDurationM;
};

}

namespace halt
{
class BeginOfHalt
    {};
class EndOfHalt
    {};

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

    MSDetectorHaltingContainerWrapper(void)
    {}

    MSDetectorHaltingContainerWrapper(
        MSLane &lane,
        MSUnit::Steps timeThreshold,
        MSUnit::CellsPerStep speedThreshold,
        SUMOReal jamDistThreshold)
            : MSDetectorContainerWrapper< WrappedContainer >(),
            MSUpdateEachTimestep<
            MSDetectorHaltingContainerWrapper< WrappedContainer > >(),
            timeThresholdM(timeThreshold),
            speedThresholdM(speedThreshold),
            jamDistThresholdM(jamDistThreshold),
            myLane(lane)
    {}

    MSDetectorHaltingContainerWrapper(
        MSLane &lane,
        const MSDetectorOccupancyCorrection& occupancyCorrection,
        MSUnit::Steps timeThreshold,
        MSUnit::CellsPerStep speedThreshold,
        SUMOReal jamDistThreshold)
            : MSDetectorContainerWrapper< WrappedContainer >(occupancyCorrection),
            MSUpdateEachTimestep<
            MSDetectorHaltingContainerWrapper< WrappedContainer > >(),
            timeThresholdM(timeThreshold),
            speedThresholdM(speedThreshold),
            jamDistThresholdM(jamDistThreshold),
            myLane(lane)
    {}

    bool updateEachTimestep(void)
    {
        // set posM, isHaltingM and haltingDurationM
        for (HaltingsIt haltIt = this->containerM.begin();
                haltIt != this->containerM.end(); ++haltIt) {
            haltIt->posM = haltIt->vehM->getPositionOnLane();
            if (&myLane!=&haltIt->vehM->getLane()) {
                haltIt->posM += myLane.length();
            }
            if (haltIt->vehM->getSpeed() >= speedThresholdM) {
                if (haltIt->isHaltingM) {
                    HaltEndSubject::notify(*haltIt);
                }
                haltIt->timeBelowSpeedThresholdM = 0;
                haltIt->isHaltingM = false;
                haltIt->haltingDurationM = 0.0;
            } else {
                haltIt->timeBelowSpeedThresholdM++;
                if (haltIt->timeBelowSpeedThresholdM >= timeThresholdM) {
                    if (! haltIt->isHaltingM) {
                        haltIt->isHaltingM = true;
                        // time to detect halting contributes to
                        // halting-duration
                        haltIt->haltingDurationM =
                            haltIt->timeBelowSpeedThresholdM/*++*/;
                        HaltBeginSubject::notify(*haltIt);
                    } else {
                        haltIt->haltingDurationM++;
                    }
                }
            }
        }

        // set isInJamM
        if (this->containerM.size() >= 1) {
            this->containerM.begin()->isInJamM = false;
        }
        if (this->containerM.size() == 1) {
            this->containerM.begin()->isInJamM = this->containerM.begin()->isHaltingM;
        }
        if (this->containerM.size() > 1) {
            for (HaltingsIt jamIt = this->containerM.begin();
                    jamIt != --this->containerM.end(); /* empty */) {
                HaltingsIt rearIt = jamIt;
                HaltingsIt frontIt = ++jamIt;
                if (! rearIt->isHaltingM || ! frontIt->isHaltingM) {
                    rearIt->isInJamM = false;
                    continue;
                }
                SUMOReal p12 = frontIt->posM;
                SUMOReal l1 = frontIt->vehM->getLength();
                SUMOReal p22 = rearIt->posM;
                SUMOReal dist = p12-l1-p22;
                if (dist <= jamDistThresholdM) {
//                        if ( rearIt == containerM.begin() ) {
                    rearIt->isInJamM = true;
                    //                      }
                    frontIt->isInJamM = true;
                } else {
                    frontIt->isInJamM = false;
                }
            }
            HaltingsIt first = this->containerM.end();
            first--;
            (first)->isInJamM = (first)->isHaltingM;
        }
        return false;
    }

    void attach(HaltBeginObserver* toAttach)
    {
        HaltBeginSubject::attach(toAttach);
    }
    void detach(HaltBeginObserver* toDetach)
    {
        HaltBeginSubject::detach(toDetach);
    }
    void attach(HaltEndObserver* toAttach)
    {
        HaltEndSubject::attach(toAttach);
    }
    void detach(HaltEndObserver* toDetach)
    {
        HaltEndSubject::detach(toDetach);
    }

    MSUnit::Steps timeThresholdM;
    MSUnit::CellsPerStep speedThresholdM;
    SUMOReal jamDistThresholdM;
    MSLane &myLane;
};

/*
// template< class T >
struct MSDetectorHaltingMapWrapper
            :
//     public MSDetectorMapWrapper< T >
//     , public MSUpdateEachTimestep< MSDetectorHaltingMapWrapper< T > >
            public MSDetectorVehicleInitMapWrapper< DetectorContainer::E3Halting >
            , public MSUpdateEachTimestep< MSDetectorHaltingMapWrapper >
{
    typedef DetectorContainer::E3Halting Type;
    typedef WrappedContainer::iterator HaltingsIt;
    typedef WrappedContainer::const_iterator HaltingsConstIt;
    typedef WrappedContainer InnerContainer;

    MSDetectorHaltingMapWrapper(
        MSUnit::Steps timeThreshold,
        MSUnit::CellsPerStep speedThreshold
    )
            :
            MSDetectorVehicleInitMapWrapper< Type >(),
            MSUpdateEachTimestep<
            MSDetectorHaltingMapWrapper >(),
            timeThresholdM(timeThreshold),
            speedThresholdM(speedThreshold)
    {}

    MSDetectorHaltingMapWrapper(
    //        MSLane &lane,
        const MSDetectorOccupancyCorrection& occupancyCorrection,
        MSUnit::Steps timeThreshold,
        MSUnit::CellsPerStep speedThreshold
    )
            :
            MSDetectorVehicleInitMapWrapper< Type >(occupancyCorrection),
            MSUpdateEachTimestep<
            MSDetectorHaltingMapWrapper >(),
            timeThresholdM(timeThreshold),
            speedThresholdM(speedThreshold)
    {}

    void enterDetectorByMove(MSVehicle* veh)
    {
//            assert( ! hasVehicle( veh ) );
        containerM.insert(std::make_pair(
                              veh, DetectorContainer::E3Halting(veh)));
    }

    bool updateEachTimestep(void)
    {
        for (HaltingsIt pair = containerM.begin();
                pair != containerM.end(); ++pair) {
            MSVehicle* veh = pair->first;
            DetectorContainer::E3Halting& halting = pair->second;
            halting.posM += veh->getMovedDistance();
            /                halting.posM = veh->pos();
                            if(&myLane!=&veh->getLane()) {
                                halting.posM += myLane.length();
                            }/
            if (veh->getSpeed() >= speedThresholdM) {
                halting.timeBelowSpeedThresholdM = 0;
                halting.isHaltingM = false;
                halting.haltingDurationM = 0.0;
            } else {
                halting.timeBelowSpeedThresholdM++;
                if (halting.timeBelowSpeedThresholdM > timeThresholdM) {
                    if (! halting.isHaltingM) {
                        // beginning of new halt detected
                        halting.isHaltingM = true;
                        // time to detect halting contributes to
                        // halting-duration
                        halting.haltingDurationM =
                            halting.timeBelowSpeedThresholdM++;
                        halting.nHalts++;
                    } else {
                        halting.haltingDurationM++;
                    }
                }
            }
        }
        return false; // to please MSVC++
    }

    MSUnit::Steps timeThresholdM;
    MSUnit::CellsPerStep speedThresholdM;
    SUMOReal jamDistThresholdM;
//    MSLane &myLane;
};

*/


namespace DetectorContainer
{
typedef MSDetectorHaltingContainerWrapper<
std::list< Halting > > HaltingsList;

typedef MSUpdateEachTimestep< HaltingsList > UpdateHaltings;

//     typedef MSDetectorHaltingMapWrapper<
//         std::map< MSVehicle*, E3Halting > > HaltingsMap;
//typedef MSDetectorHaltingMapWrapper HaltingsMap;

//typedef MSUpdateEachTimestep< HaltingsMap > UpdateE3Haltings;
}


namespace Predicate
{
// specialization
template<>
struct PosGreaterC< DetectorContainer::Halting > :
            public std::binary_function< DetectorContainer::Halting,
            SUMOReal, bool >
{
    bool operator()(const DetectorContainer::Halting& item,
                    SUMOReal pos) const
    {
        return item.posM > pos;
    }
};

// specialization
template<>
struct VehEqualsC< DetectorContainer::Halting > :
            public std::binary_function< DetectorContainer::Halting,
            MSVehicle*, bool >
{
    bool operator()(DetectorContainer::Halting item,
                    const MSVehicle* veh) const
    {
        return item.vehM == veh;
    }
};
}


#endif

/****************************************************************************/

