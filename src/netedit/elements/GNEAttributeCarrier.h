/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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

#include <utils/foxtools/fxheader.h>
#include <netedit/GNEReferenceCounter.h>

#include "GNETagProperties.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEUndoList;
class GUIGlObject;
class GNEHierarchicalElement;
class GNELane;
class GNEEdge;

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
    friend class GNEChange_EnableAttribute;
    friend class GNEFrameAttributeModules;
    friend class GNEAttributesCreatorRow;
    friend class GNEFlowEditor;

public:

    /**@brief Constructor
     * @param[in] tag SUMO Tag assigned to this type of object
     * @param[in] net GNENet in which this AttributeCarrier is stored
     */
    GNEAttributeCarrier(const SumoXMLTag tag, GNENet* net);

    /// @brief Destructor
    virtual ~GNEAttributeCarrier();

    /// @brief get ID (all Attribute Carriers have one)
    const std::string getID() const;

    /// @brief get pointer to net
    GNENet* getNet() const;

    /// @brief select attribute carrier using GUIGlobalSelection
    void selectAttributeCarrier(const bool changeFlag = true);

    /// @brief unselect attribute carrier using GUIGlobalSelection
    void unselectAttributeCarrier(const bool changeFlag = true);

    /// @brief check if attribute carrier is selected
    bool isAttributeCarrierSelected() const;

    /// @brief check if attribute carrier must be drawn using selecting color.
    bool drawUsingSelectColor() const;

    /// @brief get GNEHierarchicalElement associated with this AttributeCarrier
    virtual GNEHierarchicalElement* getHierarchicalElement() = 0;

    /// @name Function related with graphics (must be implemented in all children)
    /// @{
    /// @brief get GUIGlObject associated with this AttributeCarrier
    virtual GUIGlObject* getGUIGlObject() = 0;

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @}

    /// @brief reset attribute carrier to their default values
    void resetDefaultValues();

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
    bool hasAttribute(SumoXMLAttr key) const {
        return myTagProperty.hasAttribute(key);
    }

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
    const GNETagProperties& getTagProperty() const;

    /// @brief get tagProperty associated to the given tag
    static const GNETagProperties& getTagProperty(SumoXMLTag tag);

    /// @brief get tagProperties associated to the given GNETagProperties::TagType (NETWORKELEMENT, ADDITIONALELEMENT, VEHICLE, etc.)
    static const std::vector<GNETagProperties> getTagPropertiesByType(const int tagPropertyCategory);

    /// @brief true if a value of type T can be parsed from string
    template<typename T>
    static bool canParse(const std::string& string) {
        try {
            GNEAttributeCarrier::parse<T>(string);
        } catch (EmptyData&) {
            // general
            return false;
        } catch (FormatException&) {
            // numbers, time, boolean, colors
            return false;
        }
        return true;
    }

    /// @brief parses a value of type T from string (used for basic types: int, double, bool, etc.)
    template<typename T>
    static T parse(const std::string& string);

    /// @brief true if a value of type T can be parsed from string
    template<typename T>
    static bool canParse(GNENet* net, const std::string& value, bool report) {
        try {
            parse<T>(net, value);
        } catch (FormatException& exception) {
            if (report) {
                WRITE_WARNING(exception.what())
            }
            return false;
        }
        return true;
    }

    /// @brief parses a complex value of type T from string (use for list of edges, list of lanes, etc.)
    template<typename T>
    static T parse(GNENet* net, const std::string& value);

    /// @brief parses a list of specific Attribute Carriers into a string of IDs
    template<typename T>
    static std::string parseIDs(const std::vector<T>& ACs);

    /// @brief check if lanes are consecutives
    static bool lanesConsecutives(const std::vector<GNELane*>& lanes);

    /// @brief write machine readable attribute help to file
    static void writeAttributeHelp();

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
    /// @}

    /// @brief max number of attributes allowed for every tag
    static const size_t MAXNUMBEROFATTRIBUTES;

    /// @brief empty parameter maps (used by ACs without parameters)
    static const Parameterised::Map PARAMETERS_EMPTY;

    /// @brief true value in string format (used for comparing boolean values in getAttribute(...))
    static const std::string True;

    /// @brief true value in string format(used for comparing boolean values in getAttribute(...))
    static const std::string False;

