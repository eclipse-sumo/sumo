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
/// @file    CEP.h
/// @author  Martin Dippold
/// @author  Michael Behrisch
/// @date    July 2016
///
//
/****************************************************************************/
#pragma once

#define _USE_MATH_DEFINES
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <utility>
#include "CEPHandler.h"

//C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace PHEMlightdllV5 { class VEHPHEMLightJSON; }
namespace PHEMlightdllV5 { class VEH; }
namespace PHEMlightdllV5 { class Helpers; }

#define FLEET

namespace PHEMlightdllV5 {
    class CEP {
    public:
        enum eNormalizingType {
            eNormalizingType_RatedPower,
            eNormalizingType_DrivingPower
        };

    private:
        bool privateHeavyVehicle;
        std::string privateFuelType;
        std::string privateCalcType;
        double privateRatedPower;

    public:
        CEP(VEHPHEMLightJSON::VEH* Vehicle, std::vector<std::string>& headerLineFCvalues, std::vector<std::vector<double> >& matrixFCvalues, std::vector<std::string>& headerLinePollutants, std::vector<std::vector<double> >& matrixPollutants, std::vector<double>& idlingFCvalues, std::vector<double>& idlingPollutants);


        const bool&  getHeavyVehicle() const;
        const std::string&  getFuelType() const;
        void setFuelType(const std::string& value);
        const std::string&  getCalcType() const;
        void setCalcType(const std::string& value);

    public:
        const double&  getRatedPower() const;
        void setRatedPower(const double&  value);
        double getAuxPower() const {
            return _auxPower * getRatedPower();
        }
        double getVehicleMass() const {
            return _massVehicle;
        }
        double getVehicleLoading() const {
            return _vehicleLoading;
        }
        double getVehicleMassRot() const {
            return _vehicleMassRot;
        }
        double getCrossSectionalArea() const {
            return _crossSectionalArea;
        }
        double getCWValue() const {
            return _cWValue;
        }
        double getResistance(const double speed, const bool full=false) const {
            if (full) {
                return _resistanceF0 + _resistanceF1 * speed + std::pow(_resistanceF2 * speed, 2) + std::pow(_resistanceF3 * speed, 3) + std::pow(_resistanceF4 * speed, 4);
            }
            return _resistanceF0 + _resistanceF1 * speed + _resistanceF4 * std::pow(speed, 4);
        }
        double getFMot(const double speed, const double ratedPower);

    protected:
        double _massVehicle;
        double _vehicleLoading;
        double _vehicleMassRot;
        double _crossSectionalArea;
        double _cWValue;
        double _resistanceF0;
        double _resistanceF1;
        double _resistanceF2;
        double _resistanceF3;
        double _resistanceF4;
        double _axleRatio;
        double _auxPower;
        double _pNormV0;
        double _pNormP0;
        double _pNormV1;
        double _pNormP1;

        double _engineRatedSpeed;
        double _engineIdlingSpeed;
        double _effectiveWheelDiameter;

        std::vector<double> _speedPatternRotational;
        std::vector<double> _normalizedPowerPatternFCvalues;
        std::vector<double> _normalizedPowerPatternPollutants;

        std::map<std::string, std::vector<double> > _normedCepCurveFCvalues;
        std::vector<double> _gearTransmissionCurve;
        std::vector<double> _speedCurveRotational;
        std::map<std::string, std::vector<double> > _cepNormalizedCurvePollutants;
        std::map<std::string, double> _FleetMix;
        std::map<std::string, double> _idlingValueFCvalues;
        std::map<std::string, double> _idlingValuesPollutants;

        std::vector<double> _nNormTable;
        std::vector<double> _dragNormTable;

    public:
        double CalcPower(double speed, double acc, double gradient, bool HBEV);

        double CalcWheelPower(double speed, double acc, double gradient);

        double CalcEngPower(double power);

        double GetEmission(const std::string& pollutant, double power, double speed, Helpers* VehicleClass, const double drivingPower, const double ratedPower);

        double GetCO2Emission(double _FC, double _CO, double _HC, Helpers* VehicleClass);

        //Calculate the weighted fuel factor values for Fleetmix
    private:
        bool CalcfCValMix(double& _fCBr, double& _fCHC, double& _fCCO, double& _fCCO2, Helpers* VehicleClass);

        // Get the fuel factor values
        bool GetfcVals(const std::string& _fuelTypex, double& _fCBr, double& _fCHC, double& _fCCO, double& _fCCO2, Helpers* VehicleClass);

    public:
        double GetDecelCoast(double speed, double acc, double gradient, const double ratedPower);

        double GetRotationalCoeffecient(double speed);


    private:
        void FindLowerUpperInPattern(int& lowerIndex, int& upperIndex, const std::vector<double>& pattern, double value, double scale=1.);

        double Interpolate(double px, double p1, double p2, double e1, double e2);

    public:
        double GetMaxAccel(double speed, double gradient, bool HBEV);

        double GetPMaxNorm(double speed);

        //--------------------------------------------------------------------------------------------------
        // Operators for fleetmix
        //--------------------------------------------------------------------------------------------------


    private:
        void InitializeInstanceFields();
    };
}
