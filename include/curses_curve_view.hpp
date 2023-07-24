#ifndef Ua9d54c253e94a9684a8a9e07000717b
#define Ua9d54c253e94a9684a8a9e07000717b

#include "curve.hpp"
#include "ui_curses.hpp"

class CursesCurveView {
public:
    enum direction {
        up,
        flat,
        down
    };

    CursesCurveView(const Curve& c, int h, int w, int y, int x);

    ~CursesCurveView() noexcept;

private:
    WINDOW* win;
};

#endif
