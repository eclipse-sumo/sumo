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
/// @file    GNEAdditional.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2016
///
// A abstract class for representation of additional elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/GNEContour.h>
#include <netedit/elements/GNEHierarchicalElement.h>
#include <netedit/elements/GNEPathElement.h>
#include <netedit/elements/moving/GNEMoveElement.h>
#include <utils/gui/div/GUIGeometry.h>
#include <utils/gui/globjects/GUIGlObject.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNENetworkElement;
class GUIGLObjectPopupMenu;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAdditional : public GNEAttributeCarrier, public GNEHierarchicalElement, public GUIGlObject, public GNEPathElement {

public:
    /// @brief declare friend class
    friend class GNEAdditionalListed;
    friend class GNEAdditionalSquared;

    /**@brief Constructor
     * @param[in] id Gl-id of the additional element (Must be unique)
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] filename file in which this AttributeCarrier is stored
     * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
     * @param[in] additionalName Additional name
     */
    GNEAdditional(const std::string& id, GNENet* net, const std::string& filename,
                  SumoXMLTag tag, const std::string& additionalName);

    /**@brief Constructor for additional with parents
     * @param[in] additionalParent pointer to additional parent
     * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
     * @param[in] additionalName Additional name
     */
    GNEAdditional(GNEAdditional* additionalParent, SumoXMLTag tag, const std::string& additionalName);

    /// @brief Destructor
    ~GNEAdditional();

    /// @brief methods to retrieve the elements linked to this additional
    /// @{

    /// @brief get GNEHierarchicalElement associated with this additional
    GNEHierarchicalElement* getHierarchicalElement() override;

    /// @brief get GUIGlObject associated with this additional
    GUIGlObject* getGUIGlObject() override;

    /// @brief get GUIGlObject associated with this additional (constant)
    const GUIGlObject* getGUIGlObject() const override;

    /// @}

    /// @brief Returns the name of the object (default "")
    virtual const std::string getOptionalName() const;

    /// @brief obtain additional geometry
    const GUIGeometry& getAdditionalGeometry() const;

    /// @brief set special color
    void setSpecialColor(const RGBColor* color);

    /// @brief reset additional contour
    void resetAdditionalContour();

    /// @name members and functions relative to write additionals into XML
    /// @{

    /**@brief write additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    virtual void writeAdditional(OutputDevice& device) const = 0;

    /// @brief check if current additional is valid to be written into XML (by default true, can be reimplemented in children)
    virtual bool isAdditionalValid() const = 0;

    /// @brief return a string with the current additional problem (by default empty, can be reimplemented in children)
    virtual std::string getAdditionalProblem() const = 0;

    /// @brief fix additional problem (by default throw an exception, has to be reimplemented in children)
    virtual void fixAdditionalProblem() = 0;

    /// @}

    /// @name functions related with geometry
    /// @{

    /**@brief open Additional Dialog
     * @note: if additional needs an additional dialog, this function has to be implemented in childrens (see GNERerouter and GNEVariableSpeedSign)
     * @throw invalid argument if additional doesn't have an additional Dialog
     */
    virtual void openAdditionalDialog();

    /// @brief Returns position of additional in view
    virtual Position getPositionInView() const = 0;

    /// @brief return exaggeration associated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;

    /// @brief Returns the boundary to which the view shall be centered in order to show the object
    Boundary getCenteringBoundary() const;

    /// @brief update centering boundary (implies change in RTREE)
    virtual void updateCenteringBoundary(const bool updateGrid) = 0;

    /// @brief split geometry
    virtual void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) = 0;

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
    virtual GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief Returns the additional name
    const std::string& getOptionalAdditionalName() const;

    /// @brief check if element is locked
    bool isGLObjectLocked() const;

    /// @brief mark element as front element
    void markAsFrontElement();

    /// @brief delete element
    void deleteGLObject();

    /// @brief select element
    void selectGLObject();

    /// @brief update GLObject (geometry, ID, etc.)
    void updateGLObject();

    /// @}

    /// @name inherited from GNEPathElement
    /// @{

    /// @brief compute pathElement
    virtual void computePathElement();

    /// @brief check if path element is selected
    bool isPathElementSelected() const;

    /**@brief Draws partial object over lane
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment lane segment
     * @param[in] offsetFront front offset
     */
    virtual void drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const;

    /**@brief Draws partial object over junction
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment junction segment
     * @param[in] offsetFront front offset
     */
    virtual void drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const;

    /// @brief get first path lane
    GNELane* getFirstPathLane() const;

    /// @brief get last path lane
    GNELane* getLastPathLane() const;

    /// @}

    /// @brief draw parent and child lines
    void drawParentChildLines(const GUIVisualizationSettings& s, const RGBColor& color, const bool onlySymbols = false) const;

    /// @brief draw up geometry point
    void drawUpGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const Position& pos,
                             const double rot, const RGBColor& baseColor, const bool ignoreShift = false) const;

    /// @brief draw down geometry point
    void drawDownGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const Position& pos,
                               const double rot, const RGBColor& baseColor, const bool ignoreShift = false) const;

    /// @brief draw left geometry point
    void drawLeftGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const Position& pos,
                               const double rot, const RGBColor& baseColor, const bool ignoreShift = false) const;

    /// @brief draw right geometry point
    void drawRightGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const Position& pos,
                                const double rot, const RGBColor& baseColor, const bool ignoreShift = false) const;

    /// @brief check if the given lanes are consecutive
    static bool areLaneConsecutives(const std::vector<GNELane*>& lanes);

    /// @brief check if the given lanes are connected
    static bool areLaneConnected(const std::vector<GNELane*>& lanes);

