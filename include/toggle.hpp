#ifndef t4b3112aab1b43e78b1002c29101a5cd
#define t4b3112aab1b43e78b1002c29101a5cd

#include "defs.hpp"

template<typename T>
class Toggle {
public:
    void update(bool tog)
    {
        if (tog) {
            pink_side = !pink_side;
        }
    }

    T get(T green, T pink) const
    {
        if (pink_side) {
            return pink;
        } else {
            return green;
        }
    }

    T get() const {
        return pink_side;
    }

private:
    bool pink_side = false;
};

#endif
