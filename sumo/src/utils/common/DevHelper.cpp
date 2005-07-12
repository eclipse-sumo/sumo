/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/options/OptionsCont.h>
#include "DevHelper.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
void
DevHelper::insertDevOptions(OptionsCont &options)
{
    options.doRegister("version", new Option_Bool(false));
}

