/****************************************************************************/
/// @file    GNELane.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id: GNELane.h 4695 2015-08-17 10:31:16Z erdm_ja $
///
// A class for visualizing Lane geometry (adapted from GUILaneWrapper)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNELane_h
#define GNELane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/gui/globjects/GUIGlObject.h>
#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectPopupMenu;
class PositionVector;
class GNEEdge;
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNELane
 * @brief This lane is powered by an underlying GNEEdge and basically knows how
 * to draw itself
 */
class GNELane : public GUIGlObject, public GNEAttributeCarrier {
public:
    /** @brief Constructor
     * @param[in] idStorage The storage of gl-ids to get the one for this lane representation from
     * @param[in] the edge this lane belongs to
     * @param[in] the index of this lane
     */
    GNELane(GNEEdge& edge, const unsigned int index);


    /// @brief Destructor
    virtual ~GNELane() ;


    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app,
                                       GUISUMOAbstractView& parent) ;


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app,
            GUISUMOAbstractView& parent) ;


    /** @brief Returns underlying parent edge
     *
     * @return The underlying GNEEdge
     */
    GNEEdge& getParentEdge() {
        return myParentEdge;
    };


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const ;


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const ;
    //@}

    const PositionVector& getShape() const;
    const std::vector<SUMOReal>& getShapeRotations() const;
    const std::vector<SUMOReal>& getShapeLengths() const;

    ///@brief returns the boundry (including lanes)
    Boundary getBoundary() const;

    ///@brief update pre-computed geometry information
    //  @note: must be called when geometry changes (i.e. junction moved)
    void updateGeometry();

    unsigned int getIndex() {
        return myIndex;
    }

    //@name inherited from GNEAttributeCarrier
    //@{
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    bool isValid(SumoXMLAttr key, const std::string& value);
    //@}

    void setSpecialColor(const RGBColor* color) {
        mySpecialColor = color;
    }

private:
    /// The Edge that to which this lane belongs
    GNEEdge& myParentEdge;

    /// The index of this lane
    const unsigned int myIndex;

    /// @name computed only once (for performance) in updateGeometry()
    //@{
    /// The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;
    //@}

    /// @brief optional special color
    const RGBColor* mySpecialColor;

private:
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNELane(const GNELane&);

    /// @brief Invalidated assignment operator.
    GNELane& operator=(const GNELane&);

    //@brief draw lane markings
    void drawMarkings(const bool& selectedEdge, SUMOReal scale) const;

    // drawing methods
    void drawLinkNo() const;
    void drawTLSLinkNo() const;
    void drawLinkRules() const;
    void drawArrows() const;
    void drawLane2LaneConnections() const;

    // @brief return value for lane coloring according to the given scheme
    SUMOReal getColorValue(size_t activeScheme) const;

    /// @brief whether to draw this lane as a railway
    bool drawAsRailway(const GUIVisualizationSettings& s) const;

    /* @brief draw crossties for railroads 
     * @todo: XXX This duplicates the code of GUILane::drawCrossties and needs to be */
    void drawCrossties(SUMOReal length, SUMOReal spacing, SUMOReal halfWidth) const;
};


#endif

/****************************************************************************/

