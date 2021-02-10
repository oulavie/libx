#ifndef PBX_EXECUTE_H
#define PBX_EXECUTE_H

#include "pbx_string.h"
#include <string>
#include <vector>
#include <sstream>

namespace pbx {

//--------------------------------------------------------------------------------------------------
std::vector<std::string> execute( const std::string& cmd_, std::ostream *oss_ = nullptr)
{
  std::vector<std::string> v;
  std::ostringstream ostrstr;
  ostrstr << cmd_.data();
  ostrstr << " 2>&1";
  if( oss_)
    *oss_ << cmd_ << std::endl;
  FILE* fp = popen( ostrstr.str().data(), "r");
  if( fp != NULL)
  {
    const size_t sz { 65536};
    char buffer[sz];
    while( fgets( buffer, sz, fp) != NULL)
    {
      std::string s{ buffer};
      s = remove_endl( s);
      v.push_back( s);
      if( oss_)
        *oss_ << s << std::endl;
    }
  }
  fclose(fp);
  std::string cmd("$ ");
  cmd += cmd_;
  v.insert( v.begin(), cmd);
  return v;
}

//--------------------------------------------------------------------------------------------------
template< typename T >
void execute( const std::string& cmd_, T&& t_ )
{
  std::ostringstream ostrstr;
  ostrstr << cmd_.data();
  ostrstr << " 2>&1";
  FILE* fp = popen( ostrstr.str().data(), "r" );
  if( fp != NULL )
  {
    const size_t sz { 65536};
    char buffer[sz];
    while( fgets( buffer, sz, fp ) != NULL)
    {
      std::string s{ buffer };
      s = remove_endl( s );
      t_( s );
    }
  }
  fclose( fp );
}

}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

