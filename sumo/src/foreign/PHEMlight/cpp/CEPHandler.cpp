#include "CEPHandler.h"
#include "Helpers.h"
#include "Constants.h"
#include "CEP.h"


namespace PHEMlightdll {

    CEPHandler::CEPHandler() {
        _ceps = std::map<std::string, CEP*>();
    }

    std::map<std::string, CEP*> CEPHandler::getCEPS() const {
        return _ceps;
    }

    const std::map<std::string, std::map<std::string, double> >& CEPHandler::getFleetShares() const {
        return _fleetShares;
    }

    bool CEPHandler::GetCEP(const std::string& DataPath, Helpers* Helper, CEP*& Data) {
        if (getCEPS().find(Helper->getgClass()) == getCEPS().end()) {
            if (!Load(DataPath, Helper)) {
//C# TO C++ CONVERTER WARNING: C# to C++ Converter converted the original 'null' assignment to a call to 'delete', but you should review memory allocation of all pointer variables in the converted code:
                delete Data;
                return false;
            }
        }
        Data = getCEPS()[Helper->getgClass()];
        return true;
    }

    bool CEPHandler::Load(const std::string& DataPath, Helpers* Helper) {
        //Deklaration
        // get string identifier for PHEM emission class
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        std::string emissionRep = Helper->getgClass();

        // to hold everything.
        std::vector<std::vector<double> > matrixSpeedInertiaTable;
        std::vector<std::vector<double> > normedTragTableSpeedInertiaTable;
        std::vector<std::vector<double> > matrixFC;
        std::vector<std::vector<double> > matrixPollutants;
        std::vector<double> idlingValuesFC;
        std::vector<double> idlingValuesPollutants;
        std::vector<std::string> headerFC;
        std::vector<std::string> headerPollutants;

        double vehicleMass;
        double vehicleLoading;
        double vehicleMassRot;
        double crosssectionalArea;
        double cwValue;
        double f0;
        double f1;
        double f2;
        double f3;
        double f4;
        double axleRatio;
        std::vector<double> transmissionGearRatios;
        double auxPower;
        double ratedPower;
        double engineIdlingSpeed;
        double engineRatedSpeed;
        double effectiveWhellDiameter;
        std::string vehicleMassType;
        std::string vehicleFuelType;
        double pNormV0;
        double pNormP0;
        double pNormV1;
        double pNormP1;

        if (!ReadVehicleFile(DataPath, emissionRep, Helper, vehicleMass, vehicleLoading, vehicleMassRot, crosssectionalArea, cwValue, f0, f1, f2, f3, f4, axleRatio, auxPower, ratedPower, engineIdlingSpeed, engineRatedSpeed, effectiveWhellDiameter, transmissionGearRatios, vehicleMassType, vehicleFuelType, pNormV0, pNormP0, pNormV1, pNormP1, matrixSpeedInertiaTable, normedTragTableSpeedInertiaTable)) {
            return false;
        }

        if (!ReadEmissionData(true, DataPath, emissionRep, Helper, headerFC, matrixFC, idlingValuesFC)) {
            return false;
        }

        if (!ReadEmissionData(false, DataPath, emissionRep, Helper, headerPollutants, matrixPollutants, idlingValuesPollutants)) {
            return false;
        }

        _ceps.insert(std::make_pair(Helper->getgClass(), new CEP(vehicleMassType == Constants::HeavyVehicle, vehicleMass, vehicleLoading, vehicleMassRot, crosssectionalArea, cwValue, f0, f1, f2, f3, f4, axleRatio, transmissionGearRatios, auxPower, ratedPower, engineIdlingSpeed, engineRatedSpeed, effectiveWhellDiameter, pNormV0, pNormP0, pNormV1, pNormP1, vehicleFuelType, matrixFC, headerPollutants, matrixPollutants, matrixSpeedInertiaTable, normedTragTableSpeedInertiaTable, idlingValuesFC.front(), idlingValuesPollutants)));

        return true;
    }

