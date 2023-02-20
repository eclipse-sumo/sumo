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
/// @file    CEPHandler.cpp
/// @author  Martin Dippold
/// @author  Michael Behrisch
/// @date    July 2016
///
//
/****************************************************************************/
#include <config.h>

#include <fstream>
#include <sstream>
#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <foreign/nlohmann/json.hpp>
#include <utils/common/StringUtils.h>
#include "CEPHandler.h"
#include "CEP.h"
#include "Correction.h"
#include "Helpers.h"


namespace PHEMlightdllV5 {

    CEPHandler::CEPHandler() {
    }

    const std::map<std::string, CEP*>& CEPHandler::getCEPS() const {
        return _ceps;
    }

    bool CEPHandler::GetCEP(std::vector<std::string>& DataPath, Helpers* Helper, Correction* DataCor) {
        if (getCEPS().find(Helper->getgClass()) == getCEPS().end()) {
            if (!Load(DataPath, Helper, DataCor)) {
                return false;
            }
        }
        return true;
    }

    bool CEPHandler::CalcCorrection(Correction* DataCor, Helpers* Helper, VEHPHEMLightJSON::Vehicle_Data* vehicle_Data) {
            if (DataCor->getUseDet()) {
                DataCor->setVehMileage(-1);
                if (vehicle_Data->getMileage() > 0.) {
                    DataCor->setVehMileage(vehicle_Data->getMileage());
                }

                if (!DataCor->IniDETfactor(Helper)) {
                    return false;
                }
            }
            if (DataCor->getUseTNOx()) {
                if (!DataCor->IniTNOxfactor(Helper)) {
                    return false;
                }
            }

            //Return value
            return true;
    }

    bool CEPHandler::Load(std::vector<std::string>& DataPath, Helpers* Helper, Correction* DataCor, bool fleetMix) {
        //Deklaration
        // get string identifier for PHEM emission class
        std::string emissionRep = Helper->getgClass();

        // to hold everything.
        std::vector<std::vector<double> > matrixFCvalues;
        std::vector<std::vector<double> > matrixPollutants;
        std::vector<double> idlingValuesFCvalues;
        std::vector<double> idlingValuesPollutants;
        std::vector<std::string> headerFCvalues;
        std::vector<std::string> headerPollutants;
        VEHPHEMLightJSON::VEH* Vehicle;

        if (!ReadVehicleFile(DataPath, emissionRep, Helper, fleetMix, Vehicle)) {
            delete Vehicle;
            return false;
        }

        if (DataCor != nullptr) {
            if (!CalcCorrection(DataCor, Helper, Vehicle->getVehicleData())) {
                delete Vehicle;
                return false;
            }
        }

        if (!ReadEmissionData(true, DataPath, emissionRep, Helper, fleetMix, DataCor, headerFCvalues, matrixFCvalues, idlingValuesFCvalues)) {
            delete Vehicle;
            return false;
        }
        if (!ReadEmissionData(false, DataPath, emissionRep, Helper, fleetMix, DataCor, headerPollutants, matrixPollutants, idlingValuesPollutants)) {
            delete Vehicle;
            return false;
        }

        _ceps.insert(std::make_pair(Helper->getgClass(), new CEP(Vehicle, headerFCvalues, matrixFCvalues, headerPollutants, matrixPollutants, idlingValuesFCvalues, idlingValuesPollutants)));
        delete Vehicle;
        return true;
    }

    double json2double(const nlohmann::json& vd, const std::string& key) {
        if (vd.contains(key)) {
            return vd.at(key).get<double>();
        }
        return 0.;
    }

