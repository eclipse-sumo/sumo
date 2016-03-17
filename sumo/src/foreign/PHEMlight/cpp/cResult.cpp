#include "cResult.h"


namespace PHEMlightdll {

    VehicleResult::VehicleResult(double time, double speed, double grad, double power, double pPos, double pNormRated, double pNormDrive, double acc, double fc, double fcel, double cO2, double nOx, double hC, double pM, double cO) {
        InitializeInstanceFields();
        _time = time;
        _speed = speed;
        _grad = grad;
        _power = power;
        _pPos = pPos;
        _pNormRated = pNormRated;
        _pNormDrive = pNormDrive;
        _accelaration = acc;
        _emissionData = new EmissionData(fc, fcel, cO2, nOx, hC, pM, cO);
    }

    const double& VehicleResult::getTime() const {
        return _time;
    }

    const double& VehicleResult::getSpeed() const {
        return _speed;
    }

    const double& VehicleResult::getGrad() const {
        return _grad;
    }

    const double& VehicleResult::getPower() const {
        return _power;
    }

    const double& VehicleResult::getPPos() const {
        return _pPos;
    }

    const double& VehicleResult::getPNormRated() const {
        return _pNormRated;
    }

    void VehicleResult::setPNormRated(const double& value) {
        _pNormRated = value;
    }

    const double& VehicleResult::getPNormDrive() const {
        return _pNormDrive;
    }

    void VehicleResult::setPNormDrive(const double& value) {
        _pNormDrive = value;
    }

    const double& VehicleResult::getAccelaration() const {
        return _accelaration;
    }

    PHEMlightdll::EmissionData* VehicleResult::getEmissionData() const {
        return _emissionData;
    }

    void VehicleResult::InitializeInstanceFields() {
        _time = 0;
        _speed = 0;
        _grad = 0;
        _power = 0;
        _pPos = 0;
        _pNormRated = 0;
        _pNormDrive = 0;
        _accelaration = 0;
    }

    EmissionData::EmissionData(double fc, double fcel, double cO2, double nOx, double hC, double pM, double cO) {
        InitializeInstanceFields();
        _fc = fc;
        _fcel = fcel;
        _cO2 = cO2;
        _nOx = nOx;
        _hC = hC;
        _pM = pM;
        _cO = cO;
    }

    const double& EmissionData::getFC() const {
        return _fc;
    }

    const double& EmissionData::getFCel() const {
        return _fcel;
    }

    const double& EmissionData::getCO2() const {
        return _cO2;
    }

    const double& EmissionData::getNOx() const {
        return _nOx;
    }

    const double& EmissionData::getHC() const {
        return _hC;
    }

    const double& EmissionData::getPM() const {
        return _pM;
    }

    const double& EmissionData::getCO() const {
        return _cO;
    }

    void EmissionData::InitializeInstanceFields() {
        _fc = 0;
        _fcel = 0;
        _cO2 = 0;
        _nOx = 0;
        _hC = 0;
        _pM = 0;
        _cO = 0;
    }
}
