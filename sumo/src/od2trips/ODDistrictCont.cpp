#include <string>
#include <utils/common/UtilExceptions.h>
#include "ODDistrict.h"
#include <utils/common/NamedObjectCont.h>
#include "ODDistrictCont.h"

using namespace std;

ODDistrictCont::ODDistrictCont()
{
}


ODDistrictCont::~ODDistrictCont()
{
}


std::string
ODDistrictCont::getRandomSourceFromDistrict(const std::string &name) const
{
    ODDistrict *district = get(name);
    if(district==0) {
        throw InvalidArgument(
            string("There is no district '") + name + string("'."));
    }
    return district->getRandomSource();
}


std::string
ODDistrictCont::getRandomSinkFromDistrict(const std::string &name) const
{
    ODDistrict *district = get(name);
    if(district==0) {
        throw InvalidArgument(
            string("There is no district '") + name + string("'."));
    }
    return district->getRandomSink();
}

