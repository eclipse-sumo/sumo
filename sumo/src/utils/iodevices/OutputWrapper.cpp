/****************************************************************************/
/// @file    OutputWrapper.cpp
/// @author  Michael Behrisch
/// @date    2007-07-04
/// @version $Id: OutputWrapper.cpp 4084 2007-06-06 07:51:13Z behrisch $
///
// The holder/builder of output devices
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>
#include "OutputWrapper.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
OutputWrapperMap myOutputWrappers;


// ===========================================================================
// method definitions
// ===========================================================================
OutputWrapper::OutputWrapper(const std::string &name)
{
    if (name=="stdout") {
        myStream = cout;
    } else {
        myStream = new std::ofstream(name.c_str());
        if (!myStream->good()) {
            delete myStream;
            throw FileBuildError("Could not build output file '" + name + "'.");
        }
    }
    (*myStream) << setprecision(OUTPUT_ACCURACY) << setiosflags(ios::fixed);
}


OutputWrapper::~OutputWrapper()
{
    delete myStream;
}


bool
OutputWrapper::ok()
{
    return myStream->good();
}


void
OutputWrapper::close()
{
    myStream->close();
}


OutputWrapper *
OutputWrapper::getOutputWrapper(const std::string &name)
{
    // check whether the device has already been aqcuired
    OutputWrapperMap::iterator i = myOutputWrappers.find(name);
    if (i==myOutputWrappers.end()) {
        myOutputWrappers[name] = new OutputWrapper(name);
    }
    return myOutputWrappers[name];
}


OutputWrapper *
OutputWrapper::getOutputWrapperChecking(const std::string &base,
        const std::string &name)
{
    return getOutputWrapper(FileHelpers::checkForRelativity(name, base));
}

/****************************************************************************/

