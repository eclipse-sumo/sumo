#ifndef MSMAXJAMLENGTH_H
#define MSMAXJAMLENGTH_H

/**
 * @file   MSMaxJamLength.h
 * @author Christian Roessel
 * @date   Started Mon Sep 29 09:39:10 2003
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

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include "MSDetectorHaltingContainerWrapper.h"
#include <microsim/MSUnit.h>
#include <string>

class MSMaxJamLengthInVehicles
{
protected:
    typedef double DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::HaltingsConstIt HaltingsConstIt;
    typedef Container::InnerContainer Haltings;

    MSMaxJamLengthInVehicles( double,
                              const Container& container )
        : containerM( container )
        {}

    virtual ~MSMaxJamLengthInVehicles( void )
        {}

    DetectorAggregate getDetectorAggregate( void )
        {
            double maxNVeh = 0.0;
            double vehCount = 0.0;
            int pos = 0;
            for ( HaltingsConstIt it = containerM.containerM.begin();
                  it != containerM.containerM.end(); ++it ) {
                if ( it->isInJamM ) {
                    ++vehCount;
                    if(pos==0) {
                        double corr = containerM.occupancyCorrectionM->getOccupancyEntryCorrection();
                        if(corr!=0) {
                            vehCount -= (1.0 - corr);
                        }
                    }
                    if(pos==containerM.containerM.size()-1) {
                        double corr = containerM.occupancyCorrectionM->getOccupancyLeaveCorrection();
                        if(corr!=0) {
                            vehCount -= (1.0 - corr);
                        }
                    }
                    if ( vehCount > maxNVeh ) {
                        maxNVeh = vehCount;
                    }
                }
                else {
                    vehCount = 0;
                }
                pos++;
            }
            return maxNVeh;
        }

    static std::string getDetectorName( void )
        {
            return "maxJamLengthInVehicles";
        }
private:
    const Container& containerM;
};


class MSMaxJamLengthInMeters
{
protected:
    typedef double DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::HaltingsConstIt HaltingsConstIt;
    typedef Container::InnerContainer Haltings;

    MSMaxJamLengthInMeters( double,
                            const Container& container )
        : containerM( container )
        {}

    virtual ~MSMaxJamLengthInMeters( void )
        {}

    DetectorAggregate getDetectorAggregate( void )
        {
            double maxDist = 0.0;
            const MSVehicle* startVeh = 0;
            const MSVehicle* stopVeh = 0;
            for ( HaltingsConstIt it = containerM.containerM.begin();
                  it != containerM.containerM.end(); ++it ) {
                if ( it->isInJamM ) {
                    if ( startVeh == 0 ) {
                        startVeh = it->vehM;
                    }
                    stopVeh = it->vehM;
                    double dist = stopVeh->pos() - startVeh->pos() +
                        startVeh->length();
                    if(startVeh==containerM.containerM.begin()->vehM) {
                        double corr = containerM.occupancyCorrectionM->getOccupancyEntryCorrection();
                        if(corr!=0) {
                            dist -= ((1.0 - corr) * startVeh->length());
                        }
                    }
                    if(stopVeh==(--containerM.containerM.end())->vehM) {
                        double corr = containerM.occupancyCorrectionM->getOccupancyLeaveCorrection();
                        if(corr!=0) {
                            dist -= ((1.0 - corr) * stopVeh->length());
                        }
                    }
                    if ( dist > maxDist ) {
                        maxDist = dist;
                    }
                }
                else {
                    startVeh = 0;
                }
            }
            return MSUnit::getInstance()->getMeters( maxDist );
        }

    static std::string getDetectorName( void )
        {
            return "maxJamLengthInMeters";
        }

private:
    const Container& containerM;

};

#endif // MSMAXJAMLENGTH_H

// Local Variables:
// mode:C++
// End:
