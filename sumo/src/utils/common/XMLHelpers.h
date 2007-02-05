/****************************************************************************/
/// @file    XMLHelpers.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 21 Jan 2004
/// @version $Id$
///
// Some utility functions for xml usage
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
#ifndef XMLHelpers_h
#define XMLHelpers_h
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

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>


// ===========================================================================
// class definitions
// ===========================================================================
#if !defined(XERCES_HAS_CPP_NAMESPACE)
#define XERCES_CPP_NAMESPACE_QUALIFIER
#endif

/**
 * @class XMLHelpers
 * This class stores some basic, commonly used functions for processing
 * xml-files.
 */
class XMLHelpers
{
public:
    /// Builds a reader and assigns the handler to it
    static XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader *
    getSAXReader(XERCES_CPP_NAMESPACE_QUALIFIER DefaultHandler &handler);

    /// Builds a reader and assigns the handler to it
    static void runParser(XERCES_CPP_NAMESPACE_QUALIFIER DefaultHandler &handler,
                          const std::string &file);

    /// Sets a feature of the given parser
    static void setFeature(XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader &,
                           const std::string &feature, bool value);

};


#endif

/****************************************************************************/

