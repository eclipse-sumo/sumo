/****************************************************************************/
/// @file    OutputDevice.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Mario Krumnow
/// @date    2004
/// @version $Id$
///
// Static storage of an output device and its base (abstract) implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/common/ToString.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "PlainXMLFormatter.h"
#include "BinaryFormatter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDevice
 * @brief Static storage of an output device and its base (abstract) implementation
 *
 * OutputDevices are basically a capsule around an std::ostream, which give a
 *  unified access to sockets, files and stdout.
 *
 * Usually, an application builds as many output devices as needed. Each
 *  output device may also be used to save outputs from several sources
 *  (several detectors, for example). Building is done using OutputDevice::getDevice()
 *  what also parses the given output description in order to decide
 *  what kind of an OutputDevice shall be built. OutputDevices are
 *  closed via OutputDevice::closeAll(), normally called at the application's
 *  end.
 *
 * Although everything that can be written to a stream can also be written
 *  to an OutputDevice, there is special support for XML tags (remembering
 *  all open tags to close them at the end). OutputDevices are still lacking
 *  support for function pointers with the '<<' operator (no endl, use '\n').
 *  The most important method to implement in subclasses is getOStream,
 *  the most used part of the interface is the '<<' operator.
 *
 * The Boolean markers are used rarely and might get removed in future versions.
 */
class OutputDevice {
public:
    /// @name static access methods to OutputDevices
    /// @{

    /** @brief Returns the described OutputDevice
     *
     * Creates and returns the named device. "stdout" and "stderr" refer to the relevant console streams,
     * "hostname:port" initiates socket connection. Otherwise a filename
     * is assumed (where "nul" and "/dev/null" do what you would expect on both platforms).
     * If there already is a device with the same name this one is returned.
     *
     * @param[in] name The description of the output name/port/whatever
     * @return The corresponding (built or existing) device
     * @exception IOError If the output could not be built for any reason (error message is supplied)
     */
    static OutputDevice& getDevice(const std::string& name);


    /** @brief Creates the device using the output definition stored in the named option
     *
     * Creates and returns the device named by the option. Asks whether the option
     *  and retrieves the name from the option if so. Optionally the XML header
     *  gets written as well. Returns whether a device was created (option was set).
     *
     * Please note, that we do not have to consider the "application base" herein,
     *  because this call is only used to get file names of files referenced
     *  within XML-declarations of structures which paths already is aware of the
     *  cwd.
     *
     * @param[in] optionName  The name of the option to use for retrieving the output definition
     * @param[in] rootElement The root element to use (XML-output)
     * @param[in] schemaFile  The basename of the schema file to use (XML-output)
     * @return Whether a device was built (the option was set)
     * @exception IOError If the output could not be built for any reason (error message is supplied)
     */
    static bool createDeviceByOption(const std::string& optionName,
                                     const std::string& rootElement = "",
                                     const std::string& schemaFile = "");


    /** @brief Returns the device described by the option
     *
     * Returns the device named by the option. If the option is unknown, unset
     * or the device was not created before, InvalidArgument is thrown.
     *
     * Please note, that we do not have to consider the "application base" herein.
     *
     * @param[in] name The name of the option to use for retrieving the output definition
     * @return The corresponding (built or existing) device
     * @exception IOError If the output could not be built for any reason (error message is supplied)
     * @exception InvalidArgument If the option with the given name does not exist
     */
    static OutputDevice& getDeviceByOption(const std::string& name);


    /**  Closes all registered devices
     */
    static void closeAll();
    /// @}


    /** @brief Helper method for string formatting
     *
     * @param[in] v The floating point value to be formatted
     * @param[in] precision the precision to achieve
     * @return The formatted string
     */
    static std::string realString(const SUMOReal v, const int precision = OUTPUT_ACCURACY);


public:
    /// @name OutputDevice member methods
    /// @{

    /// @brief Constructor
    OutputDevice(const bool binary = false, const unsigned int defaultIndentation = 0);


    /// @brief Destructor
    virtual ~OutputDevice();


    /** @brief returns the information whether one can write into the device
     * @return Whether the device can be used (stream is good)
     */
    virtual bool ok();


    /** @brief Closes the device and removes it from the dictionary
     */
    void close();


    /** @brief Sets the precison or resets it to default
     * @param[in] precision The accuracy (number of digits behind '.') to set
     */
    void setPrecision(unsigned int precision = OUTPUT_ACCURACY);