    bool CEPHandler::ReadVehicleFile(const std::string& DataPath, const std::string& emissionClass, Helpers* Helper, double& vehicleMass, double& vehicleLoading, double& vehicleMassRot, double& crossArea, double& cWValue, double& f0, double& f1, double& f2, double& f3, double& f4, double& axleRatio, double& auxPower, double& ratedPower, double& engineIdlingSpeed, double& engineRatedSpeed, double& effectiveWheelDiameter, std::vector<double>& transmissionGearRatios, std::string& vehicleMassType, std::string& vehicleFuelType, double& pNormV0, double& pNormP0, double& pNormV1, double& pNormP1, std::vector<std::vector<double> >& matrixSpeedInertiaTable, std::vector<std::vector<double> >& normedDragTable) {
        vehicleMass = 0;
        vehicleLoading = 0;
        vehicleMassRot = 0;
        crossArea = 0;
        cWValue = 0;
        f0 = 0;
        f1 = 0;
        f2 = 0;
        f3 = 0;
        f4 = 0;
        axleRatio = 0;
        ratedPower = 0;
        auxPower = 0;
        engineIdlingSpeed = 0;
        engineRatedSpeed = 0;
        effectiveWheelDiameter = 0;
        vehicleMassType = "";
        vehicleFuelType = "";
        pNormV0 = 0;
        pNormP0 = 0;
        pNormV1 = 0;
        pNormP1 = 0;
        transmissionGearRatios = std::vector<double>();
        matrixSpeedInertiaTable = std::vector<std::vector<double> >();
        normedDragTable = std::vector<std::vector<double> >();
        std::string line;
        std::string cell;
        int dataCount = 0;

        //Open file
        std::string path = DataPath + std::string("\\") + emissionClass + std::string(".PHEMLight.veh");
        if (!File::Exists(path)) {
            Helper->setErrMsg(std::string("File do not exist! (") + path + std::string(")"));
            return false;
        }
        StreamReader* vehicleReader = File::OpenText(path);

        // skip header
        ReadLine(vehicleReader);

        while ((line = ReadLine(vehicleReader)) != "" && dataCount <= 49) {
            if (line.substr(0, 1) == Helper->getCommentPrefix()) {
                continue;
            }
            else {
                dataCount++;
            }

            cell = split(line, ',')[0];

            // reading Mass
            if (dataCount == 1) {
                vehicleMass = todouble(cell);
            }

            // reading vehicle loading
            if (dataCount == 2) {
                vehicleLoading = todouble(cell);
            }

            // reading cWValue
            if (dataCount == 3) {
                cWValue = todouble(cell);
            }

            // reading crossectional area
            if (dataCount == 4) {
                crossArea = todouble(cell);
            }

            // reading vehicle mass rotational
            if (dataCount == 7) {
                vehicleMassRot = todouble(cell);
            }

            // reading rated power
            if (dataCount == 9) {
                auxPower = todouble(cell);
            }

            // reading rated power
            if (dataCount == 10) {
                ratedPower = todouble(cell);
            }

            // reading engine rated speed
            if (dataCount == 11) {
                engineRatedSpeed = todouble(cell);
            }

            // reading engine idling speed
            if (dataCount == 12) {
                engineIdlingSpeed = todouble(cell);
            }

            // reading f0
            if (dataCount == 14) {
                f0 = todouble(cell);
            }

            // reading f1
            if (dataCount == 15) {
                f1 = todouble(cell);
            }

            // reading f2
            if (dataCount == 16) {
                f2 = todouble(cell);
            }

            // reading f3
            if (dataCount == 17) {
                f3 = todouble(cell);
            }

            // reading f4
            if (dataCount == 18) {
                f4 = todouble(cell);
            }

            // reading axleRatio
            if (dataCount == 21) {
                axleRatio = todouble(cell);
            }

            // reading effective wheel diameter
            if (dataCount == 22) {
                effectiveWheelDiameter = todouble(cell);
            }

            if (dataCount >= 23 && dataCount <= 40) {
                transmissionGearRatios.push_back(todouble(cell));
            }

            // reading vehicleMassType
            if (dataCount == 45) {
                vehicleMassType = cell;
            }

            // reading vehicleFuelType
            if (dataCount == 46) {
                vehicleFuelType = cell;
            }

            // reading pNormV0
            if (dataCount == 47) {
                pNormV0 = todouble(cell);
            }

            // reading pNormP0
            if (dataCount == 48) {
                pNormP0 = todouble(cell);
            }

            // reading pNormV1
            if (dataCount == 49) {
                pNormV1 = todouble(cell);
            }

            // reading pNormP1
            if (dataCount == 50) {
                pNormP1 = todouble(cell);
            }
        }

        while ((line = ReadLine(vehicleReader)) != "" && line.substr(0, 1) != Helper->getCommentPrefix()) {
            if (line.substr(0, 1) == Helper->getCommentPrefix()) {
                continue;
            }

            std::vector<double>& entries = todoubleList(line, ',');
            matrixSpeedInertiaTable.push_back(entries);
        }

        while ((line = ReadLine(vehicleReader)) != "") {
            if (line.substr(0, 1) == Helper->getCommentPrefix()) {
                continue;
            }

            std::vector<double>& entries = todoubleList(line, ',');
            normedDragTable.push_back(entries);
        }

        vehicleReader->Close();
        return true;
    }

