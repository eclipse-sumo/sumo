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
    /** @brief Constructor
     * @param[in] oc The options used while loading the type map
     * @param[out] con The container to fill
     */
    PCTypeDefHandler(OptionsCont &oc, PCTypeMap &con) throw();


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
     * @todo Completely unsecure currently (invalid values may force abortion with no error message)
     */
    void myStartElement(SumoXMLTag element,
                        const SUMOSAXAttributes &attrs) throw(ProcessError);
    //@}


protected:
    /// @brief The options (program settings)
    OptionsCont &myOptions;

    /// @brief The type map to fill
    PCTypeMap &myContainer;


private:
    /** @brief invalid copy constructor */
    PCTypeDefHandler(const PCTypeDefHandler &src);

    /** @brief invalid assignment operator */
    PCTypeDefHandler &operator=(const PCTypeDefHandler &src);

};


#endif

/****************************************************************************/

