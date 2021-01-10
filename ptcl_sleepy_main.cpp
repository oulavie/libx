#include "ptcl_sleepy.h"
#include <iostream>

void test_1( const char* data )
{
  std::cout << __FUNCTION__ << std::endl;
  size_t len = strlen(data);
  int rtn = sleepy::parse_messages( data, len, 
    []( const std::vector< sleepy::atom >& tokens_, const char* data_, int length_ )
    {
      std::cout << std::string( data_, length_ ) << std::endl;
      std::cout << sleepy::tokens_to_string( tokens_ ) << std::endl;
    } );
  std::cout << std::endl;
}

//--------------------------------------------------------------------------------------------------
void test_A( const char* data )
{
  std::cout << __FUNCTION__ << std::endl;
  auto str = std::istringstream( std::string{ data } );
  sleepy2::parse_messages( str, 
    []( const std::vector< std::string >& tokens_ ) -> void 
    {
      for( auto& it : tokens_ )
        std::cout << it << ";";
    } );
  std::cout << std::endl;
}

//--------------------------------------------------------------------------------------------------
int main()
{
  test_1( "sr 1 ,ty 1 ,id status ,e but ,dstt 12.34 ,;" ); 
  test_1( "sr 1 ,ty 1 ,id status ,e but ,dstt 12.34 ,;" 
          "sr 1 ,ty 2 ,id status ,e but ,dstt 12.35 ,;" );
  test_1( "sr 1 ,ty 1 ,id status ,e ,dstt 12.34 ,;" ); 

  test_A( "sr 1 ,ty 1 ,id status ,e but ,dstt 12.34 ,;" );

  return 0;
}

// g++ ptcl_sleepy_main.cpp

