
#ifndef ValueSource_h
#define ValueSource_h

template<typename _T>
class ValueSource {
public:
    ValueSource() { }
    virtual ~ValueSource() { }
    virtual _T getValue() const = 0;
    virtual ValueSource *copy() const = 0;

};


#endif
