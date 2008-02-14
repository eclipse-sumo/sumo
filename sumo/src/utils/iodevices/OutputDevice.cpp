/****************************************************************************/
/// @file    OutputDevice.cpp
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// Static storage of an output device and its base (abstract) implementation
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


// ===========================================================================
// static member definitions
// ===========================================================================
OutputDevice::DeviceMap OutputDevice::myOutputDevices;


// ===========================================================================
// static method definitions
// ===========================================================================
OutputDevice&
OutputDevice::getDevice(const std::string &name,
                        const std::string &base) throw(IOError)
{
    // check whether the device has already been aqcuired
    if (myOutputDevices.find(name)!=myOutputDevices.end()) {
        return *myOutputDevices[name];
    }
    // build the device
    OutputDevice *dev = 0;
    // check whether the device shall print to stdout
    if (name=="stdout" || name=="-") {
        dev = new OutputDevice_COUT();
    } else if (FileHelpers::isSocket(name)) {
        try {
            int port = TplConvert<char>::_2int(name.substr(name.find(":")+1).c_str());
            dev = new OutputDevice_Network(name.substr(0, name.find(":")), port);
        } catch (NumberFormatException &) {
            throw IOError("Given port number '" + name.substr(name.find(":")+1) + "' is not numeric.");
        } catch (EmptyData &) {
            throw IOError("No port number given.");
        }
    } else {
        std::string fullName = FileHelpers::checkForRelativity(name, base);
        dev = new OutputDevice_File(fullName);
    }
    dev->setPrecision();
    dev->getOStream() << setiosflags(ios::fixed);
    myOutputDevices[name] = dev;
    return *dev;
}


bool
OutputDevice::createDeviceByOption(const std::string &optionName,
                                   const std::string &rootElement) throw(IOError)
{
    if (!OptionsCont::getOptions().isSet(optionName)) {
        return false;
    }
    OutputDevice& dev = OutputDevice::getDevice(OptionsCont::getOptions().getString(optionName));
    if (rootElement != "") {
        dev.writeXMLHeader(rootElement);
    }
    return true;
}


OutputDevice&
OutputDevice::getDeviceByOption(const std::string &optionName) throw(IOError, InvalidArgument)
{
    string devName = OptionsCont::getOptions().getString(optionName);
    if (myOutputDevices.find(devName)==myOutputDevices.end()) {
        throw InvalidArgument("Device '" + devName + "' has not been created.");
    }
    return OutputDevice::getDevice(devName);
}


void
OutputDevice::closeAll() throw()
{
    while (myOutputDevices.size()!=0) {
        myOutputDevices.begin()->second->close();
    }
    myOutputDevices.clear();
}



// ===========================================================================
// member method definitions
// ===========================================================================
bool
OutputDevice::ok() throw()
{
    return getOStream().good();
}

void
OutputDevice::close() throw()
{
    while (closeTag());
    for (DeviceMap::iterator i=myOutputDevices.begin(); i!=myOutputDevices.end(); ++i) {
        if (i->second == this) {
            myOutputDevices.erase(i);
            break;
        }
    }
    delete this;
}


void
OutputDevice::setPrecision(unsigned int precision) throw()
{
    getOStream() << setprecision(precision);
}


bool
OutputDevice::writeXMLHeader(const string &rootElement, const bool writeConfig,
                             const string &attrs, const string &comment) throw()
{
    if (myXMLStack.empty()) {
        OptionsCont::getOptions().writeXMLHeader(getOStream(), writeConfig);
        if (comment != "") {
            getOStream() << comment << "\n";
        }
        openTag(rootElement);
        if (attrs != "") {
            getOStream() << " " << attrs;
        }
        getOStream() << ">\n";
        return true;
    }
    return false;
}


OutputDevice&
OutputDevice::openTag(const string &xmlElement) throw()
{
    string indent(3*myXMLStack.size(), ' ');
    myXMLStack.push_back(xmlElement);
    getOStream() << indent << "<" << xmlElement;
    postWriteHook();
    return *this;
}


bool
OutputDevice::closeTag() throw()
{
    if (!myXMLStack.empty()) {
        string indent(3*(myXMLStack.size()-1), ' ');
        getOStream() << indent << "</" << myXMLStack.back() << ">" << endl;
        myXMLStack.pop_back();
        postWriteHook();
        return true;
    }
    return false;
}


void
OutputDevice::postWriteHook() throw()
{}


bool
OutputDevice::getBoolMarker(const std::string &name) const throw()
{
    if (myBoolMarkers.find(name)==myBoolMarkers.end()) {
        return false;
    }
    return myBoolMarkers.find(name)->second;
}


void
OutputDevice::setBoolMarker(const std::string &name, bool value) throw()
{
    myBoolMarkers[name] = value;
}



/****************************************************************************/

