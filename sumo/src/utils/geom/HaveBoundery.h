#ifndef HaveBoundery_h
#define HaveBoundery_h

class HaveBoundery {
public:
    HaveBoundery() { }
    virtual ~HaveBoundery() { }
    virtual Boundery getBoundery() const = 0;
};

#endif
