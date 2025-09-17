/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2025 German Aerospace Center (DLR) and others.
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
/// @file    emissionsMap_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 21.08.2013
///
// Main for an emissions map writer
/****************************************************************************/
#include <config.h>

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <string>
#include <ctime>
#include <memory>
#if __cplusplus >= 201703L
#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
#endif
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/emissions/EnergyParams.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileHelpers.h>
#include <utils/iodevices/OutputDevice.h>
#include "VTypesHandler.h"


// ===========================================================================
// functions
// ===========================================================================
void single(const OptionsCont& oc, const std::string& of, const std::string& className, SUMOEmissionClass c,
            double vMin, double vMax, double vStep,
            double aMin, double aMax, double aStep,
            double sMin, double sMax, double sStep,
            bool verbose) {
    if (verbose) {
        WRITE_MESSAGEF(TL("Writing map of '%' into '%'."), className, of);
    }
    std::ofstream o(of.c_str());
    if (!o.good()) {
        throw ProcessError(TLF("Could not open file '%' for writing.", of));
    }

    std::unique_ptr<EnergyParams> energyParams;
    std::map<std::string, SUMOVTypeParameter*> vTypes;
    if (oc.isSet("vtype") || oc.isSet("additional-files")) {
        if (!oc.isSet("additional-files")) {
            throw ProcessError(TL("Option --vtype requires option --additional-files for loading vehicle types"));
        }
        if (!oc.isUsableFileList("additional-files")) {
            throw ProcessError();
        }
        for (const std::string& file : oc.getStringVector("additional-files")) {
            VTypesHandler typesHandler(file, vTypes);
            if (!XMLSubSys::runParser(typesHandler, file)) {
                throw ProcessError(TLF("Loading of % failed.", file));
            }
        }
        if (!oc.isSet("vtype") && vTypes.size() != 1) {
            throw ProcessError(TL("Vehicle type is not unique."));
        }
        const auto vTypeIt = oc.isSet("vtype") ? vTypes.find(oc.getString("vtype")) : vTypes.begin();
        if (vTypeIt == vTypes.end()) {
            throw ProcessError(TLF("Vehicle type '%' is not defined.", oc.getString("vtype")));
        }
        if (oc.isDefault("emission-class")) {
            c = vTypeIt->second->emissionClass;
        }
        energyParams = std::unique_ptr<EnergyParams>(new EnergyParams(vTypeIt->second));
    } else {
        energyParams = std::unique_ptr<EnergyParams>(new EnergyParams());
    }
    for (double v = vMin; v <= vMax; v += vStep) {
        for (double a = aMin; a <= aMax; a += aStep) {
            for (double s = sMin; s <= sMax; s += sStep) {
                const PollutantsInterface::Emissions result = PollutantsInterface::computeAll(c, v, a, s, energyParams.get());
                o << v << ";" << a << ";" << s << ";" << "CO" << ";" << result.CO << std::endl;
                o << v << ";" << a << ";" << s << ";" << "CO2" << ";" << result.CO2 << std::endl;
                o << v << ";" << a << ";" << s << ";" << "HC" << ";" << result.HC << std::endl;
                o << v << ";" << a << ";" << s << ";" << "PMx" << ";" << result.PMx << std::endl;
                o << v << ";" << a << ";" << s << ";" << "NOx" << ";" << result.NOx << std::endl;
                o << v << ";" << a << ";" << s << ";" << "fuel" << ";" << result.fuel << std::endl;
                o << v << ";" << a << ";" << s << ";" << "electricity" << ";" << result.electricity << std::endl;
            }
        }
    }
}




