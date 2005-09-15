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
#include <config.h>
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
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC       // exclude standard memory alloc procedures
#endif


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
                add = check(oc, argv[i], argv[i+1]);
            } else {
                add = check(oc, argv[i]);
            }
            // move the pointer forward
            if(add>0) {
                i += add;
            } else {
                i += 1;
                ok = false;
            }
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(
                "Error on processing option '" + string(argv[i]) + "':");
            MsgHandler::getErrorInstance()->inform(
                " " + e.msg());
            i++;
            ok = false;
        }
    }
    return ok;
}


int
OptionsParser::check(OptionsCont *oc, char *arg1)
{
    // the last stand-alone argument should be a switch
    if(!checkParameter(arg1)) return -1;
    // check switch
    bool error = false;
    if(isAbbreviation(arg1)) {
        // set all switches when abbreviated
        for(int i=1; arg1[i]!=0; i++) {
            // process boolean switches
            if(oc->isBool(convert(arg1[i]))) {
                error = !oc->set(convert(arg1[i]), true);
            // process non-boolean switches
            } else {
                return processNonBooleanSingleSwitch(oc, arg1+i);
            }
        }
    } else {
        string tmp(arg1+2);
        size_t idx1 = tmp.find('=');
        // check whether a parameter was submitted
        if(idx1!=string::npos) {
            error = !oc->set(tmp.substr(0, idx1), tmp.substr(idx1+1));
        } else {
            error = !oc->set(convert(arg1+2), true);
        }
    }
    if(error) {
        return -1;
    }
    return 1;
}


int
OptionsParser::check(OptionsCont *oc, char *arg1, char *arg2)
{
    // the first argument should be an option
    // (only the second may be a free string)
    if(!checkParameter(arg1)) return -1;
    // process not abbreviated switches
    if(!isAbbreviation(arg1)) {
        string tmp(arg1+2);
        size_t idx1 = tmp.find('=');
        // check whether a parameter was submitted
        if(idx1!=string::npos) {
            if(oc->set(tmp.substr(0, idx1), tmp.substr(idx1+1))) {
                return 1;
            }
            return -1;
        } else {
            if(oc->isBool(convert(arg1+2))) {
                if(oc->set(convert(arg1+2), true))
                    return 1;
            } else {
                if(oc->set(convert(arg1+2), convert(arg2)))
                    return 2;
            }
            // an error occured
            return -1;
        }
    }
    // process abbreviated switches
    else {
        bool error = false;
        // go through the abbreviated switches
        for(int i=1; arg1[i]!=0; i++) {
            // set boolean switches
            if(oc->isBool(convert(arg1[i]))) {
                error = !oc->set(convert(arg1[i]), true);
            // set non-boolean switches
            } else {
                // check whether the parameter comes directly after the switch
                //  and process if so
                if(arg1[i+1]!=0) {
                    return processNonBooleanSingleSwitch(oc, arg1+i);
                // process parameter following after a space
                } else {
                    error = !oc->set(convert(arg1[i]), convert(arg2));
                    // option name and attribute were in two arguments
                    if(!error) {
                        return 2;
                    } else {
                        return -1;
                    }
                }
            }
        }
        // all switches within the current argument were boolean switches
        if(!error) {
            return 1;
        } else {
            return -1;
        }
    }
}


int
OptionsParser::processNonBooleanSingleSwitch(OptionsCont *oc, char *arg)
{
    bool error = false;
    if(arg[1]=='=') {
        error = !oc->set(convert(arg[0]), string(arg+2));
    } else {
        error = !oc->set(convert(arg[0]), string(arg+1));
    }
    if(error) {
        return -1;
    } else {
        return 1;
    }
}


bool
OptionsParser::checkParameter(char *arg1)
{
    if(arg1[0]!='-') {
        MsgHandler::getErrorInstance()->inform(
            "The parameter '" + string(arg1) + "' is not allowed in this context");
        MsgHandler::getErrorInstance()->inform(
            "Switch or parameter name expected.");
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


