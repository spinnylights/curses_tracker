#include "chk_errno.hpp"

void chk_errno(bool err, std::string msg)
{
    if (err){
        std::stringstream s;
        s << "ERROR: " << strerror(errno) << " <" << msg << ">\n";
        throw std::runtime_error(s.str());
    }
}
