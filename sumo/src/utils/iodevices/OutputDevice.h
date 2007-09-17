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
#include <vector>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDevice
 * An abstract output device. OutputDevices are basically a capsule around
 * std::ostream, which give a unified access to sockets, files and stdout.
 * Although everything that can be written to a stream can also be written
 * to an OutputDevice, there is special support for XML tags (remembering
 * all open tags to close them at the end). OutputDevices are still lacking
 * support for function pointers with the '<<' operator (no endl, use '\n').
 * The most important method to implement in subclasses is getOStream,
 * the most used part of the interface is the '<<' operator.
 *
 * The Boolean markers are used rarely and might get removed in future versions.
 */
class OutputDevice
{
public:
    /**
     * Creates and returns the named device. "stdout" and "-" refer to standard out,
     * "hostname:port" initiates socket connection. Otherwise a filename
     * is assumed and the second parameter may be used to give a base directory.
     * If there already is a device with the same name this one is returned.
     */
    static OutputDevice& getDevice(const std::string &name, const std::string &base="");

    /**
     * Creates and returns the device named by the option. Asks whether the option
     * and retrieves the name from the option if so. Optionally the XML header
     * gets written as well. Returns whether a device was created (option was set).
     */
    static bool createDeviceByOption(const std::string &optionName, const std::string &rootElement="");

    /**
     * Returns the device named by the option. If the option is unknown, unset
     * or the device was not created before, IllegalArgument is thrown.
     */
    static OutputDevice& getDeviceByOption(const std::string &name);

    /// Checks for the existence of the named device
    static bool hasDevice(const std::string &name);

    /// Closes all registered devices
    static void closeAll();

    /// Constructor
    OutputDevice()
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

    /// Writes an XML header with optional configuration
    bool writeXMLHeader(const std::string &rootElement,
                        const bool writeConfig=true,
                        const std::string &attrs="",
                        const std::string &comment="");

    /// Opens an XML tag
    OutputDevice& openTag(const std::string &xmlElement);

    /// Closes the most recently opened tag
    bool closeTag();

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
    /// Returns the associated ostream
    virtual std::ostream &getOStream() = 0;

    /// Is called after every write access. Default implementation does nothing.
    virtual void postWriteHook();

private:
    /// the XML root element
    std::vector<std::string> myXMLStack;

    /// Map of boolean markers
    std::map<std::string, bool> myBoolMarkers;

    /// Definition of a map from names to output devices
    typedef std::map<std::string, OutputDevice*> DeviceMap;

    /// map from names to output devices
    static DeviceMap myOutputDevices;

};


#endif

/****************************************************************************/

