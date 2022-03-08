/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2022 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_Seeroad.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    11.06.2013
///
// A device which stands as an implementation Seeroad and which outputs movereminder calls
/****************************************************************************/
#include <config.h>

#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_Seeroad.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Seeroad::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Seeroad Device");
    insertDefaultAssignmentOptions("Seeroad", "Seeroad Device", oc);

    oc.doRegister("device.Seeroad.stdev", new Option_Float(0.1)); //default .1
    oc.addDescription("device.Seeroad.stdev", "Seeroad Device", "The measurement noise parameter which can be applied to the Seeroad device");

	oc.doRegister("device.Seeroad.offset", new Option_Float(0.0)); //default no offset
	oc.addDescription("device.Seeroad.offset", "Seeroad Device", "The measurement offset parameter which can be applied to the Seeroad device -> e.g. to force false measurements");
}


void
MSDevice_Seeroad::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "Seeroad", v, false)) {

    	//check options
		// stdev noise deviation
		double stdev = getStdev(v);

		// static offset
		double offset = getOffset(v);

        // build the device
        // Should get a Value for initializing the device        
        MSDevice_Seeroad* device = new MSDevice_Seeroad(v, "Seeroad_" + v.getID(),
				(double)(1.0), //Building the Vehicle it doesnt have an active Lane yet!
                (double)(stdev),
                (double)(offset)); //Custom Parameters from Example - default to 0.0 and 0.0
        into.push_back(device);
    }
}

void
MSDevice_Seeroad::cleanup() {
    // cleaning up global state (if any)
}

// ---------------------------------------------------------------------------
// MSDevice_Seeroad-methods
// ---------------------------------------------------------------------------
MSDevice_Seeroad::MSDevice_Seeroad(SUMOVehicle& holder, const std::string& id,
                                   double frictionCoefficient, double stdev, double offset) :
    MSVehicleDevice(holder, id),
    myMeasuredFrictionCoefficient(frictionCoefficient),
	myStdDeviation(stdev),
    myOffset(offset){
	
    //std::cout << "initialized device '" << id << "' with myMeasuredFrictionCoefficient=" << myMeasuredFrictionCoefficient << "\n";
}


MSDevice_Seeroad::~MSDevice_Seeroad() {
}


bool
MSDevice_Seeroad::notifyMove(SUMOTrafficObject& tObject, double /* oldPos */,
                             double /* newPos */, double newSpeed) {
    std::cout << "device '" << getID() << "' notifyMove: newSpeed=" << newSpeed << "\n";
    if (tObject.isVehicle()) {
        SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
      	//Update the myFrictionCoeficient
		myRawFriction = veh.getLane()->getFrictionCoefficient();
		myMeasuredFrictionCoefficient = applyModel(myRawFriction);
    }
    return true; // keep the device
}


bool
MSDevice_Seeroad::notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    std::cout << "device '" << getID() << "' notifyEnter: reason=" << reason << " currentEdge=" << veh.getEdge()->getID() << "\n";
    return true; // keep the device
}


bool
MSDevice_Seeroad::notifyLeave(SUMOTrafficObject& veh, double /*lastPos*/, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    std::cout << "device '" << getID() << "' notifyLeave: reason=" << reason << " currentEdge=" << veh.getEdge()->getID() << "\n";
    return true; // keep the device
}


void
MSDevice_Seeroad::generateOutput(OutputDevice* tripinfoOut) const {
    if (tripinfoOut != nullptr) {
        tripinfoOut->openTag("seeroad_device");
        tripinfoOut->writeAttr("Measured Friction", toString(myMeasuredFrictionCoefficient));
        //tripinfoOut->writeAttr("customValue2", toString(stdev));
        //tripinfoOut->writeAttr("customValue1", toString(offset));
        tripinfoOut->closeTag();
    }
}

