/****************************************************************************/
/// @file    OutputDevice.cpp
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
#include "OutputDevice.h"
#include "OutputDevice_File.h"
#include "OutputDevice_COUT.h"
#include "OutputDevice_Network.h"
#include <utils/common/TplConvert.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

OutputDevice::DeviceMap OutputDevice::myOutputDevices;
// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice *
OutputDevice::getOutputDevice(const std::string &name, const std::string &base)
{
    // check whether the device has already been aqcuired
    DeviceMap::iterator i = myOutputDevices.find(name);
    if (i!=myOutputDevices.end()) {
        return (*i).second;
    }
    // build the device
    OutputDevice *dev = 0;
    int colonPos = name.find(":");
    // check whether the device shall print to stdout
    if (name=="stdout" || name=="-") {
        dev = new OutputDevice_COUT();
    } else if (colonPos > 1) {
        int port = TplConvert<char>::_2int(name.substr(colonPos+1).c_str());
        dev = new OutputDevice_Network(name.substr(0, colonPos), port);
    } else {
        std::string fullName = FileHelpers::checkForRelativity(name, base);
        std::ofstream *strm = new std::ofstream(fullName.c_str());
        if (!strm->good()) {
            delete strm;
            throw IOError("Could not build output file '" + fullName + "'.");
        }
        dev = new OutputDevice_File(strm);
    }
    dev->setPrecision();
    dev->getOStream() << setiosflags(ios::fixed);
    myOutputDevices[name] = dev;
    return dev;
}


void
OutputDevice::closeAll()
{
    while (myOutputDevices.size()!=0) {
        myOutputDevices.begin()->second->close();
    }
    myOutputDevices.clear();
}

bool
OutputDevice::ok()
{
    return getOStream().good();
}

void
OutputDevice::close()
{
    writeXMLFooter();
    for (DeviceMap::iterator i=myOutputDevices.begin(); i!=myOutputDevices.end(); ++i) {
        if (i->second == this) {
            delete(*i).second;
            myOutputDevices.erase(i);
            break;
        }
    }
}


void
OutputDevice::setPrecision(unsigned int precision)
{
    getOStream() << setprecision(precision);
}


bool
OutputDevice::writeXMLHeader(const string &rootElement, const bool writeConfig,
                             const string &attrs, const string &comment)
{
    if (myRootElement == "") {
        myRootElement = rootElement;
        OptionsCont::getOptions().writeXMLHeader(getOStream(), writeConfig);
        getOStream() << "<" << myRootElement;
        if (attrs != "") {
            getOStream() << " " << attrs;
        }
        getOStream() << ">" << endl;
        postWriteHook();
        return true;
    }
    return false;
}


bool
OutputDevice::writeXMLFooter()
{
    if (myRootElement != "") {
        getOStream() << "</" << myRootElement << ">" << std::endl;
        postWriteHook();
        myRootElement = "";
        return true;
    }
    return false;
}


void
OutputDevice::postWriteHook()
{}


/****************************************************************************/

