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
/// @file    VehicleEngineHandler.h
/// @author  Michele Segata
/// @date    4 Feb 2015
///
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <utils/xml/GenericSAXHandler.h>
#include "EngineParameters.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * SAX handler used to parse engine parameters
 */
class VehicleEngineHandler : public GenericSAXHandler {
public:
    /**
     * Constructor
     *
     * @param[in] toLoad id of the vehicle to be loaded
     */
    VehicleEngineHandler(const std::string& toLoad);

    /** @brief Destructor */
    virtual ~VehicleEngineHandler();

    const EngineParameters& getEngineParameters() {
        return engineParameters;
    }

protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * In dependence to the obtained type, an appropriate parsing
     *  method is called ("addEdge" if an edge encounters, f.e.).
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(int element, const SUMOSAXAttributes& attrs);

    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(int element);
    //@}

    /**
     * Load the mapping between engine rpm and output power in terms of linear
     * function, i.e., slope and intercept
     */
    void loadEngineModelData(const XERCES_CPP_NAMESPACE::Attributes& attrs);

private:
    //vehicle type to load
    std::string vehicleToLoad;
    //skip loading of current vehicle data
    bool skip;
    //current loaded gear
    int currentGear;
    //where to store loaded data
    EngineParameters engineParameters;
    //vector of gear ratios
    std::vector<double> gearRatios;

    /**
     * @enum EngineXMLTag
     * @brief Numbers representing engine-XML - element names
     * @see GenericSAXHandler
     */
    enum EngineXMLTag {
        ENGINE_TAG_NOTHING,
        ENGINE_TAG_VEHICLES,
        ENGINE_TAG_VEHICLE,
        ENGINE_TAG_GEARS,
        ENGINE_TAG_GEAR,
        ENGINE_TAG_GEAR_DIFFERENTIAL,
        ENGINE_TAG_MASS,
        ENGINE_TAG_WHEELS,
        ENGINE_TAG_DRAG,
        ENGINE_TAG_ENGINE,
        ENGINE_TAG_ENGINE_POWER,
        ENGINE_TAG_SHIFTING,
        ENGINE_TAG_BRAKES
    };


    /**
     * @enum EngineXMLAttr
     * @brief Numbers representing engine-XML - attributes
     * @see GenericSAXHandler
     */
    enum EngineXMLAttr {
        ENGINE_ATTR_NOTHING,
        ENGINE_ATTR_ID,
        ENGINE_ATTR_GEAR_N,
        ENGINE_ATTR_GEAR_RATIO,
        ENGINE_ATTR_MASS,
        ENGINE_ATTR_MASS_FACTOR,
        ENGINE_ATTR_WHEELS_DIAMETER,
        ENGINE_ATTR_WHEELS_FRICTION,
        ENGINE_ATTR_WHEELS_CR1,
        ENGINE_ATTR_WHEELS_CR2,
        ENGINE_ATTR_DRAG_CAIR,
        ENGINE_ATTR_DRAG_SECTION,
        ENGINE_ATTR_ENGINE_TYPE,
        ENGINE_ATTR_ENGINE_EFFICIENCY,
        ENGINE_ATTR_ENGINE_CYLINDERS,
        ENGINE_ATTR_ENGINE_MINRPM,
        ENGINE_ATTR_ENGINE_MAXRPM,
        ENGINE_ATTR_ENGINE_TAU_EX,
        ENGINE_ATTR_ENGINE_TAU_BURN,
        ENGINE_ATTR_ENGINE_POWER_RPM,
        ENGINE_ATTR_ENGINE_POWER_HP,
        ENGINE_ATTR_ENGINE_POWER_KW,
        ENGINE_ATTR_ENGINE_POWER_SLOPE,
        ENGINE_ATTR_ENGINE_POWER_INTERCEPT,
        ENGINE_ATTR_SHIFTING_RPM,
        ENGINE_ATTR_SHIFTING_DELTARPM,
        ENGINE_ATTR_BRAKES_TAU
    };

    /// The names of engine-XML elements (for passing to GenericSAXHandler)
    static StringBijection<int>::Entry engineTags[];

    /// The names of engine-XML attributes (for passing to GenericSAXHandler)
    static StringBijection<int>::Entry engineAttrs[];


private:
    /// @brief invalidated copy constructor
    VehicleEngineHandler(const VehicleEngineHandler& s);

    /// @brief invalidated assignment operator
    const VehicleEngineHandler& operator=(const VehicleEngineHandler& s);

};
