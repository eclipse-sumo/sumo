#include <utility>
#include <vector>
#include <bitset>
#include "MSEventControl.h"
#include "MSNet.h"
#include "MSTrafficLightLogic.h"
#include "MSSimpleTrafficLightLogic.h"


template< size_t N >
std::bitset<64> MSSimpleTrafficLightLogic<N>::_allClear;

template< size_t N >
MSSimpleTrafficLightLogic<N>::MSSimpleTrafficLightLogic<N>(
    const std::string &id, const Phases &phases)
    : MSTrafficLightLogic(id), _phases(phases),
    _allRed(true)
{
}


template< size_t N >
MSSimpleTrafficLightLogic<N>::~MSSimpleTrafficLightLogic<N>()
{
}


template< size_t N > void
MSSimpleTrafficLightLogic<N>::applyPhase(MSLogicJunction::Request &request,
                                         size_t currentStep) const
{
    if(_allRed) {
        for(size_t i=0; i<request.size(); i++) {
            request[i] = false;
        }
    } else {
        std::bitset<64> allowed = _phases[currentStep].driveMask;
        for(size_t i=0; i<request.size(); i++) {
            request[i] = request[i] & allowed.test(i);
        }
    }
}


template< size_t N > const std::bitset<64> &
MSSimpleTrafficLightLogic<N>::linkPriorities(size_t currentStep) const
{
    if(_allRed) {
        return _allClear;
    } else {
        return _phases[currentStep].breakMask;
    }
}


template< size_t N > size_t
MSSimpleTrafficLightLogic<N>::nextStep(size_t currentStep)
{
    // increment the index to the current phase
    if(!_allRed) {
        _allRed = true;
        return currentStep;
    }
    _allRed = false;
    currentStep++;
    if(currentStep==_phases.size()) {
        currentStep = 0;
    }
    return currentStep;
}


template< size_t N > MSNet::Time
MSSimpleTrafficLightLogic<N>::duration(size_t currentStep) const
{
    if(_allRed) {
        return 20;
    }
    return _phases[currentStep].duration;
}

template< size_t N > 
bool
MSSimpleTrafficLightLogic<N>::linkClosed(size_t currentStep, 
                                         size_t pos) const
{
    if(_allRed) {
        return true;
    }
    return !_phases[currentStep].driveMask.test(pos);
}

