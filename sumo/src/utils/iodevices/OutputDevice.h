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
    /**
     * Returns the named device. "stdout" and "-" refer to standard out,
     * "hostname:port" initiates socket connection. Otherwise a filename
     * is assumed and the second parameter may be used to give a base directory.
     */
    static OutputDevice *getOutputDevice(const std::string &name, const std::string &base="");

    /// Closes all registered devices
    static void closeAll();

    /// Constructor
    OutputDevice() : myRootElement("")
    { }

    /// Destructor
    virtual ~OutputDevice()
    { }

    /// returns the information whether one can write into the device
    virtual bool ok();

    /// Closes the device and removes it from the dictionary
    void close();

    /// Sets the precison or resets it to default
    void setPrecision(unsigned int precision=OUTPUT_ACCURACY);

    /// Returns the associated ostream
    virtual std::ostream &getOStream() = 0;

    /// Writes an XML header with optional configuration
    bool writeXMLHeader(const std::string &rootElement,
                        const bool writeConfig=true,
                        const std::string &attrs="",
                        const std::string &comment="");

    /// Writes the corresponding XML footer
    bool writeXMLFooter();

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

    template <class T>
    OutputDevice &operator<<(const T &t)
    {
        getOStream() << t;
        postWriteHook();
        return *this;
    }

protected:
    /// Is called after every write access. Default implememntation does nothing.
    virtual void postWriteHook();

private:
    /// the XML root element
    std::string myRootElement;

    /// Map of boolean markers
    std::map<std::string, bool> myBoolMarkers;

    /// Definition of a map from names to output devices
    typedef std::map<std::string, OutputDevice*> DeviceMap;

    /// map from names to output devices
    static DeviceMap myOutputDevices;
};


#endif

/****************************************************************************/

