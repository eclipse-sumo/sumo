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
/// @file    GNEGenericData.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for generic datas
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/GNEHierarchicalElement.h>
#include <netedit/elements/GNEPathElement.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/common/Parameterised.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEDataInterval;
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEGenericData : public GNEAttributeCarrier, public GNEHierarchicalElement, public GUIGlObject, public GNEPathElement, public Parameterised {

public:
    /// @brief default Constructor
    GNEGenericData(SumoXMLTag tag, GNENet* net);

    /**@brief Constructor
     * @param[in] tag generic data Tag (edgeData, laneData, etc.)
     * @param[in] dataIntervalParent pointer to data interval parent
     * @param[in] parameters parameters map
     */
    GNEGenericData(const SumoXMLTag tag, GNEDataInterval* dataIntervalParent,
                   const Parameterised::Map& parameters);

    /// @brief Destructor
    virtual ~GNEGenericData();

    /// @brief methods to retrieve the elements linked to this dataSet
    /// @{

    /// @brief get GNEHierarchicalElement associated with this genericData
    GNEHierarchicalElement* getHierarchicalElement() override;

    /// @brief get GNEMoveElement associated with this genericData
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this genericData
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this genericData (constant)
    const Parameterised* getParameters() const override;

    /// @brief get GUIGlObject associated with this genericData
    GUIGlObject* getGUIGlObject() override;

    /// @brief get GUIGlObject associated with this genericData (constant)
    const GUIGlObject* getGUIGlObject() const override;

    /// @}

    /// @brief check if current generic data is visible
    virtual bool isGenericDataVisible() const = 0;

    /// @brief get data interval parent
    GNEDataInterval* getDataIntervalParent() const;

    // @brief draw attribute
    void drawAttribute(const PositionVector& shape) const;

    /// @brief Returns element position in view
    virtual Position getPositionInView() const = 0;

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

    /// @name members and functions relative to write data sets into XML
    /// @{
    /**@brief write data set element into a xml file
     * @param[in] device device in which write parameters of data set element
     */
    virtual void writeGenericData(OutputDevice& device) const = 0;

    /// @brief check if current data set is valid to be written into XML (by default true, can be reimplemented in children)
    virtual bool isGenericDataValid() const;

    /// @brief return a string with the current data set problem (by default empty, can be reimplemented in children)
    virtual std::string getGenericDataProblem() const;

    /// @brief fix data set problem (by default throw an exception, has to be reimplemented in children)
    virtual void fixGenericDataProblem();
    /// @}

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief delete element
    void deleteGLObject();

    /// @brief select element
    void selectGLObject();

    /// @brief update GLObject (geometry, ID, etc.)
    void updateGLObject();

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{

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

    /// @}

    /// @name inherited from GNEPathElement
    /// @{

    /// @brief check if path element is selected
    bool isPathElementSelected() const;

    /// @}

protected:
    /// @brief dataInterval Parent
    GNEDataInterval* myDataIntervalParent;

    /// @brief draw filtered attribute
    void drawFilteredAttribute(const GUIVisualizationSettings& s, const PositionVector& laneShape, const std::string& attribute, const GNEDataInterval* dataIntervalParent) const;

    /// @brief check if attribute is visible in inspect, delete or select mode
    bool isVisibleInspectDeleteSelect() const;

    /// @brief replace the first parent edge
    void replaceFirstParentEdge(const std::string& value);

    /// @brief replace the last parent edge
    void replaceLastParentEdge(const std::string& value);

    /// @brief replace the first parent TAZElement
    void replaceParentTAZElement(const int index, const std::string& value);

    /// @brief get partial ID
    std::string getPartialID() const;

private:
    /// @brief Invalidated copy constructor.
    GNEGenericData(const GNEGenericData&) = delete;

    /// @brief Invalidated assignment operator.
    GNEGenericData& operator=(const GNEGenericData&) = delete;
};

/****************************************************************************/
