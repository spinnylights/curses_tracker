#ifndef U221a86059c14228879904b6894f8bc5
#define U221a86059c14228879904b6894f8bc5

#include <stdexcept>

class runtime_error : public std::runtime_error {
public:
    static const std::string err_preamble;
    static const std::string err_postamble;

    runtime_error(std::string msg)
        : std::runtime_error {err_preamble + msg + err_postamble}
    {}
};

#endif
