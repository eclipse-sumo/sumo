#include <string>
#include "StringUtils.h"

using namespace std;

std::string
StringUtils::prune(std::string str) {
    size_t idx = str.find_first_not_of(" ");
    if(idx!=string::npos) {
        str = str.substr(idx);
    }
    idx = str.find_last_not_of(" ");
    if(idx!=string::npos&&idx!=str.length()-1) {
        str = str.substr(0, idx+1);
    }
    return str;
}


std::string
StringUtils::to_lower_case(std::string str)
{
    for(int i=0; i<str.length(); i++) {
        if(str[i]>='A'&&str[i]<='Z') {
            str[i] = str[i] + 'a' - 'A';
        }
    }
    return str;
}