protected:
    /// @brief reference to tagProperty associated with this attribute carrier
    const GNETagProperties& myTagProperty;

    /// @brief pointer to net
    GNENet* myNet;

    /// @brief boolean to check if this AC is selected (instead of GUIGlObjectStorage)
    bool mySelected;

    /// @brief whether the current object is a template object (not drawn in the view)
    bool myIsTemplate;

    /// @brief method for enable or disable the attribute and nothing else (used in GNEChange_EnableAttribute)
    virtual void toggleAttribute(SumoXMLAttr key, const bool value);

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief reset attributes to their default values without undo-redo (used in GNEFrameAttributeModules)
    void resetAttributes();

    /// @brief fill Attribute Carriers
    static void fillAttributeCarriers();

    /// @brief fill network elements
    static void fillNetworkElements();

    /// @brief fill additional elements
    static void fillAdditionalElements();

    /// @brief fill shape elements
    static void fillShapeElements();

    /// @brief fill TAZ elements
    static void fillTAZElements();

    /// @brief fill Wire elements
    static void fillWireElements();

    /// @brief fill demand elements
    static void fillDemandElements();

    /// @brief fill vehicle elements
    static void fillVehicleElements();

    /// @brief fill stop elements
    static void fillStopElements();

    /// @brief fill waypoint elements
    static void fillWaypointElements();

    /// @brief fill person elements
    static void fillPersonElements();

    /// @brief fill person plan trips
    static void fillPersonPlanTrips();

    /// @brief fill person plan walks
    static void fillPersonPlanWalks();

    /// @brief fill person plan rides
    static void fillPersonPlanRides();

    /// @brief fill stopPerson elements
    static void fillStopPersonElements();

    /// @brief fill container elements
    static void fillContainerElements();

    /// @brief fill container transport elements
    static void fillContainerTransportElements();

    /// @brief fill container tranship elements
    static void fillContainerTranshipElements();

    /// @brief fill container stop elements
    static void fillContainerStopElements();

    /// @brief fill common vehicle attributes (used by vehicles, trips, routeFlows and flows)
    static void fillCommonVehicleAttributes(SumoXMLTag currentTag);

    /// @brief fill common flow attributes (used by flows, routeFlows and personFlows)
    static void fillCommonFlowAttributes(SumoXMLTag currentTag, SumoXMLAttr perHour);

    /// @brief fill Car Following Model of Vehicle/Person Types
    static void fillCarFollowingModelAttributes(SumoXMLTag currentTag);

    /// @brief fill Junction Model Attributes of Vehicle/Person Types
    static void fillJunctionModelAttributes(SumoXMLTag currentTag);

    /// @brief fill Junction Model Attributes of Vehicle/Person Types
    static void fillLaneChangingModelAttributes(SumoXMLTag currentTag);

    /// @brief fill common person attributes (used by person and personFlows)
    static void fillCommonPersonAttributes(SumoXMLTag currentTag);

    /// @brief fill common container attributes (used by container and containerFlows)
    static void fillCommonContainerAttributes(SumoXMLTag currentTag);

    /// @brief fill stop person attributes
    static void fillCommonStopAttributes(SumoXMLTag currentTag, const bool waypoint);

    /// @brief fill Data elements
    static void fillDataElements();

    /// @brief fill stop person attributes
    static void fillCommonMeanDataAttributes(SumoXMLTag currentTag);

    /// @brief map with the tags properties
    static std::map<SumoXMLTag, GNETagProperties> myTagProperties;

    /// @brief Invalidated copy constructor.
    GNEAttributeCarrier(const GNEAttributeCarrier&) = delete;

    /// @brief Invalidated assignment operator
    GNEAttributeCarrier& operator=(const GNEAttributeCarrier& src) = delete;
};
