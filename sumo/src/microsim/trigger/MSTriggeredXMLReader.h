/****************************************************************************/
/// @file    MSTriggeredXMLReader.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for classes that read XML-triggers
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
#ifndef MSTriggeredXMLReader_h
#define MSTriggeredXMLReader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <utils/xml/SUMOSAXHandler.h>
#include "MSTriggeredReader.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTriggeredXMLReader
 * Base class for triggered file readers which work with XML-files
 */
class MSTriggeredXMLReader : public MSTriggeredReader,
            public SUMOSAXHandler {
public:
    /// Destructor
    virtual ~MSTriggeredXMLReader() throw();

protected:
    /// Constructor (for derived classes)
    MSTriggeredXMLReader(MSNet &net, const std::string &filename);

    /// reads from the XML-file (parses from file)
    bool readNextTriggered();

    virtual bool nextRead() = 0;

protected:
    void myInit();

protected:
    /// The used SAX-parser
    SAX2XMLReader* myParser;

    /// Position within the XML-file
    XMLPScanToken  myToken;

    bool myHaveMore;

private:
    /// @brief Invalidated copy constructor.
    MSTriggeredXMLReader(const MSTriggeredXMLReader&);

    /// @brief Invalidated assignment operator.
    MSTriggeredXMLReader& operator=(const MSTriggeredXMLReader&);


};


#endif

/****************************************************************************/

