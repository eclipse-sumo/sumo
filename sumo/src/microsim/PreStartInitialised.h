#ifndef PreStartInitialised_h
#define PreStartInitialised_h

class MSNet;
class MSEventControl;

class PreStartInitialised {
public:
    PreStartInitialised(MSNet &net);
    virtual ~PreStartInitialised();
    virtual void init(MSNet &net) = 0;
};

#endif
