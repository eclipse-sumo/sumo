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
// Revision 1.3  2002/06/11 13:43:35  dkrajzew
// Windows eol removed
//
// Revision 1.2  2002/06/10 08:33:23  dkrajzew
// Parsing of strings into other data formats generelized; Options now recognize false numeric values; documentation added
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include "Option.h"
#include "OptionsCont.h"
#include "OptionsParser.h"
#include "UtilExceptions.h"

/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
bool OptionsParser::parse(OptionsCont *oc, int argc, char **argv) {
    bool ok = true;
    for(int i=1; i<argc; ) {
        try {
            int add;
            if(i<argc-1)
                add = check(oc, argv[i], argv[i+1]);
            else
                add = check(oc, argv[i]);
            if(add>0)
                i += add;
            else {
                i += 1;
                ok = false;
            }
        } catch (InvalidArgument &e) {
            cerr << "Error on processing option '" << argv[i] << "':" << endl;
            cerr << " " << e.msg() << endl;
            i++;
            ok = false;
        }
    }
    return ok;
}

int OptionsParser::check(OptionsCont *oc, char *arg1) {
    // the last stand-alone argument should be a switch
    if(!checkParameter(arg1)) return -1;
    // check switch
    bool error = false;
    if(isAbbreviation(arg1)) {
        // set all switches when abbreviated
        for(int i=1; i<arg1[i]!=0; i++) {
            if(!oc->set(convert(arg1[i]), true)) {
                error = true;
            }
        }
    } else {
	    // set single switch
        if(!oc->set(convert(arg1+2), true)) {
            error = true;
        }
    }
    if(error) return -1;
    return 1;
}

int OptionsParser::check(OptionsCont *oc, char *arg1, char *arg2) {
    // the first argument should be an option
    // (only the second may be a free string)
    if(!checkParameter(arg1)) return -1;
    // process not abbreviated parameters
    if(!isAbbreviation(arg1)) {
        if(oc->isBool(convert(arg1+2))) {
            if(!oc->set(convert(arg1+2), true))
                return -1;
            return 1;
        } else {
            if(!oc->set(convert(arg1+2), convert(arg2)))
                return -1;
            return 2;
        }
    }
    // process abbreviated parameters
    else {
        bool error = false;
        char file = 0;
        for(int i=1; arg1[i]!=0; i++) {
            if(oc->isBool(convert(arg1[i]))) {
                if(!oc->set(convert(arg1[i]), true)) {
                    error = true;
                }
            } else {
                if(file!=0) {
                    cout << "Error: The current parameter '" << arg1[i] << "' and the parameter '" << file << "' do both need a value." << endl;
                    error = true;
                }
                file = arg1[i];
            }
        }
        if(file!=0) {
            if(!oc->set(convert(file), convert(arg2))) {
                error = true;
            } else
                return 2;
        }
        if(error) return -1;
            return 1;
    }
}


bool OptionsParser::checkParameter(char *arg1) {
    if(arg1[0]!='-') {
        cerr << "The parameter " << arg1 << " is not allowed in this context" << endl;
        cerr << "Switch or parameter name expected." << endl;
        return false;
    }
    return true;
}

bool OptionsParser::isAbbreviation(char *arg1) {
    return arg1[1]!='-';
}

string OptionsParser::convert(char *arg) {
    string s(arg);
    return s;
}

string OptionsParser::convert(char abbr) {
    char buf[2];
    buf[0] = abbr;
    buf[1] = 0;
    string s(buf);
    return buf;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "OptionsParser.icc"
//#endif

// Local Variables:
// mode:C++
// End:


