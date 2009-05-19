/****************************************************************************/
/// @file    SUMOSAXHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// SAX-handler base for SUMO-files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMOSAXHandler_h
#define SUMOSAXHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/xml/GenericSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOSAXHandler
 * @brief SAX-handler base for SUMO-files
 *
 * This class is a GenericSAXHandler which knows all tags SUMO uses, so all
 *  SUMO-XML - loading classes should be derived from it.
 *
 * Besides the functionality of GenericSAXHandler it 
 *  implements methods for dealing with SAX-parser errors.
 */
class SUMOSAXHandler : public GenericSAXHandler {
public:
    /**
     * @brief Constructor
     *
     * @param[in] file The name of the processed file
     */
    SUMOSAXHandler(const std::string &file="") throw();


    /// Destructor
    virtual ~SUMOSAXHandler() throw();



    /// @name SAX ErrorHandler callbacks
    //@{

    /**
     * @brief Handler for XML-warnings
     *
     * The message is built using buildErrorMessage and reported
     *  to the warning-instance of the MsgHandler.
     *
     * @param[in] exception The occured exception to process
     */
    void warning(const SAXParseException& exception) throw();


    /**
     * @brief Handler for XML-errors
     *
     * The message is built using buildErrorMessage and thrown within a ProcessError.
     *
     * @param[in] exception The occured exception to process
     * @exception ProcessError On any call
     */
    void error(const SAXParseException& exception) throw(ProcessError);


    /**
     * @brief Handler for XML-errors
     *
     * The message is built using buildErrorMessage and thrown within a ProcessError.
     *
     * @exception ProcessError On any call
     * @param[in] exception The occured exception to process
     */
    void fatalError(const SAXParseException& exception) throw(ProcessError);
    //@}



protected:
    /**
     * @brief Builds an error message
     *
     * The error message includes the file name and the line/column information
     *  as supported by the given SAXParseException
     *
     * @param[in] exception The name of the currently processed file
     * @return A string describing the given exception
     */
    std::string buildErrorMessage(const SAXParseException& exception) throw();


private:

private:
    /// @brief invalidated copy constructor
    SUMOSAXHandler(const SUMOSAXHandler &s);

    /// @brief invalidated assignment operator
    const SUMOSAXHandler &operator=(const SUMOSAXHandler &s);

};


#endif

/****************************************************************************/

