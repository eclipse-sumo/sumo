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
// Revision 1.9  2006/01/09 13:38:04  dkrajzew
// debugging error handling
//
// Revision 1.8  2005/10/07 11:46:56  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/15 12:21:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/07/12 12:52:56  dkrajzew
// build number output added
//
// Revision 1.5  2005/05/04 09:28:01  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/12/16 12:18:39  dkrajzew
// debugging
//
// Revision 1.3  2004/11/23 10:36:02  dkrajzew
// debugging
//
// Revision 1.2  2003/10/27 10:55:10  dkrajzew
// problems on setting gui options patched - the configuration is not loaded directly any more
//
// Revision 1.1  2003/06/24 08:12:59  dkrajzew
// a subsystem for option handling common to most of the applications added
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

#include <cassert>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/StringTokenizer.h>
#include "OptionsSubSys.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
OptionsSubSys::init(bool loadConfig, int argc, char **argv,
                    fill_options *fill_f,
                    check_options *check_f)
{
    (*fill_f)(myOptions);
    // parse options
    if(OptionsIO::getOptions(loadConfig, &myOptions, argc, argv)) {
		// 01.06.2005: version retrieval patched
        // check the settings
        if(check_f!=0&&!check_f(myOptions)) {
            return false;
        }
    } else {
        // the options could not be parsed
        //  something is wrong with the calling parameter
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
    if(!OptionsIO::getOptions(true, &myOptions, 1, 0)) {
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


bool
OptionsSubSys::helper_CSVOptionMatches(const std::string &optionName,
                                       const std::string &itemName)
{
    if(myOptions.isSet(optionName)) {
        StringTokenizer st(myOptions.getString(optionName), ";");
        while(st.hasNext()) {
            if(st.next()==itemName) {
                return true;
            }
        }
    }
    return false;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
