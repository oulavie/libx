#ifndef PBX_HEX_H
#define PBX_HEX_H

namespace pbx
{

char uchar_to_hexchar( unsigned char c )
{
  return c<10 ? c+'0' : c-10+'a';
}

template< typename T >
std::string T_to_hex( T&& t )
{
  std::ostringstream oss;
  //oss << t << " " << typeid(t).name() << " 0x";
  for( size_t i(0); i<sizeof(T); ++i )
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
std::string T_to_hex( const char* && p )
{
  std::ostringstream oss;
  //oss << p << " 0x";
  for( auto q = p; *q++; )
  {
    oss << uchar_to_hexchar( (*q & 0xF0) >> 4 );
    oss << uchar_to_hexchar( *q & 0xF );
    oss << " ";
  }
  return oss.str();
}

template<>
std::string T_to_hex( const unsigned char* && p )
{
  std::ostringstream oss;
  //oss << p << " 0x";
  for( auto q = p; *q; q++)
  {
    oss << uchar_to_hexchar( (*q & 0xF0) >> 4 );
    oss << uchar_to_hexchar( *q & 0xF );
    oss << " ";
  }
  return oss.str();
}

template<>
std::string T_to_hex( const std::string&& s )
{
  return T_to_hex( s.data() );
}

//--------------------------------------------------------------------------------------------------
int hexCharToInt(int c)                                                                                                                                   
{
  if( isdigit(c) )
    return c - '0';
  return tolower(c) - 'a' + 10;
}
std::ostringstream char_to_hex( const unsigned char* data_, size_t len_)
{
  std::ostringstream oss;
  if( data_)
  {
    for( size_t i(0); i < len_; ++i)
      oss << std::hex << std::setfill('0') << std::uppercase << std::setw(2) << (unsigned)data_[i] << " ";
  }
  return oss;
}
std::string char_to_hex_string( const unsigned char* data_, size_t len_)
{
  auto rtn = char_to_hex( data_, len_);
  std::string s( rtn.str());
  return s.substr( 0, s.length()-1);
}
std::string uint8_to_hex( uint16_t a)
{
  char buffer[20];
  sprintf( buffer, "%.2X", a );
  return std::string( buffer );
}
std::string uint16_to_hex( uint16_t a)
{
  char buffer[20];
  sprintf( buffer, "%.4X", a );
  std::string s( buffer );
  //auto b = char_to_hex( (const unsigned char*)&a, 2 ); // todo
  //s += b.str();
  return s;
}
std::string mac_to_str( unsigned char *mac)
{
  char buffer[20];
  sprintf( buffer, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return std::string( buffer);
} 

std::ostream& render_printable_chars( std::ostream& os, const char* buffer, size_t bufsize) 
{
  os << " | ";
  for( size_t i = 0; i < bufsize; ++i )
  {
    if( std::isprint( buffer[i]) )
    {
      os << buffer[i];
    }
    else
    {
      os << ".";
    }
  }
  return os;
}

}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

