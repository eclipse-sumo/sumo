/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDataSet.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data elements
/****************************************************************************/
#ifndef GNEDataSet_h
#define GNEDataSet_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/elements/GNEHierarchicalParentElements.h>
#include <netedit/elements/GNEHierarchicalChildElements.h>
#include <netedit/GNEGeometry.h>
#include <utils/common/Parameterised.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <netbuild/NBVehicle.h>
#include <netbuild/NBEdge.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEAdditional;
class GNEDataSet;
class GNENetworkElement;
class GNEEdge;
class GNELane;
class GNEJunction;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEDataSet
 * @brief An Element which don't belongs to GNENet but has influency in the simulation
 */
class GNEDataSet : public GNEAttributeCarrier {

public:
    /**@brief Constructor
     * @param[in] id Gl-id of the data element element (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this data element element belongs
     */
    GNEDataSet(const std::string& id, GNEViewNet* viewNet);

    /**@brief Constructor
     * @param[in] dataElementParent pointer to parent data element pointer (used to generate an ID)
     * @param[in] viewNet pointer to GNEViewNet of this data element element belongs
     */
    GNEDataSet(GNEDataSet* dataElementParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEDataSet();

    /// @name members and functions relative to write data elements into XML
    /// @{
    /**@brief writte data element element into a xml file
     * @param[in] device device in which write parameters of data element element
     */
    virtual void writeDataSet(OutputDevice& device) const = 0;

    /// @brief check if current data element is valid to be writed into XML (by default true, can be reimplemented in children)
    virtual bool isDataSetValid() const;

    /// @brief return a string with the current data element problem (by default empty, can be reimplemented in children)
    virtual std::string getDataSetProblem() const;

    /// @brief fix data element problem (by default throw an exception, has to be reimplemented in children)
    virtual void fixDataSetProblem();
    /// @}

    /// @brief Returns a pointer to GNEViewNet in which data element element is located
    GNEViewNet* getViewNet() const;

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /// @brief select attribute carrier using GUIGlobalSelection
    virtual void selectAttributeCarrier(bool changeFlag = true) = 0;

    /// @brief unselect attribute carrier using GUIGlobalSelection
    virtual void unselectAttributeCarrier(bool changeFlag = true) = 0;

    /// @brief check if attribute carrier is selected
    bool isAttributeCarrierSelected() const;

    /// @brief check if attribute carrier must be drawn using selecting color.
    bool drawUsingSelectColor() const;

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    virtual double getAttributeDouble(SumoXMLAttr key) const = 0;

    /**@brief method for setting the attribute and letting the object perform data element changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /**@brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;

    /* @brief method for enable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    virtual void enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) = 0;

    /* @brief method for disable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    virtual void disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) = 0;

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    virtual bool isAttributeEnabled(SumoXMLAttr key) const = 0;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    virtual std::string getPopUpID() const = 0;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    virtual std::string getHierarchyName() const = 0;
    /// @}

protected:
    /// @brief The GNEViewNet this data element element belongs
    GNEViewNet* myViewNet;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEDataSet(const GNEDataSet&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDataSet& operator=(const GNEDataSet&) = delete;
};


#endif

/****************************************************************************/

