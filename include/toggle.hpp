#ifndef t4b3112aab1b43e78b1002c29101a5cd
#define t4b3112aab1b43e78b1002c29101a5cd

#include "defs.hpp"

class Toggle {
public:
    void update(bool tog);

    template<typename T>
    T get(T green, T pink) const
    {
        if (pink_side) {
            return pink;
        } else {
            return green;
        }
    }

private:
    bool pink_side = false;
};

#endif
