#ifndef MSTriggeredXMLReader_h
#define MSTriggeredXMLReader_h
//---------------------------------------------------------------------------//
//                        MSTriggeredXMLReader.h -
//  The basic class for classes that read XML-triggers
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "MSTriggeredReader.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * class MSTriggeredXMLReader
 * Base class for triggered file readers which work with XML-files
 */
class MSTriggeredXMLReader : public MSTriggeredReader,
                             public SUMOSAXHandler {
public:
    /// Destructor
    virtual ~MSTriggeredXMLReader();

protected:
    /// Constructor (for derived classes)
    MSTriggeredXMLReader(MSNet &net, const std::string &filename);

    /// Initialises the reader
    void init(MSNet &net);

    /// reads from the XML-file (parses from file)
    bool readNextTriggered();

private:
    /// The used SAX-parser
    SAX2XMLReader* myParser;

    /// Position within the XML-file
    XMLPScanToken  myToken;

    /** @brief Information whether the next element was read
        This must be set by derived classes */
    bool _nextRead;

private:
    /// invalidated default constructor
    MSTriggeredXMLReader();

    /// invalidated copy constructor
    MSTriggeredXMLReader(const MSTriggeredXMLReader &s);

    /// invalidated assignment operator
    const MSTriggeredXMLReader &operator=(const MSTriggeredXMLReader &s);


};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSTriggeredXMLReader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

