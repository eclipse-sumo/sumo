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
// Revision 1.1  2003/06/24 08:12:59  dkrajzew
// a subsystem for option handling common to most of the applications added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cassert>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/HelpPrinter.h>
#include "OptionsSubSys.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
OptionsCont OptionsSubSys::myOptions;


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
OptionsSubSys::init(int argc, char **argv,
                    fill_options *fill_f,
                    check_options *check_f,
                    char *help[])
{
    (*fill_f)(myOptions);
    // parse options
    if(OptionsIO::getOptions(&myOptions, argc, argv)) {
        // check whether the help shall be printed
        if(myOptions.getBool("help")) {
            HelpPrinter::print(help);
            // end application
            return false;
        }
        // check whether the settings shall be printed
        if(myOptions.getBool("p")) {
            cout << myOptions;
        }
        // check the settings
        if(check_f!=0&&!check_f(myOptions)) {
            return false;
        }
    } else {
        // the options could not be parsed
        //  - something is wrong with the calling parameter
        return false;
    }
    return true;
}


bool
OptionsSubSys::guiInit(fill_options *fill_f, const std::string &conf)
{
    (*fill_f)(myOptions);
    myOptions.set("c", conf);
    // parse options
    if(!OptionsIO::getOptions(&myOptions, 1, 0)) {
        // the options could not be parsed
        //  - something is wrong with the calling parameter
        return false;
    }
    return true;
}


OptionsCont &
OptionsSubSys::getOptions()
{
    return myOptions;
}


void
OptionsSubSys::close()
{
    myOptions.clear();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "OptionsSubSys.icc"
//#endif

// Local Variables:
// mode:C++
// End:
