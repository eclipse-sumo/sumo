#ifndef MSInterface_NetRun_h
#define MSInterface_NetRun_h

class MSInterface_NetRun {
public:
    MSInterface_NetRun() { }
    virtual ~MSInterface_NetRun() {}
    virtual bool simulate( size_t start, size_t stop ) = 0;
    virtual void initialiseSimulation() = 0;
    virtual void closeSimulation(size_t start, size_t stop) = 0;
    virtual void simulationStep( size_t start, size_t step ) = 0;
    virtual bool haveAllVehiclesQuit() = 0;
    virtual size_t getCurrentTimeStep() const = 0;

};

#endif
