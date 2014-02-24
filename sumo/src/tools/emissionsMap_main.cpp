/****************************************************************************/
/// @file    emissionsMap_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 21.08.2013
/// @version $Id$
///
// Main for an emissions map writer
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
void single(const std::string &of, const std::string &className, SUMOEmissionClass c, 
            SUMOReal vMin, SUMOReal vMax, SUMOReal vStep,
            SUMOReal aMin, SUMOReal aMax, SUMOReal aStep,
            SUMOReal sMin, SUMOReal sMax, SUMOReal sStep,
            bool verbose) {
            if(verbose) {
                WRITE_MESSAGE("Writing map of '" + className + "' into '" + of + "'.");
            }
		std::ofstream o(of.c_str());
		for(SUMOReal v=vMin; v<=vMax; v+=vStep) {
			for(SUMOReal a=aMin; a<=aMax; a+=aStep) {
    			for(SUMOReal s=sMin; s<=sMax; s+=sStep) {
                    o << v << ";" << a << ";" << s << ";" << "CO" << ";" << PollutantsInterface::computeCO(c, v, a, s) << std::endl;
                    o << v << ";" << a << ";" << s << ";" << "CO2" << ";" << PollutantsInterface::computeCO2(c, v, a, s) << std::endl;
                    o << v << ";" << a << ";" << s << ";" << "HC" << ";" << PollutantsInterface::computeHC(c, v, a, s) << std::endl;
                    o << v << ";" << a << ";" << s << ";" << "PMx" << ";" << PollutantsInterface::computePMx(c, v, a, s) << std::endl;
                    o << v << ";" << a << ";" << s << ";" << "NOx" << ";" << PollutantsInterface::computeNOx(c, v, a, s) << std::endl;
                    o << v << ";" << a << ";" << s << ";" << "fuel" << ";" << PollutantsInterface::computeFuel(c, v, a, s) << std::endl;
                }
			}
		}
}




int
main(int argc, char **argv) {
    // build options
    OptionsCont &oc = OptionsCont::getOptions();
    //  give some application descriptions
    oc.setApplicationDescription("Builds and writes an emissions map.");
    oc.setApplicationName("emissionsMap", "SUMO emissionsMap Version " + (std::string)VERSION_STRING);
    //  add options
    oc.addOptionSubTopic("Processing");
    oc.doRegister("iterate", 'i', new Option_Bool(false));
    oc.addDescription("iterate", "Processing", "If set, maps for all available emissions are written.");

    oc.doRegister("emission-class", 'e', new Option_String());
    oc.addDescription("emission-class", "Processing", "Defines the name of the emission class to generate the map for.");

    oc.doRegister("v-min", new Option_Float(0.));
    oc.addDescription("v-min", "Processing", "Defines the minimum velocity boundary of the map to generate (in [m/s]).");
    oc.doRegister("v-max", new Option_Float(50.));
    oc.addDescription("v-max", "Processing", "Defines the maximum velocity boundary of the map to generate (in [m/s]).");
    oc.doRegister("v-step", new Option_Float(2.));
    oc.addDescription("v-step", "Processing", "Defines the velocity step size (in [m/s]).");
    oc.doRegister("a-min", new Option_Float(-4.));
    oc.addDescription("a-min", "Processing", "Defines the minimum acceleration boundary of the map to generate (in [m/s^2]).");
    oc.doRegister("a-max", new Option_Float(4.));
    oc.addDescription("a-max", "Processing", "Defines the maximum acceleration boundary of the map to generate (in [m/s^2]).");
    oc.doRegister("a-step", new Option_Float(.5));
    oc.addDescription("a-step", "Processing", "Defines the acceleration step size (in [m/s^2]).");
    oc.doRegister("s-min", new Option_Float(-10.));
    oc.addDescription("s-min", "Processing", "Defines the minimum slope boundary of the map to generate (in [°]).");
    oc.doRegister("s-max", new Option_Float(10.));
    oc.addDescription("s-max", "Processing", "Defines the maximum slope boundary of the map to generate (in [°]).");
    oc.doRegister("s-step", new Option_Float(1.));
    oc.addDescription("s-step", "Processing", "Defines the slope step size (in [°]).");

    oc.addOptionSubTopic("Output");
    oc.doRegister("output-file", 'o', new Option_String());
    oc.addSynonyme("output", "output-file");
    oc.addDescription("emission-class", "Output", "Defines the file (or the path if --iterate was set) to write the map(s) into.");

    oc.addOptionSubTopic("Emissions");
    oc.doRegister("phemlight-path", 'p', new Option_FileName("./PHEMlight/"));
    oc.addDescription("phemlight-path", "Emissions", "Determines where to load PHEMlight definitions from.");

    oc.addOptionSubTopic("Report");
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.addDescription("verbose", "Report", "Switches to verbose output.");

    // run
    int ret = 0;
    try {
        // initialise the application system (messaging, xml, options)
        XMLSubSys::init();
        OptionsIO::getOptions(true, argc, argv);
        OptionsCont &oc = OptionsCont::getOptions();


        SUMOReal vMin = oc.getFloat("v-min");
		SUMOReal vMax = oc.getFloat("v-max");
		SUMOReal vStep = oc.getFloat("v-step");
		SUMOReal aMin = oc.getFloat("a-min");
		SUMOReal aMax = oc.getFloat("a-max");
		SUMOReal aStep = oc.getFloat("a-step");
		SUMOReal sMin = oc.getFloat("s-min");
		SUMOReal sMax = oc.getFloat("s-max");
		SUMOReal sStep = oc.getFloat("s-step");
        if(!oc.getBool("iterate")) {
            if(!oc.isSet("emission-class")) {
                throw ProcessError("The emission class (-e) must be given.");
            }
            if(!oc.isSet("output-file")) {
                throw ProcessError("The output file (-o) must be given.");
            }
    		SUMOEmissionClass c = getVehicleEmissionTypeID(oc.getString("emission-class"));
            single(oc.getString("output-file"), oc.getString("emission-class"), 
                c, vMin, vMax, vStep, aMin, aMax, aStep, sMin, sMax, sStep, oc.getBool("verbose"));
        } else {
            if(!oc.isSet("output-file")) {
                oc.set("output-file", "./");
            }
            // let's assume it's an old, plain enum
            for(int ci=SVE_UNKNOWN; ci!=SVE_META_PHEMLIGHT_END; ++ci) {
                SUMOEmissionClass c = (SUMOEmissionClass) ci;
                if (SumoEmissionClassStrings.has(c)) {
                    single(oc.getString("output-file")+getVehicleEmissionTypeName(c)+".csv", getVehicleEmissionTypeName(c), 
                        c, vMin, vMax, vStep, aMin, aMax, aStep, sMin, sMax, sStep, oc.getBool("verbose"));
                }
            }
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    } catch (ProcessError &e) {
        if (std::string(e.what())!=std::string("Process Error") && std::string(e.what())!=std::string("")) {
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
    if (ret==0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}



/****************************************************************************/