int
main(int argc, char** argv) {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.setApplicationDescription(TL("Builds and writes an emissions map for SUMO's emission models."));
    oc.setApplicationName("emissionsMap", "Eclipse SUMO emissionsMap " VERSION_STRING);
    // add options
    SystemFrame::addConfigurationOptions(oc);
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Processing");
    oc.doRegister("iterate", 'i', new Option_Bool(false));
    oc.addDescription("iterate", "Processing", TL("If set, maps for all available emissions are written."));

    oc.doRegister("emission-class", 'e', new Option_String());
    oc.addDescription("emission-class", "Processing", TL("Defines the name of the emission class to generate the map for."));

    oc.doRegister("additional-files", new Option_FileName());
    oc.addDescription("additional-files", "Input", TL("Load emission parameters (vTypes) from FILE(s)"));

    oc.doRegister("vtype", new Option_String());
    oc.addDescription("vtype", "Input", TL("Defines the vehicle type to use for emission parameters."));

    oc.doRegister("v-min", new Option_Float(0.));
    oc.addDescription("v-min", "Processing", TL("Defines the minimum velocity boundary of the map to generate (in m/s)."));
    oc.doRegister("v-max", new Option_Float(50.));
    oc.addDescription("v-max", "Processing", TL("Defines the maximum velocity boundary of the map to generate (in m/s)."));
    oc.doRegister("v-step", new Option_Float(2.));
    oc.addDescription("v-step", "Processing", TL("Defines the velocity step size (in m/s)."));
    oc.doRegister("a-min", new Option_Float(-4.));
    oc.addDescription("a-min", "Processing", TL("Defines the minimum acceleration boundary of the map to generate (in m/s^2)."));
    oc.doRegister("a-max", new Option_Float(4.));
    oc.addDescription("a-max", "Processing", TL("Defines the maximum acceleration boundary of the map to generate (in m/s^2)."));
    oc.doRegister("a-step", new Option_Float(.5));
    oc.addDescription("a-step", "Processing", TL("Defines the acceleration step size (in m/s^2)."));
    oc.doRegister("s-min", new Option_Float(-10.));
    oc.addDescription("s-min", "Processing", TL("Defines the minimum slope boundary of the map to generate (in deg)."));
    oc.doRegister("s-max", new Option_Float(10.));
    oc.addDescription("s-max", "Processing", TL("Defines the maximum slope boundary of the map to generate (in deg)."));
    oc.doRegister("s-step", new Option_Float(1.));
    oc.addDescription("s-step", "Processing", TL("Defines the slope step size (in deg)."));

    oc.addOptionSubTopic("Output");
    oc.doRegister("output-file", 'o', new Option_String());
    oc.addSynonyme("output", "output-file");
    oc.addDescription("output", "Output", TL("Defines the file (or the path if --iterate was set) to write the map(s) into."));

    oc.addOptionSubTopic("Emissions");
    oc.doRegister("emissions.volumetric-fuel", new Option_Bool(false));
    oc.addDescription("emissions.volumetric-fuel", "Emissions", TL("Return fuel consumption values in (legacy) unit l instead of mg"));

    oc.doRegister("phemlight-path", new Option_FileName(StringVector({ "./PHEMlight/" })));
    oc.addDescription("phemlight-path", "Emissions", TL("Determines where to load PHEMlight definitions from"));

    oc.doRegister("phemlight-year", new Option_Integer(0));
    oc.addDescription("phemlight-year", "Emissions", TL("Enable fleet age modelling with the given reference year in PHEMlight5"));

    oc.doRegister("phemlight-temperature", new Option_Float(INVALID_DOUBLE));
    oc.addDescription("phemlight-temperature", "Emissions", TL("Set ambient temperature to correct NOx emissions in PHEMlight5"));

    SystemFrame::addReportOptions(oc);

    // run
    int ret = 0;
    try {
        // initialise the application system (messaging, xml, options)
        XMLSubSys::init();
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions();
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }

        double vMin = oc.getFloat("v-min");
        double vMax = oc.getFloat("v-max");
        double vStep = oc.getFloat("v-step");
        double aMin = oc.getFloat("a-min");
        double aMax = oc.getFloat("a-max");
        double aStep = oc.getFloat("a-step");
        double sMin = oc.getFloat("s-min");
        double sMax = oc.getFloat("s-max");
        double sStep = oc.getFloat("s-step");
        if (!oc.getBool("iterate")) {
            if (!oc.isSet("emission-class")) {
                throw ProcessError(TL("The emission class (-e) must be given."));
            }
            if (!oc.isSet("output-file")) {
                throw ProcessError(TL("The output file (-o) must be given."));
            }
            const SUMOEmissionClass c = PollutantsInterface::getClassByName(oc.getString("emission-class"));
            single(oc, oc.getString("output-file"), oc.getString("emission-class"),
                   c, vMin, vMax, vStep, aMin, aMax, aStep, sMin, sMax, sStep, oc.getBool("verbose"));
        } else {
            if (!oc.isSet("output-file")) {
                oc.set("output-file", "./");
            }
#if __cplusplus >= 201703L
            std::vector<std::string> phemPath;
            phemPath.push_back(OptionsCont::getOptions().getString("phemlight-path") + "/");
            if (getenv("PHEMLIGHT_PATH") != nullptr) {
                phemPath.push_back(std::string(getenv("PHEMLIGHT_PATH")) + "/");
            }
            if (getenv("SUMO_HOME") != nullptr) {
                phemPath.push_back(std::string(getenv("SUMO_HOME")) + "/data/emissions/PHEMlight/");
                phemPath.push_back(std::string(getenv("SUMO_HOME")) + "/data/emissions/PHEMlight5/");
            }
            for (const std::string& p : phemPath) {
                std::error_code ec;
                for (const auto& entry : fs::directory_iterator(p, ec)) {
                    if (entry.path().extension() == ".veh") {
                        if (entry.path().parent_path().filename().string().back() == '5') {
                            PollutantsInterface::getClassByName("PHEMlight5/" + entry.path().filename().stem().stem().string());
                        } else {
                            PollutantsInterface::getClassByName("PHEMlight/" + entry.path().filename().stem().stem().string());
                        }
                    }
                }
            }
#endif
            const std::vector<SUMOEmissionClass> classes = PollutantsInterface::getAllClasses();
            for (std::vector<SUMOEmissionClass>::const_iterator ci = classes.begin(); ci != classes.end(); ++ci) {
                SUMOEmissionClass c = *ci;
                single(oc, oc.getString("output-file") + PollutantsInterface::getName(c) + ".csv",
                       PollutantsInterface::getName(c),
                       c, vMin, vMax, vStep, aMin, aMax, aStep, sMin, sMax, sStep, oc.getBool("verbose"));
            }
        }
    } catch (InvalidArgument& e) {
        MsgHandler::getErrorInstance()->inform(e.what());
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    } catch (ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    SystemFrame::close();
    if (ret == 0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}


/****************************************************************************/
