/****************************************************************************/
/// @file    GNECalibrator.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
///
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
#ifndef GNECalibrator_h
#define GNECalibrator_h


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
// class declaration
// ===========================================================================

class GNERouteProbe;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibrator
 * ------------
 */
class GNECalibrator : public GNEAdditional {
public:
    /// @brief calibrator flow 
    struct CalibratorFlow {
        // Parameters of vehicles
        std::string type;
        std::string route;
        std::string color;
        std::string departLane;
        std::string departPos;
        std::string departSpeed;
        std::string arrivalLane;
        std::string arrivalPos;
        std::string arrivalSpeed;
        std::string line;
        int personNumber;
        int containerNumber;
        // Parameters of flows
        SUMOTime begin;
        SUMOTime end;
        SUMOReal vehsPerHour;
        SUMOReal period;
        SUMOReal probability;
        int number;
    };

    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] edge Lane of this StoppingPlace belongs
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position of the calibrator on the edge (Currently not used)
     * @param[in] frequency the aggregation interval in which to calibrate the flows
     * @param[in] output The output file for writing calibrator information
     * @param[in] flowValues values with the flow of calibrator
     * @param[in] blocked set initial blocking state of item
     */
    GNECalibrator(const std::string& id, GNEEdge* edge, GNEViewNet* viewNet, SUMOReal pos, SUMOTime frequency, const std::string& output, const std::map<std::string, CalibratorFlow> &flowValues, bool blocked);

    /// @brief Destructor
    ~GNECalibrator();

    /// @brief change the position of the calibrator geometry 
    void moveAdditional(SUMOReal, SUMOReal, GNEUndoList*);

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry c6hanges (i.e. lane moved)
    void updateGeometry();

    /// @brief Returns position of Calibrator in view
    Position getPositionInView() const;

    /// @brief open Calibrator Dialog
    void openAdditionalDialog();

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device, const std::string &);

    /// @brief get Calbratorflow values
    std::map<std::string, CalibratorFlow> getFlowValues() const;

    /// @brief set Calbratorflow values
    void setFlowValues(std::map<std::string, CalibratorFlow> calibratorFlowValues);

    /// @brief insert a new flow
    void insertFlow(const std::string & id, const CalibratorFlow &flow);

    /// @brief remove a existent flow
    void removeFlow(const std::string & id);
    
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
    /// @brief edge in which this calibrator is placed
    GNEEdge *myEdge;

    /// @brief Frequency of calibrator
    SUMOTime myFrequency;

    /// @brief output of calibrator
    std::string myOutput;

    /// @brief pointer to RouteProbe
    GNERouteProbe* myRouteProbe;

    /// @brief Calbratorflow values
    std::map<std::string, CalibratorFlow> myFlowValues;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNECalibrator(const GNECalibrator&);

    /// @brief Invalidated assignment operator.
    GNECalibrator& operator=(const GNECalibrator&);
};

#endif
/****************************************************************************/
