#ifndef ROLane_h
#define ROLane_h

class ROLane {
private:
    double _length;
    double _maxSpeed;
public:
    ROLane(double length, double maxSpeed);
    ~ROLane();
//    double getWeight() const;
    double getLength() const;
};

#endif
