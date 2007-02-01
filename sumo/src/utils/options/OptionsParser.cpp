/****************************************************************************/
/// @file    OptionsParser.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id: $
///
// Parses the command line arguments
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
#include "Option.h"
#include "OptionsCont.h"
#include "OptionsParser.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
bool
OptionsParser::parse(OptionsCont *oc, int argc, char **argv)
{
    bool ok = true;
    for (int i=1; i<argc;) {
        try {
            int add;
            // try to set the current option
            if (i<argc-1) {
                add = check(oc, argv[i], argv[i+1], ok);
            } else {
                add = check(oc, argv[i], ok);
            }
            i += add;
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform("On processing option '" + string(argv[i]) + "':\n " + e.msg());
            i++;
            ok = false;
        }
    }
    return ok;
}


int
OptionsParser::check(OptionsCont *oc, char *arg1, bool &ok)
{
    // the last stand-alone argument should be a switch
    if (!checkParameter(arg1)) return 1;
    // check switch
    if (isAbbreviation(arg1)) {
        // set all switches when abbreviated
        for (int i=1; arg1[i]!=0; i++) {
            if (oc->isBool(convert(arg1[i]))) {
                // process boolean switches
                ok &= oc->set(convert(arg1[i]), true);
            } else {
                // process non-boolean switches
                ok &= processNonBooleanSingleSwitch(oc, arg1+i);
            }
        }
    } else {
        string tmp(arg1+2);
        size_t idx1 = tmp.find('=');
        // check whether a parameter was submitted
        if (idx1!=string::npos) {
            ok &= oc->set(tmp.substr(0, idx1), tmp.substr(idx1+1));
        } else {
            ok &= oc->set(convert(arg1+2), true);
        }
    }
    return 1;
}


int
OptionsParser::check(OptionsCont *oc, char *arg1, char *arg2, bool &ok)
{
    // the first argument should be an option
    // (only the second may be a free string)
    if (!checkParameter(arg1)) {
        ok = false;
        return 1;
    }
    // process not abbreviated switches
    if (!isAbbreviation(arg1)) {
        string tmp(arg1+2);
        size_t idx1 = tmp.find('=');
        // check whether a parameter was submitted
        if (idx1!=string::npos) {
            if (!oc->set(tmp.substr(0, idx1), tmp.substr(idx1+1))) {
                ok = false;
            }
            return 1;
        } else {
            if (oc->isBool(convert(arg1+2))) {
                if (!oc->set(convert(arg1+2), true)) {
                    ok = false;
                }
                return 1;
            } else {
                if (!oc->set(convert(arg1+2), convert(arg2))) {
                    ok = false;
                }
                return 2;
            }
            // an error occured
            ok = false;
            return 1;
        }
    }
    // process abbreviated switches
    else {
        // go through the abbreviated switches
        for (int i=1; arg1[i]!=0; i++) {
            // set boolean switches
            if (oc->isBool(convert(arg1[i]))) {
                ok &= oc->set(convert(arg1[i]), true);
                // set non-boolean switches
            } else {
                // check whether the parameter comes directly after the switch
                //  and process if so
                if (arg1[i+1]!=0&&arg1[i+1]!='=') {
                    ok &= processNonBooleanSingleSwitch(oc, arg1+i);
                    // process parameter following after a space
                } else {
                    if (arg1[i+1]=='=') {
                        string val = arg1;
                        val = val.substr(i+2);
                        ok &= oc->set(convert(arg1[i]), val);
                        // option name and attribute were in one argument
                        return 1;
                    } else {
                        ok &= oc->set(convert(arg1[i]), convert(arg2));
                        // option name and attribute were in two arguments
                        return 2;
                    }
                }
            }
        }
        // all switches within the current argument were boolean switches
        return 1;
    }
}


bool
OptionsParser::processNonBooleanSingleSwitch(OptionsCont *oc, char *arg)
{
    if (arg[1]=='=') {
        if (strlen(arg)<3) {
            MsgHandler::getErrorInstance()->inform("Missing value for parameter '" + string(arg).substr(0, 1) + "'.");
            return false;
        } else {
            return oc->set(convert(arg[0]), string(arg+2));
        }
    } else {
        if (strlen(arg)<2) {
            MsgHandler::getErrorInstance()->inform("Missing value for parameter '" + string(arg) + "'.");
            return false;
        } else {
            return oc->set(convert(arg[0]), string(arg+1));
        }
    }
}


bool
OptionsParser::checkParameter(char *arg1)
{
    if (arg1[0]!='-') {
        MsgHandler::getErrorInstance()->inform("The parameter '" + string(arg1) + "' is not allowed in this context");
        MsgHandler::getErrorInstance()->inform("Switch or parameter name expected.");
        return false;
    }
    return true;
}


bool
OptionsParser::isAbbreviation(char *arg1)
{
    return arg1[1]!='-';
}


string
OptionsParser::convert(char *arg)
{
    string s(arg);
    return s;
}


string
OptionsParser::convert(char abbr)
{
    char buf[2];
    buf[0] = abbr;
    buf[1] = 0;
    string s(buf);
    return buf;
}



/****************************************************************************/