protected:
    /// @brief Additional Boundary (used only by additionals placed over grid)
    Boundary myAdditionalBoundary;

    /// @brief geometry to be precomputed in updateGeometry(...)
    GUIGeometry myAdditionalGeometry;

    /// @brief variable used for draw additional contours
    GNEContour myAdditionalContour;

    /// @brief name of additional
    std::string myAdditionalName;

    /// @brief pointer to special color (used for drawing Additional with a certain color, mainly used for selections)
    const RGBColor* mySpecialColor = nullptr;

    /// @brief write common additional attributes
    void writeAdditionalAttributes(OutputDevice& device) const;

    /// @name Functions relative to change values in setAttribute(...)
    /// @{

    /// @brief check if a new additional ID is valid
    bool isValidAdditionalID(const std::string& value) const;

    /// @brief check if a new additional ID is valid
    bool isValidAdditionalID(const std::vector<SumoXMLTag>& tags, const std::string& value) const;

    /// @brief check if a new detector ID is valid
    bool isValidDetectorID(const std::string& value) const;

    /// @brief check if a new detector ID is valid
    bool isValidDetectorID(const std::vector<SumoXMLTag>& tags, const std::string& value) const;

    /// @brief set additional ID
    void setAdditionalID(const std::string& newID);

    /// @}

    /// @brief draw additional ID
    void drawAdditionalID(const GUIVisualizationSettings& s) const;

    /// @brief draw additional name
    void drawAdditionalName(const GUIVisualizationSettings& s) const;

    /// @brief replace additional parent edges
    void replaceAdditionalParentEdges(const std::string& value);

    /// @brief replace additional parent lanes
    void replaceAdditionalParentLanes(const std::string& value);

    /// @brief replace additional child edges
    void replaceAdditionalChildEdges(const std::string& value);

    /// @brief replace additional child lanes
    void replaceAdditionalChildLanes(const std::string& value);

    /// @brief replace additional parent
    void replaceAdditionalParent(SumoXMLTag tag, const std::string& value, const int parentIndex);

    /// @brief replace demand element parent
    void replaceDemandElementParent(SumoXMLTag tag, const std::string& value, const int parentIndex);

    /// @brief shift lane index
    void shiftLaneIndex();

    /// @brief calculate perpendicular line between lane parents
    void calculatePerpendicularLine(const double endLaneposition);

    /// @brief draw demand element children
    void drawDemandElementChildren(const GUIVisualizationSettings& s) const;

    /// @name JuPedSim values
    /// @{

    /// @brief get JuPedSim type
    static std::string getJuPedSimType(SumoXMLTag tag);

    /// @brief get JuPedSim color
    static RGBColor getJuPedSimColor(SumoXMLTag tag);

    /// @brief get JuPedSim fill
    static bool getJuPedSimFill(SumoXMLTag tag);

    /// @brief get JuPedSim color
    static double getJuPedSimLayer(SumoXMLTag tag);

    /// @}

    /// @name calculate contours
    /// @{

    /// @brief calculate contour for polygons
    void calculateContourPolygons(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                  const double layer, const double exaggeration, const bool filledShape) const;

    /// @}

private:
    /**@brief check restriction with the number of children
     * @throw ProcessError if is called without be reimplemented in child class
     */
    virtual bool checkChildAdditionalRestriction() const;

    /// @brief draw geometry point
    void drawSemiCircleGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                     const Position& pos, const double rot, const RGBColor& baseColor,
                                     const double fromAngle, const double toAngle, const bool ignoreShift) const;

    /// @brief Invalidated copy constructor.
    GNEAdditional(const GNEAdditional&) = delete;

    /// @brief Invalidated assignment operator.
    GNEAdditional& operator=(const GNEAdditional&) = delete;
};
