#ifndef ROLane_h
#define ROLane_h

#include <utils/common/Named.h>

class ROLane : public Named {
private:
    double _length;
    double _maxSpeed;
public:
    ROLane(const std::string &id, double length, double maxSpeed);
    ~ROLane();
//    double getWeight() const;
    double getLength() const;
};

#endif
