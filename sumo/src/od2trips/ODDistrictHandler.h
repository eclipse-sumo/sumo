/****************************************************************************/
/// @file    ODDistrictHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The XML-Handler for district loading
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
#ifndef ODDistrictHandler_h
#define ODDistrictHandler_h
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

#include <string>
#include <utility>
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ODDistrict;
class ODDistrictCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ODDistrictHandler
 * A XML-handler to load districts from a sumo network file.
 */
class ODDistrictHandler : public SUMOSAXHandler
{
public:
    /// Constructor
    ODDistrictHandler(ODDistrictCont &cont);

    /// Destructor
    ~ODDistrictHandler();

protected:
    /// Called when an opening-tag occures
    void myStartElement(int element, const std::string &name,
                        const Attributes &attrs);

    /// Called when a closing tag occures
    void myEndElement(int element, const std::string &name);

    /// Called on characters
    void myCharacters(int element, const std::string &name,
                      const std::string &chars);

private:
    /// Begins the parsing of a district
    void openDistrict(const Attributes &attrs);

    /// Adds a read source to the current district
    void addSource(const Attributes &attrs);

    /// Adds a read sink to the current district
    void addSink(const Attributes &attrs);

    /// Closes the processing of the current district
    void closeDistrict();

    /// Returns the values for a sink/source (name and weight)
    std::pair<std::string, SUMOReal> getValues(const Attributes &attrs,
            const std::string &type);

private:
    /// The container to add read districts to
    ODDistrictCont &myContainer;

    /// The currently parsed district
    ODDistrict *myCurrentDistrict;

};


#endif

/****************************************************************************/

