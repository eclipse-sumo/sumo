#ifndef StringUtils_h
#define StringUtils_h

#include <string>

class StringUtils {
public:
    static std::string prune(std::string str);
    static std::string to_lower_case(std::string str);
};

#endif
