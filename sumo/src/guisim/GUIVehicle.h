#ifndef GUIVehicle_h
#define GUIVehicle_h

#include <vector>
#include <string>
#include <microsim/MSVehicle.h>

class GUIVehicle : public MSVehicle {
public:
    ~GUIVehicle();
    static std::vector<std::string> getNames();
private:
    GUIVehicle();
};

#endif
