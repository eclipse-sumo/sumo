#ifndef MFXInterThreadEventClient_h
#define MFXInterThreadEventClient_h

class MFXInterThreadEventClient {
public:
    MFXInterThreadEventClient() {}
    virtual ~MFXInterThreadEventClient() { }
    virtual void eventOccured() = 0;
};

#endif
