#include <string>
#include "OptionsSaveStorage.h"
#include <utils/common/StringTokenizer.h>
#include <fstream>

using namespace std;


OptionsSaveStorage::OptionsSaveStorage(const std::string &structure)
    : myStructure(structure)
{
}


OptionsSaveStorage::~OptionsSaveStorage()
{
}


void
OptionsSaveStorage::insert(const std::string &name, const std::string &value)
{
    myMap[name] = value;
}


bool
OptionsSaveStorage::save(const std::string &file) const
{
    StringTokenizer st(myStructure, ";");
    ofstream os(file.c_str());
    if(!os.good()) {
        return false;
    }
    os << "<configuration>" << endl;
    while(st.hasNext()) {
        string pre = st.next();
        os << "   <" << pre << ">" << endl;
        for(StringMap::const_iterator i=myMap.begin(); i!=myMap.end(); i++) {
            string name = (*i).first;
            if(name.substr(0, pre.length())!=pre) {
                continue;
            }
            string value = (*i).second;
            os << "      <" << name.substr(pre.length()+1) << ">" << value
                << "</" << name.substr(pre.length()+1) << ">" << endl;
        }
        os << "   </" << pre << ">" << endl;
    }
    os << "</configuration>" << endl;
    bool ret = os.good();
    os.close();
    return ret;
}

