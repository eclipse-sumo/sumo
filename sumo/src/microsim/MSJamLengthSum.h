#ifndef MSJAMLENGTHSUM_H
#define MSJAMLENGTHSUM_H

/**
 * @file   MSJamLengthSum.h
 * @author Christian Roessel
 * @date   Started Mon Sep 29 09:42:08 2003
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

#include "MSDetectorHaltingContainerWrapper.h"
#include "MSUnit.h"
#include "MSLane.h"
#include <string>

class MSJamLengthSumInVehicles
{
protected:
    typedef double DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::HaltingsConstIt HaltingsConstIt;
    typedef Container::InnerContainer Haltings;

    MSJamLengthSumInVehicles( double,
                              const Container& container )
        : containerM( container )
        {}

    virtual ~MSJamLengthSumInVehicles( void )
        {}

    DetectorAggregate getDetectorAggregate( void )
        {
            double nVeh = 0.0;
            for ( HaltingsConstIt it = containerM.containerM.begin();
                  it != containerM.containerM.end(); ++it ) {
                if ( it->isInJamM ) {
                    ++nVeh;
                }
            }
            return nVeh;
        }

    static std::string getDetectorName( void )
        {
            return "jamLengthSumInVehicles";
        }
private:
    const Container& containerM;
};


class MSJamLengthSumInMeters
{
protected:
    typedef double DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::HaltingsConstIt HaltingsConstIt;
    typedef Container::InnerContainer Haltings;

    MSJamLengthSumInMeters( double,
                            const Container& container )
        : containerM( container.containerM )
        {}

    virtual ~MSJamLengthSumInMeters( void )
        {}

    DetectorAggregate getDetectorAggregate( void )
        {
            double distSum = 0.0;
            for ( HaltingsConstIt front = containerM.begin();
                  front != containerM.end(); ++front ) {
                if ( front->isInJamM ) {
                    if ( front == containerM.begin() ) {
                        distSum += front->vehM->length();
                        assert (distSum >= 0);
                    }
                    else {
                        HaltingsConstIt rear = front;
                        --rear;
                        if ( rear->isInJamM ) {
                            if ( front->vehM->pos() < rear->vehM->pos() ) {
                                distSum += rear->vehM->getLane().length() -
                                    rear->vehM->pos();
                            }
                            else {
                                distSum +=
                                    front->vehM->pos() - rear->vehM->pos();
                            }
                        }
                        else {
                            distSum += front->vehM->length();
                        }
                    }
                }
            }
            assert (distSum >= 0);
            return MSUnit::getInstance()->getMeters( distSum );
        }

    static std::string getDetectorName( void )
        {
            return "jamLengthSumInMeters";
        }
private:
    const Haltings& containerM;
};



#endif // MSJAMLENGTHSUM_H

// Local Variables:
// mode:C++
// End:
