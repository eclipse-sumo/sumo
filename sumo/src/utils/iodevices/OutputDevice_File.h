#ifndef OutputDevice_File_h
#define OutputDevice_File_h
//---------------------------------------------------------------------------//
//                        OutputDevice_File.h -
//  An output device that encapsulates an ofstream
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <fstream>
#include "OutputDevice.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class OutputDevice_File
 * An output device that encapsulates an ofstream
 */
class OutputDevice_File : public OutputDevice {
public:
    /// Constructor
    OutputDevice_File(std::ofstream *strm);

    /// Destructor
    ~OutputDevice_File();

    /// returns the information whether one can write into the device
    bool ok();

    /// Closes the device
    void close();

    /** @brief returns the information whether a stream is available
        If not, one has to use the "writeString" API */
    bool supportsStreams() const;

    /// Returns the assiciated ostream
    std::ostream &getOStream();

    //{
    /// Writes the given string
    XMLDevice &writeString(const std::string &str);

    void closeInfo();
    //}

private:
    /// The wrapped ofstream
    std::ofstream *myFileStream;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

