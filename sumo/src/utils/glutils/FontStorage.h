#ifndef FontStorage_h
#define FontStorage_h

#include <string>
#include <map>
#include "lfontrenderer.h"

class FontStorage {
public:
    FontStorage();
    ~FontStorage();
    void add(const std::string &name, const std::string &file);
    LFont get(const std::string &name) const;
    bool has(const std::string &name) const;
private:
    typedef std::map<std::string, LFont> FontMap;
    FontMap myFonts;
};

#endif
