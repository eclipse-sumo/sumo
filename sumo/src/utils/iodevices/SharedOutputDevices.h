#ifndef SharedOutputDevices_h
#define SharedOutputDevices_h
//---------------------------------------------------------------------------//
//                        SharedOutputDevices.h -
//  The holder of output devices
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
// Revision 1.2  2004/08/02 13:01:16  dkrajzew
// documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
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
    /// Get the singleton instance
    static SharedOutputDevices *getInstance();

    /// Destructor
    ~SharedOutputDevices();

    /// Returns the named file
    OutputDevice *getOutputFile(const std::string &name);

private:
    /// Private constructor
    SharedOutputDevices();

private:
    /// the singleton instance
    static SharedOutputDevices *myInstance;

    /// Definition of a map from names to output devices
    typedef std::map<std::string, OutputDevice*> FileMap;

    /// map from names to output devices
    FileMap myOutputFiles;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

