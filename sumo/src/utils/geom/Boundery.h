#ifndef Boundery_h
#define Boundery_h

#include <utility>

class Boundery {
public:
    Boundery();
    Boundery(double x1, double y1, double x2, double y2);
    ~Boundery();
    void add(double x, double y);
    std::pair<double, double> getCenter() const;
    double xmin() const;
    double xmax() const;
    double ymin() const;
    double ymax() const;
    double getWidth() const;
    double getHeight() const;
private:
    double _xmin, _xmax, _ymin, _ymax;
};

#endif
