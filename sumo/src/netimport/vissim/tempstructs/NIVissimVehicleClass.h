#ifndef NIVissimVehicleClass_h
#define NIVissimVehicleClass_h

class NIVissimVehicleClass {
public:
    NIVissimVehicleClass(int type, double percentage, int vwish);
    ~NIVissimVehicleClass();
private:
    int myType;
    double myPercentage;
    int myVWish;
};

#endif

