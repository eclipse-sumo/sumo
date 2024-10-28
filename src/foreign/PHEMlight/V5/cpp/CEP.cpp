/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2016-2024 German Aerospace Center (DLR) and others.
// PHEMlight module
// Copyright (C) 2016-2023 Technische Universitaet Graz, https://www.tugraz.at/
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
/// @file    CEP.cpp
/// @author  Martin Dippold
/// @author  Michael Behrisch
/// @date    July 2016
///
//
/****************************************************************************/
#include <config.h>

#include "CEP.h"
#include "CEPHandler.h"
#include "Constants.h"
#include "Helpers.h"


namespace PHEMlightdllV5 {

    CEP::CEP(VEHPHEMLightJSON::VEH* Vehicle, std::vector<std::string>& headerLineFCvalues, std::vector<std::vector<double> >& matrixFCvalues, std::vector<std::string>& headerLinePollutants, std::vector<std::vector<double> >& matrixPollutants, std::vector<double>& idlingFCvalues, std::vector<double>& idlingPollutants) {
        InitializeInstanceFields();
        _resistanceF0 = Vehicle->getRollingResData()->getFr0();
        _resistanceF1 = Vehicle->getRollingResData()->getFr1();
        _resistanceF2 = Vehicle->getRollingResData()->getFr2();
        _resistanceF3 = Vehicle->getRollingResData()->getFr3();
        _resistanceF4 = Vehicle->getRollingResData()->getFr4();
        _cWValue = Vehicle->getVehicleData()->getCw();
        _crossSectionalArea = Vehicle->getVehicleData()->getA();
        _massVehicle = Vehicle->getVehicleData()->getMass();
        _vehicleLoading = Vehicle->getVehicleData()->getLoading();
        _vehicleMassRot = Vehicle->getVehicleData()->getRedMassWheel();
        setCalcType(Vehicle->getVehicleData()->getCalcType());
//C# TO C++ CONVERTER NOTE: The following 'switch' operated on a string variable and was converted to C++ 'if-else' logic:
//        switch (CalcType)
//ORIGINAL LINE: case "Conv":
        if (getCalcType() == "Conv") {
                setRatedPower(Vehicle->getEngineData()->getICEData()->getPrated());
                _engineRatedSpeed = Vehicle->getEngineData()->getICEData()->getnrated();
                _engineIdlingSpeed = Vehicle->getEngineData()->getICEData()->getIdling();
        }
//ORIGINAL LINE: case "HEV":
        else if (getCalcType() == "HEV") {
                // Power von beiden zusammen Rest bezogen auf ICE
                setRatedPower(Vehicle->getEngineData()->getICEData()->getPrated() + Vehicle->getEngineData()->getEMData()->getPrated());
                _engineRatedSpeed = Vehicle->getEngineData()->getICEData()->getnrated();
                _engineIdlingSpeed = Vehicle->getEngineData()->getICEData()->getIdling();
        }
//ORIGINAL LINE: case "BEV":
        else if (getCalcType() == "BEV") {
                setRatedPower(Vehicle->getEngineData()->getEMData()->getPrated());
                _engineRatedSpeed = Vehicle->getEngineData()->getEMData()->getnrated();
                _engineIdlingSpeed = 0;
        }

        _effectiveWheelDiameter = Vehicle->getVehicleData()->getWheelDiameter();
        privateHeavyVehicle = Vehicle->getVehicleData()->getMassType() == Constants::HeavyVehicle;
        setFuelType(Vehicle->getVehicleData()->getFuelType());
        _axleRatio = Vehicle->getTransmissionData()->getAxelRatio();
        _auxPower = Vehicle->getAuxiliariesData()->getPauxnorm();

        _pNormV0 = Vehicle->getFLDData()->getP_n_max_v0() / 3.6;
        _pNormP0 = Vehicle->getFLDData()->getP_n_max_p0();
        _pNormV1 = Vehicle->getFLDData()->getP_n_max_v1() / 3.6;
        _pNormP1 = Vehicle->getFLDData()->getP_n_max_p1();

        // Init pollutant identifiers, unit and measures
        std::vector<std::string> FCvaluesIdentifier;
        std::vector<std::vector<double> > normalizedFCvaluesMeasures;
        for (int i = 0; i < (int)headerLineFCvalues.size(); i++) {
            FCvaluesIdentifier.push_back(headerLineFCvalues[i]);
            normalizedFCvaluesMeasures.push_back(std::vector<double>());
        }

        // Init pollutant identifiers, unit and measures
        std::vector<std::string> pollutantIdentifier;
        std::vector<std::vector<double> > normalizedPollutantMeasures;
        for (int i = 0; i < (int)headerLinePollutants.size(); i++) {
            pollutantIdentifier.push_back(headerLinePollutants[i]);
            normalizedPollutantMeasures.push_back(std::vector<double>());
        }

        // Assigning values for speed rotational table
        _speedPatternRotational = std::vector<double>();
        for (int i = 0; i < (int)Vehicle->getTransmissionData()->getTransm()["Speed"].size(); i++) {
            _speedPatternRotational.push_back(Vehicle->getTransmissionData()->getTransm()["Speed"][i] / 3.6);
        }

        _gearTransmissionCurve = Vehicle->getTransmissionData()->getTransm()["GearRatio"];
        _speedCurveRotational = Vehicle->getTransmissionData()->getTransm()["RotMassF"];

        // Assigning values for drag table
        _nNormTable = Vehicle->getFLDData()->getDragCurve()["n_norm"];
        _dragNormTable = Vehicle->getFLDData()->getDragCurve()["pe_drag_norm"];

        // Looping through matrix and assigning values for FC values
        _normalizedPowerPatternFCvalues = std::vector<double>();

        int headerFCCount = (int)headerLineFCvalues.size();
        for (int i = 0; i < (int)matrixFCvalues.size(); i++) {
            for (int j = 0; j < (int)matrixFCvalues[i].size(); j++) {
                if ((int)matrixFCvalues[i].size() != headerFCCount + 1) {
                    return;
                }

                if (j == 0) {
                    _normalizedPowerPatternFCvalues.push_back(matrixFCvalues[i][j]);
                }
                else {
                    normalizedFCvaluesMeasures[j - 1].push_back(matrixFCvalues[i][j]);
                }
            }
        }

        _idlingValueFCvalues = std::map<std::string, double>();
        _normedCepCurveFCvalues = std::map<std::string, std::vector<double> >();

        for (int i = 0; i < (int)headerLineFCvalues.size(); i++) {
            _normedCepCurveFCvalues.insert(std::make_pair(FCvaluesIdentifier[i], normalizedFCvaluesMeasures[i]));
            _idlingValueFCvalues.insert(std::make_pair(FCvaluesIdentifier[i], idlingFCvalues[i]));
        }

        _normalizedPowerPatternPollutants = std::vector<double>();
        _cepNormalizedCurvePollutants = std::map<std::string, std::vector<double> >();

        int headerCount = (int)headerLinePollutants.size();
        for (int i = 0; i < (int)matrixPollutants.size(); i++) {
            for (int j = 0; j < (int)matrixPollutants[i].size(); j++) {
                if ((int)matrixPollutants[i].size() != headerCount + 1) {
                    return;
                }

                if (j == 0) {
                    _normalizedPowerPatternPollutants.push_back(matrixPollutants[i][j]);
                }
                else {
                    normalizedPollutantMeasures[j - 1].push_back(matrixPollutants[i][j]);
                }
            }
        }

        _idlingValuesPollutants = std::map<std::string, double>();

        for (int i = 0; i < (int)headerLinePollutants.size(); i++) {
            _cepNormalizedCurvePollutants.insert(std::make_pair(pollutantIdentifier[i], normalizedPollutantMeasures[i]));
            _idlingValuesPollutants.insert(std::make_pair(pollutantIdentifier[i], idlingPollutants[i]));
        }

        _FleetMix = std::map<std::string, double>();
        _FleetMix.insert(std::make_pair(Constants::strGasoline, 0));
        _FleetMix.insert(std::make_pair(Constants::strDiesel, 0));
        _FleetMix.insert(std::make_pair(Constants::strCNG, 0));
        _FleetMix.insert(std::make_pair(Constants::strLPG, 0));
    }

