#ifndef NINavTeqHelper_h
#define NINavTeqHelper_h

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/StdDefs.h>

class NINavTeqHelper {
public:
	static SUMOReal getSpeed(const std::string &id, const std::string &speedClassS);
	static size_t getLaneNumber(const std::string &id,
		const std::string &laneNoS, SUMOReal speed, bool useNewLaneNumberInfoPlain);
};

#endif
