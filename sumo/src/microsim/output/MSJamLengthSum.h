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

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSDetectorHaltingContainerWrapper.h"
#include <microsim/MSUnit.h>
#include <microsim/MSLane.h>
#include <string>

class MSJamLengthSumInVehicles
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::HaltingsConstIt HaltingsConstIt;
    typedef Container::InnerContainer Haltings;

    MSJamLengthSumInVehicles( SUMOReal,
                              const Container& container )
        : containerM( container )
        {}

    virtual ~MSJamLengthSumInVehicles( void )
        {}

    DetectorAggregate getDetectorAggregate( void )
        {
            int pos = 0;
            SUMOReal nVeh = 0.0;
            for ( HaltingsConstIt it = containerM.containerM.begin();
                  it != containerM.containerM.end(); ++it ) {
                if ( it->isInJamM ) {
                    ++nVeh;
                    if(pos==0) {
                        SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyEntryCorrection();
                        if(corr!=0) {
                            nVeh -= (SUMOReal) (1.0 - corr);
                        }
                    }
                    if(pos==containerM.containerM.size()-1) {
                        SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyLeaveCorrection();
                        if(corr!=0) {
                            nVeh -= (SUMOReal) (1.0 - corr);
                        }
                    }
                }
                pos++;
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
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::HaltingsConstIt HaltingsConstIt;
    typedef Container::InnerContainer Haltings;

    MSJamLengthSumInMeters( //const MSDetectorOccupancyCorrection& occupancyCorrection ,
        SUMOReal, const Container& container )
        : containerM( container/*.containerM */)/*,
        myOccupancyCorrection(occupancyCorrection)*/
        {}

    virtual ~MSJamLengthSumInMeters( void )
        {}

    DetectorAggregate getDetectorAggregate( void )
        {
            SUMOReal distSum = 0.0;
            for ( HaltingsConstIt front = containerM.containerM.begin();
                  front != containerM.containerM.end(); ++front ) {
                if ( front->isInJamM ) {
                    if ( front == containerM.containerM.begin() ) {
                        distSum += front->vehM->length();
                        SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyEntryCorrection();
                        if(corr!=0) {
                            distSum -= (SUMOReal) ((1.0 - corr) * front->vehM->length());
                        }
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
                            if(rear==(--containerM.containerM.end())) {
                                SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyLeaveCorrection();
                                if(corr!=0) {
                                    distSum -= (SUMOReal) ((1.0 - corr) * rear->vehM->length());
                                }
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
    const Container& containerM;
//    const MSDetectorOccupancyCorrection& myOccupancyCorrection;
};



#endif // MSJAMLENGTHSUM_H

// Local Variables:
// mode:C++
// End:
