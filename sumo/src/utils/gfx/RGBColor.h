#ifndef RGBColor_h
#define RGBColor_h

class RGBColor {
private:
    double _red, _green, _blue;
public:
    RGBColor(double red, double green, double blue);
    ~RGBColor();
    double red() const;
    double green() const;
    double blue() const;
};

#endif

