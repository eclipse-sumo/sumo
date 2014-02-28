/****************************************************************************/
/// @file    PHEMCEP.cpp
/// @author  Nikolaus Furian
/// @date    Thu, 13.06.2013
/// @version $Id$
///
// Helper class for PHEM Light, holds a specific CEP for a PHEM emission class
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cmath>
#include <string>
#include <utils/common/StringBijection.h>
#include <utils/common/UtilExceptions.h>
#include "PHEMCEP.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
PHEMCEP::PHEMCEP(bool heavyVehicel, SUMOEmissionClass emissionClass,
                 double vehicleMass, double vehicleLoading, double vehicleMassRot,
                 double crossArea, double cWValue,
                 double f0, double f1, double f2, double f3, double f4,
                 double ratedPower, double pNormV0, double pNormP0, double pNormV1,
                 double pNormP1, std::string vehicelFuelType,
                 const std::vector< std::vector<double> >& matrixFC,
                 const std::vector<std::string>& headerLinePollutants,
                 const std::vector< std::vector<double> >& matrixPollutants,
                 const std::vector< std::vector<double> > matrixSpeedRotational) {
    _emissionClass = emissionClass;
    _resistanceF0 = f0;
    _resistanceF1 = f1;
    _resistanceF2 = f2;
    _resistanceF3 = f3;
    _resistanceF4 = f4;
    _cwValue = cWValue;
    _crossSectionalArea = crossArea;
    _massVehicle = vehicleMass;
    _vehicleLoading = vehicleLoading;
    _massRot = vehicleMassRot;
    _ratedPower = ratedPower;
    _vehicleFuelType = vehicelFuelType;

    _pNormV0 = pNormV0 / 3.6;
    _pNormP0 = pNormP0;
    _pNormV1 = pNormV1 / 3.6;
    _pNormP1 = pNormP1;

    std::vector<std::string> pollutantIdentifier;
    std::vector< std::vector<double> > pollutantMeasures;

    // init pollutant identifiers
    for (int i = 0; i < (int)headerLinePollutants.size(); i++) {
        pollutantIdentifier.push_back(headerLinePollutants[i]);
    } // end for

    // get size of powerPatterns
    _sizeOfPatternFC = matrixFC.size();
    _sizeOfPatternPollutants = matrixPollutants.size();

    // initialize measures
    for (int i = 0; i < (int)headerLinePollutants.size(); i++) {
        pollutantMeasures.push_back(std::vector<double>());
    } // end for

    // looping through matrix and assigning values for speed rotational table
    _speedCurveRotational.clear();
    _speedPatternRotational.clear();
    for (int i = 0; i < (int)matrixSpeedRotational.size(); i++) {
        if (matrixSpeedRotational[i].size() != 2) {
            throw InvalidArgument("Error loading vehicle file for: " + SumoEmissionClassStrings.getString(emissionClass));
        }

        _speedPatternRotational.push_back(matrixSpeedRotational[i][0] / 3.6);
        _speedCurveRotational.push_back(matrixSpeedRotational[i][1]);

    } // end for

    // looping through matrix and assigning values for Fuel consumption
    _cepCurveFC.clear();
    for (int i = 0; i < (int)matrixFC.size(); i++) {
        if (matrixFC[i].size() != 2) {
            throw InvalidArgument("Error loading vehicle file for: " + SumoEmissionClassStrings.getString(emissionClass));
        }

        _powerPatternFC.push_back(matrixFC[i][0] * _ratedPower);
        _cepCurveFC.push_back(matrixFC[i][1]);

    } // end for


    // looping through matrix and assigning values for pollutants
    double normalizingPower = 0;

    if (heavyVehicel) {
        normalizingPower = _ratedPower;
    } else {
        normalizingPower = PHEMCEP::CalcPower(NORMALIZING_SPEED, NORMALIZING_ACCELARATION, 0);
    } // end if

    const int headerCount = headerLinePollutants.size();
    for (int i = 0; i < (int)matrixPollutants.size(); i++) {
        for (int j = 0; j < (int)matrixPollutants[i].size(); j++) {
            if ((int)matrixPollutants[i].size() != headerCount + 1) {
                return;
            }

            if (j == 0) {
                _powerPatternPollutants.push_back(matrixPollutants[i][j] * normalizingPower);
            } else {
                pollutantMeasures[j - 1].push_back(matrixPollutants[i][j]);
            } // end if
        } // end for
    } // end for

    for (int i = 0; i < headerCount; i++) {
        _cepCurvePollutants.insert(pollutantIdentifier[i], pollutantMeasures[i]);
    } // end for

} // end of Cep


PHEMCEP::~PHEMCEP() {
    // free power pattern
    _powerPatternFC.clear();
    _powerPatternPollutants.clear();
    _cepCurveFC.clear();
    _speedCurveRotational.clear();
    _speedPatternRotational.clear();
} // end of ~Cep


