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
/// @file    Correction.cpp
/// @author  Martin Dippold
/// @author  Michael Behrisch
/// @date    July 2016
///
//
/****************************************************************************/
#include <config.h>

#include <fstream>
#include <utils/common/StringUtils.h>
#include "Correction.h"
#include "Helpers.h"
#include "Constants.h"


namespace PHEMlightdllV5 {

    Correction::Correction(const std::vector<std::string>& dataPath)
        : privateDataPath(dataPath) {
        setUseDet(false);
        setUseTNOx(false);
        setAmbTemp(20);
        setYear(2022);
        setVehMileage(-1);
        setDETFilePath("Deterioration.det");
        setVMAFilePath("Mileage.vma");
        setTNOxFilePath("NOxCor.tno");
    }

    const bool& Correction::getUseDet() const {
        return privateUseDet;
    }

    void Correction::setUseDet(const bool& value) {
        privateUseDet = value;
    }

    const std::string& Correction::getDETFilePath() const {
        return privateDETFilePath;
    }

    void Correction::setDETFilePath(const std::string& value) {
        privateDETFilePath = value;
    }

    const std::string& Correction::getVMAFilePath() const {
        return privateVMAFilePath;
    }

    void Correction::setVMAFilePath(const std::string& value) {
        privateVMAFilePath = value;
    }

    const int& Correction::getYear() const {
        return privateYear;
    }

    void Correction::setYear(const int& value) {
        privateYear = value;
    }

    const double& Correction::getVehMileage() const {
        return privateVehMileage;
    }

    void Correction::setVehMileage(const double& value) {
        privateVehMileage = value;
    }

    bool Correction::ReadDet(std::string& ErrMSG) {
        //Read Detoriation file
        if (!ReadDETFile(ErrMSG)) {
            return false;
        }

        if (!ReadVMAFile(ErrMSG)) {
            return false;
        }

        // Return value
        return true;
    }

    bool Correction::ReadDETFile(std::string& ErrMSG) {
        //Open file
        std::ifstream detReader;
        for (const std::string& p : privateDataPath) {
            detReader.open((p + getDETFilePath()).c_str());
            if (detReader.good()) {
                break;
            }
        }
        if (!detReader.good()) {
            ErrMSG = "File does not exist! (" + getDETFilePath() + ")";
            return false;
        }

        //**** DET Datei einlesen ****
        try {
            detReader >> DETdata;
        } catch (...) {
            ErrMSG = "Error during file read! (" + getDETFilePath() + ")";
            return false;
        }

        // Return value
        return true;
    }

    bool Correction::ReadVMAFile(std::string& ErrMSG) {
        //Open file
        std::ifstream vmaReader;
        for (const std::string& p : privateDataPath) {
            vmaReader.open((p + getVMAFilePath()).c_str());
            if (vmaReader.good()) {
                break;
            }
        }
        if (!vmaReader.good()) {
            ErrMSG = "File does not exist! (" + getVMAFilePath() + ")";
            return false;
        }

        //**** VMA Datei einlesen ****
        try {
            vmaReader >> VMAdata;
        } catch (...) {
            ErrMSG = "Error during file read! (" + getVMAFilePath() + ")";
            return false;
        }

        // Return value
        return true;
    }

