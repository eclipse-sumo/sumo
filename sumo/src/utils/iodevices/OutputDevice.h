/****************************************************************************/
/// @file    OutputDevice.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// An abstract output device
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
#ifndef OutputDevice_h
#define OutputDevice_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDevice
 * An abstract output device
 */
class OutputDevice
{
public:
    /// Returns the named file
    static OutputDevice *getOutputDevice(const std::string &name);

    /// Returns the named file checking whether the path is completely given
    static OutputDevice *getOutputDeviceChecking(
        const std::string &base, const std::string &name);

#ifdef USE_SOCKETS
    // returns the netework target denoted by 'host', 'port' and 'protocol'
    static OutputDevice *getOutputDevice(const std::string &host, const int port, const std::string &protocol);
#endif //#ifdef USE_SOCKETS

    /// Closes all registered devices
    static void closeAll();

    /// Constructor
    OutputDevice() :
            myNeedHeader(true), myNeedTail(true), myNeedDetectorName(false)
    { }

    /// Destructor
    virtual ~OutputDevice()
    { }

    /// returns the information whether one can write into the device
    virtual bool ok();

    /// Closes the device
    virtual void close();

    /// Returns the associated ostream
    virtual std::ostream &getOStream() = 0;

    virtual void closeInfo() = 0;

    /// Returns the information whether the detector's name should always be printed
    bool needsDetectorName() const
    {
        return myNeedDetectorName;
    }
    //}

    /// Returns the information whether a header shall be printed
    bool needsHeader() const
    {
        return myNeedHeader;
    }

    /// Sets the information whether a header is wished
    void setNeedsHeader(bool value)
    {
        myNeedHeader = value;
    }

    /// Returns the information whether a header shall be printed
    bool needsTail() const
    {
        return myNeedTail;
    }

    /// Sets the information whether a header is wished
    void setNeedsTail(bool value)
    {
        myNeedTail = value;
    }

    /// Sets the information whether the detector's name should always be printed
    void setNeedsDetectorName(bool value)
    {
        myNeedDetectorName = value;
    }

    /// @name methods for saving/reading an abstract state
    //@{
    /** @brief Returns the value of the named boolean marker
     *
     * If the boolean marker was not set before, false is returned
     */
    bool getBoolMarker(const std::string &name) const
    {
        if (myBoolMarkers.find(name)==myBoolMarkers.end()) {
            return false;
        }
        return myBoolMarkers.find(name)->second;
    }

    /** @brief Sets the named boolean marker to the given state
     *
     * The previous value will be overwritten
     */
    void setBoolMarker(const std::string &name, bool value)
    {
        myBoolMarkers[name] = value;
    }
    //@}

    OutputDevice &operator<<(const std::string &str);

protected:
    /// The information whether a header shall be written
    bool myNeedHeader;

    /// The information whether a tail shall be written
    bool myNeedTail;

    /// The information wheter the detector's name should be printed
    bool myNeedDetectorName;

    /// Map of boolean markers
    std::map<std::string, bool> myBoolMarkers;


private:
    /// Definition of a map from names to output devices
    typedef std::map<std::string, OutputDevice*> DeviceMap;

    /// map from names to output devices
    static DeviceMap myOutputDevices;
};


#endif

/****************************************************************************/

