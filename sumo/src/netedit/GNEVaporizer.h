/****************************************************************************/
/// @file    GNEVaporizer.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEVaporizer_h
#define GNEVaporizer_h


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
 * @class GNEVaporizer
 * ------------
 */
class GNEVaporizer : public GNEAdditional {
public:
    /**@brief Constructor
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] edge edge in which this vaporizer is placed
     * @param[in] startTime start time of vaporizer
     * @param[in] end end time of vaporizer
     */
    GNEVaporizer(GNEViewNet* viewNet, const std::string& edge, double startTime, double end);

    /// @brief Destructor
    ~GNEVaporizer();

    /// @brief change the position of the RouteProbe geometry
    void moveAdditionalGeometry(double, double);

    /// @brief updated geometry changes in the attributes of additional
    void commmitAdditionalGeometryMoved(double, double, GNEUndoList*);

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved)
    void updateGeometry();

    /// @brief Returns position of Vaporizer in view
    Position getPositionInView() const;

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief remove reference to edge
    /// @note this function will be called automatically in destructor of GNEEdge
    void removeEdgeReference();

    /// @brief get start time
    double getStartTime() const;

    /// @brief get end
    double getEnd() const;

    /// @brief set start time
    void setStartTime(double startTime);

    /// @brief set end
    void setEndTime(double end);

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
    /// @brief start time of vaporizer
    double myStartTime;

    /// @brief end time in which this vaporizer is placed
    double myEnd;

    /// @brief number of lanes of edge (To improve efficiency)
    int myNumberOfLanes;

    /// @brief relative position regarding to other route probes
    int myRelativePosition;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEVaporizer(const GNEVaporizer&);

    /// @brief Invalidated assignment operator.
    GNEVaporizer& operator=(const GNEVaporizer&);
};

#endif

/****************************************************************************/
