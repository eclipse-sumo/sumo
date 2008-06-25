/****************************************************************************/
/// @file    NIXMLTypesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id:NIXMLTypesHandler.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for edge type information stored in XML
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
#ifndef NIXMLTypesHandler_h
#define NIXMLTypesHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBTypeCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIArcView_Loader
 * @brief Importer for edge type information stored in XML
 *
 * This SAX-handler parses edge type information and stores it in the given
 *  container.
 */
class NIXMLTypesHandler : public SUMOSAXHandler
{
public:
    /** @brief Constructor
     *
     * @param[in, filled] tc The type container to fill
     */
    NIXMLTypesHandler(NBTypeCont &tc);


    /// @brief Destructor
    ~NIXMLTypesHandler() throw();


protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag; Parses edge type information
     *
     * Tries to parse a type. If the type can be parsed, it is stored within
     *  "myTypeCont". Otherwise an error is generated. No exception is thrown
     *  so that the parsing continues and further errors can be reported.
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails (not used herein)
     * @policy No exception in order to allow further processing
     */
    void myStartElement(SumoXMLTag element,
                        const SUMOSAXAttributes &attrs) throw(ProcessError);
    //@}


private:
    /// @brief The type container to fill
    NBTypeCont &myTypeCont;

    /// @brief Information whether one edge with a function-attribute occured and was reported
    bool myHaveReportedAboutFunctionDeprecation;


private:
    /** @brief invalid copy constructor */
    NIXMLTypesHandler(const NIXMLTypesHandler &s);

    /** @brief invalid assignment operator */
    NIXMLTypesHandler &operator=(const NIXMLTypesHandler &s);


};


#endif

/****************************************************************************/

