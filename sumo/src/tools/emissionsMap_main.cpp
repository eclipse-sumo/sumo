/****************************************************************************/
/// @file    emissionsMap_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 21.08.2013
/// @version $Id$
///
// Main for an emissions map writer
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <utils/common/TplConvert.h>
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// functions
// ===========================================================================


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
void single(const std::string& of, const std::string& className, SUMOEmissionClass c,
            SUMOReal vMin, SUMOReal vMax, SUMOReal vStep,
            SUMOReal aMin, SUMOReal aMax, SUMOReal aStep,
            SUMOReal sMin, SUMOReal sMax, SUMOReal sStep,
            bool verbose) {
    if (verbose) {
        WRITE_MESSAGE("Writing map of '" + className + "' into '" + of + "'.");
    }
    std::ofstream o(of.c_str());
    for (SUMOReal v = vMin; v <= vMax; v += vStep) {
        for (SUMOReal a = aMin; a <= aMax; a += aStep) {
            for (SUMOReal s = sMin; s <= sMax; s += sStep) {
                const PollutantsInterface::Emissions result = PollutantsInterface::computeAll(c, v, a, s);
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
    // build options
    OptionsCont& oc = OptionsCont::getOptions();
    //  give some application descriptions
    oc.setApplicationDescription("Builds and writes an emissions map.");
    oc.setApplicationName("emissionsMap", "SUMO emissionsMap Version " VERSION_STRING);
    //  add options
    SystemFrame::addConfigurationOptions(oc);
    oc.addOptionSubTopic("Processing");
    oc.doRegister("iterate", 'i', new Option_Bool(false));
    oc.addDescription("iterate", "Processing", "If set, maps for all available emissions are written.");

    oc.doRegister("emission-class", 'e', new Option_String());
    oc.addDescription("emission-class", "Processing", "Defines the name of the emission class to generate the map for.");

    oc.doRegister("v-min", new Option_Float(0.));
    oc.addDescription("v-min", "Processing", "Defines the minimum velocity boundary of the map to generate (in m/s).");
    oc.doRegister("v-max", new Option_Float(50.));
    oc.addDescription("v-max", "Processing", "Defines the maximum velocity boundary of the map to generate (in m/s).");
    oc.doRegister("v-step", new Option_Float(2.));
    oc.addDescription("v-step", "Processing", "Defines the velocity step size (in m/s).");
    oc.doRegister("a-min", new Option_Float(-4.));
    oc.addDescription("a-min", "Processing", "Defines the minimum acceleration boundary of the map to generate (in m/s^2).");
    oc.doRegister("a-max", new Option_Float(4.));
    oc.addDescription("a-max", "Processing", "Defines the maximum acceleration boundary of the map to generate (in m/s^2).");
    oc.doRegister("a-step", new Option_Float(.5));
    oc.addDescription("a-step", "Processing", "Defines the acceleration step size (in m/s^2).");
    oc.doRegister("s-min", new Option_Float(-10.));
    oc.addDescription("s-min", "Processing", "Defines the minimum slope boundary of the map to generate (in deg).");
    oc.doRegister("s-max", new Option_Float(10.));
    oc.addDescription("s-max", "Processing", "Defines the maximum slope boundary of the map to generate (in deg).");
    oc.doRegister("s-step", new Option_Float(1.));
    oc.addDescription("s-step", "Processing", "Defines the slope step size (in deg).");

    oc.addOptionSubTopic("Output");
    oc.doRegister("output-file", 'o', new Option_String());
    oc.addSynonyme("output", "output-file");
    oc.addDescription("output", "Output", "Defines the file (or the path if --iterate was set) to write the map(s) into.");

    oc.addOptionSubTopic("Emissions");
    oc.doRegister("phemlight-path", new Option_FileName("./PHEMlight/"));
    oc.addDescription("phemlight-path", "Emissions", "Determines where to load PHEMlight definitions from.");

    SystemFrame::addReportOptions(oc);

    // run
    int ret = 0;
    try {
        // initialise the application system (messaging, xml, options)
        XMLSubSys::init();
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions();
        OptionsCont& oc = OptionsCont::getOptions();
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }

        SUMOReal vMin = oc.getFloat("v-min");
        SUMOReal vMax = oc.getFloat("v-max");
        SUMOReal vStep = oc.getFloat("v-step");
        SUMOReal aMin = oc.getFloat("a-min");
        SUMOReal aMax = oc.getFloat("a-max");
        SUMOReal aStep = oc.getFloat("a-step");
        SUMOReal sMin = oc.getFloat("s-min");
        SUMOReal sMax = oc.getFloat("s-max");
        SUMOReal sStep = oc.getFloat("s-step");
        if (!oc.getBool("iterate")) {
            if (!oc.isSet("emission-class")) {
                throw ProcessError("The emission class (-e) must be given.");
            }
            if (!oc.isSet("output-file")) {
                throw ProcessError("The output file (-o) must be given.");
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

