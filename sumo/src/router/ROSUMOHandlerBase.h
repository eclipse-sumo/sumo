#ifndef ROSUMOHandlerBase_h
#define ROSUMOHandlerBase_h

#include <string>
#include "ROTypedXMLRoutesLoader.h"
#include <utils/gfx/RGBColor.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif



class ROVehicleType;
class RORouteDef;

class ROSUMOHandlerBase :
    public ROTypedXMLRoutesLoader {
public:
    ROSUMOHandlerBase(RONet &net,
        const std::string &dataName, const std::string &file="");

    ~ROSUMOHandlerBase();

    /** @brief Returns the name of the data
        "precomputed sumo route alternatives" is returned here */
    std::string getDataName() const;

protected:
    /// Retrieves a float from the attributes and reports errors, if any occure
    float getFloatReporting(const Attributes &attrs, AttrEnum attr,
        const std::string &id, const std::string &name);

    /// Retrieves the routes's color definition
    RGBColor parseColor(const Attributes &attrs,
        const std::string &type, const std::string &id);

    /// Parses and returns the type of the vehicle
    ROVehicleType* getVehicleType(const Attributes &attrs,
        const std::string &id);

    /// Parses and returns the departure time of the current vehicle
    long getVehicleDepartureTime(const Attributes &attrs,
        const std::string &id);

    /// Parses and returns the route of the vehicle
    RORouteDef *getVehicleRoute(const Attributes &attrs,
        const std::string &id);

    /// Parses a vehicle
    void startVehicle(const Attributes &attrs);

    /// Parses a vehicle type
    void startVehType(const Attributes &attrs);

protected:
    std::string myDataName;

    /// The color of the current route
    RGBColor myCurrentColor;

};

#endif
