#ifndef MSTrafficLightJunction_h
#define MSTrafficLightJunction_h

#include <string>
#include <helpers/Command.h>
#include <microsim/MSNet.h>
#include "MSTrafficLightLogic.h"
#include "MSRightOfWayJunction.h"

class MSEventControl;
class MSLink;

class MSTrafficLightJunction : public MSRightOfWayJunction {
protected:
    typedef std::map<MSLink*, size_t> LinkMap;
    LinkMap _linkMap;
    MSTrafficLightLogic *_tlLogic;
    size_t _step;
public:
    MSTrafficLightJunction(const std::string &id, double x, double y,
        InLaneCont* in, MSJunctionLogic* logic, MSTrafficLightLogic *tlLogic,
        size_t delay, size_t initStep, MSEventControl &ec);
    virtual ~MSTrafficLightJunction();
    virtual void postloadInit();
    virtual MSNet::Time nextPhase();
    virtual bool moveFirstVehicles();
    virtual bool linkClosed(const MSLink * link) const;
    friend class SwitchCommand;
protected:
	virtual void deadlockKiller();
private:
    void setLinkPriorities();

    class SwitchCommand : public Command {
    private:
        MSTrafficLightJunction &_junction;
    public:
        SwitchCommand(MSTrafficLightJunction &junction)
            : _junction(junction) { }
        ~SwitchCommand() { }
        MSNet::Time execute() {
            return _junction.nextPhase();
        }
    };
};

#endif
