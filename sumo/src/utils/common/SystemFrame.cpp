//---------------------------------------------------------------------------//
//                        SystemFrame.cpp -
//  A set of actions common to all applications
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 23.06.2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.4  2003/06/24 08:10:23  dkrajzew
// extended by the options sub system; dcumentation added
//
// Revision 1.3  2003/06/24 08:09:29  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "SystemFrame.h"
#include <string>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/LogFile.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
LogFile *SystemFrame::myLogFile = 0;


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
SystemFrame::init(bool gui, int argc, char **argv,
                    fill_options *fill_f,
                    check_options *check_f,
                    char *help[])
{
    // initialise the output for option processing
    MsgHandler::getErrorInstance()->report2cout(true);
    MsgHandler::getWarningInstance()->report2cout(true);
    MsgHandler::getMessageInstance()->report2cout(true);
    // initialise the xml-subsystem
    if(!XMLSubSys::init()) {
        return false;
    }
    // initialise the options-subsystem
    if(!OptionsSubSys::init(argc, argv, fill_f, check_f, help)) {
        return false;
    }
    // initialise the output
        // check whether it is a gui-version or not, first
    if(gui) {
        // within gui-based applications, nothing is reported to the console
        MsgHandler::getErrorInstance()->report2cout(false);
        MsgHandler::getWarningInstance()->report2cout(false);
        MsgHandler::getMessageInstance()->report2cout(false);
    } else {
        // within console-based applications, report everything to the console
        MsgHandler::getErrorInstance()->report2cout(true);
        MsgHandler::getWarningInstance()->report2cout(true);
        MsgHandler::getMessageInstance()->report2cout(true);
    }
        // then, check whether be verbose
    if(!gui&&!OptionsSubSys::getOptions().getBool("v")) {
        MsgHandler::getMessageInstance()->report2cout(false);
    }
        // check whether to suppress warnings
    if(!gui&&OptionsSubSys::getOptions().getBool("suppress-warnings")) {
        MsgHandler::getWarningInstance()->report2cout(false);
    }
    // build the logger if possible
    if(!gui&&OptionsSubSys::getOptions().isSet("log-file")) {
        myLogFile =
            new LogFile(OptionsSubSys::getOptions().getString("log-file"));
        if(!myLogFile->good()) {
            delete myLogFile;
            myLogFile = 0;
            MsgHandler::getErrorInstance()->inform(
                string("Could not build logging file '")
                + OptionsSubSys::getOptions().getString("log-file")
                + string("'"));
            return false;
        } else {
            MsgHandler::getErrorInstance()->addRetriever(myLogFile);
            MsgHandler::getWarningInstance()->addRetriever(myLogFile);
            MsgHandler::getMessageInstance()->addRetriever(myLogFile);
        }
    }
    // return the state
    return true;
}


void
SystemFrame::close()
{
    delete myLogFile;
    // delete messages
    MsgHandler::cleanupOnEnd();
    // close the xml-subsystem
    XMLSubSys::close();
    // delete build program options
    OptionsSubSys::close();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "SystemFrame.icc"
//#endif

// Local Variables:
// mode:C++
// End:
