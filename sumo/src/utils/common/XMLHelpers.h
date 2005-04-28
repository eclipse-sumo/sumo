#ifndef XMLHelpers_h
#define XMLHelpers_h
/***************************************************************************
                          XMLHelpers.h
			  Some utility functions for xml usage
                             -------------------
    project              : SUMO
    begin                : Wed, 21 Jan 2004
    copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.3  2005/04/28 09:02:47  dkrajzew
// level3 warnings removed
//
// Revision 1.2  2004/02/06 08:48:11  dkrajzew
// memory leaks removed
//
// Revision 1.1  2004/01/26 07:27:04  dkrajzew
// added some xml-helping functions
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sax2/SAX2XMLReader.hpp>
#include <sax2/DefaultHandler.hpp>


/* =========================================================================
 * class definitions
 * ======================================================================= */
#if !defined(XERCES_HAS_CPP_NAMESPACE)
#define XERCES_CPP_NAMESPACE_QUALIFIER
#endif

/**
 * @class XMLHelpers
 * This class stores some basic, commonly used functions for processing
 * xml-files.
 */
class XMLHelpers {
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

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
