/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_ElecHybrid.cpp
/// @author  Jakub Sevcik (RICE)
/// @author  Jan Prikryl (RICE)
/// @date    2019-12-15
///
// The ElecHybrid device simulates internal electric parameters of an
// battery-assisted electric vehicle (typically a trolleybus), i.e. a vehicle
// that is being powered by overhead wires and has also a battery pack
// installed, that is being charged from the overhead wires.
/****************************************************************************/
#include <config.h>

#include <string.h> //due to strncmp
#include <ctime> //due to clock()
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/common/SUMOTime.h>
#include <utils/emissions/HelpersEnergy.h>
#include <utils/traction_wire/Node.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSEdgeControl.h>
#include <mesosim/MEVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_Emissions.h"
#include "MSDevice_ElecHybrid.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_ElecHybrid::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("ElecHybrid Device");
    insertDefaultAssignmentOptions("elechybrid", "ElecHybrid Device", oc);
}


void
MSDevice_ElecHybrid::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    // Check if vehicle should get an 'elecHybrid' device.
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "elechybrid", v, false)) {
        // Yes, build the device.
        // Fetch the battery capacity (if present) from the vehicle descriptor.
        const SUMOVTypeParameter& typeParams = v.getVehicleType().getParameter();
        const SUMOVehicleParameter& vehicleParams = v.getParameter();
        double actualBatteryCapacity = 0;
        /* The actual battery capacity can be a parameter of the vehicle or its vehicle type.
           The vehicle parameter takes precedence over the type parameter. */
        std::string attrName = toString(SUMO_ATTR_ACTUALBATTERYCAPACITY);
        if (vehicleParams.hasParameter(attrName)) {
            const std::string abc = vehicleParams.getParameter(attrName, "-1");
            try {
                actualBatteryCapacity = StringUtils::toDouble(abc);
            } catch (...) {
                WRITE_WARNING("Invalid value '" + abc + "'for vehicle parameter '" + attrName + "'. Using the default of " + std::to_string(actualBatteryCapacity));
            }
        } else {
            if (typeParams.hasParameter(attrName)) {
                const std::string abc = typeParams.getParameter(attrName, "-1");
                try {
                    actualBatteryCapacity = StringUtils::toDouble(abc);
                    WRITE_WARNING("Vehicle '" + v.getID() + "' does not provide vehicle parameter '" + attrName + "'. Using the vehicle type value of " + std::to_string(actualBatteryCapacity));
                } catch (...) {
                    WRITE_WARNING("Invalid value '" + abc + "'for vehicle type parameter '" + attrName + "'. Using the default of " + std::to_string(actualBatteryCapacity));
                }
            } else {
                WRITE_WARNING("Vehicle '" + v.getID() + "' does not provide vehicle or vehicle type parameter '" + attrName + "'. Using the default of " + std::to_string(actualBatteryCapacity));
            }
        }

        // obtain maximumBatteryCapacity
        double maximumBatteryCapacity = 0;
        attrName = toString(SUMO_ATTR_MAXIMUMBATTERYCAPACITY);
        if (typeParams.hasParameter(attrName)) {
            const std::string mbc = typeParams.getParameter(attrName, "-1");
            try {
                maximumBatteryCapacity = StringUtils::toDouble(mbc);
            } catch (...) {
                WRITE_WARNINGF(TL("Invalid value '%'for vType parameter '%'"), mbc, attrName);
            }
        } else {
            WRITE_WARNING("Vehicle '" + v.getID() + "' is missing the vType parameter '" + attrName + "'. Using the default of " + std::to_string(maximumBatteryCapacity));
        }

        // obtain overheadWireChargingPower
        double overheadWireChargingPower = 0;
        attrName = toString(SUMO_ATTR_OVERHEADWIRECHARGINGPOWER);
        if (typeParams.hasParameter(attrName)) {
            const std::string ocp = typeParams.getParameter(attrName, "-1");
            try {
                overheadWireChargingPower = StringUtils::toDouble(ocp);
            } catch (...) {
                WRITE_WARNINGF(TL("Invalid value '%'for vType parameter '%'"), ocp, attrName);
            }
        } else {
            WRITE_WARNING("Vehicle '" + v.getID() + "' is missing the vType parameter '" + attrName + "'. Using the default of " + std::to_string(overheadWireChargingPower));
        }

        // elecHybrid constructor
        MSDevice_ElecHybrid* device = new MSDevice_ElecHybrid(v, "elecHybrid_" + v.getID(),
                actualBatteryCapacity, maximumBatteryCapacity, overheadWireChargingPower);

        // Add device to vehicle
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_ElecHybrid-methods
// ---------------------------------------------------------------------------
MSDevice_ElecHybrid::MSDevice_ElecHybrid(SUMOVehicle& holder, const std::string& id,
        const double actualBatteryCapacity, const double maximumBatteryCapacity, const double overheadWireChargingPower) :
    MSVehicleDevice(holder, id),
    myActualBatteryCapacity(0),   // [actualBatteryCapacity <= maximumBatteryCapacity]
    myMaximumBatteryCapacity(0),  // [maximumBatteryCapacity >= 0]t
    myOverheadWireChargingPower(0),
    myLastAngle(NAN),
    myConsum(0),
    myBatteryDischargedLogic(false),
    myCharging(false),            // Initially vehicle don't charge
    myEnergyCharged(0),           // Initially the energy charged is zero
    myCircuitCurrent(NAN),        // Initially the current is unknown
    myCircuitVoltage(NAN),        // Initially the voltage is unknown as well
    myMaxBatteryCharge(NAN),      // Initial maximum of the battery energy during the simulation is unknown
    myMinBatteryCharge(NAN),      // Initial minimum of the battery energy during the simulation is unknown
    myTotalEnergyConsumed(0),     // No energy spent yet
    myTotalEnergyRegenerated(0),  // No energy regenerated
    myTotalEnergyWasted(0),       // No energy wasted on resistors
    // RICE_TODO: make these two parameters user configurable
    mySOCMin(0.005),              // Minimum SOC of the battery
    mySOCMax(0.980),              // Maximum SOC of the battery
    myActOverheadWireSegment(nullptr),         // Initially the vehicle isn't under any overhead wire segment
    myPreviousOverheadWireSegment(nullptr),    // Initially the vehicle wasn't under any overhead wire segment
    veh_elem(nullptr),
    veh_pos_tail_elem(nullptr),
    pos_veh_node(nullptr) {

    EnergyParams* const params = myHolder.getEmissionParameters();
    params->setDouble(SUMO_ATTR_MAXIMUMPOWER, holder.getVehicleType().getParameter().getDouble(toString(SUMO_ATTR_MAXIMUMPOWER), 100000.));

    if (maximumBatteryCapacity < 0) {
        WRITE_WARNINGF(TL("ElecHybrid builder: Vehicle '%' doesn't have a valid value for parameter % (%)."), getID(), toString(SUMO_ATTR_MAXIMUMBATTERYCAPACITY), toString(maximumBatteryCapacity));
    } else {
        myMaximumBatteryCapacity = maximumBatteryCapacity;
    }

    if (actualBatteryCapacity > maximumBatteryCapacity) {
        WRITE_WARNING("ElecHybrid builder: Vehicle '" + getID() + "' has a " + toString(SUMO_ATTR_ACTUALBATTERYCAPACITY) + " (" + toString(actualBatteryCapacity) + ") greater than it's " + toString(SUMO_ATTR_MAXIMUMBATTERYCAPACITY) + " (" + toString(maximumBatteryCapacity) + "). A max battery capacity value will be asigned");
        myActualBatteryCapacity = myMaximumBatteryCapacity;
    } else {
        myActualBatteryCapacity = actualBatteryCapacity;
    }

    if (overheadWireChargingPower < 0) {
        WRITE_WARNINGF(TL("ElecHybrid builder: Vehicle '%' doesn't have a valid value for parameter % (%)."), getID(), toString(SUMO_ATTR_OVERHEADWIRECHARGINGPOWER), toString(overheadWireChargingPower));
    } else {
        myOverheadWireChargingPower = overheadWireChargingPower;
    }
}


MSDevice_ElecHybrid::~MSDevice_ElecHybrid() {
}


bool
MSDevice_ElecHybrid::notifyMove(SUMOTrafficObject& tObject, double /* oldPos */, double /* newPos */, double /* newSpeed */) {
    if (!tObject.isVehicle()) {
        return false;
    }
    SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
    // Do not compute the current consumption here anymore:
    // myConsum is (non-systematically, we agree) set in MSVehicle so that the vehicle `vNext` value can
    // be influenced by the maximum traction power of the vehicle (i.e. installing a 80 kWh powertrain will
    // limit the acceleration regardless of the acceleration specified in vehicleType params, in case that
    // the vehicleType acceleration is too high).
    //
    // myParam[SUMO_ATTR_ANGLE] = myLastAngle == std::numeric_limits<double>::infinity() ? 0. : GeomHelper::angleDiff(myLastAngle, veh.getAngle());
    // myConsum = PollutantsInterface::getEnergyHelper().compute(0, PollutantsInterface::ELEC, veh.getSpeed(), veh.getAcceleration(), veh.getSlope(), &myParam);
    assert(!std::isnan(myConsum));

    // is battery pack discharged (from previous timestep)
    if (myActualBatteryCapacity < mySOCMin * myMaximumBatteryCapacity) {
        myBatteryDischargedLogic = true;
    } else {
        myBatteryDischargedLogic = false;
    }

    /* If battery is discharged we will force the vehicle to slowly come to
       a halt (freewheel motion). It could still happen that some energy will
       be recovered in later steps due to regenerative braking. */
    if (isBatteryDischarged()) {
        std::vector<std::pair<SUMOTime, double> > speedTimeLine;
        /// @todo modify equation for deceleration, getNoEnergyDecel
        /// @todo doublecheck this mode, we probably assume here that the acceleration is negative
        /// @todo check the value of myConsum here, it should be probably zero
        double accel = acceleration(veh, 0, veh.getSpeed());  // or use veh.getAcceleration() method???
        const double nextSpeed = MAX2(0., veh.getSpeed() + ACCEL2SPEED(accel));
        speedTimeLine.push_back(
            std::make_pair(
                MSNet::getInstance()->getCurrentTimeStep(),
                veh.getSpeed()));
        speedTimeLine.push_back(
            std::make_pair(
                MSNet::getInstance()->getCurrentTimeStep() + DELTA_T,
                nextSpeed));

        static_cast<MSVehicle*>(&veh)->getInfluencer().setSpeedTimeLine(speedTimeLine);
    }

    /* Check if there is an overhead wire either over the lane where the vehicle is or over a
       neighboring lanes. This check has to be performed at every simulation step as the
       overhead wires for trolleybuses will typically end at a bus stop that is located somewhere
       in the middle of the lane. */
    std::string overheadWireSegmentID = MSNet::getInstance()->getStoppingPlaceID(veh.getLane(), veh.getPositionOnLane(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT);

    //check overhead line on the left neighboring lane
    if (overheadWireSegmentID == "" && veh.getEdge()->leftLane(veh.getLane()) != nullptr) {
        overheadWireSegmentID = MSNet::getInstance()->getStoppingPlaceID(veh.getEdge()->leftLane(veh.getLane()), veh.getPositionOnLane(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT);
    }
    //check overhead line on the right neighboring lane
    if (overheadWireSegmentID == "" && veh.getEdge()->rightLane(veh.getLane()) != nullptr) {
        overheadWireSegmentID = MSNet::getInstance()->getStoppingPlaceID(veh.getEdge()->rightLane(veh.getLane()), veh.getPositionOnLane(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT);
    }

    /* Store the amount of power that could not be recuperated. */
    double energyWasted = 0.0;
    /* If vehicle has access to an overhead wire (including the installation on neighboring lanes) */
    if (overheadWireSegmentID != "") {
        /* Update the actual overhead wire segment of this device */
        myActOverheadWireSegment =
            static_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(overheadWireSegmentID, SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        /* Store the traction substation of the actual overhead wire segment */
        MSTractionSubstation* actualSubstation = myActOverheadWireSegment->getTractionSubstation();

        /* Disable charging from previous (not the actual) overhead wire segment.
           REASON:
           If there is no gap between two different overhead wire segments that are
           places on the same lane, the vehicle switches from the one segment to another
           in one timestep. */
        if (myPreviousOverheadWireSegment != myActOverheadWireSegment) {
            if (myPreviousOverheadWireSegment != nullptr) {
                /* Remove the vehicle from the list of vehicles powered by the previous segment. */
                myPreviousOverheadWireSegment->eraseVehicle(veh);
                MSTractionSubstation* ts = myPreviousOverheadWireSegment->getTractionSubstation();
                if (ts != nullptr) {
                    ts->decreaseElecHybridCount();
                    ts->eraseVehicle(this);
                }
            }
            /* Add the vehicle reference to the current segment. */
            myActOverheadWireSegment->addVehicle(veh);
            if (actualSubstation != nullptr) {
                actualSubstation->increaseElecHybridCount();
                actualSubstation->addVehicle(this);
            }
        }

        /* Do we simulate the behaviour of the overhead wire electric circuit? */
        if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
            /// @todo Should this part of the code be #ifdefed in case that EIGEN is not installed?
            /* Circuit update due to vehicle movement:
               Delete vehicle resistor element, vehicle resistor nodes and vehicle resistor
               tails in the circuit used in the previous timestep. */
            deleteVehicleFromCircuit(veh);

            /* Add the vehicle to the circuit in case that there is a substation that provides
               power to it. */
            if (actualSubstation != nullptr) {
                /* Add a resistor (current source in the future?) representing trolleybus
                   vehicle to the circuit.
                   pos/neg_veh_node elements
                    [0] .... vehicle_resistor
                    [1] .... leading resistor
                    [2] .... tail resistor pos/neg_tail_vehID
                */

                // pos_veh_node and veh_elem should be NULL
                if (pos_veh_node != nullptr || veh_elem != nullptr) {
                    WRITE_WARNING("pos_veh_node or neg_veh_node or veh_elem is not NULL (and they should be at the beginning of adding elecHybrid to the circuit)");
                }

                // create pos and veh_elem
                Circuit* owc = myActOverheadWireSegment->getCircuit();
                pos_veh_node = owc->addNode("pos_" + veh.getID());
                assert(pos_veh_node != nullptr);
                // adding current source element representing elecHybrid vehicle. The value of current is computed from wantedPower later by circuit solver. Thus NAN is used as an initial value.
                veh_elem = owc->addElement("currentSrc" + veh.getID(), NAN,
                                           pos_veh_node, owc->getNode("negNode_ground"),
                                           Element::ElementType::CURRENT_SOURCE_traction_wire);

                // Connect vehicle to an existing overhead wire segment = add elecHybridVehicle to the myActOverheadWireSegment circuit
                // Find pos resistor element of the actual overhead line section and their end nodes
                Element* element_pos = owc->getElement("pos_" + myActOverheadWireSegment->getID());
                Node*  node_pos = element_pos->getNegNode();
                double resistance = element_pos->getResistance();

                /* Find the correct position of the vehicle at the overhead line.
                   We start the while loop at the end of the actual overhead line section and go against the direction of vehicle movement.
                   The decision rule is based on the resistance value:
                     * from the vehicle position to the end of lane,
                     * sum of resistance of elements (going from the end of overhead line section in the contrary direction).

                   The original solution runs into problems when a vehicle is going on a neighboring lane and the two lanes have different lengths:
                   while (resistance < (veh.getLane()->getLength() - veh.getPositionOnLane())*WIRE_RESISTIVITY) {
                   Improvement: take the relative distance of the vehicle to the end of its lane and map it to the segment's lane length. (This works also in case that the segment's lane and the vehicle's lane are identical.)
                */
                double relativePosOnSegment =
                    myActOverheadWireSegment->getLane().getLength() * (1 -
                            (veh.getPositionOnLane() / veh.getLane()->getLength()));

                while (resistance < relativePosOnSegment * WIRE_RESISTIVITY) {
                    node_pos = element_pos->getPosNode();
                    element_pos = node_pos->getElements()->at(2);
                    resistance += element_pos->getResistance();
                    if (strncmp(element_pos->getName().c_str(), "pos_tail_", 9) != 0) {
                        WRITE_WARNING("splitting element is not 'pos_tail_XXX'")
                    }
                }

                node_pos = element_pos->getPosNode();
                //resistance of vehicle tail nodes
                resistance -= relativePosOnSegment * WIRE_RESISTIVITY;

                /* dividing element_pos
                   before:   |node_pos|---------------------------------------------|element_pos|----
                   after:    |node_pos|----|veh_pos_tail_elem|----|pos_veh_node|----|element_pos|----
                */
                element_pos->setPosNode(pos_veh_node);
                node_pos->eraseElement(element_pos);
                pos_veh_node->addElement(element_pos);

                veh_pos_tail_elem = owc->addElement("pos_tail_" + veh.getID(),
                                                    resistance, node_pos, pos_veh_node, Element::ElementType::RESISTOR_traction_wire);

                if (element_pos->getResistance() - resistance < 0) {
                    WRITE_WARNINGF(TL("The resistivity of overhead wire segment connected to vehicle % is < 0. Set to 1e-6."), veh.getID());
                }

                element_pos->setResistance(element_pos->getResistance() - resistance);


                // Set the power requirement to the consumption + charging power.
                // RICE_TODO: The charging power could be different when moving and when not. Add a parameter.
                // Note that according to PMDP data, the charging power seems to be the same in both situations,
                // ignoring the potential benefits of using a higher charging power when the vehicle is moving.
                if (myActualBatteryCapacity < mySOCMax * myMaximumBatteryCapacity) {
                    veh_elem->setPowerWanted(WATTHR2WATT(myConsum) + myOverheadWireChargingPower);
                } else {
                    veh_elem->setPowerWanted(WATTHR2WATT(myConsum));
                }

                // No recuperation to overheadwire (only to the batterypack)
                if (!MSGlobals::gOverheadWireRecuperation && veh_elem->getPowerWanted() < 0.0) {
                    // the value of energyWasted is properly computed and updated after solving circuit by the solver
                    // energyWasted = 0;
                    veh_elem->setPowerWanted(0.0);
                }

                // RICE_TODO: The voltage in the solver should never exceed or drop below some limits. Maximum allowed voltage is typically 800 V.
                // The numerical solver that computes the circuit state needs initial values of electric currents and
                // voltages from which it will start the iterative solving process. We prefer to reuse the "old" values
                // as it is likely that the new values will not be far away from the old ones. The safety limits of
                // 10 and 1500 Volts that are used below are chosen fairly arbitrarily to keep the initial values within
                // reasonable limits.
                double voltage = myCircuitVoltage;
                if (voltage < 10.0 || voltage > 1500.0 || std::isnan(voltage)) {
                    // RICE_TODO : It seems to output warning whenever a vehicle is appearing under the overhead wire for the first time?
                    // WRITE_WARNINGF(TL("The initial voltage is was % V, replacing it with substation voltage % V."), toString(voltage), toString(actualSubstation->getSubstationVoltage()));
                    voltage = actualSubstation->getSubstationVoltage();
                }
                // Initial value of the electric current flowing into the vehicle that will be used by the solver
                double current = -(veh_elem->getPowerWanted() / voltage);
                veh_elem->setCurrent(current);

                // Set the device as charging the battery by default
                myCharging = true;

                // And register the call to solver at the end of the simulation step
                actualSubstation->addSolvingCircuitToEndOfTimestepEvents();
            } else {
                /*
                    No substation on this wire ...
                */
                // RICE_TODO myCharging = false; current 0 or nan, voltage 0 or nan, maybe write warning that the overhead wire is not connected to any substation,

                // Energy flowing to/from the battery pack [Wh] has to completely cover vehicle consumption.
                myEnergyCharged = -myConsum;
                // Update Battery charge
                myActualBatteryCapacity += myEnergyCharged;
                // No substation is connected to this segment and the charging output is therefore zero.
                myActOverheadWireSegment->addChargeValueForOutput(0, this, false);
            }
#else
            WRITE_ERROR(TL("Overhead wire solver is on, but the Eigen library has not been compiled in!"))
#endif
        } else {
            /*
                Faster approximation without circuit solving at every simulation step.
            */

            // First check that there is a traction substation connected to the overhead wire
            double voltage = 0.0;
            if (actualSubstation != nullptr) {
                voltage = actualSubstation->getSubstationVoltage();
            }

            // At this point the voltage can be (a) NAN if the substation voltage was not specified,
            // (b) 0 if no substation powers the current segment or if someone put its power to zero,
            // (c) >0 if the substation can provide energy to the circuit.
            if (voltage > 0.0) {
                // There is a power source connected to this segment.
                // Set the simplified power requirement to the consumption + charging power.
                // RICE_TODO: The charging power could be different when moving and when not. Add a parameter. See a similar code snippet above.
                // Note that according to PMDP data, the charging power seems to be the same in both situations,
                // ignoring the potential benefits of using a higher charging power when the vehicle is moving.
                double powerWanted = WATTHR2WATT(myConsum);
                if (myActualBatteryCapacity < mySOCMax * myMaximumBatteryCapacity) {
                    // Additional `myOverheadWireChargingPower` due to charging of battery pack
                    powerWanted += myOverheadWireChargingPower;
                }

                // No recuperation to overhead wire (only to the battery pack)
                // RICE_TODO: How to recuperate into the circuit without solver? (energy balance?)
                //            - solution: assume, that any power is possible to recuperate
                if (!MSGlobals::gOverheadWireRecuperation && powerWanted < 0.0) {
                    // the value of energyWasted is properly computed and updated below
                    powerWanted = 0.0;
                }

                // Set the actual current and voltage of the global circuit
                // RICE_TODO: Process the traction station current limiting here as well.
                myCircuitCurrent = powerWanted / voltage;
                myCircuitVoltage = voltage;

                // Calculate energy charged
                double energyIn = WATT2WATTHR(powerWanted);

                // Calculate energy flowing to/from the battery in this step [Wh]
                // RICE_TODO: It should be possible to define different efficiency values for direction overhead wire -> battery; motor -> battery.
                // We use a simplification here. The biggest contributor to the total losses is the battery pack itself
                // (the input LC filter is probably more efficient -- eta_LC ~ 0.99 -- compared to the induction motor
                // with eta_motor ~ 0.95).
                myEnergyCharged = computeChargedEnergy(energyIn);

                // Update the energy that has been stored in the battery pack and return the real energy charged in this step
                // considering SOC limits of the battery pack.
                double realEnergyCharged = storeEnergyToBattery(myEnergyCharged);
                // Set energy wasted
                energyWasted = myEnergyCharged - realEnergyCharged;

                // Add the energy provided by the overhead wire segment to the output of the segment
                myActOverheadWireSegment->addChargeValueForOutput(energyIn, this);
            } else {
                /*
                    Overhead wire without a connected substation
                */
                // RICE_TODO myCharging = false; current 0 or nan, voltage 0 or nan, maybe write warning that the overhead wire is not connected to any substation,

                // Energy for the powertrain is provided by the battery pack
                myEnergyCharged = -myConsum;
                // Update battery charge
                myActualBatteryCapacity += myEnergyCharged;
                // No energy was provided by the overhead wire segment
                myActOverheadWireSegment->addChargeValueForOutput(0.0, this);
            }
        }
        assert(myActOverheadWireSegment != nullptr);
        myPreviousOverheadWireSegment = myActOverheadWireSegment;
    } else {
        /*
            No overhead wires, no charging.
        */

        // Disable charing flag
        myCharging = false;

        // Invalidate the circuit voltage and current
        myCircuitCurrent = NAN;
        myCircuitVoltage = NAN;

        // Additional bookkeeping in case that the circuit solver is used
        if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
            /*
                Delete vehicle resistor element, vehicle resistor nodes and vehicle resistor tails
                in the previous circuit (i.e. the circuit used in the previous timestep)
            */
            deleteVehicleFromCircuit(veh);
#else
            WRITE_ERROR(TL("Overhead wire solver is on, but the Eigen library has not been compiled in!"))
#endif
        }

        // Vehicle is not under an overhead wire
        myActOverheadWireSegment = nullptr;

        // Remove the vehicle from overhead wire if it was under it in the previous step.
        // This has to be called after deleteVehicleFromCircuit() as the file uses myPreviousOverheadWire.
        if (myPreviousOverheadWireSegment != nullptr) {
            myPreviousOverheadWireSegment->eraseVehicle(veh);
            MSTractionSubstation* ts = myPreviousOverheadWireSegment->getTractionSubstation();
            if (ts != nullptr) {
                ts->decreaseElecHybridCount();
                ts->eraseVehicle(this);
            }
            myPreviousOverheadWireSegment = nullptr;
        }

        // Energy for the powertrain is provided by the battery pack
        myEnergyCharged = -myConsum;
        // Update battery charge
        myActualBatteryCapacity += myEnergyCharged;
    }

    // Update the statistical values
    // RICE_TODO: update these statistical values also after solving circuit by electric circuit
    if (std::isnan(myMaxBatteryCharge) || myMaxBatteryCharge < myActualBatteryCapacity) {
        myMaxBatteryCharge = myActualBatteryCapacity;
    }
    if (std::isnan(myMinBatteryCharge) || myMinBatteryCharge > myActualBatteryCapacity) {
        myMinBatteryCharge = myActualBatteryCapacity;
    }

    if (myConsum > 0.0) {
        myTotalEnergyConsumed += myConsum;
    } else {
        myTotalEnergyRegenerated -= myConsum;
    }
    myTotalEnergyWasted += energyWasted;

    myLastAngle = veh.getAngle();
    return true; // keep the device
}

// Note: This is called solely in the mesoscopic mode to mimic the `notifyMove()` reminder
void
MSDevice_ElecHybrid::notifyMoveInternal(
    const SUMOTrafficObject& tObject,
    const double frontOnLane,
    const double timeOnLane,
    const double meanSpeedFrontOnLane,
    const double meanSpeedVehicleOnLane,
    const double travelledDistanceFrontOnLane,
    const double travelledDistanceVehicleOnLane,
    const double meanLengthOnLane) {
    UNUSED_PARAMETER(tObject);
    UNUSED_PARAMETER(frontOnLane);
    UNUSED_PARAMETER(timeOnLane);
    UNUSED_PARAMETER(meanSpeedFrontOnLane);
    UNUSED_PARAMETER(meanSpeedVehicleOnLane);
    UNUSED_PARAMETER(travelledDistanceFrontOnLane);
    UNUSED_PARAMETER(travelledDistanceVehicleOnLane);
    UNUSED_PARAMETER(meanLengthOnLane);
}

void
MSDevice_ElecHybrid::deleteVehicleFromCircuit(SUMOVehicle& veh) {
    if (myPreviousOverheadWireSegment != nullptr) {
        if (myPreviousOverheadWireSegment->getTractionSubstation() != nullptr) {
            //check if all pointers to vehicle elements and nodes are not nullptr
            if (veh_elem == nullptr || veh_pos_tail_elem == nullptr || pos_veh_node == nullptr) {
                WRITE_ERRORF("During deleting vehicle '%' from circuit some init previous Nodes or Elements was not assigned.", veh.getID());
            }
            //check if pos_veh_node has 3 elements - they should be: veh_elem, veh_pos_tail_elem and an overhead line resistor element "ahead" of vehicle.
            if (pos_veh_node->getElements()->size() != 3) {
                WRITE_ERRORF("During deleting vehicle '%' from circuit the size of element-vector of pNode or nNode was not 3. It should be 3 by Jakub's opinion.", veh.getID());
            }
            //delete vehicle resistor element "veh_elem" in the previous circuit,
            pos_veh_node->eraseElement(veh_elem);
            myPreviousOverheadWireSegment->getCircuit()->eraseElement(veh_elem);
            delete veh_elem;
            veh_elem = nullptr;

            //erasing of tail elements (the element connected to the veh_node is after then only the ahead overhead line resistor element)
            pos_veh_node->eraseElement(veh_pos_tail_elem);

            if (pos_veh_node->getElements()->size() != 1) {
                WRITE_ERRORF("During deleting vehicle '%' from circuit the size of element-vector of pNode or nNode was not 1. It should be 1 by Jakub's opinion.", veh.getID());
            }

            // add the resistance value of veh_tail element to the resistance value of the ahead overhead line element
            pos_veh_node->getElements()->front()->setResistance(pos_veh_node->getElements()->front()->getResistance() + veh_pos_tail_elem->getResistance());
            //set PosNode of the ahead overhead line element to the posNode value of tail element
            Element* aux = pos_veh_node->getElements()->front();
            //set node = 3 operations
            aux->setPosNode(veh_pos_tail_elem->getPosNode());
            aux->getPosNode()->eraseElement(aux);
            veh_pos_tail_elem->getPosNode()->addElement(aux);

            // erase tail element from its PosNode
            veh_pos_tail_elem->getPosNode()->eraseElement(veh_pos_tail_elem);
            // delete veh_pos_tail_elem
            myPreviousOverheadWireSegment->getCircuit()->eraseElement(veh_pos_tail_elem);
            delete veh_pos_tail_elem;
            veh_pos_tail_elem = nullptr;

            //erase pos_veh_node
            myPreviousOverheadWireSegment->getCircuit()->eraseNode(pos_veh_node);
            //modify id of other elements (the id of erasing element should be the greatest)
            int lastId = myPreviousOverheadWireSegment->getCircuit()->getLastId() - 1;
            if (pos_veh_node->getId() != lastId) {
                Node* node_last = myPreviousOverheadWireSegment->getCircuit()->getNode(lastId);
                if (node_last != nullptr) {
                    node_last->setId(pos_veh_node->getId());
                } else {
                    Element* elem_last = myPreviousOverheadWireSegment->getCircuit()->getVoltageSource(lastId);
                    if (elem_last != nullptr) {
                        elem_last->setId(pos_veh_node->getId());
                    } else {
                        WRITE_ERROR(TL("The element or node with the last Id was not found in the circuit!"));
                    }
                }
            }
            myPreviousOverheadWireSegment->getCircuit()->decreaseLastId();
            delete pos_veh_node;
            pos_veh_node = nullptr;
        }
    }
}

bool
MSDevice_ElecHybrid::notifyEnter(
    SUMOTrafficObject& tObject,
    MSMoveReminder::Notification /* reason */,
    const MSLane* /* enteredLane */) {
    if (!tObject.isVehicle()) {
        return false;
    }
#ifdef ELECHYBRID_MESOSCOPIC_DEBUG
    SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
    std::cout << "device '" << getID() << "' notifyEnter: reason=" << reason << " currentEdge=" << veh.getEdge()->getID() << "\n";
#endif

    return true; // keep the device
}

bool
MSDevice_ElecHybrid::notifyLeave(
    SUMOTrafficObject& tObject,
    double /*lastPos*/,
    MSMoveReminder::Notification reason,
    const MSLane* /* enteredLane */) {
    if (!tObject.isVehicle()) {
        return false;
    }
    SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
#ifdef ELECHYBRID_MESOSCOPIC_DEBUG
    SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
    std::cout << "device '" << getID() << "' notifyLeave: reason=" << reason << " currentEdge=" << veh.getEdge()->getID() << "\n";
    SUMOVehicle* sumoVehicle = *veh;
    if (MSGlobals::gUseMesoSim) {
        MEVehicle& v = dynamic_cast<MEVehicle&>(veh);
        std::cout << "***************** MESO - notifyLeave*** START ****************** '" << v.getID() << "' \n";
        //TODO the second argument of getStoptime should change
        std::cout << "getSpeed: '" << v.getSpeed() << "' | getAverageSpeed: '" << v.getAverageSpeed() << "' | getStoptime: '"  << v.getStoptime(v.getSegment(), 0) << "' \n";
        std::cout << "getStopEdges: '"  << "' | getLastEntryTime: '" << v.getLastEntryTime() << "' | getBlockTimeSeconds: '" << v.getBlockTimeSeconds() << "' \n";
        std::cout << "getWaitingTime: '" << v.getWaitingTime() << "' | getAccumulatedWaitingTime: '" << v.getWaitingTime(true) << "' | getLastEntryTimeSeconds: '" << v.getLastEntryTimeSeconds() << "' \n";
        std::cout << "***************** MESO - notifyLeave***  END  ****************** '" << v.getID() << "' \n";
    }
#endif

    // The MSMoveReminders are sorted so that we can do `<`. See also BTreceiver and BTsender devices...
    if (reason < MSMoveReminder::NOTIFICATION_TELEPORT) {
        return true;
    }

    if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
        // ------- *** ------- delete vehicle resistor element, vehicle resistor nodes and vehicle resistor tails in the previous circuit (circuit used in the previous timestep)------- *** -------
        deleteVehicleFromCircuit(veh);
#else
        UNUSED_PARAMETER(veh);
        WRITE_ERROR(TL("Overhead wire solver is on, but the Eigen library has not been compiled in!"))
#endif
    }
    // disable charging vehicle from overhead wire and segment and traction station
    if (myPreviousOverheadWireSegment != nullptr) {
        myPreviousOverheadWireSegment->eraseVehicle(veh);
        MSTractionSubstation* prevSubstation = myPreviousOverheadWireSegment->getTractionSubstation();
        if (prevSubstation != nullptr) {
            prevSubstation->decreaseElecHybridCount();
            prevSubstation->eraseVehicle(this);
        }
        myPreviousOverheadWireSegment = nullptr;
    }
    return true;
}


void
MSDevice_ElecHybrid::generateOutput(OutputDevice* tripinfoOut) const {
    if (tripinfoOut != nullptr) {
        // Write the summary elecHybrid information into tripinfo output
        tripinfoOut->openTag("elechybrid");
        tripinfoOut->writeAttr("maxBatteryCharge", myMaxBatteryCharge);
        tripinfoOut->writeAttr("minBatteryCharge", myMinBatteryCharge);
        tripinfoOut->writeAttr("totalEnergyConsumed", myTotalEnergyConsumed);
        tripinfoOut->writeAttr("totalEnergyRegenerated", myTotalEnergyRegenerated);
        tripinfoOut->writeAttr("totalEnergyWasted", myTotalEnergyWasted);
        tripinfoOut->closeTag();
    }
}


double
MSDevice_ElecHybrid::getActualBatteryCapacity() const {
    return myActualBatteryCapacity;
}


double
MSDevice_ElecHybrid::getMaximumBatteryCapacity() const {
    return myMaximumBatteryCapacity;
}

std::string
MSDevice_ElecHybrid::getParameter(const std::string& key) const {
    // RICE_TODO: full traci support
    if (key == toString(SUMO_ATTR_ACTUALBATTERYCAPACITY)) {
        return toString(myActualBatteryCapacity);
    } else if (key == toString(SUMO_ATTR_ENERGYCONSUMED)) {
        return toString(myConsum);
    } else if (key == toString(SUMO_ATTR_ENERGYCHARGED)) {
        return toString(myEnergyCharged);
    } else if (key == toString(SUMO_ATTR_MAXIMUMBATTERYCAPACITY)) {
        return toString(myMaximumBatteryCapacity);
    } else if (key == toString(SUMO_ATTR_OVERHEADWIREID)) {
        return getOverheadWireSegmentID();
    } else if (key == toString(SUMO_ATTR_SUBSTATIONID)) {
        return getTractionSubstationID();
    } else if (key == toString(SUMO_ATTR_VEHICLEMASS)) {
        WRITE_WARNING(TL("Getting the vehicle mass via parameters is deprecated, please use getMass for the vehicle or its type."));
        return toString(myHolder.getEmissionParameters()->getDouble(SUMO_ATTR_MASS));
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


double
MSDevice_ElecHybrid::getParameterDouble(const std::string& key) const {
    if (key == toString(SUMO_ATTR_MAXIMUMPOWER)) {
        return myHolder.getEmissionParameters()->getDouble(SUMO_ATTR_MAXIMUMPOWER);
    } else if (key == toString(SUMO_ATTR_RECUPERATIONEFFICIENCY)) {
        return myHolder.getEmissionParameters()->getDouble(SUMO_ATTR_RECUPERATIONEFFICIENCY);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


double MSDevice_ElecHybrid::computeChargedEnergy(double energyIn) {
    double energyCharged = energyIn - myConsum;
    /*
    Apply recuperation or propulsion efficiency if necessary
        1. if (energyIn > 0.0 && energyCharged > 0 && it->getConsum() >= 0) = > recuper eff for energyCharged
        2. if (energyIn > 0.0 && energyCharged > 0 && it->getConsum() < 0)  => recuper eff only for energyIn
        3. if (energyIn < 0.0 && it->getConsum() > 0) => 1/propulsion eff only for energyIn
        4. if (energyIn < 0.0 && energyCharged < 0 && it->getConsum() < 0) => 1/propulsion eff only for energyCharged
    */
    if (energyIn > 0.0 && energyCharged > 0.0) {
        // the vehicle is charging battery from overhead wire
        if (myConsum >= 0) {
            energyCharged *= myHolder.getEmissionParameters()->getDouble(SUMO_ATTR_RECUPERATIONEFFICIENCY);
        } else {
            energyCharged = myHolder.getEmissionParameters()->getDouble(SUMO_ATTR_RECUPERATIONEFFICIENCY) * energyIn - myConsum;
        }
    } else if (energyIn < 0.0 && energyCharged < 0.0) {
        // the vehicle is recuperating energy into the overhead wire and discharging batterypack at the same time
        if (myConsum >= 0) {
            energyCharged *= energyIn / myHolder.getEmissionParameters()->getDouble(SUMO_ATTR_PROPULSIONEFFICIENCY) - myConsum;
        } else {
            energyCharged /= myHolder.getEmissionParameters()->getDouble(SUMO_ATTR_PROPULSIONEFFICIENCY);
        }
    }
    return energyCharged;
}

double
MSDevice_ElecHybrid::getConsum() const {
    return myConsum;
}

void
MSDevice_ElecHybrid::setConsum(const double consumption) {
    myConsum = consumption;
}

void
MSDevice_ElecHybrid::updateTotalEnergyWasted(const double energyWasted) {
    myTotalEnergyWasted += energyWasted;
}

double
MSDevice_ElecHybrid::getEnergyCharged() const {
    return myEnergyCharged;
}

void
MSDevice_ElecHybrid::setEnergyCharged(double energyCharged) {
    myEnergyCharged = energyCharged;
}

double
MSDevice_ElecHybrid::getCircuitAlpha() const {
    if (myActOverheadWireSegment != nullptr && MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
        Circuit* owc = myActOverheadWireSegment->getCircuit();
        if (owc != nullptr) {
            return owc->getAlphaBest() ;
        }
#else
        WRITE_ERROR(TL("Overhead wire solver is on, but the Eigen library has not been compiled in!"))
#endif
    }
    return NAN;
}

double
MSDevice_ElecHybrid::getPowerWanted() const {
    if (veh_elem != nullptr) {
        return veh_elem->getPowerWanted();
    }
    return NAN;
}

double
MSDevice_ElecHybrid::getCurrentFromOverheadWire() const {
    return myCircuitCurrent;
}

void
MSDevice_ElecHybrid::setCurrentFromOverheadWire(double current) {
    myCircuitCurrent = current;
}

double
MSDevice_ElecHybrid::getVoltageOfOverheadWire() const {
    return myCircuitVoltage;
}

void
MSDevice_ElecHybrid::setVoltageOfOverheadWire(double voltage) {
    myCircuitVoltage = voltage;
}

std::string
MSDevice_ElecHybrid::getOverheadWireSegmentID() const {
    if (myActOverheadWireSegment != nullptr) {
        return myActOverheadWireSegment->getID();
    } else {
        return "";
    }
}

std::string
MSDevice_ElecHybrid::getTractionSubstationID() const {
    if (myActOverheadWireSegment != nullptr) {
        MSTractionSubstation* ts = myActOverheadWireSegment->getTractionSubstation();
        if (ts != nullptr) {
            return ts->getID();
        }
    }
    return "";
}

bool
MSDevice_ElecHybrid::isBatteryDischarged() const {
    return myBatteryDischargedLogic;
}

double
MSDevice_ElecHybrid::storeEnergyToBattery(const double energy) {
    // Original energy state of the battery pack
    double previousEnergyInBattery = myActualBatteryCapacity;
    // Push as much energy as the battery pack can accomodate.
    // The battery has SOC limits that will prevent it from being overcharged, hence some energy may remain "free".
    setActualBatteryCapacity(myActualBatteryCapacity + energy);
    // The "free" energy that still remains and has to be stored elsewhere or burned on resistors is the difference.
    return myActualBatteryCapacity - previousEnergyInBattery;
}

void
MSDevice_ElecHybrid::setActualBatteryCapacity(const double actualBatteryCapacity) {
    // Use the SOC limits to cap the actual battery capacity
    if (actualBatteryCapacity < mySOCMin * myMaximumBatteryCapacity) {
        //WRITE_WARNINGF(TL("The Battery of vehicle '%' has been exhausted."), getID());
        myActualBatteryCapacity = MIN2(mySOCMin * myMaximumBatteryCapacity, myActualBatteryCapacity);
    } else if (actualBatteryCapacity > mySOCMax * myMaximumBatteryCapacity) {
        myActualBatteryCapacity = MAX2(mySOCMax * myMaximumBatteryCapacity, myActualBatteryCapacity);
    } else {
        myActualBatteryCapacity = actualBatteryCapacity;
    }
}

void
MSDevice_ElecHybrid::setParameter(const std::string& key, const std::string& value) {
    double doubleValue;
    try {
        doubleValue = StringUtils::toDouble(value);
    } catch (NumberFormatException&) {
        throw InvalidArgument("Setting parameter '" + key + "' requires a number for device of type '" + deviceName() + "'");
    }
    if (key == toString(SUMO_ATTR_ACTUALBATTERYCAPACITY)) {
        myActualBatteryCapacity = doubleValue;
    } else if (key == toString(SUMO_ATTR_MAXIMUMBATTERYCAPACITY)) {
        myMaximumBatteryCapacity = doubleValue;
    } else if (key == toString(SUMO_ATTR_OVERHEADWIRECHARGINGPOWER)) {
        myOverheadWireChargingPower = doubleValue;
    } else if (key == toString(SUMO_ATTR_VEHICLEMASS)) {
        WRITE_WARNING(TL("Setting the vehicle mass via parameters is deprecated, please use setMass for the vehicle or its type."));
        myHolder.getEmissionParameters()->setDouble(SUMO_ATTR_MASS, doubleValue);
    } else {
        throw InvalidArgument("Setting parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
    }
}

double
MSDevice_ElecHybrid::acceleration(SUMOVehicle& veh, double power, double oldSpeed) {
    myHolder.getEmissionParameters()->setDouble(SUMO_ATTR_ANGLE, std::isnan(myLastAngle) ? 0. : GeomHelper::angleDiff(myLastAngle, veh.getAngle()));
    return PollutantsInterface::getEnergyHelper().acceleration(0, PollutantsInterface::ELEC, oldSpeed, power, veh.getSlope(), myHolder.getEmissionParameters());
}

double
MSDevice_ElecHybrid::consumption(SUMOVehicle& veh, double a, double newSpeed) {
    myHolder.getEmissionParameters()->setDouble(SUMO_ATTR_ANGLE, std::isnan(myLastAngle) ? 0. : GeomHelper::angleDiff(myLastAngle, veh.getAngle()));
    return PollutantsInterface::getEnergyHelper().compute(0, PollutantsInterface::ELEC, newSpeed, a, veh.getSlope(), myHolder.getEmissionParameters()) * TS;
}


/****************************************************************************/
