/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2014-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSTrainHelper.h
/// @author  Benjamin Coueraud
/// @date    Wed, 07 Fev 2024
///
// A class that helps computing positions of a train's carriages.
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTrainHelper
 * @brief A class that helps computing positions of a train's carriages.
 *
 */
class MSTrainHelper {
public:
    MSTrainHelper(const MSVehicle* vehicle, bool reversed=false, bool secondaryShape=false, double exaggeration=1.0) 
        : myTrain(vehicle) {
        computeTrainDimensions(exaggeration);
        computeCarriages(secondaryShape, reversed);
    }

    ~MSTrainHelper() {
        for (const Carriage* carriage: myCarriages) {
            delete carriage;
        }
    }

    inline double getUpscaleLength(void) const {
        return myUpscaleLength;
    }

    inline double getLocomotiveLength(void) const {
        return myLocomotiveLength;
    } 

    inline double getDefaultLength(void) const {
        return myDefaultLength;
    }

    inline double getCarriageGap(void) const {
        return myCarriageGap;
    }

    inline double getLength(void) const {
        return myLength;
    }

    inline double getHalfWidth(void) const {
        return myHalfWidth;
    }

    inline int getNumCarriages(void) const {
        return myNumCarriages;
    }

    inline double getCarriageLengthWithGap(void) const {
        return myCarriageLengthWithGap;
    }

    inline double getCarriageLength(void) const {
        return myCarriageLength;
    }

    inline double getFirstCarriageLength(void) const {
        return myFirstCarriageLength;
    }

    inline int getCarriageDoors(void) const {
        return myCarriageDoors;
    }

    inline int getFirstCarriageNo(void) const {
        return myFirstCarriageNo;
    }

    inline int getFirstPassengerCarriage(void) const {
        return myFirstPassengerCarriage;
    }
    
    inline bool isReversed(void) const {
        return myIsReversed;
    }

    struct Carriage {
        Position front;
        Position back;
        std::vector<Position> doors;
    };

    inline std::vector<Carriage*> getCarriages(void) const {
        return myCarriages;
    }

    static const double CARRIAGE_DOOR_WIDTH;
    
private:
    const MSVehicle* myTrain;
    double myUpscaleLength;
    double myLocomotiveLength;
    double myDefaultLength;
    double myCarriageGap;
    double myLength;
    double myHalfWidth;
    int myNumCarriages;
    double myCarriageLengthWithGap;
    double myCarriageLength;
    double myFirstCarriageLength;
    int myCarriageDoors;
    int myFirstCarriageNo;
    int myFirstPassengerCarriage;
    bool myIsReversed;
    std::vector<Carriage*> myCarriages;

    void computeTrainDimensions(double exaggeration);
    void computeCarriages(bool secondaryShape, bool reversed);
};
