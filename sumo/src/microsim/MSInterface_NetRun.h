#ifndef MSInterface_NetRun_h
#define MSInterface_NetRun_h

#include <utils/common/SUMOTime.h>
#include <multimodal/MMLayer.h>

class MSInterface_NetRun : public MMLayer {
public:
    MSInterface_NetRun() { }
    virtual ~MSInterface_NetRun() {}
    virtual bool simulate( SUMOTime start, SUMOTime stop ) = 0;
    virtual void initialiseSimulation() = 0;
    virtual void closeSimulation(SUMOTime start, SUMOTime stop) = 0;
    virtual void simulationStep( SUMOTime start, SUMOTime step ) = 0;
    virtual bool haveAllVehiclesQuit() = 0;
    virtual SUMOTime getCurrentTimeStep() const = 0;
    virtual void preSimStepOutput() const = 0;
    virtual void postSimStepOutput() const = 0;
    virtual long getSimStepDurationInMillis() const = 0;

};

#endif
