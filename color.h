#ifndef COLOR_H
#define COLOR_H

#include <ostream>
#include <sstream>

// https://misc.flogisoft.com/bash/tip_colors_and_formatting

namespace color {

enum Code 
{
  FG_BLACK         = 30,
  FG_RED           = 31,
  FG_GREEN         = 32,
  FG_YELLOW        = 33,
  FG_BLUE          = 34,
  FG_MAGENTA       = 35,
  FG_CYAN          = 36,
  FG_LIGHT_GRAY    = 37,
  FG_DARK_GRAY     = 90,
  FG_LIGHT_RED     = 91,
  FG_LIGHT_GREEN   = 92,
  FG_LIGHT_YELLOW  = 93,
  FG_LIGHT_BLUE    = 94,
  FG_LIGHT_MAGENTA = 95,
  FG_LIGHT_CYAN    = 96,
  FG_WHITE         = 97,
  FG_DEFAULT       = 39,
  
  BG_RED      = 41,
  BG_GREEN    = 42,
  BG_BLUE     = 44,
  BG_DEFAULT  = 49
};

class set 
{
  Code code;
public:
  set(Code pCode) : code(pCode) {}
  friend std::ostream& operator<<( std::ostream& os, const set& mod) 
  {
    return os << "\033[" << mod.code << "m";
  }
  friend std::ostringstream& operator<<( std::ostringstream& os, const set& mod) 
  {
    std::string clr = std::to_string(mod.code);
    os << "\033[";
    os << clr.data();
    os << "m";
    return os;
  }
};

}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