    bool Correction::IniDETfactor(Helpers* Helper) {
        //Initialise
        DETFactors = std::map<std::string, double>();

        if (DETdata.at("Vehicle").contains(Helper->getvClass())) {
            if (DETdata.at("Vehicle").at(Helper->getvClass()).at("PropulsionClass").contains(Helper->getpClass())) {
                for (const auto& it : DETdata.at("Vehicle").at(Helper->getvClass()).at("PropulsionClass").at(Helper->getpClass()).at("Emission").items()) {
                    std::string EUclass = StringUtils::replace(Helper->geteClass(), "EU", "EURO ");

                    //PC special classes check (ab, c, d, d-Temp). If available use otherwise use "EURO 6" if available
                    if ((Helper->getvClass() == Constants::strPKW || Helper->getvClass() == Constants::strLNF) && EUclass.length() > 6) {
                        std::string EUclassShort = EUclass.substr(0, 6);

                        if (!it.value().at("EUClass").contains(EUclass) && it.value().at("EUClass").contains(EUclassShort)) {
                            EUclass = EUclassShort;
                        }
                    }

                    //Store in upper case because retrieval will be upper case as well
                    std::string key = it.key();
                    std::transform(key.begin(), key.end(), key.begin(), [](char c) { return (char)::toupper(c); });
                    //Get the factor
                    if (it.value().at("EUClass").contains(EUclass)) {
                        const std::vector<double>& Mileage = it.value().at("Mileage");
                        const std::vector<double>& Factor = it.value().at("EUClass").at(EUclass);
                        if (getVehMileage() < 0) {
                            setVehMileage(GetMileage(Helper));
                        }

                        for (int i = 1; i < (int)Mileage.size(); i++) {
                            if (i == 1 && Mileage[i] > getVehMileage()) {
                                DETFactors.insert(std::make_pair(key, Factor[0]));
                                break;
                            }
                            else if (i == (int)Mileage.size() - 1 && getVehMileage() > Mileage[i]) {
                                DETFactors.insert(std::make_pair(key, Factor[i]));
                                break;
                            }
                            else if (getVehMileage() < Mileage[i]) {
                                DETFactors.insert(std::make_pair(key, Interpolate(getVehMileage(), Mileage[i - 1], Mileage[i], Factor[i - 1], Factor[i])));
                                break;
                            }
                        }
                    }
                    else {
                        DETFactors.insert(std::make_pair(key, 1));
                    }
                }
            }
        }

        //Return value
        return true;
    }

    double Correction::GetMileage(Helpers* Helper) {
        // Initialise
        double Mileage = 0;

        if (VMAdata.at("Vehicle").contains(Helper->getvClass())) {
            if (VMAdata.at("Vehicle").at(Helper->getvClass()).at("PropulsionClass").contains(Helper->getpClass())) {
                std::string Sclass = "0";
//C# TO C++ CONVERTER NOTE: The following 'switch' operated on a string variable and was converted to C++ 'if-else' logic:
//                switch (Helper.sClass)
//ORIGINAL LINE: case "":
                if (Helper->getsClass() == "") {
                        Sclass = "0";
                }
//ORIGINAL LINE: case "I":
                else if (Helper->getsClass() == "I") {
                        Sclass = "1";
                }
//ORIGINAL LINE: case "II":
                else if (Helper->getsClass() == "II") {
                        Sclass = "2";
                }
//ORIGINAL LINE: case "III":
                else if (Helper->getsClass() == "III") {
                        Sclass = "3";
                }

                if (VMAdata.at("Vehicle").at(Helper->getvClass()).at("PropulsionClass").at(Helper->getpClass()).at("SizeClass").contains(Sclass)) {
                    const nlohmann::json& sclassJson = VMAdata.at("Vehicle").at(Helper->getvClass()).at("PropulsionClass").at(Helper->getpClass()).at("SizeClass").at(Sclass);
                    std::string EUclass = StringUtils::replace(Helper->geteClass(), "EU", "EURO ");

                    //PC special classes check (ab, c, d, d-Temp). If available use otherwise use "EURO 6" if available
                    if ((Helper->getvClass() == Constants::strPKW || Helper->getvClass() == Constants::strLNF) && EUclass.length() > 6) {
                        std::string EUclassShort = EUclass.substr(0, 6);

                        if (!sclassJson.at("EUClass").contains(EUclass) && sclassJson.at("EUClass").contains(EUclassShort)) {
                            EUclass = EUclassShort;
                        }
                    }

                    if (sclassJson.contains(EUclass)) {
                        //Calculate Mileage
                        const std::vector<double>& Factor = sclassJson.at(EUclass);
                        int AnzYear = getYear() - 2020 + 1;
                        Mileage = Factor[0] * std::pow(AnzYear, 3) + Factor[1] * std::pow(AnzYear, 2) + Factor[2] * AnzYear + Factor[3];

                        //Check calculated mileage
                        if (Mileage < 0) {
                            Mileage = 0;
                        }
                    }
                }
            }
        }


        //Return value
        return Mileage;
    }

