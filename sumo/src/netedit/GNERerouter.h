/****************************************************************************/
/// @file    GNERerouter.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNERerouter_h
#define GNERerouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditionalSet.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNERerouter
 * ------------
 */
class GNERerouter : public GNEAdditionalSet {
public:

    // ===========================================================================
    // class closingReroute
    // ===========================================================================

    class closingReroute {
    public:
        /// @brief constructor
        closingReroute(std::string closedEdgeId, std::vector<std::string> allowVehicles, std::vector<std::string> disallowVehicles);

        /// @brief destructor
        ~closingReroute();

        /// @brief insert an allow vehicle
        /// @throw ProcessError if allowed vehicle was already inserted
        void insertAllowVehicle(std::string vehicleid);

        /// @brief remove a previously inserted allow vehicle
        /// @throw ProcessError if allowed vehicle cannot be found in the container
        void removeAllowVehicle(std::string vehicleid);

        /// @brief insert a disallow vehicle
        /// @throw ProcessError if disallowed vehicle was already inserted
        void insertDisallowVehicle(std::string vehicleid);

        /// @brief remove a previously inserted disallow vehicle
        /// @throw ProcessError if disallowed vehicle cannot be found in the container
        void removeDisallowVehicle(std::string vehicleid);

        /// @brief get allow vehicles
        std::vector<std::string> getAllowVehicles() const;

        /// @brief get disallow vehicles
        std::vector<std::string> getDisallowVehicles() const;

        /// @brief get closed edge Id
        std::string getClosedEdgeId() const;

    private:
        /// @brief edge ID
        std::string myClosedEdgeId;

        /// @brief vector of allow vehicles
        std::vector<std::string> myAllowVehicles;

        /// @brief vector of disallow vehicles
        std::vector<std::string> myDisallowVehicles;
    };

    // ===========================================================================
    // class destProbReroute
    // ===========================================================================

    class destProbReroute {
    public:
        /// @brief constructor
        destProbReroute(std::string newDestinationId, SUMOReal probability);

        /// @brief destructor
        ~destProbReroute();

        /// @brief id of new edge destination
        std::string getNewDestinationId() const;

        /// @brief get probability
        SUMOReal getProbability() const;

        /// @brief set probability
        /// @throw InvalidArgument if probability isn't valid
        void setProbability(SUMOReal probability);

    private:
        /// @brief id of new edge destination
        std::string myNewDestinationId;

        /// @brief probability with which a vehicle will use the given edge as destination
        SUMOReal myProbability;
    };

    // ===========================================================================
    // class routeProbReroute
    // ===========================================================================

    class routeProbReroute {
    public:
        /// @brief constructor
        routeProbReroute(std::string newRouteId, SUMOReal probability);

        /// @brief destructor
        ~routeProbReroute();

        /// @brief get new route id
        std::string getNewRouteId() const;

        /// @brief get probability
        SUMOReal getProbability() const;

        /// @brief set probability
        /// @throw InvalidArgument if probability isn't valid
        void setProbability(SUMOReal probability);

    private:
        /// @brief id of new route
        std::string myNewRouteId;

        /// @brief probability with which a vehicle will use the given edge as destination
        SUMOReal myProbability;
    };

    // ===========================================================================
    // class rerouterInterval
    // ===========================================================================

    class rerouterInterval : public std::pair<SUMOTime, SUMOTime> {
    public:
        /// @brief constructor
        rerouterInterval(SUMOTime begin, SUMOTime end);

        /// @brief destructor
        ~rerouterInterval();

        /// @brief insert a new closing reroute
        /// @throw ProcessError if closing reroute was already inserted
        void insertClosingReroutes(closingReroute* cr);

        /// @brief remove a previously inserted closing reroute
        /// @throw ProcessError if closing reroute cannot be found in the container
        void removeClosingReroutes(closingReroute* cr);

        /// @brief insert destiny probability reroute
        /// @throw ProcessError if destiny probability reroute was already inserted
        void insertDestProbReroutes(destProbReroute* dpr);

