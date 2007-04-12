/****************************************************************************/
/// @file    SharedOutputDevices.cpp
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
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

#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>
#include "SharedOutputDevices.h"
#include "OutputDevice.h"
#include "OutputDevice_File.h"
#include "OutputDevice_COUT.h"
#include "OutputDevice_Network.h"
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
SharedOutputDevices *SharedOutputDevices::myInstance = 0;


// ===========================================================================
// method definitions
// ===========================================================================
SharedOutputDevices::SharedOutputDevices()
{}


SharedOutputDevices *
SharedOutputDevices::getInstance()
{
    if (myInstance==0) {
        myInstance = new SharedOutputDevices();
    }
    return myInstance;
}


void
SharedOutputDevices::setInstance(SharedOutputDevices *inst)
{
    assert(myInstance==0);
    myInstance = inst;
}


SharedOutputDevices::~SharedOutputDevices()
{
    for (DeviceMap::iterator i=myOutputDevices.begin(); i!=myOutputDevices.end(); ++i) {
        delete(*i).second;
    }
    myOutputDevices.clear();
    myInstance = 0;
}


OutputDevice *
SharedOutputDevices::getOutputDevice(const std::string &name)
{
    // check whether the device has already been aqcuired
    DeviceMap::iterator i = myOutputDevices.find(name);
    if (i!=myOutputDevices.end()) {
        // marks that extra definitions of the outputted values are needed
        (*i).second->setNeedsDetectorName(true);
        // return
        return (*i).second;
    }
    // build the device
    OutputDevice *dev = 0;
    // check whether the device shall print to stdout
    if(name=="stdout") {
        dev = new OutputDevice_COUT();
    } else {
        std::ofstream *strm = new std::ofstream(name.c_str());
        if (!strm->good()) {
            delete strm;
            throw FileBuildError("Could not build output file '" + name + "'.");
        }
        (*strm) << setprecision(OUTPUT_ACCURACY) << setiosflags(ios::fixed);
        dev = new OutputDevice_File(strm);
    }
    myOutputDevices[name] = dev;
    return dev;
}


OutputDevice *
SharedOutputDevices::getOutputDeviceChecking(const std::string &base,
        const std::string &name)
{
    return getOutputDevice(FileHelpers::checkForRelativity(name, base));
}



#ifdef USE_SOCKETS

OutputDevice *
SharedOutputDevices::getOutputDevice(const std::string &host, const int port, const std::string &protocol)
{
    std::ostringstream os;
    std::string deviceName;

    os << port;
    deviceName= host + "-" + os.str() + "-" + protocol;

    // check wether this device does already exist
    DeviceMap::iterator i = myOutputDevices.find(deviceName);
    if (i!=myOutputDevices.end()) {
        (*i).second->setNeedsDetectorName(true);
        return (*i).second;
    }

    // create device
    OutputDevice *dev = new OutputDevice_Network(host, port, protocol);
    myOutputDevices[deviceName] = dev;
    return dev;
}

#endif //#ifdef USE_SOCKETS

/****************************************************************************/

