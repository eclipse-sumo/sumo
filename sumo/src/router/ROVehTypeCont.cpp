#include <string>
#include <map>
#include "ROVehicleType.h"
#include "ROVehTypeCont.h"
#include <utils/common/NamedObjectCont.h>

using namespace std;


ROVehTypeCont::ROVehTypeCont(ROVehicleType *defType)
    : _defaultType(defType)
{
}


ROVehTypeCont::~ROVehTypeCont()
{
    delete _defaultType;
}


ROVehicleType *
ROVehTypeCont::getDefault() const {
    return _defaultType;
}
