#ifndef RandHelper_h
#define RandHelper_h

class OptionsCont;

class RandHelper {
public:
    static void insertRandOptions(OptionsCont &options);
    static void initRandGlobal(const OptionsCont &options);
};


#endif
