#ifndef MSTrafficLightLogic_h
#define MSTrafficLightLogic_h

#include <map>
#include <string>
#include <bitset>
#include "MSNet.h"
#include "MSLogicJunction.h"

class MSTrafficLightLogic {
protected:
    typedef std::map<std::string, MSTrafficLightLogic*> DictType;
    static DictType _dict;
    std::string _id;
public:
    MSTrafficLightLogic(const std::string &id);
    virtual ~MSTrafficLightLogic();
    static bool dictionary(const std::string &name,
        MSTrafficLightLogic *logic);
    static MSTrafficLightLogic *dictionary(const std::string &name);
    virtual void applyPhase(MSLogicJunction::Request &request,
        size_t currentStep) const = 0;
    virtual const std::bitset<64> &linkPriorities(
        size_t currentStep) const = 0;
    //virtual MSNet::Time nextPhase() = 0;
    virtual size_t nextStep(size_t currentStep) = 0;
    virtual MSNet::Time duration(size_t currentStep) const = 0;
    virtual bool linkClosed(size_t currentStep, size_t pos) const = 0;
private:
    MSTrafficLightLogic(const MSTrafficLightLogic &s);
    MSTrafficLightLogic& operator=(const MSTrafficLightLogic &s);
};


#endif
