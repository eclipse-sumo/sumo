/****************************************************************************/
/// @file    OutputWrapper.h
/// @author  Michael Behrisch
/// @date    2007-07-04
/// @version $Id: OutputWrapper.h 4084 2007-06-06 07:51:13Z behrisch $
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
#ifndef OutputWrapper_h
#define OutputWrapper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputWrapper
 */
class OutputWrapper
{
public:
    /// Returns the named file
    static OutputWrapper *getOutputWrapper(const std::string &name);

    /// returns the information whether one can write into the device
    bool ok();

    /// Closes the device
    void close();

    /// Writes an XML header with optional configuration
    void writeXMLHeader(const std::string &rootElement, bool writeConfig);

    /// Writes the corresponding XML footer
    void writeXMLFooter();

    /// @name methods for saving/reading an abstract state
    //@{
    /** @brief Returns the value of the named boolean marker
     *
     * If the boolean marker was not set before, false is returned
     */
    bool getBoolMarker(const std::string &name) const;

    /** @brief Sets the named boolean marker to the given state
     *
     * The previous value will be overwritten
     */
    void setBoolMarker(const std::string &name, bool value);
    //@}

private:
    /// constructor
    OutputWrapper(const std::string &name);

    /// Destructor
    virtual ~OutputWrapper();

    /// the output stream
    std::ostream *myStream;

    /// the XML root element
    std::string myRootElement;

    /// Map of boolean markers
    std::map<std::string, bool> myBoolMarkers;

    /// Definition of a map from names to output devices
    typedef std::map<std::string, OutputWrapper*> OutputWrapperMap;

    /// map from names to output wrappers
    static OutputWrapperMap myOutputWrappers;

};


#endif

/****************************************************************************/

