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
/// @file    Correction.h
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
#include <cmath>
#include <utility>
#include <foreign/nlohmann/json.hpp>

namespace PHEMlightdllV5 { class Helpers; }

namespace PHEMlightdllV5 {
    class Correction {
    private:
        bool privateUseDet;
        const std::vector<std::string> privateDataPath;
        std::string privateDETFilePath;
        std::string privateVMAFilePath;
        int privateYear;
        double privateVehMileage;
        bool privateUseTNOx;
        std::string privateTNOxFilePath;
        double privateAmbTemp;
        double privateTNOxFactor;

    public:
        Correction(const std::vector<std::string>& dataPath);

    public:
        std::map<std::string, double> DETFactors;

        const bool&  getUseDet() const;
        void setUseDet(const bool&  value);

        const std::string&  getDETFilePath() const;
        void setDETFilePath(const std::string& value);

        const std::string&  getVMAFilePath() const;
        void setVMAFilePath(const std::string& value);

        const int&  getYear() const;
        void setYear(const int&  value);

        const double&  getVehMileage() const;
        void setVehMileage(const double&  value);

        //Read the file
        bool ReadDet(std::string& ErrMSG);
    private:
        bool ReadDETFile(std::string& ErrMSG);
        bool ReadVMAFile(std::string& ErrMSG);

        //Initialise the Detoriation Factor for the vehicle
    public:
        bool IniDETfactor(Helpers* Helper);

        //Get the milage of the vehicle
    private:
        double GetMileage(Helpers* Helper);

    public:
        const bool&  getUseTNOx() const;
        void setUseTNOx(const bool&  value);

        const std::string&  getTNOxFilePath() const;
        void setTNOxFilePath(const std::string& value);

        const double&  getAmbTemp() const;
        void setAmbTemp(const double&  value);

        const double&  getTNOxFactor() const;
        void setTNOxFactor(const double&  value);

        //Read the file
        bool ReadTNOx(std::string& ErrMSG);

        bool IniTNOxfactor(Helpers* Helper);

    private:
        double Interpolate(double px, double p1, double p2, double e1, double e2);

    private:
        nlohmann::json DETdata;
        nlohmann::json VMAdata;
        nlohmann::json TNOxdata;

    };
}
