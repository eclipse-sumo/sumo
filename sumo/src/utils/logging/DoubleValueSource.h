#ifndef DoubleValueSource_h
#define DoubleValueSource_h

class DoubleValueSource {
public:
    DoubleValueSource() { }
    virtual ~DoubleValueSource() { }
    virtual double getValue() const = 0;
    virtual DoubleValueSource *copy() const = 0;
};


#endif