    bool CEPHandler::ReadVehicleFile(const std::vector<std::string>& DataPath, const std::string& emissionClass, Helpers* Helper, bool /* fleetMix */, VEHPHEMLightJSON::VEH*& Vehicle) {
        std::string path = "";
        Vehicle = new VEHPHEMLightJSON::VEH();

        //Open file
        std::ifstream vehicleReader;
        for (std::vector<std::string>::const_iterator i = DataPath.begin(); i != DataPath.end(); i++) {
            vehicleReader.open(((*i) + emissionClass + ".PHEMLight.veh").c_str());
            if (vehicleReader.good()) {
                break;
            }
        }
        if (!vehicleReader.good()) {
            Helper->setErrMsg("File does not exist! (" + emissionClass + ".PHEMLight.veh)");
            return false;
        }

        //**** VEH Datei einlesen ****
        nlohmann::json json;
        try {
            vehicleReader >> json;
        } catch (...) {
            Helper->setErrMsg("Error during file read! (" + emissionClass + ".PHEMLight.veh)");
            return false;
        }

        //*** Get the vehicle data
        nlohmann::json::iterator vehDataIt = json.find("VehicleData");
        if (vehDataIt == json.end()) {
            Helper->setErrMsg("No VehicleData in " + emissionClass + ".PHEMLight.veh!");
            return false;
        }
        const nlohmann::json& vd = *vehDataIt;
        Vehicle->getVehicleData()->setMassType(vd.contains("MassType") ? vd.at("MassType").get<std::string>() : "LV");
        Vehicle->getVehicleData()->setFuelType(vd.contains("FuelType") ? vd.at("FuelType").get<std::string>() : "D");
        Vehicle->getVehicleData()->setCalcType(vd.contains("CalcType") ? vd.at("CalcType").get<std::string>() : "Conv");
        Vehicle->getVehicleData()->setMass(json2double(vd, "Mass"));
        Vehicle->getVehicleData()->setLoading(json2double(vd, "Loading"));
        Vehicle->getVehicleData()->setRedMassWheel(json2double(vd, "RedMassWheel"));
        Vehicle->getVehicleData()->setWheelDiameter(json2double(vd, "WheelDiameter"));
        Vehicle->getVehicleData()->setCw(json2double(vd, "Cw"));
        Vehicle->getVehicleData()->setA(json2double(vd, "A"));
        Vehicle->getVehicleData()->setMileage(json2double(vd, "Mileage"));

        // Auxiliaries
        nlohmann::json::iterator auxDataIt = json.find("AuxiliariesData");
        if (auxDataIt == json.end() || !auxDataIt->contains("Pauxnorm")) {
            Vehicle->getAuxiliariesData()->setPauxnorm(0.);
        } else {
            Vehicle->getAuxiliariesData()->setPauxnorm(auxDataIt->at("Pauxnorm").get<double>());
        }

        // Engine Data
        nlohmann::json::iterator engDataIt = json.find("EngineData");
        if (engDataIt == json.end() || !engDataIt->contains("ICEData") || !engDataIt->contains("EMData")) {
            Helper->setErrMsg("Incomplete EngineData in " + emissionClass + ".PHEMLight.veh!");
            return false;
        }
        const nlohmann::json& iced = (*engDataIt)["ICEData"];
        const nlohmann::json& emd = (*engDataIt)["EMData"];
        Vehicle->getEngineData()->getICEData()->setPrated(json2double(iced, "Prated"));
        Vehicle->getEngineData()->getICEData()->setnrated(json2double(iced, "nrated"));
        Vehicle->getEngineData()->getICEData()->setIdling(json2double(iced, "Idling"));
        Vehicle->getEngineData()->getEMData()->setPrated(json2double(emd, "Prated"));
        Vehicle->getEngineData()->getEMData()->setnrated(json2double(emd, "nrated"));

        // Rolling resistance
        nlohmann::json::iterator rrDataIt = json.find("RollingResData");
        if (rrDataIt == json.end()) {
            Helper->setErrMsg("No RollingResData in " + emissionClass + ".PHEMLight.veh!");
            return false;
        }
        const nlohmann::json& rrd = *rrDataIt;
        Vehicle->getRollingResData()->setFr0(json2double(rrd, "Fr0"));
        Vehicle->getRollingResData()->setFr1(json2double(rrd, "Fr1"));
        Vehicle->getRollingResData()->setFr2(json2double(rrd, "Fr2"));
        Vehicle->getRollingResData()->setFr3(json2double(rrd, "Fr3"));
        Vehicle->getRollingResData()->setFr4(json2double(rrd, "Fr4"));

        // Transmission
        nlohmann::json::iterator trDataIt = json.find("TransmissionData");
        if (trDataIt == json.end()) {
            Helper->setErrMsg("No TransmissionData in " + emissionClass + ".PHEMLight.veh!");
            return false;
        }
        Vehicle->getTransmissionData()->setAxelRatio(json2double(*trDataIt, "AxelRatio"));
        nlohmann::json::iterator transmIt = trDataIt->find("Transm");
        if (transmIt == trDataIt->end()) {
            Helper->setErrMsg(std::string("Transmission ratios missing in vehicle file! Calculation stopped! (") + path + std::string(")"));
            return false;
        } else {
            if (!transmIt->contains("Speed")) {
                Helper->setErrMsg(std::string("No Speed signal in transmission data given! Calculation stopped! (") + path + std::string(")"));
                return false;
            }
            if (!transmIt->contains("GearRatio")) {
                Helper->setErrMsg(std::string("No GearRatio signal in transmission data given! Calculation stopped! (") + path + std::string(")"));
                return false;
            }
            if (!transmIt->contains("RotMassF")) {
                Helper->setErrMsg(std::string("No RotMassF signal in transmission data given! Calculation stopped! (") + path + std::string(")"));
                return false;
            }
        }
        Vehicle->getTransmissionData()->setTransm(transmIt->get<std::map<std::string, std::vector<double> > >());

        // Full load and drag
        nlohmann::json::iterator fldDataIt = json.find("FLDData");
        if (fldDataIt == json.end()) {
            Helper->setErrMsg("No FLDData in " + emissionClass + ".PHEMLight.veh!");
            return false;
        }
        const nlohmann::json& fld = *fldDataIt;
        Vehicle->getFLDData()->setP_n_max_v0(json2double(fld, "P_n_max_v0"));
        Vehicle->getFLDData()->setP_n_max_p0(json2double(fld, "P_n_max_p0"));
        Vehicle->getFLDData()->setP_n_max_v1(json2double(fld, "P_n_max_v1"));
        Vehicle->getFLDData()->setP_n_max_p1(json2double(fld, "P_n_max_p1"));
        nlohmann::json::iterator dragIt = fldDataIt->find("DragCurve");
        if (dragIt == fldDataIt->end()) {
            Helper->setErrMsg(std::string("Drag curve missing in vehicle file! Calculation stopped! (") + path + std::string(")"));
            return false;
        } else {
            if (!dragIt->contains("n_norm")) {
                Helper->setErrMsg(std::string("No n_norm signal in drag curve data given! Calculation stopped! (") + path + std::string(")"));
                return false;
            }
            if (!dragIt->contains("pe_drag_norm")) {
                Helper->setErrMsg(std::string("No pe_drag_norm signal in drag curve data given! Calculation stopped! (") + path + std::string(")"));
                return false;
            }
        }
        Vehicle->getFLDData()->setDragCurve(dragIt->get<std::map<std::string, std::vector<double> > >());

        return true;
    }