        /// @brief remove a previously inserted destiny probability reroute
        /// @throw ProcessError if destiny probability reroute cannot be found in the container
        void removeDestProbReroutes(destProbReroute* dpr);

        /// @brief insert route probability reroute
        /// @throw ProcessError if route probability reroute was already inserted
        void insertRouteProbReroute(routeProbReroute* rpr);

        /// @brief remove a previously inserted route probability reroute
        /// @throw ProcessError if route probability reroute cannot be found in the container
        void removeRouteProbReroute(routeProbReroute* rpr);

        /// @brief get time begin
        SUMOTime getBegin() const;

        /// @brief get time end
        SUMOTime getEnd() const;

        /// @brief get closing reroutes
        std::vector<closingReroute*> getClosingReroutes() const;

        /// @brief get destiny probability reroutes
        std::vector<destProbReroute*> getDestProbReroutes() const;

        /// @brief get reoute probability reroutes
        std::vector<routeProbReroute*> getRouteProbReroutes() const;

    private:
        /// @brief vector with the closingReroutes
        std::vector<closingReroute*> myClosingReroutes;

        /// @brief vector with the destProbReroutes
        std::vector<destProbReroute*> myDestProbReroutes;

        /// @brief vector with the routeProbReroutes
        std::vector<routeProbReroute*> myRouteProbReroutes;
    };

    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position (center) of the rerouter in the map
     * @param[in] edges vector with the edges of rerotuer
     * @param[in] filename The path to the definition file
     * @param[in] probability The probability for vehicle rerouting
     * @param[in] off Whether the router should be inactive initially
     * @param[in] rerouterIntervals set with the rerouter intervals
     * @param[in] blocked set initial blocking state of item
     */
    GNERerouter(const std::string& id, GNEViewNet* viewNet, Position pos, std::vector<GNEEdge*> edges, const std::string& filename, SUMOReal probability, bool off, const std::set<rerouterInterval>& rerouterIntervals, bool blocked);

    /// @brief Destructor
    ~GNERerouter();

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved)
    void updateGeometry();

    /// @brief Returns position of Rerouter in view
    Position getPositionInView() const;

    /// @brief open GNERerouterDialog
    void openAdditionalDialog();

    /// @brief change the position of the rerouter geometry
    void moveAdditionalGeometry(SUMOReal offsetx, SUMOReal offsety);

    /// @brief updated geometry changes in the attributes of additional 
    void commmitAdditionalGeometryMoved(SUMOReal oldPosx, SUMOReal oldPosy, GNEUndoList* undoList);

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device, const std::string&);

    /// @brief add edge to rerouter
    /// @return true if was sucesfully inserted, false if is duplicated
    bool addEdge(GNEEdge* edge);

    /// @brief remove edge of rerouter
    /// @return true if was scuesfully removed, false if wasn't found
    bool removeEdge(GNEEdge* edge);

    /// @brief get filename of rerouter
    std::string getFilename() const;

    /// @brief get probability of rerouter
    SUMOReal getProbability() const;

    /// @brief get attribute to enable or disable inactive initially
    bool getOff() const;

    /// @brief set filename of rerouter
    void setFilename(std::string filename);

    /// @brief set probability of rerouter
    void setProbability(SUMOReal probability);

    /// @brief set attribute to enable or disable inactive initially
    void setOff(bool off);

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id

    const std::string& getParentName() const;
    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

protected:
    /// @brief filename of rerouter
    std::string myFilename;

    /// @brief probability of rerouter
    SUMOReal myProbability;

    /// @brief attribute to enable or disable inactive initially
    bool myOff;

    /// @brief set with the rerouterInterval
    std::set<rerouterInterval> myRerouterIntervals;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNERerouter(const GNERerouter&);

    /// @brief Invalidated assignment operator.
    GNERerouter& operator=(const GNERerouter&);
};

#endif

/****************************************************************************/
