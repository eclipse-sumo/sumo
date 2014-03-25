/****************************************************************************/
/// @file    PollutantsInterface.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 19.08.2013
/// @version $Id$
///
// Interface to capsulate different emission models
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef PollutantsInterface_h
#define PollutantsInterface_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <limits>
#include <cmath>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOVehicleClass.h>
#include "PHEMCEP.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PollutantsInterface
 * @brief Helper methods for PHEMlight-based emission computation
 */
class PollutantsInterface {
public:

    enum EmissionType { CO2, CO, HC, FUEL, NO_X, PM_X };

    class Helper {
    public:
        Helper::Helper(std::string name) : myName(name) {}
        
        const std::string& getName() const {
            return myName;
        }
        virtual SUMOEmissionClass getClassByName(const std::string& eClass) {
            return myEmissionClassStrings.get(eClass);
        }
        const std::string getClassName(const SUMOEmissionClass c) const {
            return myName + "/" + myEmissionClassStrings.getString(c);
        }
        virtual bool isSilent(const SUMOEmissionClass c) {
            return (c & 0xffffffff & ~HEAVY_BIT) == 1;
        }
        virtual SUMOEmissionClass getClass(const SUMOEmissionClass base, const std::string& vClass, const std::string& fuel, const std::string& eClass, const double weight) const {
            return base;
        }
        virtual std::string getAmitranVehicleClass(const SUMOEmissionClass c) const {
            return "Passenger";
        }
        virtual std::string getFuel(const SUMOEmissionClass c) const {
            return "Gasoline";
        }
        virtual int getEuroClass(const SUMOEmissionClass c) const {
            return 0;
        }
        virtual SUMOReal getWeight(const SUMOEmissionClass c) const {
            return -1.;
        }
        virtual SUMOReal compute(const SUMOEmissionClass c, const EmissionType e, const double v, const double a, const double slope) const = 0;
        virtual SUMOReal getMaxAccel(SUMOEmissionClass c, double v, double a, double slope) const {
            return -1.;
        }
    protected:
        const std::string myName;
        StringBijection<SUMOEmissionClass> myEmissionClassStrings;


};

    static Helper* myHelpers[];
    static const int ZERO_EMISSIONS = 0;
    static const int HEAVY_BIT = 1 << 15;

    /** @brief Checks whether the string describes a known vehicle class
     * @param[in] eClass The string describing the vehicle emission class
     * @return whether it describes a valid emission class
     */
    static SUMOEmissionClass getClassByName(std::string model, std::string eClass);


    /** @brief Checks whether the string describes a known vehicle class
     * @param[in] eClass The string describing the vehicle emission class
     * @return whether it describes a valid emission class
     */
    static std::string getName(const SUMOEmissionClass c);


    /** @brief Checks whether the emission class describes a bus, truck or similar vehicle
     * @param[in] c The vehicle emission class
     * @return whether it describes a heavy vehicle
     */
    static bool isHeavy(const SUMOEmissionClass c);


    /** @brief Checks whether the emission class describes an electric or similar silent vehicle
     * @param[in] c The vehicle emission class
     * @return whether it describes a silent vehicle
     */
    static bool isSilent(const SUMOEmissionClass c);


    /** @brief Returns the maximum possible acceleration
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The maximum possible acceleration
     */
    static SUMOReal getMaxAccel(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the emission class fittig the given parameters
     * @param[in] base The base emission class to derive from
     * @param[in] vClass The vehicle class description (like "truck")
     * @param[in] eClass The emission class description (like "Euro5")
     * @param[in] fuel The fuel type (like "Diesel")
     * @param[in] weight The weight in kg
     * @return The best fitting emission class related to the base
     */
    static SUMOEmissionClass getClass(const SUMOEmissionClass base, const std::string& vClass, const std::string& fuel, const std::string& eClass, const double weight);


    /** @brief Returns the vehicle class described by the given emission class
     * @param[in] c The vehicle emission class
     * @return The Amitran string describing the vehicle class
     */
    static std::string getAmitranVehicleClass(const SUMOEmissionClass c);


    /** @brief Returns the fuel type of the given emission class
     * @param[in] c The vehicle emission class
     * @return "Diesel", "Gasoline", "HybridDiesel", or "HybridGasoline"
     */
    static std::string getFuel(const SUMOEmissionClass c);


    /** @brief Returns the Euro norm described by the given emission class
     * @param[in] c The vehicle emission class
     * @return A value between 0 and 6 (inclusive)
     */
    static int getEuroClass(const SUMOEmissionClass c);


    /** @brief Returns a representative weight for the given emission class
     * see http://colombo-fp7.eu/deliverables/COLOMBO_D4.2_ExtendedPHEMSUMO_v1.7.pdf
     * @param[in] c The vehicle emission class
     * @return the weight in kg if it matters, 0 otherwise
     */
    static SUMOReal getWeight(const SUMOEmissionClass c);


    /** @brief Returns the amount of emitted CO given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of CO emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeCO(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of emitted CO2 given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of CO2 emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeCO2(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of emitted HC given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of HC emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeHC(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of emitted NOx given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of NOx emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeNOx(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of emitted PMx given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of PMx emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computePMx(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of consumed fuel given the vehicle type and state (in ml/s)
     *
     * As the general function returns mg/s, this implementation scales with 790 (average density of fuel)
     *
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of fuel consumed by the given vehicle class when moving with the given velocity and acceleration [ml/s]
     */
    static SUMOReal computeFuel(SUMOEmissionClass c, double v, double a, double slope);




    /** @brief Returns the amount of emitted CO given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of CO emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefaultCO(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


    /** @brief Returns the amount of emitted CO2 given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of CO2 emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefaultCO2(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


    /** @brief Returns the amount of emitted HC given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of HC emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefaultHC(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


    /** @brief Returns the amount of emitted NOx given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of NOx emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefaultNOx(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


    /** @brief Returns the amount of emitted PMx given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of PMx emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefaultPMx(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


    /** @brief Returns the amount of fuel given the vehicle type and default values for the state (in ml)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of fuel emitted by the given vehicle class [ml]
     */
    static SUMOReal computeDefaultFuel(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


};


#endif

/****************************************************************************/