    const bool& CEP::getHeavyVehicle() const {
        return privateHeavyVehicle;
    }

    const std::string& CEP::getFuelType() const {
        return privateFuelType;
    }

    void CEP::setFuelType(const std::string& value) {
        privateFuelType = value;
    }

    const std::string& CEP::getCalcType() const {
        return privateCalcType;
    }

    void CEP::setCalcType(const std::string& value) {
        privateCalcType = value;
    }

    const double& CEP::getRatedPower() const {
        return privateRatedPower;
    }

    void CEP::setRatedPower(const double& value) {
        privateRatedPower = value;
    }

    double CEP::CalcEngPower(double power, const double ratedPower) {
        if (power < _normalizedPowerPatternFCvalues.front() * ratedPower) {
            return _normalizedPowerPatternFCvalues.front() * ratedPower;
        }
        if (power > _normalizedPowerPatternFCvalues.back() * ratedPower) {
            return _normalizedPowerPatternFCvalues.back() * ratedPower;
        }

        return power;
    }

    double CEP::GetEmission(const std::string& pollutant, double power, double speed, Helpers* VehicleClass, const double drivingPower, const double ratedPower) {
        //Declaration
        std::vector<double>* emissionCurve = nullptr;
        std::vector<double>* powerPattern = nullptr;

        // bisection search to find correct position in power pattern
        int upperIndex;
        int lowerIndex;

        double emissionMultiplier = getHeavyVehicle() ? ratedPower : 1.;
        if (std::abs(speed) <= Constants::ZERO_SPEED_ACCURACY) {
            if (_cepNormalizedCurvePollutants.find(pollutant) == _cepNormalizedCurvePollutants.end() && _normedCepCurveFCvalues.find(pollutant) == _normedCepCurveFCvalues.end()) {
                VehicleClass->setErrMsg(std::string("Emission pollutant or fuel value ") + pollutant + std::string(" not found!"));
                return 0;
            }

            if (_normedCepCurveFCvalues.find(pollutant) != _normedCepCurveFCvalues.end()) {
                return _idlingValueFCvalues[pollutant] * ratedPower;
            }
            else if (_cepNormalizedCurvePollutants.find(pollutant) != _cepNormalizedCurvePollutants.end()) {
                return _idlingValuesPollutants[pollutant] * emissionMultiplier;
            }
        }


        if (_cepNormalizedCurvePollutants.find(pollutant) == _cepNormalizedCurvePollutants.end() && _normedCepCurveFCvalues.find(pollutant) == _normedCepCurveFCvalues.end()) {
            VehicleClass->setErrMsg(std::string("Emission pollutant or fuel value ") + pollutant + std::string(" not found!"));
            return 0;
        }

        double normalizingPower = ratedPower;
        if (_normedCepCurveFCvalues.find(pollutant) != _normedCepCurveFCvalues.end()) {
            emissionCurve = &_normedCepCurveFCvalues[pollutant];
            powerPattern = &_normalizedPowerPatternFCvalues;
            emissionMultiplier = ratedPower;
        }
        else if (_cepNormalizedCurvePollutants.find(pollutant) != _cepNormalizedCurvePollutants.end()) {
            emissionCurve = &_cepNormalizedCurvePollutants[pollutant];
            powerPattern = &_normalizedPowerPatternPollutants;
            if (!getHeavyVehicle()) {
                normalizingPower = drivingPower;
            }
        }

        if (emissionCurve == nullptr || emissionCurve->empty()) {
            VehicleClass->setErrMsg(std::string("Empty emission curve for ") + pollutant + std::string(" found!"));
            return 0;
        }
        if (emissionCurve->size() == 1) {
            return emissionCurve->front() * emissionMultiplier;
        }

        // in case that the demanded power is smaller than the first entry (smallest) in the power pattern the first is returned (should never happen)
        if (power <= powerPattern->front() * normalizingPower) {
            return emissionCurve->front() * emissionMultiplier;
        }

        // if power bigger than all entries in power pattern return the last (should never happen)
        if (power >= powerPattern->back() * normalizingPower) {
            return emissionCurve->back() * emissionMultiplier;
        }

        FindLowerUpperInPattern(lowerIndex, upperIndex, *powerPattern, power, normalizingPower);
        return Interpolate(power, (*powerPattern)[lowerIndex] * normalizingPower, (*powerPattern)[upperIndex] * normalizingPower, (*emissionCurve)[lowerIndex], (*emissionCurve)[upperIndex]) * emissionMultiplier;
    }

