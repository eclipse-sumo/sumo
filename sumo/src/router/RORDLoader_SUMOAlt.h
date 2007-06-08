/****************************************************************************/
/// @file    RORDLoader_SUMOAlt.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A SAX-handler for SUMO-route-alternatives
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
#ifndef RORDLoader_SUMOAlt_h
#define RORDLoader_SUMOAlt_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "RORDLoader_SUMOBase.h"


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class RORouteDef_Alternatives;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORDLoader_SUMOAlt
 * A SAX-Handler which parses SUMO-Route-Alternatives.
 */
class RORDLoader_SUMOAlt : public RORDLoader_SUMOBase
{
public:
    /// Constructor
    RORDLoader_SUMOAlt(ROVehicleBuilder &vb, RONet &net,
                       SUMOTime begin, SUMOTime end,
                       SUMOReal gawronBeta, SUMOReal gawronA, int maxRouteNumber,
                       const std::string &file="");

    /// Destructor
    ~RORDLoader_SUMOAlt();

protected:
    /// @name inherited from GenericSAXHandler
    //@{ 
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(SumoXMLTag element, const std::string &name,
                        const Attributes &attrs) throw();

    /** the user-implemented handler method for characters */
    void myCharacters(SumoXMLTag element, const std::string &name,
                      const std::string &chars) throw();

    /** the user-implemented handler method for a closing tag */
    void myEndElement(SumoXMLTag element, const std::string &name) throw();
    //@}

protected:
    /// Begins the parsing of the next route alternative in the file
    void startAlternative(const Attributes &attrs);

    /// Begins the parsing of a route alternative of the opened route
    void startRoute(const Attributes &attrs);

    /// Ends the processing of a route alternative
    void endAlternative();

private:
    /// The current route alternatives
    RORouteDef_Alternatives *myCurrentAlternatives;

    /// The costs of the current alternative
    SUMOReal myCost;

    /// The probability of the current alternative's usage
    SUMOReal myProbability;

    /// gawron beta - value
    SUMOReal myGawronBeta;

    /// gawron beta - value
    SUMOReal myGawronA;

    /// The maximum route alternatives number
    int myMaxRouteNumber;

private:
    /// we made the copy constructor invalid
    RORDLoader_SUMOAlt(const RORDLoader_SUMOAlt &src);

    /// we made the assignment operator invalid
    RORDLoader_SUMOAlt &operator=(const RORDLoader_SUMOAlt &src);

};


#endif

/****************************************************************************/

