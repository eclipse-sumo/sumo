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
#include "MSVehicleControl.h"

// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTrainHelper
 * @brief A class that helps computing positions of a train's carriages 
 * and additional structures.
 *
 */
class MSTrainHelper {
public:
    struct Carriage {
        Position front;
        Position back;
        std::vector<Position> doorPositions;
        std::vector<Position> unboardingPositions;
    };

    MSTrainHelper(const MSVehicle* vehicle, bool reversed = false, bool secondaryShape = false, double exaggeration = 1.0, int vehicleQuality = 3) 
        : myTrain(vehicle) { 
        computeTrainDimensions(exaggeration, vehicleQuality);
        computeCarriages(reversed, secondaryShape);
    }

    ~MSTrainHelper() {
        for (const Carriage* carriage: myCarriages) {
            delete carriage;
        }
    }

    inline double getUpscaleLength() const {
        return myUpscaleLength;
    }

    inline double getHalfWidth() const {
        return myHalfWidth;
    }

    inline int getNumCarriages() const {
        return myNumCarriages;
    }

    inline double getCarriageLength() const {
        return myCarriageLength;
    }

    inline double getFirstCarriageLength() const {
        return myFirstCarriageLength;
    }

    inline int getCarriageDoors() const {
        return myCarriageDoors;
    }

    inline int getFirstCarriageNo() const {
        return myFirstCarriageNo;
    }

    inline int getFirstPassengerCarriage() const {
        return myFirstPassengerCarriage;
    }
    
    inline bool isReversed() const {
        return myIsReversed;
    }

    inline const std::vector<Carriage*>& getCarriages() const {
        return myCarriages;
    }
    
    /// @brief compute door positions on demand and fills the carriage structures
    /// @remark need to be called before retrieving carriages if door positions needed
    void computeDoorPositions();

    /// @brief compute unboarding positions on demand and fills the carriage structures
    /// @remark need to be called before retrieving carriages if unboarding positions needed
    void computeUnboardingPositions(double passengerRadius, std::vector<Position>& unboardingPositions);
    
    /// @brief return length exaggeration factor (special for long vehicles)
    static double getUpscaleLength(double upscale, double length, int vehicleQuality);

    /// @brief average door width used to compute doors positions
    static const double CARRIAGE_DOOR_WIDTH;

    /// @brief small extra tolerance used to avoid constraint violations    
    static const double PEDESTRIAN_RADIUS_EXTRA_TOLERANCE;

private:
    void computeTrainDimensions(double exaggeration, int vehicleQuality);
    void computeCarriages(bool reversed, bool secondaryShape);

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
};