    double CEP::GetCO2Emission(double _FC, double _CO, double _HC, Helpers* VehicleClass) {
        //Declaration
        double fCBr, fCHC, fCCO, fCCO2;

        fCBr = 0;
        fCHC = 0;
        fCCO = 0;
        fCCO2 = 0;

        if (getFuelType() != "Mix") {
            if (!GetfcVals(getFuelType(), fCBr, fCHC, fCCO, fCCO2, VehicleClass)) {
                return 0;
            }
        }
        else {
            if (!CalcfCValMix(fCBr, fCHC, fCCO, fCCO2, VehicleClass)) {
                return 0;
            }
        }

        return (_FC * fCBr - _CO * fCCO - _HC * fCHC) / fCCO2;
    }

    bool CEP::CalcfCValMix(double& _fCBr, double& _fCHC, double& _fCCO, double& _fCCO2, Helpers* VehicleClass) {
        //Declaration
        double Sum = 0;
        double sumfCBr, sumfCHC, sumfCCO, sumfCCO2;

        //Initialise
        sumfCBr = 0;
        sumfCHC = 0;
        sumfCCO = 0;
        sumfCCO2 = 0;

        //calculate the sum
        for (std::map<std::string, double>::const_iterator id = _FleetMix.begin(); id != _FleetMix.end(); ++id) {
            Sum += _FleetMix[id->first];
        }

        //Calculate the weighted fuel factors
        if (Sum <= 0) {
            VehicleClass->setErrMsg("All propolsion types in the fleetshares file are not known!");
            return false;
        }
        else {
            for (std::map<std::string, double>::const_iterator id = _FleetMix.begin(); id != _FleetMix.end(); ++id) {
                if (!GetfcVals(id->first, _fCBr, _fCHC, _fCCO, _fCCO2, VehicleClass)) {
                    return false;
                }
                else {
                    sumfCBr += _fCBr * _FleetMix[id->first] / Sum;
                    sumfCHC += _fCHC * _FleetMix[id->first] / Sum;
                    sumfCCO += _fCCO * _FleetMix[id->first] / Sum;
                    sumfCCO2 += _fCCO2 * _FleetMix[id->first] / Sum;
                }
            }
        }
        //Result values
        _fCBr = sumfCBr;
        _fCHC = sumfCHC;
        _fCCO = sumfCCO;
        _fCCO2 = sumfCCO2;
        return true;
    }

