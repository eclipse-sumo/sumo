#ifndef ODDistrictCont_h
#define ODDistrictCont_h

#include <utils/common/NamedObjectCont.h>
#include "ODDistrict.h"

class ODDistrictCont : public NamedObjectCont<ODDistrict*> {
public:
    ODDistrictCont();
    ~ODDistrictCont();
    std::string getRandomSourceFromDistrict(const std::string &name) const;
    std::string getRandomSinkFromDistrict(const std::string &name) const;
};

#endif
