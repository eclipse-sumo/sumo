/****************************************************************************/
/// @file    GNERouteProbe.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2016
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
#ifndef GNERouteProbe_h
#define GNERouteProbe_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNERouteProbe
 * ------------
 */
class GNERouteProbe : public GNEAdditional {
public:
    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] edge edge in which this routeProbe is placed
     * @param[in] frequency The frequency in which to report the distribution
     * @param[in] filename The file for generated output
     * @param[in] begin The time at which to start generating output
     * @param[in] blocked set initial blocking state of item
     */
    GNERouteProbe(const std::string& id, GNEViewNet* viewNet, GNEEdge *edge, int frequency, const std::string& filename, int begin, bool blocked);

    /// @brief Destructor
    ~GNERouteProbe();

    /// @brief change the position of the RouteProbe geometry 
    void moveAdditional(SUMOReal, SUMOReal, GNEUndoList*);

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved)
    void updateGeometry();

    /// @brief Returns position of RouteProbe in view
    Position getPositionInView() const;

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device, const std::string &);

    /// @brief get edge in which the RouteProbe is placed
    GNEEdge* getEdge() const;

    /// @brief remove reference to edge
    /// @note this function will be called automatically in destructor of GNEEdge
    void removeEdgeReference();

    /// @brief get filename of RouteProbe
    std::string getFilename() const;

    /// @brief get frequency of RouteProbe
    int getFrequency() const;

    /// @brief get begin of RouteProbe
    int getBegin() const;

    /// @brief set filename of RouteProbe
    void setFilename(std::string filename);

    /// @brief set frequency of RouteProbe
    void setFrequency(int frequency);

    /// @brief set begin of RouteProbe
    void setBegin(int begin);

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
    /// @brief edge of routeProbe
    GNEEdge *myEdge;

    /// @brief Frequency of RouteProbe
    int myFrequency;

    /// @brief filename of RouteProbe
    std::string myFilename;

    /// @brief begin of rerouter
    int myBegin;

    /// @brief route probe logo offset
    Position myRouteProbeLogoOffset;

    /// @brief number of lanes of edge (To improve efficiency)
    int numberOfLanes;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNERouteProbe(const GNERouteProbe&);

    /// @brief Invalidated assignment operator.
    GNERouteProbe& operator=(const GNERouteProbe&);
};

#endif

/****************************************************************************/