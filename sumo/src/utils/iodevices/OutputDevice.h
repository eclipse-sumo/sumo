#ifndef OutputDevice_h
#define OutputDevice_h
//---------------------------------------------------------------------------//
//                        OutputDevice.h -
//  An abstract output device that encapsulates an ostream
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
// Revision 1.6  2005/09/15 12:21:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/04/28 09:02:49  dkrajzew
// level3 warnings removed
//
// Revision 1.4  2004/12/16 12:22:12  dkrajzew
// got rid of an unnecessary detector parameter/debugging
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

#include <string>
#include "XMLDevice.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class OutputDevice
 * An abstract output device that encapsulates an ostream
 */
class OutputDevice : public XMLDevice {
public:
    /// Constructor
    OutputDevice() :
      myNeedHeader(true), myNeedTail(true), myNeedDetectorName(false)
      { }

    /// Destructor
    virtual ~OutputDevice() { }

    /// returns the information whether one can write into the device
    virtual bool ok() = 0;

    /// Closes the device
    virtual void close() = 0;

    /** @brief returns the information whether a stream is available
        If not, one has to use the "writeString" API */
    virtual bool supportsStreams() const = 0;

    /// Returns the assiciated ostream
    virtual std::ostream &getOStream() = 0;

    //{
    /// Writes the given string
    virtual XMLDevice &writeString(const std::string &str) = 0;

    virtual void closeInfo() = 0;

    /// Returns the information whether the detector's name should always be printed
    bool needsDetectorName() const { return myNeedDetectorName; }
    //}

    /// Returns the information whether a header shall be printed
    bool needsHeader() const { return myNeedHeader; }

    /// Sets the information whether a header is wished
    void setNeedsHeader(bool value) { myNeedHeader = value; }

    /// Returns the information whether a header shall be printed
    bool needsTail() const { return myNeedTail; }

    /// Sets the information whether a header is wished
    void setNeedsTail(bool value) { myNeedTail = value; }

    /// Sets the information whether the detector's name should always be printed
    void setNeedsDetectorName(bool value) { myNeedDetectorName = value; }


protected:
    /// The information whether a header shall be written
    bool myNeedHeader;

    /// The information whether a tail shall be written
    bool myNeedTail;

    /// The information wheter the detector's name should be printed
    bool myNeedDetectorName;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