    /** @brief Writes an XML header with optional configuration
     *
     * If something has been written (myXMLStack is not empty), nothing
     *  is written and false returned.
     *
     * @param[in] rootElement The root element to use
     * @param[in] attrs Additional attributes to save within the rootElement
     * @param[in] comment Additional comment (saved in front the rootElement)
     * @return Whether the header could be written (stack was empty)
     * @todo Check which parameter is used herein
     * @todo Describe what is saved
     */
    bool writeXMLHeader(const std::string& rootElement,
                        const std::string& attrs = "",
                        const std::string& comment = "");


    template <typename E>
    bool writeHeader(const SumoXMLTag& rootElement) {
        if (myAmBinary) {
            return static_cast<BinaryFormatter*>(myFormatter)->writeHeader<E>(getOStream(), rootElement);
        }
        return static_cast<PlainXMLFormatter*>(myFormatter)->writeHeader(getOStream(), rootElement);
    }


    /** @brief Opens an XML tag
     *
     * An indentation, depending on the current xml-element-stack size, is written followed
     *  by the given xml element ("<" + xmlElement)
     * The xml element is added to the stack, then.
     *
     * @param[in] xmlElement Name of element to open
     * @return The OutputDevice for further processing
     */
    OutputDevice& openTag(const std::string& xmlElement);


    /** @brief Opens an XML tag
     *
     * Helper method which finds the correct string before calling openTag.
     *
     * @param[in] xmlElement Id of the element to open
     * @return The OutputDevice for further processing
     */
    OutputDevice& openTag(const SumoXMLTag& xmlElement);


    /** @brief Closes the most recently opened tag
     *
     * The topmost xml-element from the stack is written into the stream
     *  as a closing element. Depending on the formatter used
     *  this may be something like "</" + element + ">" or "/>" or
     *  nothing at all.
     *
     * @return Whether a further element existed in the stack and could be closed
     * @todo it is not verified that the topmost element was closed
     */
    bool closeTag();



    /** @brief writes a line feed if applicable
     */
    void lf() {
        if (!myAmBinary) {
            getOStream() << "\n";
        }
    }


    /** @brief Returns whether we have a binary output
     * @return whether we have a binary output
     */
    bool isBinary() const {
        return myAmBinary;
    }


    /** @brief writes a named attribute
     *
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     * @return The OutputDevice for further processing
     */
    template <typename T>
    OutputDevice& writeAttr(const SumoXMLAttr attr, const T& val) {
        if (myAmBinary) {
            BinaryFormatter::writeAttr(getOStream(), attr, val);
        } else {
            PlainXMLFormatter::writeAttr(getOStream(), attr, val);
        }
        return *this;
    }


    /** @brief writes an arbitrary attribute
     *
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     * @return The OutputDevice for further processing
     */
    template <typename T>
    OutputDevice& writeAttr(const std::string& attr, const T& val) {
        if (myAmBinary) {
            BinaryFormatter::writeAttr(getOStream(), attr, val);
        } else {
            PlainXMLFormatter::writeAttr(getOStream(), attr, val);
        }
        return *this;
    }


    /** @brief writes a string attribute only if it is not the empty string and not the string "default"
     *
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     * @return The OutputDevice for further processing
     */
    OutputDevice& writeNonEmptyAttr(const SumoXMLAttr attr, const std::string& val) {
        if (val != "" && val != "default") {
            writeAttr(attr, val);
        }
        return *this;
    }


    /** @brief writes a preformatted tag to the device but ensures that any
     * pending tags are closed
     * @param[in] val The preformatted data
     * @return The OutputDevice for further processing
     */
    OutputDevice& writePreformattedTag(const std::string& val) {
        myFormatter->writePreformattedTag(getOStream(), val);
        return *this;
    }


    /** @brief Retrieves a message to this device.
     *
     * Implementation of the MessageRetriever interface. Writes the given message to the output device.
     *
     * @param[in] msg The msg to write to the device
     */
    void inform(const std::string& msg, const char progress = 0);


    /** @brief Abstract output operator
     * @return The OutputDevice for further processing
     */
    template <class T>
    OutputDevice& operator<<(const T& t) {
        getOStream() << t;
        postWriteHook();
        return *this;
    }

protected:
    /// @brief Returns the associated ostream
    virtual std::ostream& getOStream() = 0;


    /** @brief Called after every write access.
     *
     * Default implementation does nothing.
     */
    virtual void postWriteHook();


private:
    /// @brief map from names to output devices
    static std::map<std::string, OutputDevice*> myOutputDevices;


private:
    /// @brief The formatter for XML
    OutputFormatter* myFormatter;

    const bool myAmBinary;

public:
    /// @brief Invalidated copy constructor.
    OutputDevice(const OutputDevice&);

private:

    /// @brief Invalidated assignment operator.
    OutputDevice& operator=(const OutputDevice&);

};


#endif

/****************************************************************************/

