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
// Revision 1.2  2005/10/07 11:37:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.1  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/01/06 10:48:07  dksumo
// 0.8.2.1 patches
//
// Revision 1.1  2004/10/22 12:49:31  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.5  2004/07/02 09:56:40  dkrajzew
// debugging while implementing the vss visualisation
//
// Revision 1.4  2003/09/23 14:18:15  dkrajzew
// hierarchy refactored; user-friendly implementation
//
// Revision 1.3  2003/09/22 14:56:07  dkrajzew
// base debugging
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
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

    /// reads from the XML-file (parses from file)
    bool readNextTriggered();

    virtual bool nextRead() = 0;

protected:
    void init();

protected:
    /// The used SAX-parser
    SAX2XMLReader* myParser;

    /// Position within the XML-file
    XMLPScanToken  myToken;

    bool myHaveMore;

private:
    /// invalidated default constructor
    MSTriggeredXMLReader();

    /// invalidated copy constructor
    MSTriggeredXMLReader(const MSTriggeredXMLReader &s);

    /// invalidated assignment operator
    const MSTriggeredXMLReader &operator=(const MSTriggeredXMLReader &s);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

