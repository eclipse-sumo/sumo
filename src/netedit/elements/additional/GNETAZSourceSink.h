/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNETAZSourceSink.h
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEAdditional.h"

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/GNEHierarchicalElement.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNETAZSourceSink : public GNEAttributeCarrier, public GNEHierarchicalElement {

public:
    /// @brief default Constructor
    GNETAZSourceSink(SumoXMLTag sourceSinkTag, GNENet* net);

    /**@brief Constructor
     * @param[in] sourceSinkTag Child Tag (Either SUMO_TAG_TAZSOURCE or SUMO_TAG_TAZINK)
     * @param[in] parent pointer to TAZ of this TAZSourceSinks belongs
     * @param[in] edge Edge of this TAZ Child belongs
     * @param[in] departWeight depart weight of this TAZ child
     */
    GNETAZSourceSink(SumoXMLTag sourceSinkTag, GNEAdditional* TAZParent, GNEEdge* edge, const double departWeight);

    /// @brief destructor
    ~GNETAZSourceSink();

    /// @brief methods to retrieve the elements linked to this TAZSourceSink
    /// @{

    /// @brief get GNEMoveElement associated with this TAZSourceSink
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this TAZSourceSink
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this TAZSourceSink (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /// @brief get GNEHierarchicalElement associated with this AttributeCarrier
    GNEHierarchicalElement* getHierarchicalElement();

    /// @brief write TAZ sourceSink
    void writeTAZSourceSink(OutputDevice& device) const;

    /// @brief get weight
    double getWeight() const;

    /// @name Function related with graphics
    /// @{
    /// @brief get GUIGlObject associated with this AttributeCarrier
    GUIGlObject* getGUIGlObject() override;

    /// @brief get GUIGlObject associated with this AttributeCarrier (constant)
    const GUIGlObject* getGUIGlObject() const override;

    /// @brief update pre-computed geometry information
    void updateGeometry() override;

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw from contour (green)
    bool checkDrawFromContour() const override;

    /// @brief check if draw from contour (magenta)
    bool checkDrawToContour() const override;

    /// @brief check if draw related contour (cyan)
    bool checkDrawRelatedContour() const override;

    /// @brief check if draw over contour (orange)
    bool checkDrawOverContour() const override;

    /// @brief check if draw delete contour (pink/white)
    bool checkDrawDeleteContour() const override;

    /// @brief check if draw delete contour small (pink/white)
    bool checkDrawDeleteContourSmall() const override;

    /// @brief check if draw select contour (blue)
    bool checkDrawSelectContour() const override;

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const override;

    /// @}

    /// @brief inherited from GNEAttributeCarrier
    /// @{

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in double format
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    Position getAttributePosition(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in positionVector format
     * @param[in] key The attribute key
     * @return positionVector with the value associated to key
     */
    PositionVector getAttributePositionVector(SumoXMLAttr key) const override;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     * @param[in] net optionally the GNENet to inform about gui updates
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) override;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    bool isValid(SumoXMLAttr key, const std::string& value) override;

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const override;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const override;

    /// @}

protected:
    /// @brief depart Weight
    double myWeight = 0;

private:
    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNETAZSourceSink(const GNETAZSourceSink&) = delete;

    /// @brief Invalidated assignment operator
    GNETAZSourceSink& operator=(const GNETAZSourceSink&) = delete;
};
