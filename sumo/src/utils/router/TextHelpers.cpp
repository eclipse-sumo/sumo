#include <string>
#include <sstream>
#include <utils/convert/TplConvert.h>
#include "TextHelpers.h"

using namespace std;


std::string
TextHelpers::version(std::string str) {
    bool isVersion = false;
    // check whether this already is a version
    size_t pos = str.length()-1;
    if(str.at(pos)==')') {
        isVersion = true;
        while(pos>=0&&str.at(--pos)!='(') {
            if(str.at(pos)>'9'||str.at(pos)<'1') {
                isVersion = false;
            }
        }
        if(str.at(pos)!='(') {
            isVersion = false;
        }
    }
    // make a version if it's not yet one
    if(!isVersion)
        return str + " (2)";
    // change the version
    size_t idx1 = str.rfind('(');
    size_t idx2 = str.rfind(')');
    long val = TplConvert<char>::_2long(str.substr(idx1, idx2-idx1).c_str());
    ostringstream strm;
    strm << str.substr(0, idx1) << val+1 << ")";
    return strm.str();
}
