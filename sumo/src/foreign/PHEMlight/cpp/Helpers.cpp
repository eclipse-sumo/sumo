#include "Helpers.h"
#include "Constants.h"


namespace PHEMlightdll {

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

    const std::string& Helpers::getfClass() const {
        return _fClass;
    }

    void Helpers::setfClass(const std::string& value) {
        _fClass = value;
    }

    const std::string& Helpers::getsClass() const {
        return _sClass;
    }

    void Helpers::setsClass(const std::string& value) {
        _sClass = value;
    }

    const std::string& Helpers::getpClass() const {
        return _pClass;
    }

    void Helpers::setpClass(const std::string& value) {
        _pClass = value;
    }

    const std::string& Helpers::getgClass() const {
        return _Class;
    }

    void Helpers::setgClass(const std::string& value) {
        _Class = value;
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
        if (VEH.find(Constants::strPKW) >= 0) {
            _vClass = Constants::strPKW;
            return true;
        }
        else if (VEH.find(Constants::strLNF) >= 0) {
            _vClass = Constants::strLNF;
            return true;
        }
        else if (VEH.find(Constants::strLKW) >= 0) {
            _vClass = Constants::strLKW;
            return true;
        }
        else if (VEH.find(Constants::strLSZ) >= 0) {
            _vClass = Constants::strLSZ;
            return true;
        }
        else if (VEH.find(Constants::strRB) >= 0) {
            _vClass = Constants::strRB;
            return true;
        }
        else if (VEH.find(Constants::strLB) >= 0) {
            _vClass = Constants::strLB;
            Constants::setDRIVE_TRAIN_EFFICIENCY(Constants::DRIVE_TRAIN_EFFICIENCY_CB);
            return true;
        }
        else if (VEH.find(Constants::strMR2) >= 0) {
            _vClass = Constants::strMR2;
            return true;
        }
        else if (VEH.find(Constants::strMR4) >= 0) {
            _vClass = Constants::strMR4;
            return true;
        }
        else if (VEH.find(Constants::strKKR) >= 0) {
            _vClass = Constants::strKKR;
            return true;
        }
        //Should never happens
        _ErrMsg = std::string("Vehicle class not defined! (") + VEH + std::string(")");
        return false;
    }

    bool Helpers::getfclass(const std::string& VEH) {
        if (VEH.find(std::string("_") + Constants::strDiesel) > 0) {
            _fClass = Constants::strDiesel;
            return true;
        }
        else if (VEH.find(std::string("_") + Constants::strGasoline) > 0) {
            _fClass = Constants::strGasoline;
            return true;
        }
        else if (VEH.find(std::string("_") + Constants::strCNG) > 0) {
            _fClass = Constants::strCNG;
            return true;
        }
        else if (VEH.find(std::string("_") + Constants::strBEV) > 0) {
            //No fuel class by EV vehicle
            _fClass = "";
            return true;
        }
        //Should never happens
        _ErrMsg = std::string("Fuel class not defined! (") + VEH + std::string(")");
        return false;
    }

    bool Helpers::getpclass(const std::string& VEH) {
        if (VEH.find(std::string("_") + Constants::strHybrid) > 0) {
            _pClass = Constants::strHybrid;
            return true;
        }
        else if (VEH.find(std::string("_") + Constants::strBEV) > 0) {
            _pClass = Constants::strBEV;
            return true;
        }
        //default
        _pClass = "";
        return false;
    }

    bool Helpers::getsclass(const std::string& VEH) {
        if (VEH.find(Constants::strLKW) > 0) {
            if (VEH.find(std::string("_") + Constants::strSII) > 0) {
                _sClass = Constants::strSII;
                return true;
            }
            else if (VEH.find(std::string("_") + Constants::strSI) > 0) {
                _sClass = Constants::strSI;
                return true;
            }
            else {
                //Should never happen
                _ErrMsg = std::string("Size class not defined! (") + VEH + std::string(")");
                return false;
            }
        }
        else if (VEH.find(Constants::strLNF) > 0) {
            if (VEH.find(std::string("_") + Constants::strSIII) > 0) {
                _sClass = Constants::strSIII;
                return true;
            }
            else if (VEH.find(std::string("_") + Constants::strSII) > 0) {
                _sClass = Constants::strSII;
                return true;
            }
            else if (VEH.find(std::string("_") + Constants::strSI) > 0) {
                _sClass = Constants::strSI;
                return true;
            }
            else {
                _ErrMsg = std::string("Size class not defined! (") + VEH.substr(VEH.rfind("\\"), VEH.length() - VEH.rfind("\\")) + std::string(")");
                return false;
            }
        }
        else {
            _sClass = "";
            return true;
        }
    }

    bool Helpers::geteclass(const std::string& VEH) {
        if (VEH.find(std::string("_") + Constants::strEU) > 0) {
            if (VEH.find("_", VEH.find(std::string("_") + Constants::strEU) + 1) > 0) {
                _eClass = Constants::strEU + VEH.substr(VEH.find(std::string("_") + Constants::strEU) + 3, VEH.find("_", VEH.find(std::string("_") + Constants::strEU) + 1) - (VEH.find(std::string("_") + Constants::strEU) + 3));
                return true;
            }
            else if (VEH.find(".", VEH.find(std::string("_") + Constants::strEU) + 1) > 0) {
                _eClass = Constants::strEU + VEH.substr(VEH.find(std::string("_") + Constants::strEU) + 3, VEH.find(".", VEH.find(std::string("_") + Constants::strEU) + 1) - (VEH.find(std::string("_") + Constants::strEU) + 3));
                return true;
            }
            else {
                _eClass = Constants::strEU + VEH.substr(VEH.find(std::string("_") + Constants::strEU) + 3, VEH.length() - (VEH.find(std::string("_") + Constants::strEU) + 3));
                return true;
            }
        }
        //Should never happens
        _ErrMsg = std::string("Euro class not defined! (") + VEH + std::string(")");
        return false;
    }

    bool Helpers::setclass(const std::string& VEH) {
        if (getvclass(VEH)) {
            _Class = _vClass;
        }
        else {
            return false;
        }
        if (getfclass(VEH)) {
            if (_fClass != "") {
                _Class = _Class + std::string("_") + getfClass();
            }
        }
        else {
            return false;
        }
        if (geteclass(VEH)) {
            _Class = _Class + std::string("_") + geteClass();
        }
        else {
            return false;
        }
        if (getpclass(VEH)) {
            _Class = _Class + std::string("_") + getpClass();
        }
        if (getsclass(VEH)) {
            if (_sClass != "") {
                _Class = _Class + std::string("_") + getsClass();
            }
        }
        else {
            return false;
        }
        return true;
    }
}
