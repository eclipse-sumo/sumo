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
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/importio/LineReader.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// functions
// ===========================================================================


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv) {
    // build options
    OptionsCont &oc = OptionsCont::getOptions();
    //  give some application descriptions
    oc.setApplicationDescription("Computes emission by driving a time line.");
    oc.setApplicationName("emissionsTimeline", "SUMO emissionsTimeline Version " + (std::string)VERSION_STRING);
    //  add options

    oc.addOptionSubTopic("Input");
    oc.doRegister("timeline-file", 't', new Option_FileName());
    oc.addSynonyme("timeline", "timeline-file");
    oc.addDescription("timeline-file", "Input", "Defines the file to read the driving cycle from.");

    oc.doRegister("emission-class", 'e', new Option_String());
    oc.addDescription("emission-class", "Input", "Defines for which emission class the emissions shall be generated. ");
    

    oc.addOptionSubTopic("Processing");
    oc.doRegister("compute-a", 'a', new Option_Bool(false));
    oc.addDescription("compute-a", "Processing", "If set, the acceleration is computed instead of being read from the file. ");

    oc.doRegister("skip-first", 's', new Option_Bool(false));
    oc.addDescription("skip-first", "Processing", "If set, the first line of the read file is skipped.");

    oc.doRegister("kmh", new Option_Bool(false));
    oc.addDescription("kmh", "Processing", "If set, the given speed is interpreted as being given in km/h.");

    oc.doRegister("have-slope", new Option_Bool(false));
    oc.addDescription("have-slope", "Processing", "If set, the fourth column is read and used as slope (in [°]).");

    oc.doRegister("slope", new Option_Float(0));
    oc.addDescription("slope", "Processing", "Sets a global slope (in [°]) that is used if the file does not contain slope information.");

    oc.addOptionSubTopic("Output");
    oc.doRegister("output-file", 'o', new Option_String());
    oc.addSynonyme("output", "output-file");
    oc.addDescription("emission-class", "Output", "Defines the file to write the emission cycle results into. ");

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

        if(!oc.isSet("timeline-file")) {
            throw ProcessError("The timeline file must be given.");
        }
        if(!oc.isSet("output-file")) {
            throw ProcessError("The output file must be given.");
        }

        bool skipFirst = oc.getBool("skip-first");
        bool computeA = oc.getBool("compute-a");
        bool inKMH = oc.getBool("kmh");
        bool haveSlope = oc.getBool("have-slope");

        SUMOReal sumCO, sumCO2, sumHC, sumNOx, sumPMx, sumFuel;
        sumCO = sumCO2 = sumHC = sumNOx = sumPMx = sumFuel = 0;
        SUMOReal l = 0;


        SUMOEmissionClass c = getVehicleEmissionTypeID(oc.getString("emission-class"));
        std::ofstream o(oc.getString("output-file").c_str());
        LineReader lr(oc.getString("timeline-file"));
        SUMOReal lastV = 0;
        while(lr.hasMore()) {
            std::string line = lr.readLine();
            if(skipFirst) {
                skipFirst = false;
                continue;
            }
            StringTokenizer st(StringUtils::prune(line), ";");
            try {
                SUMOReal t = TplConvert::_2SUMOReal<char>(st.next().c_str());
                SUMOReal v = TplConvert::_2SUMOReal<char>(st.next().c_str());
                if(inKMH) {
                    v = v / 3.6;
                }
                l += v;
                SUMOReal a = 0;
                if(!computeA) {
                    a = TplConvert::_2SUMOReal<char>(st.next().c_str());
                } else {
                    a = v - lastV;
                }
                lastV = v;
                SUMOReal s = oc.getFloat("slope");
                if(haveSlope) {
                    s = TplConvert::_2SUMOReal<char>(st.next().c_str());
                }

                SUMOReal aCO = PollutantsInterface::computeCO(c, v, a, s);
                SUMOReal aCO2 = PollutantsInterface::computeCO2(c, v, a, s);
                SUMOReal aHC = PollutantsInterface::computeHC(c, v, a, s);
                SUMOReal aNOx = PollutantsInterface::computeNOx(c, v, a, s);
                SUMOReal aPMx = PollutantsInterface::computePMx(c, v, a, s);
                SUMOReal aFuel = PollutantsInterface::computeFuel(c, v, a, s);
                sumCO += aCO;
                sumCO2 += aCO2;
                sumHC += aHC;
                sumNOx += aNOx;
                sumPMx += aPMx;
                sumFuel += aFuel;
                o << t << ";" << v << ";" << a << ";" << s 
                    << ";" << aCO << ";" << aCO2 << ";" << aHC << ";" << aPMx << ";" << aNOx << ";" << aFuel << std::endl;
            } catch (EmptyData &) {
                throw ProcessError("Missing an entry in line '" + line + "'.");
            } catch (NumberFormatException &) {
                throw ProcessError("Not numeric entry in line '" + line + "'.");
            }
        }
        std::cout << "sums"  << std::endl
            << "length:" << l << std::endl
            << "CO:" << sumCO << std::endl
            << "CO2:" << sumCO2 << std::endl
            << "HC:" << sumHC << std::endl
            << "NOx:" << sumNOx << std::endl
            << "PMx:" << sumPMx << std::endl 
            << "fuel:" << sumFuel << std::endl;
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

