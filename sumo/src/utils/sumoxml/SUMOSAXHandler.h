#ifndef SUMOSAXHandler_h
#define SUMOSAXHandler_h
//---------------------------------------------------------------------------//
//                        SUMOSAXHandler.h -
//  The basic SAX-handler for SUMO-files
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
// Revision 1.2  2003/02/07 10:53:23  dkrajzew
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
#include <utils/xml/AttributesReadingGenericSAX2Handler.h>
#include <utils/common/FileErrorReporter.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class SUMOSAXHandler
 * Base class for XML-loading. This class knows all tags SUMO uses, so all
 * SUMO-XML - loading classes should be derived from it.
 */
class SUMOSAXHandler : public FileErrorReporter,
                       public AttributesReadingGenericSAX2Handler {
public:
    /// Constructor
    SUMOSAXHandler(const std::string &filetype,
        bool warn, bool verbose, const std::string &file="");

    /// Destructor
    virtual ~SUMOSAXHandler();

    // -----------------------------------------------------------------------
    //  Handlers for the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    /// called on a XML-warning; the error is reported to the SErrorHandler
    void warning(const SAXParseException& exception);

    /// called on a XML-error; the error is reported to the SErrorHandler
    void error(const SAXParseException& exception);

    /// called on a XML-fatal error; the error is reported to the SErrorHandler
    void fatalError(const SAXParseException& exception);

protected:
    /** the information whether warnings should be printed */
    bool  _warn;

    /** the information whether the builder shall be run in verbose mode */
    bool  _verbose;

private:
    /// invalidated copy constructo
    SUMOSAXHandler(const SUMOSAXHandler &s);

    /// invalidated assignment operator
    const SUMOSAXHandler &operator=(const SUMOSAXHandler &s);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "SUMOSAXHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

