#ifndef OptionsSaveStorage_h
#define OptionsSaveStorage_h

#include <string>
#include <map>

class OptionsSaveStorage {
public:
    OptionsSaveStorage(const std::string &structure);
    ~OptionsSaveStorage();
    void insert(const std::string &name, const std::string &value);
    bool save(const std::string &file) const;
private:
    std::string myStructure;
    typedef std::map<std::string, std::string> StringMap;
    StringMap myMap;
};

#endif
