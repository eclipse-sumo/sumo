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
/// @file    GNEAttributeCarrier.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// Abstract Base class for gui objects which carry attributes
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/GNEReferenceCounter.h>
#include <utils/foxtools/fxheader.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEHierarchicalElement;
class GNELane;
class GNENet;
class GNETagProperties;
class GNEUndoList;
class GUIGlObject;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEAttributeCarrier
 *
 * Abstract Base class for gui objects which carry attributes
 * inherits from GNEReferenceCounter for convenience
 */
class GNEAttributeCarrier : public GNEReferenceCounter {

    /// @brief declare friend class
    friend class GNEChange_Attribute;
    friend class GNEChange_ToggleAttribute;
    friend class GNEAttributesEditorType;

public:

    /**@brief Constructor
     * @param[in] tag SUMO Tag assigned to this type of object
     * @param[in] net GNENet in which this AttributeCarrier is stored
     * @param[in] filename file in which this AttributeCarrier is stored
     * @param[in] isTemplate flag to mark this AttributeCarrier as template
     */
    GNEAttributeCarrier(const SumoXMLTag tag, GNENet* net, const std::string& filename, const bool isTemplate);

    /// @brief Destructor
    virtual ~GNEAttributeCarrier();

    /// @brief get ID (all Attribute Carriers have one)
    const std::string getID() const;

    /// @brief get pointer to net
    GNENet* getNet() const;

    /// @brief get filename in which save this AC
    const std::string& getFilename() const;

    /// @brief change defaultFilename (only used in SavingFilesHandler)
    void changeDefaultFilename(const std::string& file);

    /// @brief select attribute carrier using GUIGlobalSelection
    void selectAttributeCarrier();

    /// @brief unselect attribute carrier using GUIGlobalSelection
    void unselectAttributeCarrier();

    /// @brief check if attribute carrier is selected
    bool isAttributeCarrierSelected() const;

    /// @brief check if attribute carrier must be drawn using selecting color.
    bool drawUsingSelectColor() const;

    /// @brief get GNEHierarchicalElement associated with this AttributeCarrier
    virtual GNEHierarchicalElement* getHierarchicalElement() = 0;

    /// @name Function related front elements
    /// @{

    /// @brief mark for drawing front
    void markForDrawingFront();

    /// @brief unmark for drawing front
    void unmarkForDrawingFront();

    /// @brief check if this AC is marked for drawing front
    bool isMarkedForDrawingFront() const;

    /// @brief draw element in the given layer, or in front if corresponding flag is enabled
    void drawInLayer(const double typeOrLayer, const double extraOffset = 0) const;

    /// @}

    /// @name Function related with grid (needed for elements that aren't always in grid)
    /// @{
    /// @brief mark if this AC was inserted in grid or not
    void setInGrid(bool value);

    /// @brief check if this AC was inserted in grid
    bool inGrid() const;

    /// @}

    /// @name Function related with graphics (must be implemented in all children)
    /// @{
    /// @brief get GUIGlObject associated with this AttributeCarrier
    virtual GUIGlObject* getGUIGlObject() = 0;

