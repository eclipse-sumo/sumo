#ifndef SUMOFrame_h
#define SUMOFrame_h

#include <iostream>

class OptionsCont;
class MSNet;

class SUMOFrame {
public:
    static OptionsCont *getOptions();
    static std::ostream *buildRawOutputStream(OptionsCont *oc);
    static void postbuild(MSNet &net);
};

#endif
