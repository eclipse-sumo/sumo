#ifndef MSHALTDURATION_H
#define MSHALTDURATION_H

///
/// @file    MSHaltDuration.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Mon Dec 01 2003 12:58 CET
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

#include "MSDetectorHaltingContainerWrapper.h"
#include <string>


class MSHaltDuration
    :
    public halt::HaltEndObserver
{
public:

protected:
    typedef double DetectorAggregate;
    typedef halt::HaltEndObserver::ParameterType ParameterType;
    typedef DetectorContainer::HaltingsList HaltingsList;
    typedef HaltingsList::HaltingsIt HaltListIt;
    typedef DetectorContainer::TimeMap HaltDurationMap;
    typedef HaltDurationMap::ContainerIt DurationMapIt;

    MSHaltDuration( HaltingsList& toObserve )
        :
        haltListM( toObserve )
        {
            haltListM.attach( this );
            durationMapM = new HaltDurationMap();
            for ( HaltListIt it = haltListM.containerM.begin();
                  it != haltListM.containerM.end(); ++it ) {
                durationMapM->enterDetectorByMove( it->vehM );
            }
        }
    
    virtual ~MSHaltDuration( void )
        {
            haltListM.detach( this );
            delete durationMapM;
        }

    bool hasVehicle( MSVehicle& veh ) const
        {
            return durationMapM->hasVehicle( &veh );
        }

    void update( ParameterType aObserved )
        {
            MSVehicle* veh = aObserved.vehM;
            MSUnit::Seconds toAdd = MSUnit::getInstance()->getSeconds(
                aObserved.haltingDurationM );
            
            DurationMapIt pair = durationMapM->containerM.find( veh );
            assert( pair != durationMapM->containerM.end() );

            pair->second += toAdd;
        }

    DetectorAggregate getAggregate( MSVehicle& veh ) // [s]
        {
            DurationMapIt pair = durationMapM->containerM.find( &veh );
            assert( pair != durationMapM->containerM.end() );
            
            DetectorAggregate haltDuration = pair->second;
            
            durationMapM->containerM.erase( &veh );
            return haltDuration;
        }           
    
    static std::string getDetectorName( void )
        {
            return "haltDuration";
        }
    
private:
    HaltingsList& haltListM;
    HaltDurationMap* durationMapM;
    
};


// Local Variables:
// mode:C++
// End:

#endif // MSHALTDURATION_H
