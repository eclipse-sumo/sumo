#ifndef MSVehicleQuitReminded_h
#define MSVehicleQuitReminded_h

class MSVehicleQuitReminded {
public:
    virtual void removeOnTripEnd( MSVehicle *veh ) = 0;
};

#endif