    bool CEPHandler::ReadEmissionData(bool readFC, const std::vector<std::string>& DataPath, const std::string& emissionClass, Helpers* Helper, bool /* fleetMix */, Correction* DataCor, std::vector<std::string>& header, std::vector<std::vector<double> >& matrix, std::vector<double>& idlingValues) {
        // declare file stream
        std::string line;
        std::string path = "";
        header = std::vector<std::string>();
        matrix = std::vector<std::vector<double> >();
        idlingValues = std::vector<double>();

        std::string pollutantExtension = "";
        if (readFC) {
            pollutantExtension += std::string("_FC");
        }

        std::ifstream fileReader;
        for (std::vector<std::string>::const_iterator i = DataPath.begin(); i != DataPath.end(); i++) {
            fileReader.open(((*i) + emissionClass + pollutantExtension + ".csv").c_str());
            if (fileReader.good()) {
                break;
            }
        }
        if (!fileReader.good()) {
            Helper->setErrMsg("File does not exist! (" + emissionClass + pollutantExtension + ".csv)");
            return false;
        }

        // read header line for pollutant identifiers
        if ((line = ReadLine(fileReader)) != "") {
            const std::vector<std::string>& entries = split(line, ',');
            // skip first entry "Pe"
            for (int i = 1; i < (int)entries.size(); i++) {
                header.push_back(entries[i]);
            }
        }

        // skip units
        ReadLine(fileReader);

        // skip comment
        ReadLine(fileReader);

        //readIdlingValues
        line = ReadLine(fileReader);

        std::vector<std::string> stringIdlings = split(line, ',');
        stringIdlings.erase(stringIdlings.begin());

        idlingValues = todoubleList(stringIdlings);

        while ((line = ReadLine(fileReader)) != "") {
            matrix.push_back(todoubleList(split(line, ',')));
        }

        //Data correction (Det & TNOx)
        if (!CorrectEmissionData(DataCor, header, matrix, idlingValues)) {
            Helper->setErrMsg("Error in correction calculation");
            return false;
        }

        //Return value
        return true;
    }

