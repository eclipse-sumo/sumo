/****************************************************************************/
/// @file    netconvert_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id: $
///
// }
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <netbuild/NBNetBuilder.h>
#include <netimport/NIOptionsIO.h>
#include <netimport/NILoader.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBDistrictCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBDistribution.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/MsgHandler.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    int ret = 0;
#ifndef _DEBUG
    try {
#endif
        int init_ret = SystemFrame::init(false, argc, argv, NIOptionsIO::fillOptions);
        if (init_ret<0) {
            cout << "SUMO netconvert" << endl;
            cout << " (c) DLR/ZAIK 2000-2007; http://sumo.sourceforge.net" << endl;
            cout << " Version " << VERSION << endl;
            switch (init_ret) {
            case -2:
                OptionsSubSys::getOptions().printHelp(cout);
                break;
            case -4:
                break;
            default:
                cout << " Use --help to get the list of options." << endl;
            }
            SystemFrame::close();
            return 0;
        } else if (init_ret!=0) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        NBNetBuilder nb;
        // initialise the (default) types
        nb.getTypeCont().setDefaults(oc.getInt("lanenumber"), oc.getFloat("speed"), oc.getInt("priority"));
        // load data
        nb.preCheckOptions(oc);
        NILoader nl(nb);
        nl.load(oc);
        if (oc.getBool("dismiss-loading-errors")) {
            MsgHandler::getErrorInstance()->clear();
        }
        // check whether any errors occured
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            throw ProcessError();
        }
        nb.buildLoaded();
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (conversion failed).", false);
        ret = 1;
    }
#endif
    NBDistribution::clear();
    SystemFrame::close();
    // report about ending
    if (ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}



/****************************************************************************/

