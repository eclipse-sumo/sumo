#ifndef MSActuatedPhaseDefinition_h
#define MSActuatedPhaseDefinition_h

#include "MSPhaseDefinition.h"

/**
 * The definition of a single phase.
 * We moved it out of the main class to allow later template
 * parametrisation.
 */
class MSActuatedPhaseDefinition : public MSPhaseDefinition{
public:
    /// The minimum duration of the phase
    size_t          minDuration;

    /// The maximum duration of the phase
    size_t          maxDuration;

    /// stores the timestep of the last on-switched of the phase
    MSNet::Time _lastSwitch;


    /// constructor
    MSActuatedPhaseDefinition(size_t durationArg,
        const std::bitset<64> &driveMaskArg, const std::bitset<64> &breakMaskArg,
		const std::bitset<64> &yellowMaskArg,
        size_t minDurationArg, size_t maxDurationArg)
        : MSPhaseDefinition(durationArg, driveMaskArg,
            breakMaskArg, yellowMaskArg),
    	minDuration(minDurationArg), maxDuration(maxDurationArg),
        _lastSwitch(0)
    {
        minDuration = 5; //!!!
        maxDuration = 30; //!!!
    }

    /// destructor
    ~MSActuatedPhaseDefinition() { }

private:
    /// invalidated standard constructor
    MSActuatedPhaseDefinition();

};

#endif