double
PHEMCEP::CalcPower(double v, double a, double slope) const {
    const double rotFactor = GetRotationalCoeffecient(v);
    double power = (_massVehicle + _vehicleLoading) * GRAVITY_CONST * (_resistanceF0 + _resistanceF1 * v + _resistanceF4 * pow(v, 4)) * v;
    power += (_crossSectionalArea * _cwValue * AIR_DENSITY_CONST / 2) * pow(v, 3);
    power += (_massVehicle * rotFactor + _massRot + _vehicleLoading) * a * v;
    power += (_massVehicle + _vehicleLoading) * slope * 0.01 * v;
    return power / 950.;
}


double
PHEMCEP::GetMaxAccel(double v, double a, double gradient) const {
    const double pMaxForAcc = GetPMaxNorm(v) * _ratedPower - PHEMCEP::CalcPower(v, 0, gradient);
    return (pMaxForAcc * 1000) / ((_massVehicle * GetRotationalCoeffecient(v) + _massRot + _vehicleLoading) * v);
}


double
PHEMCEP::GetEmission(const std::string& pollutant, double power) const {
    std::vector<double> emissionCurve;
    std::vector<double> powerPattern;

    if (pollutant == "FC") {
        emissionCurve = _cepCurveFC;
        powerPattern = _powerPatternFC;
    } else {
        if (!_cepCurvePollutants.hasString(pollutant)) {
            throw InvalidArgument("Emission pollutant " + pollutant + " not found!");
        }

        emissionCurve = _cepCurvePollutants.get(pollutant);
        powerPattern = _powerPatternPollutants;
    } // end if



    if (emissionCurve.size() == 0) {
        throw InvalidArgument("Empty emission curve for " + pollutant + " found!");
    }

    if (emissionCurve.size() == 1) {
        return emissionCurve[0];
    }

    // in case that the demanded power is smaller than the first entry (smallest) in the power pattern the first two entries are extrapolated
    if (power <= powerPattern.front()) {
        double calcEmission =  PHEMCEP::Interpolate(power, powerPattern[0], powerPattern[1], emissionCurve[0], emissionCurve[1]);

        if (calcEmission < 0) {
            return 0;
        } else {
            return calcEmission;
        }

    } // end if

    // if power bigger than all entries in power pattern the last two values are linearly extrapolated
    if (power >= powerPattern.back()) {
        return PHEMCEP::Interpolate(power, powerPattern[powerPattern.size() - 2], powerPattern.back(), emissionCurve[emissionCurve.size() - 2], emissionCurve.back());
    } // end if

    // bisection search to find correct position in power pattern
    int upperIndex;
    int lowerIndex;

    PHEMCEP::FindLowerUpperInPattern(lowerIndex, upperIndex, powerPattern, power);

    return PHEMCEP::Interpolate(power, powerPattern[lowerIndex], powerPattern[upperIndex], emissionCurve[lowerIndex], emissionCurve[upperIndex]);

} // end of GetEmission


double
PHEMCEP::Interpolate(double px, double p1, double p2, double e1, double e2) const {
    if (p2 == p1) {
        return e1;
    }
    return e1 + (px - p1) / (p2 - p1) * (e2 - e1);
} // end of Interpolate


double
PHEMCEP::GetRotationalCoeffecient(double speed) const {
    int upperIndex;
    int lowerIndex;

    PHEMCEP::FindLowerUpperInPattern(lowerIndex, upperIndex, _speedPatternRotational, speed);

    return PHEMCEP::Interpolate(speed,
                                _speedPatternRotational[lowerIndex],
                                _speedPatternRotational[upperIndex],
                                _speedCurveRotational[lowerIndex],
                                _speedCurveRotational[upperIndex]);
} // end of GetRotationalCoeffecient

void
PHEMCEP::FindLowerUpperInPattern(int& lowerIndex, int& upperIndex, std::vector<double> pattern, double value) const {
    if (value <= pattern.front()) {
        lowerIndex = 0;
        upperIndex = 0;
        return;

    } // end if

    if (value >= pattern.back()) {
        lowerIndex = pattern.size() - 1;
        upperIndex = pattern.size() - 1;
        return;
    } // end if

    // bisection search to find correct position in power pattern
    int middleIndex = (pattern.size() - 1) / 2;
    upperIndex = pattern.size() - 1;
    lowerIndex = 0;

    while (upperIndex - lowerIndex > 1) {
        if (pattern[middleIndex] == value) {
            lowerIndex = middleIndex;
            upperIndex = middleIndex;
            return;
        } else if (pattern[middleIndex] < value) {
            lowerIndex = middleIndex;
            middleIndex = (upperIndex - lowerIndex) / 2 + lowerIndex;
        } else {
            upperIndex = middleIndex;
            middleIndex = (upperIndex - lowerIndex) / 2 + lowerIndex;
        } // end if
    } // end while

    if (pattern[lowerIndex] <= value && value < pattern[upperIndex]) {
        return;
    } else {
        throw ProcessError("Error during calculation of position in pattern!");
    }
} // end of FindLowerUpperInPattern


double PHEMCEP::GetPMaxNorm(double speed) const {
    // Linear function between v0 and v1, constant elsewhere
    if (speed <= _pNormV0) {
        return _pNormP0;
    } else if (speed >= _pNormV1) {
        return _pNormP1;
    } else {
        return PHEMCEP::Interpolate(speed, _pNormV0, _pNormV1, _pNormP0, _pNormP1);
    }
} // end of GetPMaxNorm

/****************************************************************************/
