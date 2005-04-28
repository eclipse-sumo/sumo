/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <utils/options/OptionsCont.h>
#include "RandHelper.h"
#include <ctime>
#include <cmath>


void
RandHelper::insertRandOptions(OptionsCont &options)
{
    options.doRegister("srand", new Option_Integer(23423));
    options.doRegister("abs-rand", new Option_Bool(false));
}


void
RandHelper::initRandGlobal(const OptionsCont &options)
{
    if(options.getBool("abs-rand")) {
        srand(time(0));
    } else {
        srand(options.getInt("srand"));
    }
}
