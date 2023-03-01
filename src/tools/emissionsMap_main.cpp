/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
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

#include <utils/common/StringUtils.h>
#include <iostream>
#include <string>
#include <ctime>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileHelpers.h>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// functions
// ===========================================================================


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
void single(const std::string& of, const std::string& className, SUMOEmissionClass c,
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
    for (double v = vMin; v <= vMax; v += vStep) {
        for (double a = aMin; a <= aMax; a += aStep) {
            for (double s = sMin; s <= sMax; s += sStep) {
                const PollutantsInterface::Emissions result = PollutantsInterface::computeAll(c, v, a, s, nullptr);
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
    oc.setApplicationName("emissionsMap", "Eclipse SUMO emissionsMap Version " VERSION_STRING);
    // add options
    SystemFrame::addConfigurationOptions(oc);
    oc.addOptionSubTopic("Processing");
    oc.doRegister("iterate", 'i', new Option_Bool(false));
    oc.addDescription("iterate", "Processing", TL("If set, maps for all available emissions are written."));

    oc.doRegister("emission-class", 'e', new Option_String());
    oc.addDescription("emission-class", "Processing", TL("Defines the name of the emission class to generate the map for."));

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
            single(oc.getString("output-file"), oc.getString("emission-class"),
                   c, vMin, vMax, vStep, aMin, aMax, aStep, sMin, sMax, sStep, oc.getBool("verbose"));
        } else {
            if (!oc.isSet("output-file")) {
                oc.set("output-file", "./");
            }
            const std::vector<SUMOEmissionClass> classes = PollutantsInterface::getAllClasses();
            for (std::vector<SUMOEmissionClass>::const_iterator ci = classes.begin(); ci != classes.end(); ++ci) {
                SUMOEmissionClass c = *ci;
                single(oc.getString("output-file") + PollutantsInterface::getName(c) + ".csv", PollutantsInterface::getName(c),
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
