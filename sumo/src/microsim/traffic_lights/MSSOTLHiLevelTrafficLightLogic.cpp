#include "MSSOTLHiLevelTrafficLightLogic.h"

MSSOTLHiLevelTrafficLightLogic::MSSOTLHiLevelTrafficLightLogic(MSTLLogicControl &tlcontrol,
		const string &id, const string &subid, const Phases &phases,
		unsigned int step, SUMOTime delay,
		const std::map<std::string, std::string>& parameters) :
		MSSOTLTrafficLightLogic(tlcontrol, id, subid, phases, step, delay,
				parameters) {
	// Setting default values 

}

MSSOTLHiLevelTrafficLightLogic::MSSOTLHiLevelTrafficLightLogic(MSTLLogicControl &tlcontrol,
		const string &id, const string &subid, const Phases &phases,
		unsigned int step, SUMOTime delay,
		const std::map<std::string, std::string>& parameters,
		MSSOTLSensors *sensors) :
		MSSOTLTrafficLightLogic(tlcontrol, id, subid, phases, step, delay,
				parameters, sensors) {
	// Setting default values

}

MSSOTLHiLevelTrafficLightLogic::~MSSOTLHiLevelTrafficLightLogic() {
	for (unsigned int i = 0; i < policies.size(); i++) {
		delete (policies[i]);
	}
}

void MSSOTLHiLevelTrafficLightLogic::addPolicy(MSSOTLPolicy* policy) {
    policies.push_back(policy);
}

void MSSOTLHiLevelTrafficLightLogic::init(NLDetectorBuilder &nb) throw (ProcessError) {
	MSSOTLTrafficLightLogic::init(nb);
}

void MSSOTLHiLevelTrafficLightLogic::activate(MSSOTLPolicy* policy) {
	currentPolicy = policy;
}
