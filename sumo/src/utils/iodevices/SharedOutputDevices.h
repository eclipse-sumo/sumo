/****************************************************************************/
/// @file    SharedOutputDevices.h
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
#ifndef SharedOutputDevices_h
#define SharedOutputDevices_h
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
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SharedOutputDevices
 */
class SharedOutputDevices
{
public:
    /// constructor
    SharedOutputDevices();

    /// Destructor
    virtual ~SharedOutputDevices();

    /// Get the singleton instance
    static SharedOutputDevices *getInstance();

    /// Get the singleton instance
    static void setInstance(SharedOutputDevices*);

    /// Returns the named file
    virtual OutputDevice *getOutputDevice(const std::string &name);

    /// Returns the named file checking whether the path is completely given
    virtual OutputDevice *getOutputDeviceChecking(
        const std::string &base, const std::string &name);

#ifdef USE_SOCKETS
    // returns the netework target denoted by 'host', 'port' and 'protocol'
    virtual OutputDevice *getOutputDevice(const std::string &host, const int port, const std::string &protocol);
#endif //#ifdef USE_SOCKETS

protected:
    /// the singleton instance
    static SharedOutputDevices *myInstance;

    /// Definition of a map from names to output devices
    typedef std::map<std::string, OutputDevice*> DeviceMap;

    /// map from names to output devices
    DeviceMap myOutputDevices;

};


#endif

/****************************************************************************/

