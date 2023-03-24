/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2023 German Aerospace Center (DLR) and others.
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
/// @file    CEPHandler.h
/// @author  Martin Dippold
/// @author  Michael Behrisch
/// @date    July 2016
///
//
/****************************************************************************/
#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>

//C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace PHEMlightdllV5 { class CEP; }
namespace PHEMlightdllV5 { class Correction; }
namespace PHEMlightdllV5 { class Helpers; }
namespace PHEMlightdllV5 { class VEHPHEMLightJSON; }
namespace PHEMlightdllV5 { class Vehicle_Data; }
namespace PHEMlightdllV5 { class VEH; }
namespace PHEMlightdllV5 { class Aux_Data; }
namespace PHEMlightdllV5 { class Engine_Data; }
namespace PHEMlightdllV5 { class Rollres_Data; }
namespace PHEMlightdllV5 { class FullLoadDrag_Data; }
namespace PHEMlightdllV5 { class Transmission_Data; }
namespace PHEMlightdllV5 { class ICE_Data; }
namespace PHEMlightdllV5 { class EM_Data; }

namespace PHEMlightdllV5 {
    //PHEMLight vehicle
    class VEHPHEMLightJSON {
        // Vehicle data
    public:
        class Vehicle_Data {
        private:
            std::string privateMassType;
            std::string privateFuelType;
            std::string privateCalcType;
            double privateMass;
            double privateLoading;
            double privateRedMassWheel;
            double privateWheelDiameter;
            double privateCw;
            double privateA;
            double privateMileage;

        public:
            const std::string&  getMassType() const;
            void setMassType(const std::string& value);
            const std::string&  getFuelType() const;
            void setFuelType(const std::string& value);
            const std::string&  getCalcType() const;
            void setCalcType(const std::string& value);
            const double&  getMass() const;
            void setMass(const double&  value);
            const double&  getLoading() const;
            void setLoading(const double&  value);
            const double&  getRedMassWheel() const;
            void setRedMassWheel(const double&  value);
            const double&  getWheelDiameter() const;
            void setWheelDiameter(const double&  value);
            const double&  getCw() const;
            void setCw(const double&  value);
            const double&  getA() const;
            void setA(const double&  value);
            const double&  getMileage() const;
            void setMileage(const double&  value);
        };

        // Rolling resistance data
    public:
        class Rollres_Data {
        private:
            double privateFr0;
            double privateFr1;
            double privateFr2;
            double privateFr3;
            double privateFr4;

        public:
            const double&  getFr0() const;
            void setFr0(const double&  value);
            const double&  getFr1() const;
            void setFr1(const double&  value);
            const double&  getFr2() const;
            void setFr2(const double&  value);
            const double&  getFr3() const;
            void setFr3(const double&  value);
            const double&  getFr4() const;
            void setFr4(const double&  value);
        };

        // ICE engine
    public:
        class ICE_Data {
        private:
            double privatePrated;
            double privatenrated;
            double privateIdling;

        public:
            const double&  getPrated() const;
            void setPrated(const double&  value);
            const double&  getnrated() const;
            void setnrated(const double&  value);
            const double&  getIdling() const;
            void setIdling(const double&  value);
        };

        // EM engine
    public:
        class EM_Data {
        private:
            double privatePrated;
            double privatenrated;

        public:
            const double&  getPrated() const;
            void setPrated(const double&  value);
            const double&  getnrated() const;
            void setnrated(const double&  value);
        };

        // Engine data
    public:
        class Engine_Data {
        private:
            ICE_Data privateICEData;
            EM_Data privateEMData;

        public:
            ICE_Data* getICEData();
            EM_Data* getEMData();
        };

        // Auxiliaries data
    public:
        class Aux_Data {
        private:
            double privatePauxnorm;

        public:
            const double&  getPauxnorm() const;
            void setPauxnorm(const double&  value);
        };

        // Full load and Drag data
    public:
        class FullLoadDrag_Data {
        private:
            double privateP_n_max_v0;
            double privateP_n_max_p0;
            double privateP_n_max_v1;
            double privateP_n_max_p1;
            std::map<std::string, std::vector<double> > privateDragCurve;

        public:
            const double&  getP_n_max_v0() const;
            void setP_n_max_v0(const double&  value);
            const double&  getP_n_max_p0() const;
            void setP_n_max_p0(const double&  value);
            const double&  getP_n_max_v1() const;
            void setP_n_max_v1(const double&  value);
            const double&  getP_n_max_p1() const;
            void setP_n_max_p1(const double&  value);
            std::map<std::string, std::vector<double> >&  getDragCurve();
            void setDragCurve(const std::map<std::string, std::vector<double> >& value);

            FullLoadDrag_Data();
        };

        // Transmission data
    public:
        class Transmission_Data {
        private:
            double privateAxelRatio;
            std::map<std::string, std::vector<double> > privateTransm;

        public:
            const double&  getAxelRatio() const;
            void setAxelRatio(const double&  value);
            std::map<std::string, std::vector<double> >&  getTransm();
            void setTransm(const std::map<std::string, std::vector<double> >& value);

            Transmission_Data();
        };

        //Root object
    public:
        class VEH {
        private:
            std::string privateType;
            std::string privateVersion;
            Vehicle_Data privateVehicleData;
            Aux_Data privateAuxiliariesData;
            Engine_Data privateEngineData;
            Rollres_Data privateRollingResData;
            FullLoadDrag_Data privateFLDData;
            Transmission_Data privateTransmissionData;

        public:
            const std::string& getType() const;
            void setType(const std::string& value);
            const std::string& getVersion() const;
            void setVersion(const std::string& value);
            Vehicle_Data* getVehicleData();
            Aux_Data* getAuxiliariesData();
            Engine_Data* getEngineData();
            Rollres_Data* getRollingResData();
            FullLoadDrag_Data* getFLDData();
            Transmission_Data* getTransmissionData();
        };

    };

    class CEPHandler {
    public:
        CEPHandler();

    private:
        std::map<std::string, CEP*> _ceps;
    public:
        const std::map<std::string, CEP*>& getCEPS() const;


        bool GetCEP(std::vector<std::string>& DataPath, Helpers* Helper, Correction* DataCor);


    private:
        bool CalcCorrection(Correction* DataCor, Helpers* Helper, VEHPHEMLightJSON::Vehicle_Data* vehicle_Data);

        bool Load(std::vector<std::string>& DataPath, Helpers* Helper, Correction* DataCor, bool fleetMix = false);

        bool ReadVehicleFile(const std::vector<std::string>& DataPath, const std::string& emissionClass, Helpers* Helper, bool fleetMix, VEHPHEMLightJSON::VEH*& Vehicle);

        bool ReadEmissionData(bool readFC, const std::vector<std::string>& DataPath, const std::string& emissionClass, Helpers* Helper, bool fleetMix, Correction* DataCor, std::vector<std::string>& header, std::vector<std::vector<double> >& matrix, std::vector<double>& idlingValues);

        bool CorrectEmissionData(Correction* DataCor, std::vector<std::string>& header, std::vector<std::vector<double> >& matrix, std::vector<double>& idlingValues);

        //Calculate correction factor for detoriation and temperature correction
        double GetDetTempCor(Correction* DataCor, const std::string& Emi);


        //Split the string
        const std::vector<std::string> split(const std::string& s, char delim);

        //Convert string to double
        double todouble(const std::string& s);

        //Convert string to double list
        std::vector<double> todoubleList(const std::vector<std::string>& s);

        //Read a line from file
        std::string ReadLine(std::ifstream& s);
    };
}

