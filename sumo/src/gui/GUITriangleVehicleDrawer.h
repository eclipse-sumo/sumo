#ifndef GUITriangleVehicleDrawer_h
#define GUITriangleVehicleDrawer_h

#include "GUIViewTraffic.h"

class GUILane;
class GUIVehicle;

class GUITriangleVehicleDrawer : public GUIViewTraffic::GUIVehicleDrawer {
public:
    GUITriangleVehicleDrawer();
    ~GUITriangleVehicleDrawer();
    void initStep();
    void drawVehicle(const GUILane &lane, const GUIVehicle &veh);
    void closeStep();
private:
    void setVehicleColor(const GUIVehicle &vehicle);
};

#endif
