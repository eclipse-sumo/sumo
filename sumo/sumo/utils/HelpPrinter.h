#ifndef HelpPrinter_h
#define HelpPrinter_h

#include <iostream>

class HelpPrinter {
public:
    static void print(char *help[]) {
        for(size_t i=0; help[i]!=0; i++)
            std::cout << help[i] << std::endl;
    }
};

#endif
