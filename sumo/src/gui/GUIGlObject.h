#ifndef GUIGlObject_h
#define GUIGlObject_h

#include <string>

class GUIGlObject {
public:
    GUIGlObject(std::string fullName);
    virtual ~GUIGlObject();
    friend class GUIGlObjectStorage;
    const std::string &getFullName() const;
    size_t getGlID() const;
private:
    void setGlID(size_t id);
private:
    size_t myGlID;
    std::string myFullName;
};

#endif
