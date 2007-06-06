/****************************************************************************/
/// @file    WeightsHandler.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 30 Mar 2007
/// @version $Id: WeightsHandler.h 3486 2007-02-19 13:38:00 +0100 (Mo, 19 Feb 2007) behrisch $
///
// An XML-handler for network weights
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
#ifndef WeightsHandler_h
#define WeightsHandler_h


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
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class RONet;
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class WeightsHandler
 * @brief An XML-handler for network weights
 */
class WeightsHandler : public SUMOSAXHandler
{
public:
    /**
     * @class EdgeFloatTimeLineRetriever
     * @brief Interface for a class which obtains read weights for named edges
     */
    class EdgeFloatTimeLineRetriever
    {
    public:
        /// Constructor
        EdgeFloatTimeLineRetriever()
        { }

        /// Destructor
        virtual ~EdgeFloatTimeLineRetriever()
        { }

        /// Adds a weight for a givenedge and time period
        virtual void addEdgeWeight(const std::string &id,
                                   SUMOReal val, SUMOTime beg, SUMOTime end) = 0;

    };

    /**
     * @class ToRetrieveDefinition
     * @brief Complete definition about what shall be retrieved and where to store it
     */
    class ToRetrieveDefinition
    {
    public:
        /// Constructor
        ToRetrieveDefinition(const std::string &elementName,
                             const std::string &attributeName, bool edgeBased,
                             EdgeFloatTimeLineRetriever &destination);

        /// Destructor
        ~ToRetrieveDefinition();

    public:
        /// The element name that shall be parsed
        XMLCh* myElementName;

        /// The attribute name that shall be parsed
        XMLCh* myAttributeName;

        /// The class that shall be called when new data is avaiable
        EdgeFloatTimeLineRetriever &myDestination;

        /// Information whether edge values shall be used (lane value if false)
        bool myAmEdgeBased;

        /// aggregated value over the lanes read within the current edge
        SUMOReal myAggValue;

        /// The number of lanes read for the current edge
        size_t myNoLanes;

        /// Name of the attribute readbale for humans
        std::string myMMLAttributeName;

        /// Information whether the attribute has been found for the current edge
        bool myHadAttribute;

    };

    /// Constructor
    WeightsHandler(const std::vector<ToRetrieveDefinition*> &defs,
                   const std::string &file);

    /// Constructor
    WeightsHandler(ToRetrieveDefinition *def,
                   const std::string &file);

    /// Destructor
    ~WeightsHandler();

protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(SumoXMLTag element, const std::string &name,
                        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(SumoXMLTag element, const std::string &name,
                      const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(SumoXMLTag element, const std::string &name);

private:
    /// Parses the efforts of a lane for the previously read times
    void tryParse(const Attributes &attrs, bool isEdge);

private:
    /// List of definitions what shall be read and where stored while parsing the file
    std::vector<ToRetrieveDefinition*> myDefinitions;


    /// the begin of the time period that is currently processed
    SUMOTime myCurrentTimeBeg;

    /// the end of the time period that is currently processed
    SUMOTime myCurrentTimeEnd;

    /// the edge the is currently being processed
    std::string myCurrentEdgeID;


private:
    /// we made the copy constructor invalid
    WeightsHandler(const WeightsHandler &src);

    /// we made the assignment operator invalid
    WeightsHandler &operator=(const WeightsHandler &src);

};


#endif

/****************************************************************************/

