#ifndef SharedOutputDevices_h
#define SharedOutputDevices_h
//---------------------------------------------------------------------------//
//                        SharedOutputDevices.h -
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
// Revision 1.5  2005/09/15 12:21:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/28 09:02:49  dkrajzew
// level3 warnings removed
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <map>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OutputDevice;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class SharedOutputDevices
 */
class SharedOutputDevices {
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

protected:
    /// the singleton instance
    static SharedOutputDevices *myInstance;

    /// Definition of a map from names to output devices
    typedef std::map<std::string, OutputDevice*> DeviceMap;

    /// map from names to output devices
    DeviceMap myOutputDevices;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

