#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/SUMOTime.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


SUMOTime debug_globaltime;


#ifdef ABS_DEBUG
SUMOTime debug_searchedtime = 18193;
std::string debug_searched1 = "-----";
std::string debug_searched2 = "-----";
std::string debug_searchedJunction = "-----";
#endif

