#ifndef GUIVehicleTransfer_h
#define GUIVehicleTransfer_h


#include <microsim/MSVehicleTransfer.h>

class GUIVehicleTransfer : public MSVehicleTransfer
{
public:
    friend class GUINetBuilder;

protected:
    void removeVehicle(const std::string &id);
    GUIVehicleTransfer();


};

#endif
