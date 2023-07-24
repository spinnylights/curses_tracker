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

    CursesCurveView(const Curve& c, int h, int w, int y, int x)
        : win {newwin(h,w,y,x)}
    {
        werase(win);
        box(win, 0, 0);
        int inner_w = w - 2;
        if (inner_w < 0) { inner_w = 0; }
        int inner_h = h - 2;
        if (inner_h < 0) { inner_h = 0; }

        const Curve::seek_t dist = 1.0 / inner_w;
        Curve::seek_t read_head = 0.0;

        static constexpr Curve::seek_t epsilon = 0.00025; 

        enum direction last_dir = flat;
        double last_deriv_estim = 0;
        int cell_h_ndx = 0;
        for (int i = 1; i <= inner_w; ++i) {
            read_head += ((dist/2) - epsilon);
            auto left = c.get(read_head);
            read_head += (epsilon*2);
            auto right = c.get(read_head);
            read_head += ((dist/2) - epsilon);

            //auto deriv_estim = (right - left) / dist;
            auto deriv_estim = (right - left) / (epsilon*2);
            auto avg         = (right + left) / 2.0;

            enum direction dir;
            if (deriv_estim > 0.0) {
                dir = up;
            } else if (deriv_estim < 0.0) {
                dir = down;
            } else {
                dir = flat;
            }

            chtype ch;
            if (dir == up && last_dir == down) {
                wmove(win, cell_h_ndx, i - 1);
                //waddch(win, 'v');
                //ch = 'v';
                waddch(win, '.');
                ch = ',';
                //ch = ',';
            } else if (dir == down && last_dir == up) {
                wmove(win, cell_h_ndx, i - 1);
                //waddch(win, '^');
                //ch = '^';
                waddch(win, '\'');
                ch = '`';
                //ch = '`';
            } else if (deriv_estim > 5.0) {
                ch = ACS_UARROW;
            } else if (deriv_estim > 0.5) {
                ch = '/';
            } else if (deriv_estim >= -0.5) {
                ch = ACS_HLINE;
            } else if (deriv_estim >= -5.0) {
                ch = '\\';
            } else {
                ch = ACS_DARROW;
            }

            last_dir = dir;
            last_deriv_estim = deriv_estim;

            cell_h_ndx = ((inner_h
                               - static_cast<int>(std::round(avg*inner_h))) / 2)
                             + 1;
            if (cell_h_ndx < 1) { cell_h_ndx = 1; }
            if (cell_h_ndx > inner_h) { cell_h_ndx = inner_h; }

            wmove(win, cell_h_ndx, i);
            waddch(win, ch);
        }

        wrefresh(win);
    }

    ~CursesCurveView() noexcept
    {
        delwin(win);
    }

private:
    WINDOW* win;
};

#endif
