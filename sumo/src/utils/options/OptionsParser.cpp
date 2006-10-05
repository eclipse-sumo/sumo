/***************************************************************************
                          OptionsParser.cpp
              Parses the command line arguments
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.11  2006/10/05 09:47:40  dkrajzew
// debugging options parsing
//
// Revision 1.10  2006/08/01 07:38:46  dkrajzew
// revalidation of options messaging
//
// Revision 1.9  2006/05/16 08:15:08  dkrajzew
// catching empty parameters patched
//
// Revision 1.8  2006/01/16 13:38:23  dkrajzew
// help and error handling patched
//
// Revision 1.7  2005/10/17 09:25:12  dkrajzew
// got rid of the old MSVC memory leak checker
//
// Revision 1.6  2005/10/07 11:46:56  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 12:21:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2004/11/23 10:36:02  dkrajzew
// debugging
//
// Revision 1.3  2004/07/02 09:41:39  dkrajzew
// debugging the repeated setting of a value
//
// Revision 1.2  2003/02/07 10:51:59  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:58:18  dkrajzew
// initial release for utilities that handle program options
//
// Revision 1.5  2002/07/31 17:30:07  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.7  2002/07/18 07:37:16  dkrajzew
// long option names may now have parameter separated by a =, too
//
// Revision 1.5  2002/07/11 07:42:59  dkrajzew
// Usage of relative pathnames within configuration files implemented
//
// Revision 1.5  2002/06/11 15:58:26  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/10 06:54:30  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized; options now recognize false numerical input
//
// Revision 1.3  2002/05/14 04:45:50  dkrajzew
// Bresenham added; some minor changes; windows eol removed
//
// Revision 1.2  2002/04/26 10:08:39  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:01  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:20:38  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:59:38  dkrajzew
// Memory leak tracing added; ostrstreams replaces by ostringstreams
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <iostream>
#include "Option.h"
#include "OptionsCont.h"
#include "OptionsParser.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
OptionsParser::parse(OptionsCont *oc, int argc, char **argv)
{
    bool ok = true;
    for(int i=1; i<argc; ) {
        try {
            int add;
            // try to set the current option
            if(i<argc-1) {
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
    if(!checkParameter(arg1)) return 1;
    // check switch
    if(isAbbreviation(arg1)) {
        // set all switches when abbreviated
        for(int i=1; arg1[i]!=0; i++) {
            if(oc->isBool(convert(arg1[i]))) {
                // process boolean switches
                ok &= oc->set(convert(arg1[i]), true);
            } else {
                // process non-boolean switches
                return processNonBooleanSingleSwitch(oc, arg1+i);
            }
        }
    } else {
        string tmp(arg1+2);
        size_t idx1 = tmp.find('=');
        // check whether a parameter was submitted
        if(idx1!=string::npos) {
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
    if(!checkParameter(arg1)) {
        ok = false;
        return 1;
    }
    // process not abbreviated switches
    if(!isAbbreviation(arg1)) {
        string tmp(arg1+2);
        size_t idx1 = tmp.find('=');
        // check whether a parameter was submitted
        if(idx1!=string::npos) {
            if(!oc->set(tmp.substr(0, idx1), tmp.substr(idx1+1))) {
                ok = false;
            }
            return 1;
        } else {
            if(oc->isBool(convert(arg1+2))) {
                if(!oc->set(convert(arg1+2), true)) {
                    ok = false;
                }
                return 1;
            } else {
                if(!oc->set(convert(arg1+2), convert(arg2))) {
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
        for(int i=1; arg1[i]!=0; i++) {
            // set boolean switches
            if(oc->isBool(convert(arg1[i]))) {
                ok &= oc->set(convert(arg1[i]), true);
            // set non-boolean switches
            } else {
                // check whether the parameter comes directly after the switch
                //  and process if so
                if(arg1[i+1]!=0) {
                    ok &= processNonBooleanSingleSwitch(oc, arg1+i);
                // process parameter following after a space
                } else {
                    ok &= oc->set(convert(arg1[i]), convert(arg2));
                    // option name and attribute were in two arguments
                    return 2;
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
    if(arg[1]=='=') {
        if(strlen(arg)<3) {
            MsgHandler::getErrorInstance()->inform("Missing value for parameter '" + string(arg).substr(0, 1) + "'.");
            return false;
        } else {
            return oc->set(convert(arg[0]), string(arg+2));
        }
    } else {
        if(strlen(arg)<2) {
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
    if(arg1[0]!='-') {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


