#ifndef Position2D_h
#define Position2D_h

class Position2D {
private:
    double _x;
    double _y;
public:
    Position2D() : _x(0.0), _y(0.0) { }
    Position2D(double x, double y) 
        : _x(x), _y(y) { }
    ~Position2D() { }
    double x() const { return _x; }
    double y() const { return _y; }
};

#endif