    bool CEP::GetfcVals(const std::string& _fuelTypex, double& _fCBr, double& _fCHC, double& _fCCO, double& _fCCO2, Helpers* VehicleClass) {
        _fCHC = 0.866;
        _fCCO = 0.429;
        _fCCO2 = 0.273;

//C# TO C++ CONVERTER NOTE: The following 'switch' operated on a string variable and was converted to C++ 'if-else' logic:
//        switch (_fuelTypex)
//ORIGINAL LINE: case Constants.strGasoline:
        if (_fuelTypex == Constants::strGasoline) {
                _fCBr = 0.865;
        }
//ORIGINAL LINE: case Constants.strDiesel:
        else if (_fuelTypex == Constants::strDiesel) {
                _fCBr = 0.863;
        }
//ORIGINAL LINE: case Constants.strCNG:
        else if (_fuelTypex == Constants::strCNG) {
                _fCBr = 0.693;
                _fCHC = 0.803;
        }
//ORIGINAL LINE: case Constants.strLPG:
        else if (_fuelTypex == Constants::strLPG) {
                _fCBr = 0.825;
                _fCHC = 0.825;
        }
        else {
                VehicleClass->setErrMsg(std::string("The propulsion type is not known! (") + getFuelType() + std::string(")"));
                return false;
        }
        return true;
    }

