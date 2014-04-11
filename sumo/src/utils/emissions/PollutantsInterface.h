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
// class declarations
// ===========================================================================
class HelpersHBEFA;
class HelpersHBEFA3;
class HelpersPHEMlight;


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

    struct Emissions {
        SUMOReal CO2;
        SUMOReal CO;
        SUMOReal HC;
        SUMOReal fuel;
        SUMOReal NOx;
        SUMOReal PMx;

        Emissions(SUMOReal co2=0, SUMOReal co=0, SUMOReal hc=0, SUMOReal f=0, SUMOReal nox=0, SUMOReal pmx=0) : CO2(co2), CO(co), HC(hc), fuel(f), NOx(nox), PMx(pmx) {
        }

        void addScaled(const Emissions& a, const SUMOReal scale=1.) {
            CO2 += scale * a.CO2;
            CO += scale * a.CO;
            HC += scale * a.HC;
            fuel += scale * a.fuel;
            NOx += scale * a.NOx;
            PMx += scale * a.PMx;
        }
    };

    class Helper {
    public:
        Helper(std::string name) : myName(name) {}
        
        const std::string& getName() const {
            return myName;
        }
        virtual SUMOEmissionClass getClassByName(const std::string& eClass, const SUMOVehicleClass vc) {
            UNUSED_PARAMETER(vc);
            return myEmissionClassStrings.get(eClass);
        }
        const std::string getClassName(const SUMOEmissionClass c) const {
            return myName + "/" + myEmissionClassStrings.getString(c);
        }
        virtual bool isSilent(const SUMOEmissionClass c) {
            return (c & 0xffffffff & ~HEAVY_BIT) == 0;
        }
        virtual SUMOEmissionClass getClass(const SUMOEmissionClass base, const std::string& vClass, const std::string& fuel, const std::string& eClass, const double weight) const {
            UNUSED_PARAMETER(vClass);
            UNUSED_PARAMETER(fuel);
            UNUSED_PARAMETER(eClass);
            UNUSED_PARAMETER(weight);
            return base;
        }
        virtual std::string getAmitranVehicleClass(const SUMOEmissionClass c) const {
            UNUSED_PARAMETER(c);
            return "Passenger";
        }
        virtual std::string getFuel(const SUMOEmissionClass c) const {
            UNUSED_PARAMETER(c);
            return "Gasoline";
        }
        virtual int getEuroClass(const SUMOEmissionClass c) const {
            UNUSED_PARAMETER(c);
            return 0;
        }
        virtual SUMOReal getWeight(const SUMOEmissionClass c) const {
            UNUSED_PARAMETER(c);
            return -1.;
        }
        virtual SUMOReal compute(const SUMOEmissionClass c, const EmissionType e, const double v, const double a, const double slope) const = 0;
        virtual SUMOReal getMaxAccel(SUMOEmissionClass c, double v, double a, double slope) const {
            UNUSED_PARAMETER(c);
            UNUSED_PARAMETER(v);
            UNUSED_PARAMETER(a);
            UNUSED_PARAMETER(slope);
            return -1.;
        }
        void addAllClassesInto(std::vector<SUMOEmissionClass>& list) const {
            myEmissionClassStrings.addKeysInto(list);
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
    static SUMOEmissionClass getClassByName(const std::string& eClass, const SUMOVehicleClass vc=SVC_IGNORING);


    /** @brief Checks whether the string describes a known vehicle class
     * @param[in] eClass The string describing the vehicle emission class
     * @return whether it describes a valid emission class
     */
    static const std::vector<SUMOEmissionClass> getAllClasses();


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


    /** @brief Returns the amount of emitted pollutant given the vehicle type and state (in mg/s or ml/s for fuel)
     * @param[in] c The vehicle emission class
     * @param[in] e the type of emission (CO, CO2, ...)
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal compute(const SUMOEmissionClass c, const EmissionType e, const double v, const double a, const double slope);


    /** @brief Returns the amount of all emitted pollutants given the vehicle type and state (in mg/s or ml/s for fuel)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static Emissions computeAll(const SUMOEmissionClass c, const double v, const double a, const double slope);


    /** @brief Returns the amount of emitted pollutant given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] e the type of emission (CO, CO2, ...)
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefault(const SUMOEmissionClass c, const EmissionType e, const double v, const double a, const double slope, const SUMOReal tt);

private:
    /// @brief Instance of HBEFA2Helper which gets cleaned up automatically
    static HelpersHBEFA myHBEFA2Helper;
    /// @brief Instance of HBEFA3Helper which gets cleaned up automatically
    static HelpersHBEFA3 myHBEFA3Helper;
    /// @brief Instance of PHEMlightHelper which gets cleaned up automatically
    static HelpersPHEMlight myPHEMlightHelper;

};


#endif

/****************************************************************************/

