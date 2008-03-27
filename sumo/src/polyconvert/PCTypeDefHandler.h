/****************************************************************************/
/// @file    PCTypeDefHandler.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A handler for loading polygon type maps
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
#ifndef PCTypeDefHandler_h
#define PCTypeDefHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/options/OptionsCont.h>
#include "PCTypeDefHandler.h"


// ===========================================================================
// class declarations
// ===========================================================================
class PCTypeToDef;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PCTypeDefHandler
 * @brief A handler for loading polygon type maps
 */
class PCTypeDefHandler : public SUMOSAXHandler
{
public:
    /// Constructor
    PCTypeDefHandler(OptionsCont &oc, PCTypeMap &con);

    /// Destructor
    virtual ~PCTypeDefHandler() throw();


protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(SumoXMLTag element,
                        const SUMOSAXAttributes &attrs) throw(ProcessError);
    //@}


protected:
    /// the options (program settings)
    OptionsCont &myOptions;

    /// The type map to fill
    PCTypeMap &myContainer;


private:
    /// invalidated copy constructor
    PCTypeDefHandler(const PCTypeDefHandler &src);

    /// invalidated assignment operator
    PCTypeDefHandler &operator=(const PCTypeDefHandler &src);

};


#endif

/****************************************************************************/

