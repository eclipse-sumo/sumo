/****************************************************************************/
/// @file    GNECalibrator.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
    class GNECalibratorFlow {

    public:
        /// @briefdefault constructor
        GNECalibratorFlow(GNECalibrator* calibratorParent, std::string type, std::string route);

        /// @brief parameter constructor
        GNECalibratorFlow(GNECalibrator* calibratorParent, std::string type, std::string route, std::string color, std::string departLane, std::string departPos,
                          std::string departSpeed, std::string arrivalLane, std::string arrivalPos, std::string arrivalSpeed,
                          std::string line, int personNumber, int containerNumber, SUMOReal begin, SUMOReal end,
                          SUMOReal vehsPerHour, SUMOReal period, SUMOReal probability, int number);

        /// @brief destructor
        ~GNECalibratorFlow();

        /// @brief get pointer to calibrator parent
        GNECalibrator* getCalibratorParent() const;

        /// @brief get type of vehicle
        const std::string& getType() const;

        /// @brief get route in which this flow is used
        const std::string& getRoute() const;

        /// @brief get color of flow
        const std::string& getColor() const;

        /// @brief get depart lane
        const std::string& getDepartLane() const;

        /// @brief get depart position
        const std::string& getDepartPos() const;

        /// @brief get arrival speed
        const std::string& getDepartSpeed() const;

        /// @brief get arrival lane
        const std::string& getArrivalLane() const;

        /// @brief get arrival position
        const std::string& getArrivalPos() const;

        /// @brief get arrival speed
        const std::string& getArrivalSpeed() const;

        /// @brief get line of busStop/containerStop
        const std::string& getLine() const;

        /// @brief get number of persons
        int getPersonNumber() const;

        /// @brief get number of containers
        int getContainerNumber() const;

        /// @brief get begin time step
        SUMOReal getBegin() const;

        /// @brief get end time step
        SUMOReal getEnd() const;

        /// @brief get vehicles per hour
        SUMOReal getVehsPerHour() const;

        /// @brief get period of vehicle
        SUMOReal getPeriod() const;

        /// @brief get probability of vehicle
        SUMOReal getProbability() const;

        /// @brief get number of vehicles
        int getNumber() const;

        /**@brief set type of vehicle
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setType(std::string type);

        /**@brief set route in which this flow is used
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setRoute(std::string route);

        /**@brief set color of flow
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setColor(std::string color);

        /**@brief set depart lane
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setDepartLane(std::string departLane);

        /**@brief set depart position
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setDepartPos(std::string departPos);

        /**@brief set depart speed
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setDepartSpeed(std::string departSpeed);

        /**@brief set arrival lane
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setArrivalLane(std::string arrivalLane);

        /**@brief set arrival position
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setArrivalPos(std::string arrivalPos);

        /**@brief set arrival speed
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setArrivalSpeed(std::string arrivalSpeed);

        /**@brief set line of busStop/containerStop
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setLine(std::string line);

        /**@brief set number of persons
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setPersonNumber(int personNumber);

        /**@brief set number of container
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setContainerNumber(int containerNumber);

        /**@brief set begin step
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setBegin(SUMOReal begin);

        /**@brief set end step
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setEnd(SUMOReal end);

        /**@brief set vehicles per hour
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setVehsPerHour(SUMOReal vehsPerHour);

        /**@brief set period of vehicles
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setPeriod(SUMOReal period);

        /**@brief set probability of vehicle
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setProbability(SUMOReal probability);

        /**@brief set number of vehicles
         * @return true if was sucesfully set, or false if value isn't valid
         */
        bool setNumber(int number);

    private:
        /// @brief pointer to calibrator parent
        GNECalibrator* myCalibratorParent;

        /// @brief type of flow
        std::string myType;

        /// @brief route in which this flow is used
        std::string myRoute;

        /// @brief color of flow
        std::string myColor;

        /// @brief depart lane
        std::string myDepartLane;

        /// @brief depart position
        std::string myDepartPos;

        /// @brief depart speed
        std::string myDepartSpeed;

        /// @brief arrival lane
        std::string myArrivalLane;

        /// @brief arrival pos
        std::string myArrivalPos;

        /// @brief arrival speed
        std::string myArrivalSpeed;

        /// @brief line of bus/container stop
        std::string myLine;

        /// @brief number of person
        int myPersonNumber;

        /// @brief number of container
        int myContainerNumber;

        /// @brief time step begin
        SUMOReal myBegin;

        /// @brief time step end
        SUMOReal myEnd;

        /// @brief vehicles per hour
        SUMOReal myVehsPerHour;

        /// @brief period
        SUMOReal myPeriod;

        /// @brief probability
        SUMOReal myProbability;

        /// @brief number of vehicle
        int myNumber;

        /// @brief Invalidated copy constructor.
        GNECalibratorFlow(const GNECalibratorFlow&);

        /// @brief Invalidated assignment operator.
        GNECalibratorFlow& operator=(const GNECalibratorFlow&);
    };

    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] edge Lane of this StoppingPlace belongs
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position of the calibrator on the edge (Currently not used)
     * @param[in] frequency the aggregation interval in which to calibrate the flows
     * @param[in] output The output file for writing calibrator information
     * @param[in] flowValues values with the flow of calibrator
     */
    GNECalibrator(const std::string& id, GNEEdge* edge, GNEViewNet* viewNet, SUMOReal pos, SUMOReal frequency,
                  const std::string& output, const std::vector<GNECalibrator::GNECalibratorFlow*>& flowValues);

    /// @brief Destructor
    ~GNECalibrator();

    /// @brief change the position of the calibrator geometry
    void moveAdditionalGeometry(SUMOReal, SUMOReal);

    /// @brief updated geometry changes in the attributes of additional
    void commmitAdditionalGeometryMoved(SUMOReal, SUMOReal, GNEUndoList*);

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
    void writeAdditional(OutputDevice& device) const;

    /// @brief get Calbratorflow values
    std::vector<GNECalibrator::GNECalibratorFlow*> getFlowValues() const;

    /// @brief set Calbratorflow values
    void setFlowValues(std::vector<GNECalibrator::GNECalibratorFlow*> calibratorFlowValues);

    /// @brief insert a new flow
    void insertFlow(GNECalibratorFlow* flow);

    /// @brief remove a existent flow
    void removeFlow(GNECalibratorFlow* flow);

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
    /// @brief Frequency of calibrator
    SUMOReal myFrequency;

    /// @brief output of calibrator
    std::string myOutput;

    /// @brief pointer to RouteProbe
    GNERouteProbe* myRouteProbe;

    /// @brief Calbratorflow values
    std::vector<GNECalibratorFlow*> myFlowValues;

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
