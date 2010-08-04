/**********************************************************************************************
 * File adapted by Piotr Woznica from the code of RONetHandler.h written by Daniel Krajzewicz
 * see RONetHandler.h/cpp - GenericSAXHandler.h/cpp
 **********************************************************************************************/
#ifndef AGACTIVITYGENHANDLER_H
#define AGACTIVITYGENHANDLER_H


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
#include <utils/common/UtilExceptions.h>
#include "city/AGCity.h"
#include "city/AGBusLine.h"


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;


// ===========================================================================
// class definitions
// ===========================================================================
class AGActivityGenHandler : public SUMOSAXHandler {
public:
    /** @brief Constructor
     *
     * @param[in] net The City instance to fill
     */
    AGActivityGenHandler(AGCity &city, RONet *net);


    /// @brief Destructor
    virtual ~AGActivityGenHandler() throw();


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
    virtual void myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError);
    //@}

protected:
    /// @name called from myStartElement
    //@{

    /**
     * Parses attributes from an "general"-element in city file.
     */
    void parseGeneralCityInfo(const SUMOSAXAttributes &attrs);
    /*void parseInhabitants(const SUMOSAXAttributes &attrs);
    void parseHouseholds(const SUMOSAXAttributes &attrs);
    void parseChildrenAgeLimit(const SUMOSAXAttributes &attrs);
    void parseRetirementAgeLimit(const SUMOSAXAttributes &attrs);
    void parseCarRate(const SUMOSAXAttributes &attrs);
    void parseUnemployment(const SUMOSAXAttributes &attrs);
    void parseFootDistanceLimit(const SUMOSAXAttributes &attrs);
    void parseIncomingTraffic(const SUMOSAXAttributes &attrs);
    void parseOutgoingTraffic(const SUMOSAXAttributes &attrs);*/

    /**
     * parse parameters, all are optional, default values are given.
     */
    void parseParameters(const SUMOSAXAttributes &attrs);

    /**
     * streets are extracted: it fills the streets list of the City object
     * an additional computation is needed to normalize the number of inhabitants (and workpositions) in each street
     */
    void parseStreets(const SUMOSAXAttributes &attrs);

    /**
     * parse city entrances: the gates of the city, used for incoming and outgoing traffic
     */
    void parseCityGates(const SUMOSAXAttributes &attrs);

    /**
     * extraction of work opening and closing hours
     */
    void parseWorkHours();
    void parseOpeningHour(const SUMOSAXAttributes &attrs);
    void parseClosingHour(const SUMOSAXAttributes &attrs);

    /**
     * School extraction
     */
    void parseSchools();
    void parseSchool(const SUMOSAXAttributes &attrs);

    /**
     * Bus stations and Bus extraction
     * an additional computation have to be done to create buses' names and buses in the return way
     */
    void parseBusStation(const SUMOSAXAttributes &attrs);
    void parseBusLine(const SUMOSAXAttributes &attrs);
    void parseStation(const SUMOSAXAttributes &attrs);
    void parseFrequency(const SUMOSAXAttributes &attrs);

    /**
     * population and children accompaniment bracket extraction
     * then households and all population data will be computable
     */
    void parsePopulation();
    //void parseChildrenAccompaniment();
    void parseBracket(const SUMOSAXAttributes &attrs);


protected:
    /// @brief The city to store the information into
    AGCity &myCity;

    /// @brief The name of the object that is currently processed
    std::string myCurrentObject;
    AGBusLine* currentBusLine;

    /// @brief The loaded network
    RONet *net;

    /** @brief An indicator whether the next edge shall be read (internal edges are not read by now) */
    bool myProcess;

    /// @brief The object used to build of edges of the desired type
    //ROAbstractEdgeBuilder &myEdgeBuilder;

    /// @brief Whether deprecated usage of the "vclass" attribute was reported already
    bool myHaveWarnedAboutDeprecatedVClass;


private:
    /// @brief Invalidated copy constructor
    AGActivityGenHandler(const AGActivityGenHandler &src);

    /// @brief Invalidated assignment operator
    AGActivityGenHandler &operator=(const AGActivityGenHandler &src);

};


#endif

/****************************************************************************/

