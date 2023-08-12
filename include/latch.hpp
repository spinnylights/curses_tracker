#ifndef r0943cb8578f4ee9adb73df1cbfbd51b
#define r0943cb8578f4ee9adb73df1cbfbd51b

template<typename T>
class Latch {
public:
    void update(bool tog, T sig)
    {
        if (tog) {
            val = sig;
        }
    }

    T get() const { return val; }

private:
    T val;
};

#endif