    bool CEPHandler::CorrectEmissionData(Correction* DataCor, std::vector<std::string>& header, std::vector<std::vector<double> >& matrix, std::vector<double>& idlingValues) {
        for (int i = 0; i < (int)header.size(); i++) {
            double CorF = GetDetTempCor(DataCor, header[i]);
            if (CorF != 1) {
                for (int j = 0; j < (int)matrix.size(); j++) {
                    matrix[j][i + 1] *= CorF;
                }
                idlingValues[i] *= CorF;
            }
        }

        //Return value
        return true;
    }

    double CEPHandler::GetDetTempCor(Correction* DataCor, const std::string& Emi) {
        //Initialisation
        double CorF = 1;
        std::string emi = Emi;
        std::transform(emi.begin(), emi.end(), emi.begin(), [](char c) { return (char)::toupper(c); });

        if (DataCor != 0) {
            if (DataCor->getUseDet() && DataCor->DETFactors.count(emi) > 0) {
                CorF += DataCor->DETFactors[emi] - 1;
            }
            if (DataCor->getUseTNOx()) {
                if (emi.find("NOX") != std::string::npos) {
                    CorF += (DataCor->getTNOxFactor() - 1);
                }
            }
        }

        //Return value
        return CorF;
    }

    const std::vector<std::string> CEPHandler::split(const std::string& s, char delim) {
        std::vector<std::string> elems;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    double CEPHandler::todouble(const std::string& s) {
        std::stringstream ss(s);
        double item;
        ss >> item;
        return item;
    }

    std::vector<double> CEPHandler::todoubleList(const std::vector<std::string>& s) {
        std::vector<double> result;
        for (std::vector<std::string>::const_iterator i = s.begin(); i != s.end(); ++i) {
            result.push_back(todouble(*i));
        }
        return result;
    }

    std::string CEPHandler::ReadLine(std::ifstream& s) {
        std::string line;
        std::getline(s, line);
        size_t lastNWChar = line.find_last_not_of(" \n\r\t");
        if (lastNWChar != std::string::npos) {
            line.erase(lastNWChar + 1);
        }
        return line;
    }

    const std::string& VEHPHEMLightJSON::VEH::getType() const {
        return privateType;
    }

    void VEHPHEMLightJSON::VEH::setType(const std::string& value) {
        privateType = value;
    }

    const std::string& VEHPHEMLightJSON::VEH::getVersion() const {
        return privateVersion;
    }

    void VEHPHEMLightJSON::VEH::setVersion(const std::string& value) {
        privateVersion = value;
    }

    VEHPHEMLightJSON::Vehicle_Data* VEHPHEMLightJSON::VEH::getVehicleData() {
        return &privateVehicleData;
    }

    VEHPHEMLightJSON::Aux_Data* VEHPHEMLightJSON::VEH::getAuxiliariesData() {
        return &privateAuxiliariesData;
    }

    VEHPHEMLightJSON::Engine_Data* VEHPHEMLightJSON::VEH::getEngineData() {
        return &privateEngineData;
    }

    VEHPHEMLightJSON::Rollres_Data* VEHPHEMLightJSON::VEH::getRollingResData() {
        return &privateRollingResData;
    }

    VEHPHEMLightJSON::FullLoadDrag_Data* VEHPHEMLightJSON::VEH::getFLDData() {
        return &privateFLDData;
    }

    VEHPHEMLightJSON::Transmission_Data* VEHPHEMLightJSON::VEH::getTransmissionData() {
        return &privateTransmissionData;
    }

    const std::string& VEHPHEMLightJSON::Vehicle_Data::getMassType() const {
        return privateMassType;
    }

    void VEHPHEMLightJSON::Vehicle_Data::setMassType(const std::string& value) {
        privateMassType = value;
    }

    const std::string& VEHPHEMLightJSON::Vehicle_Data::getFuelType() const {
        return privateFuelType;
    }

    void VEHPHEMLightJSON::Vehicle_Data::setFuelType(const std::string& value) {
        privateFuelType = value;
    }

    const std::string& VEHPHEMLightJSON::Vehicle_Data::getCalcType() const {
        return privateCalcType;
    }

    void VEHPHEMLightJSON::Vehicle_Data::setCalcType(const std::string& value) {
        privateCalcType = value;
    }

    const double& VEHPHEMLightJSON::Vehicle_Data::getMass() const {
        return privateMass;
    }

    void VEHPHEMLightJSON::Vehicle_Data::setMass(const double& value) {
        privateMass = value;
    }

    const double& VEHPHEMLightJSON::Vehicle_Data::getLoading() const {
        return privateLoading;
    }

    void VEHPHEMLightJSON::Vehicle_Data::setLoading(const double& value) {
        privateLoading = value;
    }

    const double& VEHPHEMLightJSON::Vehicle_Data::getRedMassWheel() const {
        return privateRedMassWheel;
    }

    void VEHPHEMLightJSON::Vehicle_Data::setRedMassWheel(const double& value) {
        privateRedMassWheel = value;
    }

    const double& VEHPHEMLightJSON::Vehicle_Data::getWheelDiameter() const {
        return privateWheelDiameter;
    }

    void VEHPHEMLightJSON::Vehicle_Data::setWheelDiameter(const double& value) {
        privateWheelDiameter = value;
    }

    const double& VEHPHEMLightJSON::Vehicle_Data::getCw() const {
        return privateCw;
    }

    void VEHPHEMLightJSON::Vehicle_Data::setCw(const double& value) {
        privateCw = value;
    }

    const double& VEHPHEMLightJSON::Vehicle_Data::getA() const {
        return privateA;
    }

    void VEHPHEMLightJSON::Vehicle_Data::setA(const double& value) {
        privateA = value;
    }

    const double& VEHPHEMLightJSON::Vehicle_Data::getMileage() const {
        return privateMileage;
    }

    void VEHPHEMLightJSON::Vehicle_Data::setMileage(const double& value) {
        privateMileage = value;
    }

    const double& VEHPHEMLightJSON::Rollres_Data::getFr0() const {
        return privateFr0;
    }

    void VEHPHEMLightJSON::Rollres_Data::setFr0(const double& value) {
        privateFr0 = value;
    }

    const double& VEHPHEMLightJSON::Rollres_Data::getFr1() const {
        return privateFr1;
    }

    void VEHPHEMLightJSON::Rollres_Data::setFr1(const double& value) {
        privateFr1 = value;
    }

    const double& VEHPHEMLightJSON::Rollres_Data::getFr2() const {
        return privateFr2;
    }

    void VEHPHEMLightJSON::Rollres_Data::setFr2(const double& value) {
        privateFr2 = value;
    }

    const double& VEHPHEMLightJSON::Rollres_Data::getFr3() const {
        return privateFr3;
    }

    void VEHPHEMLightJSON::Rollres_Data::setFr3(const double& value) {
        privateFr3 = value;
    }

    const double& VEHPHEMLightJSON::Rollres_Data::getFr4() const {
        return privateFr4;
    }

    void VEHPHEMLightJSON::Rollres_Data::setFr4(const double& value) {
        privateFr4 = value;
    }

    VEHPHEMLightJSON::ICE_Data* VEHPHEMLightJSON::Engine_Data::getICEData() {
        return &privateICEData;
    }

    VEHPHEMLightJSON::EM_Data* VEHPHEMLightJSON::Engine_Data::getEMData() {
        return &privateEMData;
    }

    const double& VEHPHEMLightJSON::ICE_Data::getPrated() const {
        return privatePrated;
    }

    void VEHPHEMLightJSON::ICE_Data::setPrated(const double& value) {
        privatePrated = value;
    }

    const double& VEHPHEMLightJSON::ICE_Data::getnrated() const {
        return privatenrated;
    }

    void VEHPHEMLightJSON::ICE_Data::setnrated(const double& value) {
        privatenrated = value;
    }

    const double& VEHPHEMLightJSON::ICE_Data::getIdling() const {
        return privateIdling;
    }

    void VEHPHEMLightJSON::ICE_Data::setIdling(const double& value) {
        privateIdling = value;
    }

    const double& VEHPHEMLightJSON::EM_Data::getPrated() const {
        return privatePrated;
    }

    void VEHPHEMLightJSON::EM_Data::setPrated(const double& value) {
        privatePrated = value;
    }

    const double& VEHPHEMLightJSON::EM_Data::getnrated() const {
        return privatenrated;
    }

    void VEHPHEMLightJSON::EM_Data::setnrated(const double& value) {
        privatenrated = value;
    }

    const double& VEHPHEMLightJSON::Aux_Data::getPauxnorm() const {
        return privatePauxnorm;
    }

    void VEHPHEMLightJSON::Aux_Data::setPauxnorm(const double& value) {
        privatePauxnorm = value;
    }

    const double& VEHPHEMLightJSON::FullLoadDrag_Data::getP_n_max_v0() const {
        return privateP_n_max_v0;
    }

    void VEHPHEMLightJSON::FullLoadDrag_Data::setP_n_max_v0(const double& value) {
        privateP_n_max_v0 = value;
    }

    const double& VEHPHEMLightJSON::FullLoadDrag_Data::getP_n_max_p0() const {
        return privateP_n_max_p0;
    }

    void VEHPHEMLightJSON::FullLoadDrag_Data::setP_n_max_p0(const double& value) {
        privateP_n_max_p0 = value;
    }

    const double& VEHPHEMLightJSON::FullLoadDrag_Data::getP_n_max_v1() const {
        return privateP_n_max_v1;
    }

    void VEHPHEMLightJSON::FullLoadDrag_Data::setP_n_max_v1(const double& value) {
        privateP_n_max_v1 = value;
    }

    const double& VEHPHEMLightJSON::FullLoadDrag_Data::getP_n_max_p1() const {
        return privateP_n_max_p1;
    }

    void VEHPHEMLightJSON::FullLoadDrag_Data::setP_n_max_p1(const double& value) {
        privateP_n_max_p1 = value;
    }

    std::map<std::string, std::vector<double> >& VEHPHEMLightJSON::FullLoadDrag_Data::getDragCurve() {
        return privateDragCurve;
    }

    void VEHPHEMLightJSON::FullLoadDrag_Data::setDragCurve(const std::map<std::string, std::vector<double> >& value) {
        privateDragCurve = value;
    }

    VEHPHEMLightJSON::FullLoadDrag_Data::FullLoadDrag_Data() {
        setDragCurve(std::map<std::string, std::vector<double> >());
    }

    const double& VEHPHEMLightJSON::Transmission_Data::getAxelRatio() const {
        return privateAxelRatio;
    }

    void VEHPHEMLightJSON::Transmission_Data::setAxelRatio(const double& value) {
        privateAxelRatio = value;
    }

    std::map<std::string, std::vector<double> >& VEHPHEMLightJSON::Transmission_Data::getTransm() {
        return privateTransm;
    }

    void VEHPHEMLightJSON::Transmission_Data::setTransm(const std::map<std::string, std::vector<double> >& value) {
        privateTransm = value;
    }

    VEHPHEMLightJSON::Transmission_Data::Transmission_Data() {
        setTransm(std::map<std::string, std::vector<double> >());
    }
}