    bool CEPHandler::ReadEmissionData(bool readFC, const std::string& DataPath, const std::string& emissionClass, Helpers* Helper, std::vector<std::string>& header, std::vector<std::vector<double> >& matrix, std::vector<double>& idlingValues) {
        // declare file stream
        std::string line;
        header = std::vector<std::string>();
        matrix = std::vector<std::vector<double> >();
        idlingValues = std::vector<double>();

        std::string pollutantExtension = "";
        if (readFC) {
            pollutantExtension += std::string("_FC");
        }

        std::string path = DataPath + std::string("\\") + emissionClass + pollutantExtension + std::string(".csv");
        if (!File::Exists(path)) {
            Helper->setErrMsg(std::string("File do not exist! (") + path + std::string(")"));
            return false;
        }
        StreamReader* fileReader = File::OpenText(path);

        // read header line for pollutant identifiers
        if ((line = ReadLine(fileReader)) != "") {
            std::vector<std::string> entries = split(line, ',');
            // skip first entry "Pe"
            for (int i = 1; i < entries.size(); i++) {
                header.push_back(entries[i]);
            }
        }

        // skip units
        ReadLine(fileReader);

        // skip comment
        ReadLine(fileReader);

        //readIdlingValues
        line = ReadLine(fileReader);

        std::vector<std::string> stringIdlings = split(line, ',').ToList();
        stringIdlings.RemoveAt(0);

        idlingValues = stringIdlings.Select([&] (void* p) {
            todouble(p);
        }).Cast<double>().ToList();

        while ((line = ReadLine(fileReader)) != "") {
            std::vector<double>& vi = todoubleList(line, ',');
            matrix.push_back(vi);
        }
        fileReader->Close();
        return true;
    }

    bool CEPHandler::ReadFleetShares(const std::string& DataPath, Helpers* Helper) {
        //Declaration
        std::string line;
        std::string path = DataPath + std::string("\\FleetShares.csv");
        if (!File::Exists(path)) {
            Helper->setErrMsg(std::string("FleetShares file does not exist! (") + path + std::string(")"));
            return false;
        }
        StreamReader* shareReader = File::OpenText(path);

        _fleetShares = std::map<std::string, std::map<std::string, double> >();

        while ((line = ReadLine(shareReader)) != "") {
            if (line.substr(0, 1) == Helper->getCommentPrefix()) {
                continue;
            }

            std::vector<std::string> splitLine = split(line, ',');
            std::string aggregateClass = splitLine[0];

            if (FleetShares.find(aggregateClass) == getFleetShares().end()) {
                getFleetShares().insert(std::make_pair(aggregateClass, std::map<std::string, double>()));
            }

            std::string subClass = splitLine[1];

            if (FleetShares[aggregateClass].find(subClass) == getFleetShares()[aggregateClass].end()) {
                getFleetShares()[aggregateClass].insert(std::make_pair(subClass, todouble(splitLine[2])));
            }
        }
        return true;
    }

    std::vector<std::string> CEPHandler::split(const std::string& s, char delim) {
//C# TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to this .NET String method:
        return s.Split(delim);
    }

    double CEPHandler::todouble(const std::string& s) {
        return static_cast<double>(s, CultureInfo::InvariantCulture);
    }

    std::vector<double> CEPHandler::todoubleList(const std::string& s, char delim) {
        return split(s, delim).Select([&] (void* p) {
            todouble(p);
        }).Cast<double>().ToList();
    }

    std::string CEPHandler::ReadLine(StreamReader* s) {
        return s->ReadLine();
    }
}
