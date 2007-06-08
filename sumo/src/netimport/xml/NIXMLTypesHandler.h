/****************************************************************************/
/// @file    NIXMLTypesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Used to parse the XML-descriptions of types given in a XML-format
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
class NIXMLTypesHandler : public SUMOSAXHandler
{
public:
    /// standard constructor
    NIXMLTypesHandler(NBTypeCont &tc);

    /// destructor
    ~NIXMLTypesHandler();

protected:
    /// called on the opening of a tag; inherited
    void myStartElement(SumoXMLTag element, const std::string &name,
                        const Attributes &attrs) throw();

    /// called after reading intermediate characters; inherited
    void myCharacters(SumoXMLTag element, const std::string &name,
                      const std::string &chars) throw();

    /// called on the closing of a tag; inherited
    void myEndElement(SumoXMLTag element, const std::string &name) throw();

private:
    /// The type container to fill
    NBTypeCont &myTypeCont;

private:
    /** invalid copy constructor */
    NIXMLTypesHandler(const NIXMLTypesHandler &s);

    /** invalid assignment operator */
    NIXMLTypesHandler &operator=(const NIXMLTypesHandler &s);

};


#endif

/****************************************************************************/