std::string
MSDevice_Seeroad::getParameter(const std::string& key) const {
    if (key == "frictionCoefficient") {		
		//std::stringstream testst; testst << std::fixed << std::setprecision(2) << noiseFriction;
        return toString(myMeasuredFrictionCoefficient);
    } else if (key == "stdev") {
        return toString(myStdDeviation);
    } else if (key == "offset") {
        return toString(myOffset);
    } else if (key == "rawFriction") {
	 return toString(myRawFriction);
	} else if (key == "meaningOfLife") {
		return "42";
	}
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSDevice_Seeroad::setParameter(const std::string& key, const std::string& value) {
    double doubleValue;
    try {
        doubleValue = StringUtils::toDouble(value);
    } catch (NumberFormatException&) {
        throw InvalidArgument("Setting parameter '" + key + "' requires a number for device of type '" + deviceName() + "'");
    }
    if (key == "frictionCoefficient") {
		myMeasuredFrictionCoefficient = doubleValue;
	} else if (key == "stdev") {
		myStdDeviation = doubleValue;
	} else if (key == "offset") {
		myOffset = doubleValue;
	} else {
		throw InvalidArgument("Setting parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
    }
}

double MSDevice_Seeroad::getStdev(const SUMOVehicle & v)
{
	OptionsCont& oc = OptionsCont::getOptions();
	double stdev = -1;
	if (v.getParameter().knowsParameter("device.Seeroad.stdev")) {
		try {
			stdev = StringUtils::toDouble(v.getParameter().getParameter("device.Seeroad.stdev", ""));
		}
		catch (...) {
			WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.Seeroad.stdev", "") + "'for vehicle parameter 'Seeroad.stdev'");
		}
	}
	else if (v.getVehicleType().getParameter().knowsParameter("device.Seeroad.stdev")) {
		try {
			stdev = StringUtils::toDouble(v.getVehicleType().getParameter().getParameter("device.Seeroad.stdev", ""));
		}
		catch (...) {
			WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.Seeroad.stdev", "") + "'for vType parameter 'Seeroad.stdev'");
		}
	}
	else {
		stdev = oc.getFloat("device.Seeroad.stdev");
	}
	return stdev;
}

double MSDevice_Seeroad::getOffset(const SUMOVehicle & v)
{
	OptionsCont& oc = OptionsCont::getOptions();
	double offset = -1;
	if (v.getParameter().knowsParameter("device.Seeroad.offset")) {
		try {
			offset = StringUtils::toDouble(v.getParameter().getParameter("device.Seeroad.offset", ""));
		}
		catch (...) {
			WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.Seeroad.offset", "") + "'for vehicle parameter 'Seeroad.offset'");
		}
	}
	else if (v.getVehicleType().getParameter().knowsParameter("device.Seeroad.offset")) {
		try {
			offset = StringUtils::toDouble(v.getVehicleType().getParameter().getParameter("device.Seeroad.offset", ""));
		}
		catch (...) {
			WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.Seeroad.offset", "") + "'for vType parameter 'Seeroad.offset'");
		}
	}
	else {
		offset = oc.getFloat("device.Seeroad.offset");
	}
	return offset;
}

double MSDevice_Seeroad::applyModel(const double raw)
{
	std::mt19937 rng;
	uint32_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 31) };
	rng.seed(ss);
	double noiseFriction = raw;
	
	std::normal_distribution<double> dist(raw, myStdDeviation);
	//double test = dist(rng);
	noiseFriction = myOffset + dist(rng);
	
	//if (raw > 0.99) {
	//	//std::uniform_real_distribution<double> dist(-myStdDeviation * 10, myStdDeviation * 10);
	//	//noiseFriction = myOffset + (raw * 10 + dist(rng)) / 10;
	//	std::discrete_distribution<> dist({ 0,0.0008589,0.0003681,0.0019632,0.0017178,0.007362,0.0050307,0.0041718,0.010552,0.014233,0.020123,0.033742,0.040736,0.044049,0.048466,0.05362,0.062454,0.063558,0.048589,0.039877,0.053742,0.041595,0.032638,0.026748,0.025399,0.025153,0.025521,0.035215,0.022822,0.031779,0.032515,0.022822,0.025767,0.024417,0.015337,0.01362,0.009816,0.016687,0.0033129,0.0026994 });
	//	double factor = (double(dist(rng)) / 20.) - 1.;
	//	noiseFriction = myOffset + (raw * (1 - factor));
	//} 
	//else if (raw > 0.79) {
	//	//andere Verteilung -> einmal empirisch und einmal normal
	//	//std::normal_distribution<double> dist(0.0121, 0.3087);
	//	std::discrete_distribution<> dist({ 0.0011914,0.0015953,0.0020395,0.0013125,0.0038771,0.004604,0.006482,0.010056,0.0086224,0.0097936,0.013449,0.019123,0.022313,0.028008,0.03978,0.041315,0.054562,0.066011,0.067162,0.074351,0.067505,0.063608,0.05753,0.056621,0.054481,0.052663,0.037054,0.032935,0.028977,0.01987,0.014963,0.014398,0.0084407,0.0068454,0.0028472,0.0013933,0.00060579,0,0,0 });
	//	double factor = (double(dist(rng))/20.) - 1.;
	//	noiseFriction = myOffset + (raw * (1 - factor));
	//}
	//else if (raw > 0.49) {
	//	//wieder andere Verteilung -> empirisch und normal
	//	//std::normal_distribution<double> dist(0.0015, 0.1980);
	//	//noiseFriction = myOffset + (raw * (1 + dist(rng)));
	//	
	//	std::discrete_distribution<> dist({ 0.00026824,0.00015089,0.00068738,0.00057002,0.00098915,0.0010059,0.0015089,0.0014251,0.0019951,0.0036716,0.0056164,0.00793,0.012222,0.016229,0.023505,0.02949,0.0453,0.07142,0.12255,0.15583,0.15404,0.10165,0.064161,0.048049,0.035945,0.026959,0.021778,0.014401,0.010428,0.0075612,0.0039901,0.0027998,0.002213,0.0011568,0.00070414,0.00013412,8.3827e-05,0,0,0 });
	//	double factor = (double(dist(rng)) / 20.) - 1.;
	//	noiseFriction = myOffset + (raw * (1 - factor));
	//}
	//else if (raw > 0.19) {
	//	std::discrete_distribution<> dist({ 0.0001373,0.0001373,0.00022468,0.00018723,0.00014978,0.0003495,0.00027461,0.00077389,0.0010235,0.0011608,0.0010485,0.0015727,0.0015103,0.0016851,0.0027086,0.0034825,0.0060663,0.018149,0.069563,0.40127,0.34262,0.086338,0.021856,0.0099357,0.0075891,0.0072646,0.0040816,0.0026212,0.0012482,0.00086126,0.000699,0.00053673,0.00047432,0.00029957,0.00014978,0,0,0,0,0 });
	//	double factor = (double(dist(rng)) / 20.) - 1.;
	//	noiseFriction = myOffset + (raw * (1 - factor));
	//}
	 

	//std::cout << "ID: " << (this->myID) << " - Raw: " << raw << " vs. noise: " << noiseFriction << std::endl;

	return noiseFriction;
}


/****************************************************************************/
