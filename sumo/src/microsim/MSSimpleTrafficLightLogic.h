#ifndef MSSimpleTrafficLightLogic_h
#define MSSimpleTrafficLightLogic_h

#include <utility>
#include <vector>
#include <bitset>
#include "MSEventControl.h"
#include "MSNet.h"
#include "MSTrafficLightLogic.h"

template< size_t N >
class MSSimpleTrafficLightLogic : public MSTrafficLightLogic
{
public:
    class PhaseDefinition {
    public:
        size_t          duration;
        std::bitset<N>  driveMask;
        std::bitset<N>  breakMask;
        PhaseDefinition(size_t durationArg, 
            const std::bitset<N> &driveMaskArg, 
            const std::bitset<N> &breakMaskArg)
            : duration(durationArg), driveMask(driveMaskArg), 
            breakMask(breakMaskArg) { }
        ~PhaseDefinition() { }
    };
    typedef std::vector<PhaseDefinition> Phases;
private:
    Phases _phases;
    bool _allRed;
    static std::bitset<64> _allClear;
public:
    MSSimpleTrafficLightLogic(const std::string &id, const Phases &phases);
    ~MSSimpleTrafficLightLogic();
    virtual void applyPhase(MSLogicJunction::Request &request,
        size_t currentStep) const;
    virtual const std::bitset<64> &linkPriorities(size_t currentStep) const;
    //virtual MSNet::Time nextPhase();
    virtual size_t nextStep(size_t currentStep);
    virtual MSNet::Time duration(size_t currentStep) const;
    virtual bool linkClosed(size_t currentStep, size_t pos) const;
};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#ifndef MSVC
#include "MSSimpleTrafficLightLogic.cpp"
#endif
#endif // EXTERNAL_TEMPLATE_DEFINITION

#endif
