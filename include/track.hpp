#ifndef i93320c32d3445f780b4c26369ddbcb9
#define i93320c32d3445f780b4c26369ddbcb9

#include "defs.hpp"

#include <map>

template<typename InT>
class Track {
public:
    void update(ticks newpos)
    {
        pos = newpos;
    }

    void add(ticks t, InT v)
    {
        score.emplace(t, v);
    }

    InT get() const
    {
        auto cur = (*score.lower_bound(pos)).second;
        //if (cur == score.end()) {
        //    return 0;
        //} else {
          return cur;
        //}
    }

private:
    std::map<ticks, InT> score;
    ticks pos {0};
};

#endif
