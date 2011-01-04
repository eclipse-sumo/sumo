/****************************************************************************/
/// @file    NILoader.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Perfoms network import
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NILoader_h
#define NILoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <xercesc/sax2/SAX2XMLReader.hpp>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class SUMOSAXHandler;
class NBNetBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NILoader
 * @brief Perfoms network import
 *
 * A plain loader which encapsulates calls to the import modules.
 */
class NILoader {
public:
    /** @brief Constructor
     * @param[in] nb The network builder to fill with loaded data
     */
    NILoader(NBNetBuilder &nb) throw();


    /// @brief Destructor
    ~NILoader() throw();


    /** loads data from the files specified in the given option container */
    void load(OptionsCont &oc);

private:
    /** loads data from sumo-files */
    //void loadSUMO(OptionsCont &oc);

    /** loads data from XML-files */
    void loadXML(OptionsCont &oc);

    /** loads data from the list of xml-files of certain type */
    void loadXMLType(SUMOSAXHandler *handler,
                     const std::vector<std::string> &files, const std::string &type);

private:
    /// @brief The network builder to fill with loaded data
    NBNetBuilder &myNetBuilder;


private:
    /// @brief Invalidated copy constructor.
    NILoader(const NILoader&);

    /// @brief Invalidated assignment operator.
    NILoader& operator=(const NILoader&);


};


#endif

/****************************************************************************/