    /// @brief get GUIGlObject associated with this AttributeCarrier (constant)
    virtual const GUIGlObject* getGUIGlObject() const = 0;

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @}

    /// @name Function related with contourdrawing (can be implemented in children)
    /// @{

    /// @brief check if draw inspect contour (black/white)
    bool checkDrawInspectContour() const;

    /// @brief check if draw front contour (green/blue)
    bool checkDrawFrontContour() const;

    /// @brief check if draw from contour (green)
    virtual bool checkDrawFromContour() const = 0;

    /// @brief check if draw from contour (magenta)
    virtual bool checkDrawToContour() const = 0;

    /// @brief check if draw related contour (cyan)
    virtual bool checkDrawRelatedContour() const = 0;

    /// @brief check if draw over contour (orange)
    virtual bool checkDrawOverContour() const = 0;

    /// @brief check if draw delete contour (pink/white)
    virtual bool checkDrawDeleteContour() const = 0;

    /// @brief check if draw delete contour small (pink/white)
    virtual bool checkDrawDeleteContourSmall() const = 0;

    /// @brief check if draw select contour (blue)
    virtual bool checkDrawSelectContour() const = 0;

    /// @brief check if draw move contour (red)
    virtual bool checkDrawMoveContour() const = 0;

    /// @}

    /// @brief reset attribute carrier to their default values
    void resetDefaultValues(const bool allowUndoRedo);

    /// @name Functions related with attributes (must be implemented in all children)
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /* @brief method for check if new value for certain attribute is valid
     * @param[in] key The attribute key
     * @param[in] value The new value
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;

    /* @brief method for enable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    virtual void enableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for disable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    virtual void disableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    virtual bool isAttributeEnabled(SumoXMLAttr key) const;

    /* @brief method for check if the value for certain attribute is computed (for example, due a network recomputing)
     * @param[in] key The attribute key
     */
    virtual bool isAttributeComputed(SumoXMLAttr key) const;

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool hasAttribute(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    virtual std::string getPopUpID() const = 0;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    virtual std::string getHierarchyName() const = 0;

    /// @}

    /// @name Function related with parameters
    /// @{
    /// @brief get parameters map
    virtual const Parameterised::Map& getACParametersMap() const = 0;

    /// @brief get parameters
    template<typename T>
    T getACParameters() const;

    /// @brief set parameters (string)
    void setACParameters(const std::string& parameters, GNEUndoList* undoList);

    /// @brief set parameters (map)
    void setACParameters(const std::vector<std::pair<std::string, std::string> >& parameters, GNEUndoList* undoList);

    /// @brief set parameters (string vector)
    void setACParameters(const Parameterised::Map& parameters, GNEUndoList* undoList);

    /// @brief add (or update attribute) key and attribute
    void addACParameters(const std::string& key, const std::string& attribute, GNEUndoList* undoList);

    /// @brief remove keys
    void removeACParametersKeys(const std::vector<std::string>& keepKeys, GNEUndoList* undoList);

    /// @}

    /* @brief method for return an alternative value for disabled attributes. Used only in GNEFrames
     * @param[in] key The attribute key
     */
    std::string getAlternativeValueForDisabledAttributes(SumoXMLAttr key) const;

    /// @brief method for getting the attribute in the context of object selection
    virtual std::string getAttributeForSelection(SumoXMLAttr key) const;

    /// @brief get tag assigned to this object in string format
    const std::string& getTagStr() const;

    /// @brief get FXIcon associated to this AC
    FXIcon* getACIcon() const;

    /// @brief check if this AC is template
    bool isTemplate() const;

    /// @brief get tagProperty associated with this Attribute Carrier
    const GNETagProperties* getTagProperty() const;

    /// @name parse functions
    /// @{

    /// @brief true if a value of type T can be parsed from string
    template<typename T>
    static bool canParse(const std::string& string);

    /// @brief parses a value of type T from string (used for basic types: int, double, bool, etc.)
    template<typename T>
    static T parse(const std::string& string);

    /**@brief true if a value of type T can be parsed from string (requieres network)
     * @note checkConsecutivity doesn't check connectivity trought connections
     */
    template<typename T>
    static bool canParse(const GNENet* net, const std::string& value, const bool checkConsecutivity);

    /// @brief parses a complex value of type T from string (use for list of edges, list of lanes, etc.)
    template<typename T>
    static T parse(const GNENet* net, const std::string& value);

    /// @brief parses a list of specific Attribute Carriers into a string of IDs
    template<typename T>
    static std::string parseIDs(const std::vector<T>& ACs);

    /// @}

    /// @name Certain attributes and ACs (for example, connections) can be either loaded or guessed. The following static variables are used to remark it.
    /// @{

    /// @brief feature is still unchanged after being loaded (implies approval)
    static const std::string FEATURE_LOADED;

    /// @brief feature has been reguessed (may still be unchanged be we can't tell (yet)
    static const std::string FEATURE_GUESSED;

    /// @brief feature has been manually modified (implies approval)
    static const std::string FEATURE_MODIFIED;

    /// @brief feature has been approved but not changed (i.e. after being reguessed)
    static const std::string FEATURE_APPROVED;

    /// @brief lane start
    static const std::string LANE_START;

    /// @brief lane end
    static const std::string LANE_END;

    /// @}

    /// @brief true value in string format (used for comparing boolean values in getAttribute(...))
    static const std::string TRUE_STR;

    /// @brief true value in string format(used for comparing boolean values in getAttribute(...))
    static const std::string FALSE_STR;

protected:
    /// @brief reference to tagProperty associated with this attribute carrier
    const GNETagProperties* myTagProperty;

    /// @brief pointer to net
    GNENet* myNet = nullptr;

    /// @brief boolean to check if this AC is selected (more quickly as checking GUIGlObjectStorage)
    bool mySelected = false;

    /// @brief boolean to check if drawn this AC over other elements
    bool myDrawInFront = false;

    /// @brief boolean to check if this AC is in grid
    bool myInGrid = false;

    /// @brief filename in which save this AC
    std::string myFilename;

    /// @brief boolean to check if center this element after creation
    bool myCenterAfterCreation = true;

    /// @brief whether the current object is a template object (used for edit attributes)
    const bool myIsTemplate = false;

    /// @brief method for enable or disable the attribute and nothing else (used in GNEChange_ToggleAttribute)
    virtual void toggleAttribute(SumoXMLAttr key, const bool value);

    /// @name Functions related with common attributes
    /// @{
    /* @brief method for getting the common attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getCommonAttribute(const Parameterised* parameterised, SumoXMLAttr key) const;

    /* @brief method for setting the common attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setCommonAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for check if new value for certain common attribute is valid
     * @param[in] key The attribute key
     * @param[in] value The new value
     */
    bool isCommonValid(SumoXMLAttr key, const std::string& value) const;

    /// @brief method for setting the common attribute and nothing else (used in GNEChange_Attribute)
    void setCommonAttribute(Parameterised* parameterised, SumoXMLAttr key, const std::string& value);

    /// @}

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEAttributeCarrier(const GNEAttributeCarrier&) = delete;

    /// @brief Invalidated assignment operator
    GNEAttributeCarrier& operator=(const GNEAttributeCarrier& src) = delete;
};
