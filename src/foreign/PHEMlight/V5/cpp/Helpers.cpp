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
/// @file    Helpers.cpp
/// @author  Martin Dippold
/// @author  Michael Behrisch
/// @date    July 2016
///
//
/****************************************************************************/
#include <config.h>

#include "Helpers.h"
#include "Constants.h"


namespace PHEMlightdllV5 {

    const std::string& Helpers::getvClass() const {
        return _vClass;
    }

    void Helpers::setvClass(const std::string& value) {
        _vClass = value;
    }

    const std::string& Helpers::geteClass() const {
        return _eClass;
    }

    void Helpers::seteClass(const std::string& value) {
        _eClass = value;
    }

    const std::string& Helpers::getpClass() const {
        return _pClass;
    }

    void Helpers::setpClass(const std::string& value) {
        _pClass = value;
    }

    const std::string& Helpers::getsClass() const {
        return _sClass;
    }

    void Helpers::setsClass(const std::string& value) {
        _sClass = value;
    }

    const std::string& Helpers::getgClass() const {
        return _Class;
    }

    void Helpers::setgClass(const std::string& value) {
        _Class = value;
    }

    const std::string& Helpers::getuClass() const {
        return _uClass;
    }

    void Helpers::setuClass(const std::string& value) {
        _uClass = value;
    }

    const std::string& Helpers::getErrMsg() const {
        return _ErrMsg;
    }

    void Helpers::setErrMsg(const std::string& value) {
        _ErrMsg = value;
    }

    const std::string& Helpers::getCommentPrefix() const {
        return _commentPrefix;
    }

    void Helpers::setCommentPrefix(const std::string& value) {
        _commentPrefix = value;
    }

    const std::string& Helpers::getPHEMDataV() const {
        return _PHEMDataV;
    }

    void Helpers::setPHEMDataV(const std::string& value) {
        _PHEMDataV = value;
    }

    bool Helpers::getvclass(const std::string& VEH) {
        // Set the drive train efficency
        Constants::setDRIVE_TRAIN_EFFICIENCY(Constants::DRIVE_TRAIN_EFFICIENCY_All);

        //Get the vehicle class
        if (VEH.find(Constants::strPKW) != std::string::npos) {
            _vClass = Constants::strPKW;
            return true;
        }
        else if (VEH.find(Constants::strLNF) != std::string::npos) {
            _vClass = Constants::strLNF;
            return true;
        }
        else if (VEH.find(Constants::strLKW) != std::string::npos) {
            _vClass = Constants::strLKW;
            return true;
        }
        else if (VEH.find(Constants::strLSZ) != std::string::npos) {
            _vClass = Constants::strLSZ;
            return true;
        }
        else if (VEH.find(Constants::strRB) != std::string::npos) {
            _vClass = Constants::strRB;
            return true;
        }
        else if (VEH.find(Constants::strLB) != std::string::npos) {
            _vClass = Constants::strLB;
            Constants::setDRIVE_TRAIN_EFFICIENCY(Constants::DRIVE_TRAIN_EFFICIENCY_CB);
            return true;
        }
        else if (VEH.find(Constants::strMR2) != std::string::npos) {
            _vClass = Constants::strMR2;
            return true;
        }
        else if (VEH.find(Constants::strMR4) != std::string::npos) {
            _vClass = Constants::strMR4;
            return true;
        }
        else if (VEH.find(Constants::strMR) != std::string::npos) {
            _vClass = Constants::strMR;
            return true;
        }
        else if (VEH.find(Constants::strKKR) != std::string::npos) {
            _vClass = Constants::strKKR;
            return true;
        }
        //Should never happens
        _ErrMsg = std::string("Vehicle class not defined! (") + VEH + std::string(")");
        return false;
    }

    bool Helpers::getpclass(const std::string& VEH) {
        if ((int)VEH.find(std::string("_") + Constants::strBEV) > 0) {
            _pClass = Constants::strBEV;
            return true;
        }
        else if ((int)VEH.find(std::string("_") + Constants::strDiesel) > 0) {
            _pClass = Constants::strDiesel;
            return true;
        }
        else if ((int)VEH.find(std::string("_") + Constants::strGasoline) > 0) {
            _pClass = Constants::strGasoline;
            return true;
        }
        else if ((int)VEH.find(std::string("_") + Constants::strCNG) > 0) {
            _pClass = Constants::strCNG;
            return true;
        }
        else if ((int)VEH.find(std::string("_") + Constants::strLPG) > 0) {
            _pClass = Constants::strLPG;
            return true;
        }
        //Should never happens
        _ErrMsg = std::string("Fuel class not defined! (") + VEH + std::string(")");
        return false;
    }

