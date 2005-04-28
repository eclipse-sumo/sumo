#ifndef RandHelper_h
#define RandHelper_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


class OptionsCont;

class RandHelper {
public:
    static void insertRandOptions(OptionsCont &options);
    static void initRandGlobal(const OptionsCont &options);
};


#endif
