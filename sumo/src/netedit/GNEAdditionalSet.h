/****************************************************************************/
/// @file    GNEAdditionalSet.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2016
/// @version $Id$
///
/// A abstract class for representation of additionalSet elements
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
#ifndef GNEAdditionalSet_h
#define GNEAdditionalSet_h


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

class GUIGLObjectPopupMenu;
class PositionVector;
class GNEAdditional;
class GNENet;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditionalSet
 * @brief An Element wich group additionalSet elements
 */
class GNEAdditionalSet : public GNEAdditional {
public:

    /**@brief Constructor.
     * @param[in] id Gl-id of the additionalSet element (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this additionalSet element belongs
     * @param[in] tag Type of xml tag that define the additionalSet element (SUMO_TAG_DETECTORE3, SUMO_TAG_REROUTER, etc...)
     * @param[in] blocked enable or disable blocking. By default additionalSet element isn't blocked (i.e. value is false)
     * @param[in] additionalChilds
     * @param[in] edgeChilds
     * @param[in] laneChilds
     */
    GNEAdditionalSet(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, bool blocked = false,
                     std::vector<GNEAdditional*> additionalChilds = std::vector<GNEAdditional*>(),
                     std::vector<GNEEdge*> edgeChilds = std::vector<GNEEdge*>(),
                     std::vector<GNELane*> laneChilds = std::vector<GNELane*>());

    /// @brief Destructor
    ~GNEAdditionalSet();

    /// @brief update pre-computed geometry information
    //  @note: must be called when geometry changes (i.e. lane moved)
    virtual void updateGeometry() = 0;

    /**@brief writte additionalSet element into a xml file
     * @param[in] device device in which write parameters of additionalSet element
     */
    virtual void writeAdditional(OutputDevice& device, const std::string& currentDirectory) = 0;

    /**@brief add additional element to this set
     * @param[in] additionalSet pointer to GNEadditionalSet element to add
     * @return true if was sucesfully added, false in other case
     */
    bool addAdditionalChild(GNEAdditional* additional);

    /**@brief remove additional element to this set
     * @param[in] additionalSet pointer to GNEadditionalSet element to remove
     * @return true if was sucesfully removed, false in other case
     */
    bool removeAdditionalChild(GNEAdditional* additional);

    /**@brief add edge element to this set
     * @param[in] edgeSet pointer to GNEEdge element to add
     * @param[in] position position of edge in which connection will be placed
     * @return true if was sucesfully added, false in other case
     */
    bool addEdgeChild(GNEEdge* edge);

    /**@brief remove edge element to this set
     * @param[in] edgeSet pointer to GNEEdge element to remove
     * @return true if was sucesfully removed, false in other case
     */
    bool removeEdgeChild(GNEEdge* edge);

    /**@brief add lane element to this set
     * @param[in] laneSet pointer to GNELane element to add
     * @param[in] position position of edge in which connection will be placed
     * @return true if was sucesfully added, false in other case
     */
    bool addLaneChild(GNELane* lane);

    /**@brief remove lane element to this set
     * @param[in] laneSet pointer to GNELane element to remove
     * @return true if was sucesfully removed, false in other case
     */
    bool removeLaneChild(GNELane* lane);

    /// @brief get number of additional childs of this additionalSet
    int getNumberOfAdditionalChilds() const;

    /// @brief get number of edge childs of this additionalSet
    int getNumberOfEdgeChilds() const;

    /// @brief get number of lane childs of this additionalSet
    int getNumberOfLaneChilds() const;

    /// @brief get ids of additional childs
    std::vector<std::string> getAdditionalChildIds() const;

    /// @brief get ids of edge childs
    std::vector<std::string> getEdgeChildIds() const;

    /// @brief get ids of lane childs
    std::vector<std::string> getLaneChildIds() const;

    /// @brief set edge childs
    /// @note preexisting edge childs will be erased
    void setEdgeChilds(std::vector<GNEEdge*> edges);

    /// @brief set lane childs
    /// @note preexisting lane childs will be erased
    void setLaneChilds(std::vector<GNELane*> lanes);

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    virtual const std::string& getParentName() const = 0;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform additionalSet changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;
    /// @}

protected:
    /// @brief struct for edge childs
    struct edgeChild {
        GNEEdge* edge;
        std::vector<Position> positionsOverLanes;
        std::vector<SUMOReal> rotationsOverLanes;
    };

    /// @brief struct for lane childs
    struct laneChild {
        GNELane* lane;
        Position positionOverLane;
        SUMOReal rotationOverLane;
    };

    /// @brief typedef for containers
    typedef std::vector<GNEAdditional*> childAdditionals;
    typedef std::vector<edgeChild> childEdges;
    typedef std::vector<laneChild> childLanes;

    /// @brief list of additional childs (Position and rotations is derived from additional)
    std::vector<GNEAdditional*> myChildAdditionals;

    /// @brief map of child edges and their positions and rotation
    childEdges myChildEdges;

    /// @brief list of child lanes and their positions and rotation
    childLanes myChildLanes;

    /// @brief map to keep the middle position of connection
    std::map<GUIGlObject*, Position> myConnectionMiddlePosition;

    /// @brief update connections.
    /// @note must be called at end of function updateGeometry() of additionalSet
    void updateConnections();

    /// @brief draw connections.
    /// @note must be called at end of function drawGl(...) of additionalSet
    void drawConnections() const;

    /**@brief writte children of this additionalSet
     * @param[in] device device in which write parameters of additional element
     * @param[in] currentDirectory current directory in which this additional are writted
     */
    void writeAdditionalChildrens(OutputDevice& device, const std::string& currentDirectory);

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEAdditionalSet(const GNEAdditionalSet&);

    /// @brief Invalidated assignment operator.
    GNEAdditionalSet& operator=(const GNEAdditionalSet&);
};


#endif