    double CEP::getFMot(const double speed, const double ratedPower, const double wheelRadius) {
        if (speed < 10e-2) {
            return 0.;
        }
        //Declaration
        int upperIndex;
        int lowerIndex;

        FindLowerUpperInPattern(lowerIndex, upperIndex, _speedPatternRotational, speed);
        double iGear = Interpolate(speed, _speedPatternRotational[lowerIndex], _speedPatternRotational[upperIndex], _gearTransmissionCurve[lowerIndex], _gearTransmissionCurve[upperIndex]);

        double iTot = iGear * _axleRatio;

        double n = (30 * speed * iTot) / (wheelRadius * M_PI);
        double nNorm = (n - _engineIdlingSpeed) / (_engineRatedSpeed - _engineIdlingSpeed);

        FindLowerUpperInPattern(lowerIndex, upperIndex, _nNormTable, nNorm);
        return (-Interpolate(nNorm, _nNormTable[lowerIndex], _nNormTable[upperIndex], _dragNormTable[lowerIndex], _dragNormTable[upperIndex]) * ratedPower * 1000 / speed) / Constants::getDRIVE_TRAIN_EFFICIENCY();
    }

    double CEP::GetRotationalCoeffecient(double speed) {
        //Declaration
        int upperIndex;
        int lowerIndex;

        FindLowerUpperInPattern(lowerIndex, upperIndex, _speedPatternRotational, speed);
        return Interpolate(speed, _speedPatternRotational[lowerIndex], _speedPatternRotational[upperIndex], _speedCurveRotational[lowerIndex], _speedCurveRotational[upperIndex]);
    }

    void CEP::FindLowerUpperInPattern(int& lowerIndex, int& upperIndex, const std::vector<double>& pattern, double value, double scale) {
        lowerIndex = 0;
        upperIndex = 0;

        if (value <= pattern.front() * scale) {
            lowerIndex = 0;
            upperIndex = 0;
            return;
        }

        if (value >= pattern.back() * scale) {
            lowerIndex = (int)pattern.size() - 1;
            upperIndex = (int)pattern.size() - 1;
            return;
        }

        // bisection search to find correct position in power pattern
        int middleIndex = ((int)pattern.size() - 1) / 2;
        upperIndex = (int)pattern.size() - 1;
        lowerIndex = 0;

        while (upperIndex - lowerIndex > 1) {
            if (pattern[middleIndex] * scale == value) {
                lowerIndex = middleIndex;
                upperIndex = middleIndex;
                return;
            }
            else if (pattern[middleIndex] * scale < value) {
                lowerIndex = middleIndex;
                middleIndex = (upperIndex - lowerIndex) / 2 + lowerIndex;
            }
            else {
                upperIndex = middleIndex;
                middleIndex = (upperIndex - lowerIndex) / 2 + lowerIndex;
            }
        }
    }

    double CEP::Interpolate(double px, double p1, double p2, double e1, double e2) {
        if (p2 == p1) {
            return e1;
        }

        return e1 + (px - p1) / (p2 - p1) * (e2 - e1);
    }

    double CEP::GetPMaxNorm(double speed) {
        // Linear function between v0 and v1, constant elsewhere
        if (speed <= _pNormV0) {
            return _pNormP0;
        }
        else if (speed >= _pNormV1) {
            return _pNormP1;
        }
        else {
            return Interpolate(speed, _pNormV0, _pNormV1, _pNormP0, _pNormP1);
        }
    }

    void CEP::InitializeInstanceFields() {
        _massVehicle = 0;
        _vehicleLoading = 0;
        _vehicleMassRot = 0;
        _crossSectionalArea = 0;
        _cWValue = 0;
        _resistanceF0 = 0;
        _resistanceF1 = 0;
        _resistanceF2 = 0;
        _resistanceF3 = 0;
        _resistanceF4 = 0;
        _axleRatio = 0;
        _auxPower = 0;
        _pNormV0 = 0;
        _pNormP0 = 0;
        _pNormV1 = 0;
        _pNormP1 = 0;
        _engineRatedSpeed = 0;
        _engineIdlingSpeed = 0;
        _effectiveWheelDiameter = 0;
    }
}
