#include "check_errno.hpp"

void chk_errno(std::string msg)
{
    std::stringstream s;
    s << "ERROR: " << strerror(errno) << " <" << msg << ">\n";
    throw std::runtime_error(s.str());
}
