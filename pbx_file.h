#ifndef PBX_FILE_H
#define PBX_FILE_H

#include <string>
#include <fstream>

namespace pbx {

template< typename T>
bool read_file( const char* file_, T t_ )
{
  std::ifstream ifs;
  ifs.open( file_, std::ofstream::in );
  if( not ifs.good() )
  {
    return false;
  }
  std::string line;
  while( std::getline( ifs, line ) )
  {
    if( not t_( line ) )
    {
      break;
    }
  }
  ifs.close();
  return true;
}

template< typename T>
bool read_file_g( const char* file_, T t_ )
{
  std::ifstream ifs;
  ifs.open( file_, std::ofstream::in );
  if( not ifs.good() )
  {
    return false;
  }

  ifs.seekg( 0, ifs.end() );
  int file_length = ifs.tellg();
  ifs.seekg( 0, ifs.beg() );

  std::string line;
  while( std::getline( ifs, line ) )
  {
    if( not t_( line, ifs.tellg(), file_length ) )
    {
      break;
    }
  }
  ifs.close();
  return true;
}


}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