    bool Helpers::getsclass(const std::string& VEH) {
        if (VEH.find(Constants::strLKW) != std::string::npos) {
            if ((int)VEH.find(std::string("_") + Constants::strSII) > 0) {
                _sClass = Constants::strSII;
                return true;
            }
            else if ((int)VEH.find(std::string("_") + Constants::strSI) > 0) {
                _sClass = Constants::strSI;
                return true;
            }
            else {
                //Should never happen
                _ErrMsg = std::string("Size class not defined! (") + VEH + std::string(")");
                return false;
            }
        }
        else if (VEH.find(Constants::strLNF) != std::string::npos) {
            if ((int)VEH.find(std::string("_") + Constants::strSIII) > 0) {
                _sClass = Constants::strSIII;
                return true;
            }
            else if ((int)VEH.find(std::string("_") + Constants::strSII) > 0) {
                _sClass = Constants::strSII;
                return true;
            }
            else if ((int)VEH.find(std::string("_") + Constants::strSI) > 0) {
                _sClass = Constants::strSI;
                return true;
            }
            else {
                _ErrMsg = std::string("Size class not defined! (") + VEH.substr((int)VEH.rfind("\\"), VEH.length() - (int)VEH.rfind("\\")) + std::string(")");
                return false;
            }
        }
        else if (VEH.find(Constants::strLB) != std::string::npos) {
            if ((int)VEH.find(std::string("_") + Constants::strMidi) > 0) {
                _sClass = Constants::strMidi;
                return true;
            }
            else if ((int)VEH.find(std::string("_") + Constants::strHeavy) > 0) {
                _sClass = Constants::strHeavy;
                return true;
            }
            else {
                _sClass = "";
                return true;
            }
        }
        else {
            _sClass = "";
            return true;
        }
    }

    bool Helpers::geteclass(const std::string& VEH) {
        if ((int)VEH.find(std::string("_") + Constants::strEU) > 0) {
            if ((int)VEH.find("_", (int)VEH.find(std::string("_") + Constants::strEU) + 1) > 0) {
                _eClass = Constants::strEU + VEH.substr((int)VEH.find(std::string("_") + Constants::strEU) + 3, (int)VEH.find("_", (int)VEH.find(std::string("_") + Constants::strEU) + 1) - ((int)VEH.find(std::string("_") + Constants::strEU) + 3));
                return true;
            }
            else if ((int)VEH.find(".", (int)VEH.find(std::string("_") + Constants::strEU) + 1) > 0) {
                _eClass = Constants::strEU + VEH.substr((int)VEH.find(std::string("_") + Constants::strEU) + 3, (int)VEH.find(".", (int)VEH.find(std::string("_") + Constants::strEU) + 1) - ((int)VEH.find(std::string("_") + Constants::strEU) + 3));
                return true;
            }
            else {
                _eClass = Constants::strEU + VEH.substr((int)VEH.find(std::string("_") + Constants::strEU) + 3, VEH.length() - ((int)VEH.find(std::string("_") + Constants::strEU) + 3));
                return true;
            }
        }
        else if ((int)VEH.find(std::string("_") + Constants::strBEV) > 0) {
            _eClass = "";
            return true;
        }
        //Should never happens
        _ErrMsg = std::string("Euro class not defined! (") + VEH + std::string(")");
        return false;
    }

    bool Helpers::getuclass(const std::string& VEH) {
        if ((int)VEH.find(std::string("_") + Constants::strHybrid) > 0) {
            _uClass = Constants::strHybrid;
            return true;
        }
        else if ((int)VEH.find(std::string("_") + Constants::strSU) > 0) {
            _uClass = Constants::strSU;
            return true;
        }
        else {
            _uClass = "";
            return true;
        }
    }

    bool Helpers::setclass(const std::string& VEH) {
        //Get the classes
        if (!getvclass(VEH)) {
            return false;
        }
        if (!geteclass(VEH)) {
            return false;
        }
        if (!getpclass(VEH)) {
            return false;
        }
        if (!getsclass(VEH)) {
            return false;
        }
        if (!getuclass(VEH)) {
            return false;
        }

        if ((int)VEH.rfind("\\") <= 0) {
            _Class = VEH;
        }
        else {
            std::string vehstr = VEH.substr((int)VEH.rfind("\\") + 1, VEH.length() - (int)VEH.rfind("\\") - 1);
            _Class = vehstr.substr(0, (int)vehstr.find("."));
        }
        return true;
    }
}
