#ifndef DoubleValueRetriever_h
#define DoubleValueRetriever_h

class DoubleValueRetriever {
public:
    DoubleValueRetriever() { }
    virtual ~DoubleValueRetriever() { }
    virtual void addValue(double value) = 0;
};


#endif

