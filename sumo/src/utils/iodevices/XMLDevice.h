#ifndef XMLDevice_h
#define XMLDevice_h

class XMLDevice {
public:
    XMLDevice() { }
    virtual ~XMLDevice() { }
    virtual XMLDevice &writeString(const std::string &str) = 0;
    virtual void closeInfo() = 0;
    virtual bool needsDetectorName() const = 0;
};

#endif
