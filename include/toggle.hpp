#ifndef t4b3112aab1b43e78b1002c29101a5cd
#define t4b3112aab1b43e78b1002c29101a5cd

#include "defs.hpp"

class Toggle {
public:
    sample get(bool tog, sample green, sample pink);

private:
    bool pink_side = false;
};

#endif