    const bool& Correction::getUseTNOx() const {
        return privateUseTNOx;
    }

    void Correction::setUseTNOx(const bool& value) {
        privateUseTNOx = value;
    }

    const std::string& Correction::getTNOxFilePath() const {
        return privateTNOxFilePath;
    }

    void Correction::setTNOxFilePath(const std::string& value) {
        privateTNOxFilePath = value;
    }

    const double& Correction::getAmbTemp() const {
        return privateAmbTemp;
    }

    void Correction::setAmbTemp(const double& value) {
        privateAmbTemp = value;
    }

    const double& Correction::getTNOxFactor() const {
        return privateTNOxFactor;
    }

    void Correction::setTNOxFactor(const double& value) {
        privateTNOxFactor = value;
    }

    bool Correction::ReadTNOx(std::string& ErrMSG) {
        //Open file
        std::ifstream tnoxReader;
        for (const std::string& p : privateDataPath) {
            tnoxReader.open((p + getTNOxFilePath()).c_str());
            if (tnoxReader.good()) {
                break;
            }
        }
        if (!tnoxReader.good()) {
            ErrMSG = "File does not exist! (" + getTNOxFilePath() + ")";
            return false;
        }

        //**** VMA Datei einlesen ****
        try {
            tnoxReader >> TNOxdata;
        } catch (...) {
            ErrMSG = "Error during file read! (" + getTNOxFilePath() + ")";
            return false;
        }

        // Return value
        return true;
    }

    bool Correction::IniTNOxfactor(Helpers* Helper) {
        //Initialise
        setTNOxFactor(1);

        //Calculation only for diesel vehicles
        if (Helper->getpClass() != Constants::strDiesel) {
            return true;
        }

        if (TNOxdata.at("Vehicle").contains(Helper->getvClass())) {
            std::string EUclass = StringUtils::replace(Helper->geteClass(), "EU", "EURO ");

            //PC special classes check (ab, c, d, d-Temp). If available use otherwise use "EURO 6" if available
            if ((Helper->getvClass() == Constants::strPKW || Helper->getvClass() == Constants::strLNF) && EUclass.length() > 6) {
                std::string EUclassShort = EUclass.substr(0, 6);

                if (!TNOxdata.at("Vehicle").at(Helper->getvClass()).at("EUClass").contains(EUclass) && TNOxdata.at("Vehicle").at(Helper->getvClass()).at("EUClass").contains(EUclassShort)) {
                    EUclass = EUclassShort;
                }
            }


            if (TNOxdata.at("Vehicle").at(Helper->getvClass()).at("EUClass").contains(EUclass)) {
                //Check/set temperature borders, because calculation is a straight function
                const nlohmann::json& EUclassJson = TNOxdata.at("Vehicle").at(Helper->getvClass()).at("EUClass").at(EUclass);
                const double m = EUclassJson.at("m");
                const double c = EUclassJson.at("c");
                const double tb0 = EUclassJson.at("TB")[0];
                if (getAmbTemp() < tb0) {
                    setTNOxFactor(m + c * tb0);
                }
                else if (getAmbTemp() > EUclassJson.at("TB")[1]) {
                    setTNOxFactor(1);
                }
                else {
                    setTNOxFactor(m + c * getAmbTemp());
                }
            }
        }

        //Return value
        return true;
    }

    double Correction::Interpolate(double px, double p1, double p2, double e1, double e2) {
        if (p2 == p1) {
            return e1;
        }

        return e1 + (px - p1) / (p2 - p1) * (e2 - e1);
    }

}
