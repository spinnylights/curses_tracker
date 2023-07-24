#include "exceptions.hpp"

const std::string runtime_error::err_preamble =
    "Whoops, curses_tracker threw an exception.\n"
    "=======================================================\n"
    "\n";

const std::string runtime_error::err_postamble =
    "\n\n"
    "=======================================================\n"
    "This probably indicates a bug. You're welcome to email\n"
    "bugs@milky.flowers with the contents of this message if\n"
    "you like.\n"
    "\n";
