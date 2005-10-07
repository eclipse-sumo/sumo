//---------------------------------------------------------------------------//
//                        SharedOutputDevices.cpp -
//  The holder/builder of output devices
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.6  2005/10/07 11:46:44  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 12:21:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 09:26:19  dkrajzew
// reports about being unable to build a file added
//
// Revision 1.3  2004/11/23 10:35:47  dkrajzew
// debugging
//
// Revision 1.2  2004/11/22 12:54:56  dksumo
// tried to generelise the usage of detectors and output devices
//
// Revision 1.1  2004/10/22 12:50:58  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.2  2004/08/02 13:01:16  dkrajzew
// documentation added
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

#include <map>
#include <fstream>
#include <string>
#include <cassert>
#include "SharedOutputDevices.h"
#include "OutputDevice.h"
#include "OutputDevice_File.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>

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
SharedOutputDevices *SharedOutputDevices::myInstance = 0;


/* =========================================================================
 * method definitions
 * ======================================================================= */
SharedOutputDevices::SharedOutputDevices()
{
}


SharedOutputDevices *
SharedOutputDevices::getInstance()
{
    if(myInstance==0) {
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
    for(DeviceMap::iterator i=myOutputDevices.begin(); i!=myOutputDevices.end(); ++i) {
        delete (*i).second;
    }
    myOutputDevices.clear();
    myInstance = 0;
}


OutputDevice *
SharedOutputDevices::getOutputDevice(const std::string &name)
{
    DeviceMap::iterator i = myOutputDevices.find(name);
    if(i!=myOutputDevices.end()) {
        (*i).second->setNeedsDetectorName(true);
        return (*i).second;
    }
    std::ofstream *strm = new std::ofstream(name.c_str());
    if(!strm->good()) {
        delete strm;
        throw FileBuildError(
            string("Could not build output file '") + name + string("'.") );
    }
    OutputDevice *dev = new OutputDevice_File(strm);
    myOutputDevices[name] = dev;
    return dev;
}


OutputDevice *
SharedOutputDevices::getOutputDeviceChecking(const std::string &base,
                                             const std::string &name)
{
    return getOutputDevice(
        FileHelpers::checkForRelativity(name, base));
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
