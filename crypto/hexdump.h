#ifndef HEXDUMP_H
#define HEXDUMP_H
//#pragma once

#include <iostream>
#include <sstream>

void HexDump(const unsigned char *buf, size_t buf_len)
{
  for (size_t pos = 0; pos < buf_len; pos += 16)
  {
    printf("%.4zu: ", pos);

    for (size_t cur = pos; cur < pos + 16; ++cur)
    {
      if (cur < buf_len)
        printf("%02x ", buf[cur]);
      else
        printf("   ");
    }

    printf(" ");

    for (size_t cur = pos; cur < pos + 16; ++cur)
    {
      if (cur < buf_len)
      {
        if (isascii(buf[cur]) && isprint(buf[cur]))
          printf("%c", buf[cur]);
        else
          printf(".");
      }
    }
    printf("\n");
  }
}

//--------------------------------------------------------------------------------------------------

namespace pbx
{

char uchar_to_hexchar( unsigned char c )
{
  return c<10 ? c+'0' : c-10+'A';
}

template< typename T >
std::string to_hex( T&& t, size_t len = sizeof(T) )
{
  std::ostringstream oss;
  oss << t << " " << typeid(t).name() << std::endl;
  for( size_t i(0); i < len; ++i )
  {
    auto h = ((unsigned char*)&t)[i];
    char c1 = uchar_to_hexchar( (h & 0xF0) >> 4 );
    char c2 = uchar_to_hexchar( h & 0xF );
    oss << c1;
    oss << c2;
    oss << " ";
  }
  return oss.str();
}

template<>
std::string to_hex( const char* && p, size_t len )
{
  std::ostringstream oss;
  //oss << p << " 0x";
  //for( auto q = p; *q++; )
  for( auto q = p; q-p < len; q++ )
  {
    oss << uchar_to_hexchar( (*q & 0xF0) >> 4 );
    oss << uchar_to_hexchar( *q & 0xF );
    oss << " ";
  }
  return oss.str();
}

template<>
std::string to_hex( const unsigned char* && p, size_t len )
{
  std::ostringstream oss;
  //oss << p << " 0x";
  for( auto q = p; q-p < len; q++ )
  {
    oss << uchar_to_hexchar( (*q & 0xF0) >> 4 );
    oss << uchar_to_hexchar( *q & 0xF );
    oss << " ";
  }
  return oss.str();
}

/*
template<>
std::string T_to_hex( const std::string&& s_, size_t len_ = s_.length() )
{
  return T_to_hex( s_.data(), len_ );
}
*/

}
#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

