#ifndef PBX_STRING_H
#define PBX_STRING_H

#include <string>
#include <algorithm>

namespace pbx {

std::string trim( const std::string& s_, const std::string& discard = " \t" )
{
  const auto b = s_.find_first_not_of( discard );
  if( b == std::string::npos )
    return std::string();
  const auto e = s_.find_last_not_of( discard );
  return s_.substr( b, e - b + 1 ); 
}

std::string remove_endl( const std::string& s_ )
{
  size_t pos { s_.find_first_of("\r\n") };
  if( pos != std::string::npos )
    return s_.substr( 0, pos );
  return s_;
}

std::string skip_spaces( const std::string& s_ )
{
  std::string s{s_};
  s.erase( std::remove_if( s.begin(), s.end(), [](unsigned char c)->bool{ return c==' ' or c== '\n' or c=='\r' or c=='\t' or c=='\v' or c=='\f';}) );
  return s;
}

std::string string_to_lowercase( const std::string& s_ )
{
  std::string s{s_};
  std::transform( s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); } );
  return s;
}

}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

